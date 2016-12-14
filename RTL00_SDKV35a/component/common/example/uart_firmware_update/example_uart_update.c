
#include "example_uart_update.h"

int is_update_image_enable(gpio_t *gpio_uart_update_eable)
{

	HAL_GPIO_PIN  GPIO_Pin;
    u32 active_state;
//	gpio_t gpio_uart_update_eable;
	int ret = 0;
#if 0
    GPIO_Pin.pin_name = 0xC4;
	//low active
	GPIO_Pin.pin_mode = DIN_PULL_HIGH;
	active_state = GPIO_PIN_LOW;
	
    HAL_GPIO_Init(&GPIO_Pin);
	
    if (HAL_GPIO_ReadPin(&GPIO_Pin) == active_state)
    	ret = 0;
	else
		ret = 1;
	
   	HAL_GPIO_DeInit(&GPIO_Pin);
#else
	gpio_init(gpio_uart_update_eable, PIN_NAME);
	gpio_dir(gpio_uart_update_eable, PIN_INPUT);     // Direction: Input
	gpio_mode(gpio_uart_update_eable, PullUp);       // Pull-High
//	ret = gpio_read(&gpio_uart_update_eable);
#endif

	return ret;
}
void example_uart_update_thread(void *param)
{
	int count = 0;
	gpio_t gpio_uart_update_eable;

	is_update_image_enable(&gpio_uart_update_eable);
	//polling MAX_WAIT_TIME*50ms 
	while(count <= MAX_WAIT_TIME)
	{
		printf(" waitting update enable\r\n");
		if(gpio_read(&gpio_uart_update_eable) == 0){
			printf(" update image enabled!\r\n");
			uart_ymodem();
			break;
		}
		else{
			RtlMsleepOS(50);
			count++;
		}		
	}
	
	vTaskDelete(NULL);
}

void example_uart_update(void)
{
	if(xTaskCreate(example_uart_update_thread, ((const char*)"example_uart_update_thread"), 512, NULL, tskIDLE_PRIORITY + 2, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
}


