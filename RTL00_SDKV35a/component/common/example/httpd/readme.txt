HTTPD EXAMPLE

Description:
Based on HTTPD API, an HTTP/HTTPS server example with a simple homepage and GET/POST method test pages are provided

Configuration:
[platform_opts.h]
	#define CONFIG_EXAMPLE_HTTPD         1
[lwipopts.h]
	#define SO_REUSE                     1

SSL Configuration for HTTPS:
[config_rsa.h]
	#define POLARSSL_CERTS_C
	#define POLARSSL_SSL_SRV_C

Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A httpd example thread is started automatically when booting.
Both HTTP and HTTPS are supported by this exmaple, and can be changed by modifying USE_HTTPS.
Can test with a Web browser connecting to the homepage of httpd server.
Should link PolarSSL bignum.c to SRAM to speed up SSL handshake if starting HTTPS server.

