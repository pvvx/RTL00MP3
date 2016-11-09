#include "cmsis_os.h"
#include "diag.h"
#include "wifi_conf.h"
#include "wifi_ind.h"
#include "google/google_nest.h"


#include <googlenest/example_google.h>
#include "cJSON.h"

osThreadId google_thread_id;
#define malloc      pvPortMalloc
#define free        vPortFree

void google_data_retrieve(char *response_buf);

void google_data_retrieve(char *response_buf) {
	//printf("\r\n\r\n\r\nResponse_buf:\r\n%s\r\n", response_buf);
	char *event = NULL;
	char *delims = "\n";
	char *data = NULL, *backup = NULL;
	char *info = NULL;
	cJSON_Hooks memoryHook;

	event = strtok_r(response_buf, delims, &backup);
	data = strtok_r( NULL, delims, &backup );

	if (!strncmp(data, "data: ", strlen("data: "))){
		info = data + strlen("data: ");
		if(info != NULL){
			memoryHook.malloc_fn = malloc;
			memoryHook.free_fn = free;
			cJSON_InitHooks(&memoryHook);

			cJSON *infoJSObject, *pathJSObject, *dataJSObject;
			cJSON *redJSObject, *greenJSObject, *blueJSObject;
			char *red, *green, *blue;


			if((infoJSObject = cJSON_Parse(info)) != NULL) {
				pathJSObject = cJSON_GetObjectItem(infoJSObject, "path");
				dataJSObject = cJSON_GetObjectItem(infoJSObject, "data");
				if(dataJSObject != NULL) {
					
					redJSObject = cJSON_GetObjectItem(dataJSObject, "Red");
					greenJSObject = cJSON_GetObjectItem(dataJSObject, "Green");
					blueJSObject = cJSON_GetObjectItem(dataJSObject, "Blue");

					if(redJSObject) 
						red = redJSObject->valuestring;

					if(greenJSObject)
						green = greenJSObject->valuestring;

					if(blueJSObject)
						blue = blueJSObject->valuestring;

					printf("\n\rThe latest RGB information: RGB(%s, %s, %s)\n\r", red, green, blue);

					cJSON_Delete(dataJSObject);
				}
				cJSON_Delete(infoJSObject);
			}
			else
				printf("\r\nCannot parse the message to JSON!\r\n");
			
		}
		else
			printf("\r\n This is the keep alive message or cannot get the information!\r\n");
	}
	else
		printf("\r\nData structure may wrong!\r\n");
}


void gn_todevice_start(void) {

	googlenest_context googlenest;
	char *googlenest_host = HOST_ADDR;
	char *googlenest_uri = "light.json";

	printf("\r\nStart connecting to Google Nest Server...\r\n");
	memset(&googlenest, 0, sizeof(googlenest_context));

reconnect:
	if(gn_connect(&googlenest, googlenest_host, GN_PORT) == 0) {
		printf("\r\n Connection is OK!\r\n");
		
		google_retrieve_data_hook_callback(google_data_retrieve);
		if(gn_stream(&googlenest, googlenest_uri) != 0){
			printf("\r\n Connection is fail! \r\n Start Reconnecting...\r\n");
			goto reconnect;
		}
			
		gn_close(&googlenest);
	
	}
	else{ 
		printf("\r\n Connection is fail! \r\n\r\n\r\n\r\nStart Reconnecting...\r\n");
		goto reconnect;
	}

}

void gn_fromdevice_start(void) {
	googlenest_context googlenest;
	char *googlenest_uri = "MotionSensor.json";
	cJSON_Hooks memoryHook;
        int j = 0;

	memoryHook.malloc_fn = malloc;
	memoryHook.free_fn = free;
	cJSON_InitHooks(&memoryHook);
	printf("\r\nStart connecting to Google Nest Server!\r\n");
	
	while(1) {
		memset(&googlenest, 0, sizeof(googlenest_context));
		if(gn_connect(&googlenest, HOST_ADDR, GN_PORT) == 0) {
			cJSON *MSJSObject;
			char *data;

			if((MSJSObject = cJSON_CreateObject()) != NULL) {
					cJSON_AddItemToObject(MSJSObject, "MotionSenser", cJSON_CreateNumber(j++));
					data = cJSON_Print(MSJSObject);
					cJSON_Delete(MSJSObject);
			}

			if(gn_put(&googlenest, googlenest_uri, data) == 0)
				printf("\n\rUpdate the Motion Sensor's data to %d\n\r", (j-1));
			free(data);		
			gn_close(&googlenest);
		}
		else{
			printf("\n\rConnection failed!\n\r");
			break;
		}

		vTaskDelay(5 * configTICK_RATE_HZ);
	}
}

void gn_fromdevice_task(void *arg) {
	int i;

	printf("\n\r\n\r\n\r\n\r<<<<<<Waiting for 2 mins to connect Wi-Fi>>>>>>>\n\r\n\r\n\r\n\r");
	for (i = 0; i<120; i++) 
		vTaskDelay(1000 / portTICK_PERIOD_MS);

	gn_fromdevice_start();
}

void gn_todevice_task(void *arg) {
	int i;

	printf("\n\r\n\r\n\r\n\r<<<<<<Waiting for 2 mins to connect Wi-Fi>>>>>>>\n\r\n\r\n\r\n\r");
	for (i = 0; i<120; i++)
		vTaskDelay(1000 / portTICK_PERIOD_MS);

	gn_todevice_start();
}

void example_google(char *type) {


	if(strcmp(type, "FromDevice") == 0){
		osThreadDef_t google_thread;
		google_thread.instances = 1;
		google_thread.name = "google thread";
		google_thread.stacksize = 4096;
		google_thread.pthread = (os_pthread)gn_fromdevice_task;
		google_thread.tpriority = tskIDLE_PRIORITY+6;
		google_thread_id = osThreadCreate(&google_thread, NULL);
	}
	else if(strcmp(type, "ToDevice") == 0){
		osThreadDef_t google_thread;
		google_thread.instances = 1;
		google_thread.name = "google thread";
		google_thread.stacksize = 4096;
		google_thread.pthread = (os_pthread)gn_todevice_task;
		google_thread.tpriority = tskIDLE_PRIORITY+6;
		google_thread_id = osThreadCreate(&google_thread, NULL);
	}

}

