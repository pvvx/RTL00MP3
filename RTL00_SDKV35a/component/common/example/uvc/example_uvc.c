
/*
* V4L2 video capture example
*/

#include "FreeRTOS.h"
#include "task.h"
#include "example_uvc.h"
#include "videodev2.h"
#include "uvcvideo.h"
#include "v4l2_driver.h"
#include "mjpeg/mjpeg.h"
#include "rtsp/rtsp_api.h"
#include "sockets.h"
#include "lwip/netif.h"

#include "uvc_intf.h"

#include "section_config.h"
SDRAM_DATA_SECTION struct rtp_object rtp_payload[VIDEO_MAX_FRAME];

struct rtp_payload_queue payload_queue;

void example_uvc(void)
{
          /*init payload queue*/
        INIT_LIST_HEAD(&payload_queue.wait_queue);
        INIT_LIST_HEAD(&payload_queue.done_queue);
        RtlMutexInit(&payload_queue.wait_mutex);
        RtlMutexInit(&payload_queue.done_mutex);
        RtlInitSema(&payload_queue.wait_sema, 0);
        RtlInitSema(&payload_queue.done_sema, 0);
        payload_queue.flush_err = 0;
        uvc_stream_init();
}

void uvc_entry_handle(void *param)
{
        int i, ret, cnt;
        struct stream_context *stream_ctx = (struct stream_context *)param;
        struct uvc_buf_context buf;
        struct rtp_object *payload;
        
        /*initialize rtp payload*/        
        for(i = 0; i < VIDEO_MAX_FRAME; i++)
        {
                if(rtp_init_payload(stream_ctx, &rtp_payload[i]) < 0)
                {
                    for(; i>=0; i--)
                    {
                        rtp_uninit_payload(stream_ctx, &rtp_payload[i]);
                    }
                    goto exit;
                }
        }

        if(uvc_set_param(stream_ctx, UVC_FORMAT_MJPEG, 640, 480, 30)<0)
              goto exit;

        if(uvc_stream_on(stream_ctx)<0)
              goto exit;
       
        /*do buffer queue & dequeue inside the loop*/
        payload_queue.flush_err = 0;
        while(!(payload_queue.flush_err))
        {
                memset(&buf, 0, sizeof(struct uvc_buf_context));
                ret = uvc_dqbuf(stream_ctx, &buf);                              
                if(buf.index < 0)
                        continue;//empty buffer retrieved
                if((uvc_buf_check(&buf)<0)||(ret < 0)){
                        RTSP_ERROR("\n\rbuffer error!");
			ret = -ENOENT;
			goto exit;
                }
                rtp_payload[buf.index].index = buf.index;
                if(rtp_fill_payload(stream_ctx, &rtp_payload[buf.index], buf.data, buf.len) < 0)
                        goto exit;

                /*add rtp_payload into payload queue*/
                RtlDownMutex(&payload_queue.wait_mutex);
                list_add_tail(&rtp_payload[buf.index].rtp_list, &payload_queue.wait_queue);
                RtlUpMutex(&payload_queue.wait_mutex);
                RtlUpSema(&payload_queue.wait_sema);
                
                //check if any rtp payload is queued in done_queue
                while(RtlDownSemaWithTimeout(&payload_queue.done_sema, 5)==0)
                {
                        if(payload_queue.flush_err)
                            goto exit;
                }
                if(!list_empty(&payload_queue.done_queue))
                {
                        RtlDownMutex(&payload_queue.done_mutex);
                        payload = list_first_entry(&payload_queue.done_queue, struct rtp_object, rtp_list);
                        if(payload == NULL)
                        {
                                RtlUpMutex(&payload_queue.done_mutex);
                                continue;
                        }
                        list_del_init(&payload->rtp_list);
                        RtlUpMutex(&payload_queue.done_mutex);                       
                                                                
                        buf.index = payload->index;
                        buf.data = payload->data;
                        buf.len = payload->len;
                        
                        ret = uvc_qbuf(stream_ctx, &buf);                   
                        if (ret < 0){
                            RTSP_ERROR("\n\rread_frame mmap method enqueue buffer failed");
                            ret = -ENOENT;
                            goto exit;
                        }
                }
        }

exit:
        uvc_stream_off(stream_ctx);
        uvc_stream_free(stream_ctx);

        for(i = 0; i < VIDEO_MAX_FRAME; i++)
        {
                rtp_uninit_payload(stream_ctx, &rtp_payload[i]);
        }
        
        //free payload_queue memory
        INIT_LIST_HEAD(&payload_queue.wait_queue);
        INIT_LIST_HEAD(&payload_queue.done_queue);
        RtlMutexFree(&payload_queue.wait_mutex);
        RtlMutexFree(&payload_queue.done_mutex);
        RtlFreeSema(&payload_queue.wait_sema); 
        RtlFreeSema(&payload_queue.done_sema);
        
        printf("\n\rstream free success, delete task...");
        vTaskDelete(NULL);
}

    
        
int uvc_rtp_init(struct rtsp_context *rtsp_ctx);
void uvc_rtsp_handle(void *param)
{        
	struct stream_context *stream_ctx = (struct stream_context *)param;
        struct rtsp_context *rtsp_ctx;
        u8 *request_header; //buffer to hold rtsp request
        struct sockaddr_in server_addr, client_addr;
	int client_socket;
        socklen_t client_addr_len = sizeof(struct sockaddr_in);
        
        fd_set read_fds;
	struct timeval timeout;
        int ok;
        rtsp_ctx = malloc(sizeof(struct rtsp_context));
        if(rtsp_ctx == NULL)
        {
            RTSP_ERROR("\n\rrtsp context is NULL");
            goto exit;
        }
        request_header = malloc(512);
        if(request_header == NULL)
        {
                RTSP_ERROR("\n\rallocate request header buffer failed");
                goto exit;
        }
        // Delay to wait for IP by DHCP
	vTaskDelay(500);
        	/*init rtsp context to unicast udp mode*/
	if(rtsp_context_init(rtsp_ctx) < 0)
                goto exit;
        
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = *(uint32_t *)(rtsp_ctx->connect_ctx.server_ip)/*htonl(INADDR_ANY)*/;
	server_addr.sin_port = htons(rtsp_ctx->connect_ctx.port);                
		
	if(bind(rtsp_ctx->connect_ctx.socket_id, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		RTSP_ERROR("\n\rCannot bind stream socket");
		goto exit;
	}
        listen(rtsp_ctx->connect_ctx.socket_id, 1);
        printf("\n\rrtsp context initialized!");      
                
	stream_ctx->protoCtx = (void *)rtsp_ctx;
	rtsp_ctx->stream_ctx = (void *)stream_ctx;
        
        /*start rtp task*/
        uvc_rtp_init(rtsp_ctx);

        
	while(stream_ctx->allowStream)
	{
            FD_ZERO(&read_fds);
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            FD_SET(rtsp_ctx->connect_ctx.socket_id, &read_fds);
            if(select(1, &read_fds, NULL, NULL, &timeout))
            {
		client_socket = accept(rtsp_ctx->connect_ctx.socket_id,(struct sockaddr *)&client_addr, &client_addr_len);
		if(client_socket < 0)
		{
			RTSP_ERROR("client_socket error:%d\r\n", client_socket);
			close(client_socket);
			continue;
		}
                *(rtsp_ctx->connect_ctx.remote_ip + 3) = (unsigned char) (client_addr.sin_addr.s_addr >> 24);
                *(rtsp_ctx->connect_ctx.remote_ip + 2) = (unsigned char) (client_addr.sin_addr.s_addr >> 16);
                *(rtsp_ctx->connect_ctx.remote_ip + 1) = (unsigned char) (client_addr.sin_addr.s_addr >> 8);
                *(rtsp_ctx->connect_ctx.remote_ip) = (unsigned char) (client_addr.sin_addr.s_addr );

		while(stream_ctx->allowStream)
		{


                    read(client_socket, request_header, 512);
                    rtsp_readheader(request_header);
                    if(*request_header == 0)
                    {

                            //Do I need to send error response to client?
                            continue;
                    }


                    rtsp_getparam(rtsp_ctx, request_header);
                    switch(rtsp_ctx->rtsp_cmd)
                    {

                            case(CMD_OPTIONS):
                                    RTSP_PRINTF("\n\rReceive options command!");
                                    if(rtsp_ctx->state == RTSP_PLAYING)
                                           break;

                                    rtsp_cmd_options(rtsp_ctx);
                                    ok = write(client_socket, rtsp_ctx->response, strlen(rtsp_ctx->response));
                                    if (ok <= 0)
                                    {


                                           RTSP_ERROR("\n\rsend OPTIONS response failed!");
                                           goto exit;
                                    }
                                    break;



                            case(CMD_DESCRIBE):
                                    RTSP_PRINTF("\n\rReceive describe command!");
                                    if(rtsp_ctx->state == RTSP_PLAYING)
                                           break;

                                    rtsp_cmd_describe(rtsp_ctx);
                                    ok = write(client_socket, rtsp_ctx->response, strlen(rtsp_ctx->response));
                                    if (ok <= 0)
                                    {


                                           RTSP_ERROR("\n\rsend DESCRIBE response failed!");
                                           goto exit;                                                                                                       
                                    }
                                    break;



                            case(CMD_SETUP):
                                    RTSP_PRINTF("\n\rReceive setup command!");
                                    if(rtsp_ctx->state == RTSP_PLAYING)
                                           break;

                                    //fill transport parameter
                                    rtsp_cmd_setup(rtsp_ctx);
                                    ok = write(client_socket, rtsp_ctx->response, strlen(rtsp_ctx->response));
                                    if (ok <= 0)
                                    {


                                           RTSP_ERROR("\n\rsend SETUP response failed!");
                                           goto exit;
                                    }


                                    if(rtsp_ctx->state == RTSP_INIT)
                                    {

                                            rtsp_ctx->state = RTSP_READY;
                                            RTSP_PRINTF("\n\rstate changed from RTSP_INIT to RTSP_READY");
                                    };
                                    break;


                            case(CMD_TEARDOWN):
                                    RTSP_PRINTF("\n\rReceive teardown command!");
                                    rtsp_ctx->state = RTSP_INIT;
                                    rtsp_cmd_teardown(rtsp_ctx);
                                    ok = write(client_socket, rtsp_ctx->response, strlen(rtsp_ctx->response));
                                    if (ok <= 0)
                                    {


                                            RTSP_ERROR("\n\rsend TEARDOWN response failed!");
                                            goto exit;
                                    }
                                    



                                    RTSP_PRINTF("\n\rstreaming teardown, state changed back to RTSP_INIT");
                                    /*have to wait until rtp server reinit*/
                                    vTaskDelay(1000);
                                    goto out;
                                    break;


                            case(CMD_PLAY):
                                    RTSP_PRINTF("\n\rReceive play command!");
                                    if(rtsp_ctx->state == RTSP_PLAYING)
                                           break;

                                    rtsp_cmd_play(rtsp_ctx);
                                    ok = write(client_socket, rtsp_ctx->response, strlen(rtsp_ctx->response));
                                    if (ok <= 0)
                                    {


                                            RTSP_ERROR("\n\rsend PLAY response failed!");
                                            goto exit;
                                    }


                                    if(rtsp_ctx->state == RTSP_READY)
                                    {

                                            rtsp_ctx->state = RTSP_PLAYING;
                                            RTSP_PRINTF("\n\rstate changed from RTSP_READY to RTSP_PLAYING");
                                            rtsp_ctx->is_rtp_start = 1;
                                            RtlUpSema(&rtsp_ctx->start_rtp_sema);
                                    }
                                    break;




                            case(CMD_PAUSE):
                                    RTSP_PRINTF("\n\rReceive pause command!");
                                    rtsp_cmd_pause(rtsp_ctx);
                                    ok = write(client_socket, rtsp_ctx->response, strlen(rtsp_ctx->response));
                                    if (ok <= 0)
                                    {


                                            RTSP_ERROR("\n\rsend PAUSE response failed!");
                                            goto exit;
                                    }


                                    if(rtsp_ctx->state == RTSP_PLAYING)
                                    {

                                            rtsp_ctx->state = RTSP_READY;
                                            RTSP_PRINTF("\n\rstate changed from RTSP_PLAYING to RTSP_READY");
                                    }
                                    break;
                            default:



                                    RTSP_ERROR("\n\rReceive unrecognized command!");
                                    rtsp_cmd_error(rtsp_ctx);
                                    ok = write(client_socket, rtsp_ctx->response, strlen(rtsp_ctx->response));
                                    if (ok <= 0)
                                    {


                                            RTSP_ERROR("\n\rsend ERROR response failed!");
                                            goto exit;
                                    }
                                    rtsp_ctx->state = RTSP_INIT;
                    }
                    if((rtsp_ctx->is_rtp_start == 0) && (rtsp_ctx->state == RTSP_PLAYING))
                    {


                          rtsp_ctx->state = RTSP_INIT;
                          RtlUpSema(&rtsp_ctx->start_rtp_sema);
                    }


		}
out:
                rtsp_ctx->state = RTSP_INIT;
                close(client_socket);                               
            }

	}               
exit:
        if((rtsp_ctx->is_rtp_start) == 1){
               RtlUpSema(&rtsp_ctx->start_rtp_sema); 
        }
	printf("\n\rrtsp -> Available heap 0x%x\n", xPortGetFreeHeapSize());
        close(client_socket);
        close(rtsp_ctx->connect_ctx.socket_id);
        if(request_header != NULL)
        free(request_header);
        /*wait until rtp task being destroyed*/
        while((rtsp_ctx->is_rtp_start))
        {
                vTaskDelay(100);
        }
        rtsp_context_free(rtsp_ctx);
        if(rtsp_ctx != NULL)
                free(rtsp_ctx);
        RTSP_ERROR("\n\rkill rtsp server thread!");
        //printf("Available heap 0x%x\n", xPortGetFreeHeapSize());
	//thread must be killed after server socket is terminated
	vTaskDelete(NULL);
                
}


void uvc_rtp_udp_init(struct stream_context *stream_ctx)
{

	struct rtsp_context *rtsp_ctx = (struct rtsp_context *)stream_ctx->protoCtx;
        struct uvc_buf_context buf;
        struct rtp_object *payload;
	struct sockaddr_in rtp_addr;                
	int rtp_socket;                
        int i, ret;        
        socklen_t addrlen = sizeof(struct sockaddr_in);
        int rtp_port; 
        /* varibles for recording statistic use*/
        unsigned int cnt, total, total_time, time1, time2, time3;
        cnt = total = total_time = time1 = time2 = time3 = 0;
        /*init rtp socket*/
	rtp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	rtp_port = rtsp_ctx->transport.serverport_min;
	memset(&rtp_addr, 0, addrlen);
	rtp_addr.sin_family = AF_INET;
	rtp_addr.sin_addr.s_addr = *(uint32_t *)(rtsp_ctx->connect_ctx.server_ip);
	rtp_addr.sin_port = htons((u16)rtp_port);                
	if (bind(rtp_socket,(struct sockaddr *)&rtp_addr, addrlen)<0) {
                RTSP_ERROR("bind failed\r\n");
                goto exit;
	}

        
restart:
	while((stream_ctx->isProcess)&&(rtsp_ctx->state == RTSP_PLAYING))
	{
                if(RtlDownSemaWithTimeout(&payload_queue.wait_sema, 5)==0)
                    continue;
                time1 = time3;
                time2 = xTaskGetTickCount();
                /*send rtp payload*/
                if(!list_empty(&payload_queue.wait_queue))
                {
                    RtlDownMutex(&payload_queue.wait_mutex);
                    payload = list_first_entry(&payload_queue.wait_queue, struct rtp_object, rtp_list);
                    if(payload == NULL)
                    {
                            RtlUpMutex(&payload_queue.wait_mutex);
                            continue;
                    }
                    list_del_init(&payload->rtp_list);
                    RtlUpMutex(&payload_queue.wait_mutex);
                                           
                    if(rtsp_ctx->state == RTSP_PLAYING)
                    {

                        payload->connect_ctx.socket_id = rtp_socket;
                        payload->connect_ctx.port = (u16)rtsp_ctx->transport.clientport_min;
                        payload->connect_ctx.server_ip = rtsp_ctx->connect_ctx.server_ip;
                        payload->connect_ctx.remote_ip = rtsp_ctx->connect_ctx.remote_ip;
                    
                        ret = rtp_udp_send(stream_ctx, payload);                           
                    }
                    
                    //dequeue the this buffer from payload_queue 
                    RtlDownMutex(&payload_queue.done_mutex);
                    list_add_tail(&payload->rtp_list, &payload_queue.done_queue);
                    RtlUpMutex(&payload_queue.done_mutex);                        
                    RtlUpSema(&payload_queue.done_sema);
                    
                    time3 = xTaskGetTickCount();
                    cnt ++;
                    total += payload->len;
                    total_time += (time3-time1);
                    if(cnt == 100)
                    {
                      /*        print statistics info      */
                      /*1.average frame size(kB) T:2.time waited for next frame sending start(ms)-3.udp sending time(ms) 4.frame rate(fps)*/
                      printf("\n\r%dkB T:%d-%d %dfps", (total/102400), (time2 - time1), (time3 - time2), (100000/total_time));
                      cnt = 0;
                      total = 0;
                      total_time = 0;
                    }
                }

	}
        mdelay(1000);
        if(rtsp_ctx->state == RTSP_READY)
        {
            goto restart;
        }

exit:
	close(rtp_socket);
}

void uvc_rtp_tcp_init(struct stream_context *stream_ctx)
{
}

void uvc_rtp_multi_init(struct stream_context *stream_ctx)
{
}

void uvc_rtp_handle(void *param)
{
        struct stream_context *stream_ctx = (struct stream_context *)param;
        struct rtsp_context *rtsp_ctx = (struct rtsp_context *)stream_ctx->protoCtx;
   
        /*go down when rtsp state change to playing*/
	while(1)
	{
        	RtlDownSema(&rtsp_ctx->start_rtp_sema);
		/*check rtp cast mode*/
                if(rtsp_ctx->state == RTSP_PLAYING)
		{
                    printf("\n\rrtp start...");
                    switch(rtsp_ctx->transport.castMode)
                    {
                        case(UNICAST_UDP_MODE):
                            uvc_rtp_udp_init(stream_ctx);
                            break;
                        case(MULTICAST_MODE):
                            uvc_rtp_tcp_init(stream_ctx);
                            break;
                        case(UNICAST_TCP_MODE):
                            uvc_rtp_multi_init(stream_ctx);
                            break;
                        default:
                            RTSP_ERROR("\r\n unknown streaming mode! Go back to RTSP_INIT state\n");
                            rtsp_ctx->is_rtp_start = 0;
                            break;
                    }
		}else{	
                    
                    break;
                }	
                printf("\n\rrtp stop...");
	}
	rtsp_ctx->is_rtp_start = 0;
        RTSP_ERROR("\n\rkill rtp server thread!");
        //printf("Available heap 0x%x\n", xPortGetFreeHeapSize()); 		
        vTaskDelete(NULL);
}


int uvc_rtp_init(struct rtsp_context *rtsp_ctx)
{
        struct stream_context *stream_ctx = (struct stream_context *)rtsp_ctx->stream_ctx;

        if(xTaskCreate(uvc_rtp_handle, ((const signed char*)"uvc_rtp_handle"), 2048, (void *)stream_ctx, tskIDLE_PRIORITY + 2, NULL) != pdPASS) {
              RTSP_ERROR("\r\n uvc_rtp_handle: Create Task Error\n");
              return -1;
        }
        return 0;
}

void uvc_task_init(void * param)
{

/*entry to start uvc streaming -- dequeue uvc buffer*/
    if(xTaskCreate(uvc_entry_handle, ((const signed char*)"uvc_entry_handle"), 1024, param, tskIDLE_PRIORITY + 2, NULL) != pdPASS) {
        UVC_ERROR("\r\n uvc_entry_handle: Create Task Error\n");
    }

/*entry to start rtsp server*/
#if UVC_RTSP_EN
    if(xTaskCreate(uvc_rtsp_handle, ((const signed char*)"uvc_rtsp_handle"), 4096, param, tskIDLE_PRIORITY + 2, NULL) != pdPASS) {
        RTSP_ERROR("\r\n uvc_rtsp_handle: Create Task Error\n");
    }
#endif   
}


/************************************************************end of rtsp/rtp with motion-jpeg************************************************/
