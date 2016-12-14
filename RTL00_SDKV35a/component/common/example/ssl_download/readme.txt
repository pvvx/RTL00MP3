SSL DOWNLOAD EXAMPLE

Description:
Download file from Web server via https.

Configuration:
Modify SSL_MAX_CONTENT_LEN in SSL config and configTOTAL_HEAP_SIZE in freertos config for large size file
Modify SERVER_HOST, SERVER_PORT and RESOURCE in example_ssl_download.c based on your SSL server

[platform_opts.h]
    #define CONFIG_EXAMPLE_SSL_DOWNLOAD    1

Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A ssl download example thread will be started automatically when booting.

