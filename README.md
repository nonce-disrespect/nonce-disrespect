Nonce-Disrespecting Adversaries
===============================

We provide supplemental material to our research on AES-GCM nonce reuse vulnerabilities in TLS.

Research paper
==============

* Nonce-Disrespecting Adversaries: Practical Forgery Attacks on GCM in TLS (will be published soon on the Cryptology ePrint Archive)

Background
==========

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

This repository provides supplemental code and information.

Code
====

* [getnonce](getnonce/) - scan tool and OpenSSL patch used for our Internet-wide scan.
* [gcmproxy](gcmproxy/) - attack implemented in Go.
* [tool](tool/) - helper tools used by attack code.

All our code is published as CC0 / public domain.

Advisories
==========

Security advisories from affected vendors:
* [Security Bulletin: Vulnerability in IBM Domino Web Server TLS AES GCM Nonce Generation (CVE-2016-0270)](https://www-01.ibm.com/support/docview.wss?uid=swg21979604)
* [Radware / SA18456: Security Advisory Explicit Initialization Vector for AES-GCM Cipher](https://kb.radware.com/Questions/SecurityAdvisory/Public/Security-Advisory-Explicit-Initialization-Vector-f)

Misc
====

* [Youtube video showing XSS injection on visa.dk](https://www.youtube.com/watch?v=qByIrRigmyo)
* [Black Hat USA 2016 talk announcement](https://www.blackhat.com/us-16/briefings/schedule/#nonce-disrespecting-adversaries-practical-forgery-attacks-on-gcm-in-tls-3483)
