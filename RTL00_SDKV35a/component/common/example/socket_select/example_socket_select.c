#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>
#include <lwip/sockets.h>

#if LWIP_SOCKET

#define MAX_SOCKETS     10
#define SELECT_TIMEOUT  10
#define SERVER_PORT     5000
#define LISTEN_QLEN     2

static void example_socket_select_thread(void *param)
{
	struct sockaddr_in server_addr;
	int server_fd = -1;
	int socket_used[10];

	memset(socket_used, 0, sizeof(socket_used));

	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) >= 0) {
		socket_used[server_fd] = 1;
	}
	else {
		printf("socket error\n");
		goto exit;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
		printf("bind error\n");
		goto exit;
	}

	if(listen(server_fd, LISTEN_QLEN) != 0) {
		printf("listen error\n");
		goto exit;
	}

	while(1) {
		int socket_fd;
		unsigned char buf[512];
		fd_set read_fds;
		struct timeval timeout;

		FD_ZERO(&read_fds);
		timeout.tv_sec = SELECT_TIMEOUT;
		timeout.tv_usec = 0;

		for(socket_fd = 0; socket_fd < MAX_SOCKETS; socket_fd ++)
			if(socket_used[socket_fd])
				FD_SET(socket_fd, &read_fds);

		if(select(MAX_SOCKETS, &read_fds, NULL, NULL, &timeout)) {
			for(socket_fd = 0; socket_fd < MAX_SOCKETS; socket_fd ++) {
				if(socket_used[socket_fd] && FD_ISSET(socket_fd, &read_fds)) {
					if(socket_fd == server_fd) {
						struct sockaddr_in client_addr;
						unsigned int client_addr_size = sizeof(client_addr);
						int fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_size);

						if(fd >= 0) {
							printf("accept socket fd(%d)\n", fd);
							socket_used[fd] = 1;
						}
						else {
							printf("accept error\n");
						}
					}
					else {
						int read_size = read(socket_fd, buf, sizeof(buf));

						if(read_size > 0) {
							write(socket_fd, buf, read_size);
						}
						else {
							printf("socket fd(%d) disconnected\n", socket_fd);
							socket_used[socket_fd] = 0;
							close(socket_fd);
						}
					}
				}
			}
		}
		else {
			printf("TCP server: no data in %d seconds\n", SELECT_TIMEOUT);
		}
	}

exit:
	if(server_fd >= 0)
		close(server_fd);

	vTaskDelete(NULL);
}

void example_socket_select(void)
{
	if(xTaskCreate(example_socket_select_thread, ((const char*)"example_socket_select_thread"), 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
}

#endif // LWIP_SOCKET
