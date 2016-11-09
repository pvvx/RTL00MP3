#include "rtl8195a.h"
#include <main.h>
#include "rtl8195a_it.h"

/* os dependent*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#include <diag.h>
#define printf DiagPrintf

/*-----------------------------Global Variable ---------------------*/
//#ifdef CONFIG_WLAN	
//#ifdef CONFIG_ISR_THREAD_MODE_INTERRUPT
extern xSemaphoreHandle *pExportWlanIrqSemaphore;
//#endif 
//#endif 


#ifdef CONFIG_WLAN	
#ifdef CONFIG_ISR_THREAD_MODE_INTERRUPT

//TODO: chris
#define IRQ_HANDLED 1
#define IRQ_NONE 0

int wlan_Interrupt (
    IN  VOID* Data
)
{
#if 1	

	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	printf("wlan interrupt\n");
	/* This semaphore is initialized once wlan interrupt handler thread is created and initialized*/
	if(!pExportWlanIrqSemaphore)
	{
		printf("%s(%d)\n", __FUNCTION__, __LINE__);
		goto exit;
	}

	printf("%s(%d)\n", __FUNCTION__, __LINE__);
	xSemaphoreGiveFromISR( *pExportWlanIrqSemaphore, &xHigherPriorityTaskWoken );

	printf("%s(%d)\n", __FUNCTION__, __LINE__);
	/* Switch tasks if necessary. */	
	if( xHigherPriorityTaskWoken != pdFALSE )
	{
		printf("%s(%d)\n", __FUNCTION__, __LINE__);
		portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
	}

exit:

	return IRQ_HANDLED;
#else
	struct dvobj_priv *dvobj = (struct dvobj_priv *)Data;
	_adapter *adapter = dvobj->if1;
	DBG_8192C("Dma isr\n");

	if (dvobj->irq_enabled == 0) {
		return IRQ_HANDLED;
	}
DBG_871X("%s(%d)\n", __FUNCTION__, __LINE__);
	if(rtw_hal_interrupt_handler(adapter) == _FAIL)
		return IRQ_HANDLED;
		//return IRQ_NONE;
DBG_871X("%s(%d)\n", __FUNCTION__, __LINE__);
	return IRQ_HANDLED;
#endif
    
}


VOID
lextra_bus_dma_Interrupt (
    IN  VOID* Data
);


/*
  * This function register interrupt handler and is called by wlan driver 
  * Return 0 if success, Others if fail
  */
  
int irq_alloc_wlan(void *contex)
{
	int ret = 0;
	IRQ_HANDLE      IrqHandle = {0};

	printf("Register Interrupt\n");
	IrqHandle.Data = (u32) (contex);
	IrqHandle.IrqNum = WL_DMA_IRQ;
	IrqHandle.IrqFun = (IRQ_FUN)wlan_Interrupt;
	//IrqHandle.IrqFun = (IRQ_FUN)lextra_bus_dma_Interrupt;
	IrqHandle.Priority = 0;

	InterruptRegister(&IrqHandle);
	InterruptEn(&IrqHandle); 

	return ret;

}
#endif
#endif 

