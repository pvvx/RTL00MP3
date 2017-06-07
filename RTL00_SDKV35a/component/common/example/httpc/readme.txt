HTTPC EXAMPLE

Description:
Based on HTTPC API, an HTTP/HTTPS client example to access httpbin.org for test are provided

Configuration:
[platform_opts.h]
	#define CONFIG_EXAMPLE_HTTPC         1

Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A httpc example thread is started automatically when booting.
GET to http://httpbin.org/get and POST to http://httpbin.org/post will be verified.
Both HTTP and HTTPS are supported by this exmaple, and can be changed by modifying USE_HTTPS.
Should link PolarSSL bignum.c to SRAM to speed up SSL handshake for HTTPS client.
