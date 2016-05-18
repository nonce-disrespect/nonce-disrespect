/* try to get 100 AES GCM nonces
 * code: Hanno Böck, license: CC0 / public domain
 */

#include <openssl/ssl.h>
#include <openssl/bio.h>

//#define CMDLINE
SSL_CTX *ctx = NULL;

#define READBLOCKSIZE 2048

void nonceinit()
{
	SSL_library_init();

	ctx = SSL_CTX_new(TLSv1_2_method());

	SSL_CTX_set_timeout(ctx, 1);
	SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);

	SSL_CTX_set_options(ctx,
			    SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 |
			    SSL_OP_NO_COMPRESSION | SSL_OP_NO_TLSv1 |
			    SSL_OP_NO_TLSv1_1);

}

void getnonce(char *host)
{
	BIO *web = NULL;
	SSL *ssl;
	X509 *x509 = NULL;
	char buf1[READBLOCKSIZE + 1];
	char buf2[READBLOCKSIZE + 1];
	unsigned long long i, max;
	size_t l;
	max=100;
//	max<<=34;


	if (0 == (web = BIO_new_ssl_connect(ctx))) {
		goto free;
	}

	if (1 != BIO_set_conn_hostname(web, host)) {
		goto free;
	}

	BIO_get_ssl(web, &ssl);
	if (ssl == NULL) {
		goto free;
	}

	if (1 != SSL_set_cipher_list(ssl, "AESGCM")) {
		goto free;
	}

	if (1 != BIO_do_connect(web)) {
		goto free;
	}

	if (1 != BIO_do_handshake(web)) {
		goto free;
	}

	printf("AESGCMSCAN START: %s\n\n", host);

	x509 = SSL_get_peer_certificate(ssl);

	printf("\nAESGCMSCAN CERTIFICATE\n");

	PEM_write_X509(stdout, x509);



	BIO_puts(web, "GET /a HTTP/1.1\r\nHost: x.de\r\n\r\n");
	l = BIO_read(web, buf1, READBLOCKSIZE);
	printf("AESGCMSCAN HTTP DATA\n");
	fwrite(buf1, 1, l, stdout);

	for (i = 0; i < max; i++) {
		BIO_puts(web, "GET /a HTTP/1.1\r\nHost: x.de\r\n\r\n");
		l = BIO_read(web, buf2, READBLOCKSIZE);
		if (l == 0)
			break;
	}


	printf("AESGCMSCAN END: %s\n\n", host);

 free:
	if (web != NULL)
		BIO_free_all(web);

	if (NULL != x509)
		X509_free(x509);

}

int main(int argc, char *argv[])
{

#ifndef CMDLINE
	char ip[25];

	nonceinit();
	while (fgets(ip, 20, stdin)) {
		strtok(ip, "\n");
		if (ip[0] == 0)
			return 0;
		strcat(ip, ":443");
		getnonce(ip);
	}
#else
	int i;

	nonceinit();
	for (i = 1; i < argc; i++)
		getnonce(argv[i]);
#endif

	SSL_CTX_free(ctx);

	return 0;
}
