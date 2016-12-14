#include <FreeRTOS.h>
#include <task.h>
#include <platform/platform_stdlib.h>

#include <lwip/sockets.h>
#include <polarssl/config.h>
#include <polarssl/memory.h>
#include <polarssl/ssl.h>

#define SERVER_HOST    "192.168.13.27"
#define SERVER_PORT    443
#define RESOURCE       "/dummy100k.bin"

static unsigned int arc4random(void)
{
	unsigned int res = xTaskGetTickCount();
	static unsigned int seed = 0xDEADB00B;

	seed = ((seed & 0x007F00FF) << 7) ^
		((seed & 0x0F80FF00) >> 8) ^ // be sure to stir those low bits
		(res << 13) ^ (res >> 9);    // using the clock too!

	return seed;
}

static void get_random_bytes(void *buf, size_t len)
{
	unsigned int ranbuf;
	unsigned int *lp;
	int i, count;
	count = len / sizeof(unsigned int);
	lp = (unsigned int *) buf;

	for(i = 0; i < count; i ++) {
		lp[i] = arc4random();  
		len -= sizeof(unsigned int);
	}

	if(len > 0) {
		ranbuf = arc4random();
		memcpy(&lp[i], &ranbuf, len);
	}
}

static int my_random(void *p_rng, unsigned char *output, size_t output_len)
{
	get_random_bytes(output, output_len);
	return 0;
}

static void example_ssl_download_thread(void *param)
{
	int server_fd = -1, ret;
	struct sockaddr_in server_addr;
	ssl_context ssl;

	// Delay to wait for IP by DHCP
	vTaskDelay(10000);
	printf("\nExample: SSL download\n");

	memory_set_own(pvPortMalloc, vPortFree);
	memset(&ssl, 0, sizeof(ssl_context));

	if((ret = net_connect(&server_fd, SERVER_HOST, SERVER_PORT)) != 0) {
		printf("ERROR: net_connect ret(%d)\n", ret);
		goto exit;
	}

	if((ret = ssl_init(&ssl)) != 0) {
		printf("ERRPR: ssl_init ret(%d)\n", ret);
		goto exit;
	}

	ssl_set_endpoint(&ssl, SSL_IS_CLIENT);
	ssl_set_authmode(&ssl, SSL_VERIFY_NONE);
	ssl_set_rng(&ssl, my_random, NULL);
	ssl_set_bio(&ssl, net_recv, &server_fd, net_send, &server_fd);

	if((ret = ssl_handshake(&ssl)) != 0) {
		printf("ERROR: ssl_handshake ret(-0x%x)", -ret);
		goto exit;
	}
	else {
		unsigned char buf[2048];
		int read_size = 0, resource_size = 0, content_len = 0, header_removed = 0;

		printf("SSL ciphersuite %s\n", ssl_get_ciphersuite(&ssl));
		sprintf(buf, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", RESOURCE, SERVER_HOST);
		ssl_write(&ssl, buf, strlen(buf));

		while((read_size = ssl_read(&ssl, buf, sizeof(buf))) > 0) {
			if(header_removed == 0) {
				char *header = strstr(buf, "\r\n\r\n");

				if(header) {
					char *body, *content_len_pos;

					body = header + strlen("\r\n\r\n");
					*(body - 2) = 0;
					header_removed = 1;
					printf("\nHTTP Header: %s\n", buf);
					read_size = read_size - ((unsigned char *) body - buf);

					content_len_pos = strstr(buf, "Content-Length: ");
					if(content_len_pos) {
						content_len_pos += strlen("Content-Length: ");
						*(strstr(content_len_pos, "\r\n")) = 0;
						content_len = atoi(content_len_pos);
					}
				}
				else {
					printf("ERROR: HTTP header\n");
					goto exit;
				}
			}

			printf("read resource %d bytes\n", read_size);
			resource_size += read_size;
		}

		printf("final read size = %d bytes\n", read_size);
		printf("http content-length = %d bytes, download resource size = %d bytes\n", content_len, resource_size);
	}

exit:
	if(server_fd != -1)
		net_close(server_fd);

	ssl_free(&ssl);
	vTaskDelete(NULL);
}

void example_ssl_download(void)
{
	if(xTaskCreate(example_ssl_download_thread, ((const char*)"example_ssl_download_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
}
