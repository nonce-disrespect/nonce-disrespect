Nonce-Disrespecting Adversaries
===============================

We provide supplemental material to our research on AES-GCM nonce reuse vulnerabilities in TLS.

Research paper
==============

* [Nonce-Disrespecting Adversaries: Practical Forgery Attacks on GCM in TLS (final version / Usenix WOOT)](https://www.usenix.org/system/files/conference/woot16/woot16-paper-bock.pdf)
* [Nonce-Disrespecting Adversaries: Practical Forgery Attacks on GCM in TLS (preprint version / eprint.iacr.org)](https://eprint.iacr.org/2016/475)

Online check
============

* [Online check for GCM nonces](https://gcm.tlsfun.de/)

Background
==========
**Abstract**:

```
We investigate nonce reuse issues with the GCM block cipher mode as
used in TLS and focus in particular on AES-GCM, the most widely
deployed variant. With an Internet-wide scan we identified 184 HTTPS
servers repeating nonces, which fully breaks the authenticity of the
connections. Affected servers include large corporations, financial
institutions, and a credit card company. We present a proof of
concept of our attack allowing to violate the authenticity of affected
HTTPS connections which in turn can be utilized to inject seemingly
valid content into encrypted sessions. Furthermore, we discovered
over 70,000 HTTPS servers using random nonces, which puts them at risk
of nonce reuse, in the unlikely case that large amounts of data are
sent via the same session.
```

This repository provides supplemental code and information.

Code
====

* [getnonce](getnonce/) - scan tool and OpenSSL patch used for our Internet-wide scan.
* [gcmproxy](gcmproxy/) - attack implemented in Go.
* [tool](tool/) - helper tools used by attack code.

License
=======
All our code is published as [CC0 1.0 / Public
Domain](https://creativecommons.org/publicdomain/zero/1.0/).

Data
====

* [Collected data from our Internet-wide scan on scans.io](https://scans.io/study/nonce-disrespect)

Advisories
==========

Security advisories from affected vendors:
* [Security Bulletin: Vulnerability in IBM Domino Web Server TLS AES GCM Nonce Generation (CVE-2016-0270)](https://www-01.ibm.com/support/docview.wss?uid=swg21979604)
* [Radware / SA18456: Security Advisory Explicit Initialization Vector for AES-GCM Cipher](https://kb.radware.com/Questions/SecurityAdvisory/Public/Security-Advisory-Explicit-Initialization-Vector-f)
* [A10: CVE-2016-0270 GCM nonce vulnerability (fixed in 2.7.2-p8)](https://files.a10networks.com/vadc/cve-2016-0270-gcm-nonce-vulnerability/)

Media / Blogs
=============

* [Golem: TLS/GCM - Gefahr durch doppelte Nonces](http://www.golem.de/news/tls-gcm-gefahr-durch-doppelte-nonces-1605-121005.html)
* [Ars Technica: “Forbidden attack” makes dozens of HTTPS Visa sites vulnerable to tampering](http://arstechnica.com/security/2016/05/faulty-https-settings-leave-dozens-of-visa-sites-vulnerable-to-forgery-attacks/)
* [Veracode: Crypto Fun at Black Hat 2016](https://www.veracode.com/blog/2016/08/crypto-fun-black-hat-2016)
* [David Wong: Breaking https' AES-GCM (or a part of it)](https://www.cryptologie.net/article/361/nonce-disrespecting-adversaries-practical-forgery-attacks-on-gcm-in-tls/)

Misc
====

* [TLS Symmetric Crypto (Blogpost by Adam Langley with the initial idea for this research)](https://www.imperialviolet.org/2014/02/27/tlssymmetriccrypto.html)
* [Authentication Failures in NIST version of GCM (Antoine Joux, source for Forbidden Attack against GCM)](http://csrc.nist.gov/groups/ST/toolkit/BCM/documents/comments/800-38_Series-Drafts/GCM/Joux_comments.pdf)
* [Youtube video showing XSS injection on visa.dk](https://www.youtube.com/watch?v=qByIrRigmyo)
* [Black Hat USA 2016 talk announcement](https://www.blackhat.com/us-16/briefings/schedule/#nonce-disrespecting-adversaries-practical-forgery-attacks-on-gcm-in-tls-3483)
* [Usenix WOOT '16 talk announcement](https://www.usenix.org/conference/woot16/workshop-program/presentation/bock)
* [Slides from talk at BerlinSec Meetup](https://www.int21.de/slides/berlinsec-gcm/)
* [Errata on RFC5288 (AES GCM Cipher-suites in
  TLS)](https://www.rfc-editor.org/errata_search.php?rfc=5288&eid=4694)
  * [TLS-WG discussion on said
  errata](https://mailarchive.ietf.org/arch/search/?email_list=tls&gbt=1&index=pV7IzE5XmgUytI5OemV-vqjzPqE)

