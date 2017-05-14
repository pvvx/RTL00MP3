/*
 FreeRTOS V8.1.2 - Copyright (C) 2014 Real Time Engineers Ltd.
 All rights reserved

 VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

 ***************************************************************************
 *                                                                       *
 *    FreeRTOS provides completely free yet professionally developed,    *
 *    robust, strictly quality controlled, supported, and cross          *
 *    platform software that has become a de facto standard.             *
 *                                                                       *
 *    Help yourself get started quickly and support the FreeRTOS         *
 *    project by purchasing a FreeRTOS tutorial book, reference          *
 *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
 *                                                                       *
 *    Thank you!                                                         *
 *                                                                       *
 ***************************************************************************

 This file is part of the FreeRTOS distribution.

 FreeRTOS is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License (version 2) as published by the
 Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

 >>!   NOTE: The modification to the GPL is included to allow you to     !<<
 >>!   distribute a combined work that includes FreeRTOS without being   !<<
 >>!   obliged to provide the source code for proprietary components     !<<
 >>!   outside of the FreeRTOS kernel.                                   !<<

 FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  Full license text is available from the following
 link: http://www.freertos.org/a00114.html

 1 tab == 4 spaces!

 ***************************************************************************
 *                                                                       *
 *    Having a problem?  Start by reading the FAQ "My application does   *
 *    not run, what could be wrong?"                                     *
 *                                                                       *
 *    http://www.FreeRTOS.org/FAQHelp.html                               *
 *                                                                       *
 ***************************************************************************

 http://www.FreeRTOS.org - Documentation, books, training, latest versions,
 license and Real Time Engineers Ltd. contact details.

 http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
 including FreeRTOS+Trace - an indispensable productivity tool, a DOS
 compatible FAT file system, and our tiny thread aware UDP/IP stack.

 http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
 Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
 licenses offer ticketed support, indemnification and middleware.

 http://www.SafeRTOS.com - High Integrity Systems also provide a safety
 engineered and independently SIL3 certified version for use in safety and
 mission critical applications that require provable dependability.

 1 tab == 4 spaces!
 */

/*
 * A sample implementation of pvPortMalloc() that allows the heap to be defined
 * across multiple non-contigous blocks and combines (coalescences) adjacent
 * memory blocks as they are freed.
 *
 * See heap_1.c, heap_2.c, heap_3.c and heap_4.c for alternative
 * implementations, and the memory management pages of http://www.FreeRTOS.org
 * for more information.
 *
 * Usage notes:
 *
 * vPortDefineHeapRegions() ***must*** be called before pvPortMalloc().
 * pvPortMalloc() will be called if any task objects (tasks, queues, event
 * groups, etc.) are created, therefore vPortDefineHeapRegions() ***must*** be
 * called before any other objects are defined.
 *
 * vPortDefineHeapRegions() takes a single parameter.  The parameter is an array
 * of HeapRegion_t structures.  HeapRegion_t is defined in portable.h as
 *
 * typedef struct HeapRegion
 * {
 *	uint8_t *pucStartAddress; << Start address of a block of memory that will be part of the heap.
 *	size_t xSizeInBytes;	  << Size of the block of memory.
 * } HeapRegion_t;
 *
 * The array is terminated using a NULL zero sized region definition, and the
 * memory regions defined in the array ***must*** appear in address order from
 * low address to high address.  So the following is a valid example of how
 * to use the function.
 *
 * HeapRegion_t xHeapRegions[] =
 * {
 * 	{ ( uint8_t * ) 0x80000000UL, 0x10000 }, << Defines a block of 0x10000 bytes starting at address 0x80000000
 * 	{ ( uint8_t * ) 0x90000000UL, 0xa0000 }, << Defines a block of 0xa0000 bytes starting at address of 0x90000000
 * 	{ NULL, 0 }                << Terminates the array.
 * };
 *
 * vPortDefineHeapRegions( xHeapRegions ); << Pass the array into vPortDefineHeapRegions().
 *
 * Note 0x80000000 is the lower address so appears in the array first.
 *
 */
#include <string.h>
#include <stdlib.h>
#include "diag.h"
#include "platform_autoconf.h"
#include "hal_misc.h"

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
 all the API functions to use the MPU wrappers.  That should only be done when
 task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "task.h"

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* Block sizes must not get too small. */
#define heapMINIMUM_BLOCK_SIZE	( ( size_t ) ( uxHeapStructSize << 1 ) )

/* Assumes 8bit bytes! */
#define heapBITS_PER_BYTE		( ( size_t ) 8 )

/* Define the linked list structure.  This is used to link free blocks in order
 of their memory address. */
typedef struct A_BLOCK_LINK {
	struct A_BLOCK_LINK *pxNextFreeBlock; /*<< The next free block in the list. */
	size_t xBlockSize; /*<< The size of the free block. */
} BlockLink_t;

/*-----------------------------------------------------------*/

/*
 * Inserts a block of memory that is being freed into the correct position in
 * the list of free memory blocks.  The block being freed will be merged with
 * the block in front it and/or the block behind it if the memory blocks are
 * adjacent to each other.
 */
static void prvInsertBlockIntoFreeList(BlockLink_t *pxBlockToInsert);

/*-----------------------------------------------------------*/

/* The size of the structure placed at the beginning of each allocated memory
 block must by correctly byte aligned. */
static const uint32_t uxHeapStructSize = ((sizeof(BlockLink_t)
		+ ( portBYTE_ALIGNMENT - 1)) & ~portBYTE_ALIGNMENT_MASK);

/* Create a couple of list links to mark the start and end of the list. */
static BlockLink_t xStart, *pxEnd = NULL;

/* Keeps track of the number of free bytes remaining, but says nothing about
 fragmentation. */
static size_t xFreeBytesRemaining = 0;
static size_t xMinimumEverFreeBytesRemaining = 0;

/* Gets set to the top bit of an size_t type.  When this bit in the xBlockSize
 member of an BlockLink_t structure is set then the block belongs to the
 application.  When the bit is free the block is still part of the free heap
 space. */
//static size_t xBlockAllocatedBit = 0;
/* Work out the position of the top bit in a size_t variable. */
#define xBlockAllocatedBit  (( ( size_t ) 1 ) << ( ( sizeof( size_t ) * heapBITS_PER_BYTE ) - 1 ))

/* Realtek test code start */
//TODO: remove section when combine BD and BF
#if (defined(CONFIG_PLATFORM_8195A) || defined(CONFIG_PLATFORM_8711B))
#include "section_config.h"
SRAM_HEAP_SECTION
#endif
unsigned char ucHeap[configTOTAL_HEAP_SIZE];

//extern void * __sdram_bss_end__;
//extern void * __ram_heap1_start__, __ram_heap1_end__, __ram_heap2_start__, __sdram_data_start__;

extern HeapRegion_t xHeapRegions[];

#if 0
#if defined(CONFIG_PLATFORM_8195A)
HeapRegion_t xHeapRegions[] =
{
	{	(uint8_t*)0x10003000, 0x10006000 - 0x10003000}, // __ram_heap1_start__, __ram_heap1_end__ - __ram_heap1_start__
	{	ucHeap, sizeof(ucHeap)},	        // Defines a block from ucHeap
#ifdef CONFIG_SDR_EN
	{	(uint8_t*)&__sdram_bss_end__, 0x80000},
#endif
	{	NULL, 0}     // Terminates the array.
};
#elif (defined CONFIG_PLATFORM_8711B)
HeapRegion_t xHeapRegions[] =
{
	{	ucHeap, sizeof(ucHeap)}, 	// Defines a block from ucHeap
	{	NULL, 0}                // Terminates the array.
};
#else
#error NOT SUPPORT CHIP
#endif
#endif

/*-----------------------------------------------------------*/
/*
 Dump xBlock list
 */
void dump_mem_block_list(void) {
	if (pxEnd == NULL)
		vPortDefineHeapRegions(xHeapRegions);
#if CONFIG_DEBUG_LOG > 1
//	if(pxEnd == NULL) vPortDefineHeapRegions( xHeapRegions ); // test code start
	BlockLink_t *pxBlock = &xStart;
	int count = 0;
	DBG_8195A("RAM Free Heap Memory List:\n");
	for (pxBlock = pxBlock->pxNextFreeBlock; pxBlock->pxNextFreeBlock != NULL;
			pxBlock = pxBlock->pxNextFreeBlock) {
		DBG_8195A(" [%d]=%p, %d\n", ++count, pxBlock, pxBlock->xBlockSize);
	}
#endif
}

void *pvPortMalloc(size_t xWantedSize) {
	BlockLink_t *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
	void *pvReturn = NULL;

	/* Realtek test code start */
	if (pxEnd == NULL)
		vPortDefineHeapRegions(xHeapRegions);
	/* Realtek test code end */

	/* The heap must be initialised before the first call to
	 prvPortMalloc(). */
	configASSERT( pxEnd );

	vTaskSuspendAll();
	{
		/* Check the requested block size is not so large that the top bit is
		 set.  The top bit of the block size member of the BlockLink_t structure
		 is used to determine who owns the block - the application or the
		 kernel, so it must be free. */
		if ((xWantedSize & xBlockAllocatedBit) == 0) {
			/* The wanted size is increased so it can contain a BlockLink_t
			 structure in addition to the requested amount of bytes. */
			if (xWantedSize > 0) {
				xWantedSize += uxHeapStructSize;

				/* Ensure that blocks are always aligned to the required number
				 of bytes. */
				if ((xWantedSize & portBYTE_ALIGNMENT_MASK) != 0x00) {
					/* Byte alignment required. */
					xWantedSize += ( portBYTE_ALIGNMENT
							- (xWantedSize & portBYTE_ALIGNMENT_MASK));
				} else {
					mtCOVERAGE_TEST_MARKER();
				}
			} else {
				mtCOVERAGE_TEST_MARKER();
			}

			if ((xWantedSize > 0) && (xWantedSize <= xFreeBytesRemaining)) {
				/* Traverse the list from the start	(lowest address) block until
				 one	of adequate size is found. */
				pxPreviousBlock = &xStart;
				pxBlock = xStart.pxNextFreeBlock;
				while ((pxBlock->xBlockSize < xWantedSize)
						&& (pxBlock->pxNextFreeBlock != NULL)) {
					pxPreviousBlock = pxBlock;
					pxBlock = pxBlock->pxNextFreeBlock;
				}

				/* If the end marker was reached then a block of adequate size
				 was	not found. */
				if (pxBlock != pxEnd) {
					/* Return the memory space pointed to - jumping over the
					 BlockLink_t structure at its start. */
					pvReturn =
							(void *) (((uint8_t *) pxPreviousBlock->pxNextFreeBlock)
									+ uxHeapStructSize);

					/* This block is being returned for use so must be taken out
					 of the list of free blocks. */
					pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

					/* If the block is larger than required it can be split into
					 two. */
					if ((pxBlock->xBlockSize - xWantedSize)
							> heapMINIMUM_BLOCK_SIZE) {
						/* This block is to be split into two.  Create a new
						 block following the number of bytes requested. The void
						 cast is used to prevent byte alignment warnings from the
						 compiler. */
						pxNewBlockLink = (void *) (((uint8_t *) pxBlock)
								+ xWantedSize);

						/* Calculate the sizes of two blocks split from the
						 single block. */
						pxNewBlockLink->xBlockSize = pxBlock->xBlockSize
								- xWantedSize;
						pxBlock->xBlockSize = xWantedSize;

						/* Insert the new block into the list of free blocks. */
						prvInsertBlockIntoFreeList((pxNewBlockLink));
					} else {
						mtCOVERAGE_TEST_MARKER();
					}

					xFreeBytesRemaining -= pxBlock->xBlockSize;

					if (xFreeBytesRemaining < xMinimumEverFreeBytesRemaining) {
						xMinimumEverFreeBytesRemaining = xFreeBytesRemaining;
					} else {
						mtCOVERAGE_TEST_MARKER();
					}

					/* The block is being returned - it is allocated and owned
					 by the application and has no "next" block. */
					pxBlock->xBlockSize |= xBlockAllocatedBit;
					pxBlock->pxNextFreeBlock = NULL;
				} else {
					mtCOVERAGE_TEST_MARKER();
				}
			} else {
				mtCOVERAGE_TEST_MARKER();
			}
		} else {
			mtCOVERAGE_TEST_MARKER();
		} traceMALLOC( pvReturn, xWantedSize );
	}
	(void) xTaskResumeAll();
	if (pvReturn == NULL) {
		DBG_RAM_HEAP_WARN("ram_alloc(%d): freeSpace(%d)!\n", xWantedSize,
				xFreeBytesRemaining);
	} else {
//		DBG_RAM_HEAP_INFO("ram_alloc:%p[%d]\n", pvReturn , xWantedSize);
	}
#if( configUSE_MALLOC_FAILED_HOOK == 1 )
	{
		if( pvReturn == NULL )
		{
			extern void vApplicationMallocFailedHook( void );
			vApplicationMallocFailedHook();
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}
	}
#endif

	return pvReturn;
}
/*-----------------------------------------------------------*/

void __vPortFree(void *pv) {
	uint8_t *puc = (uint8_t *) pv;
	BlockLink_t *pxLink;

	if (pv != NULL) {
		/* The memory being freed will have an BlockLink_t structure immediately
		 before it. */
		puc -= uxHeapStructSize;

		/* This casting is to keep the compiler from issuing warnings. */
		pxLink = (void *) puc;

		/* Check the block is actually allocated. */
		configASSERT( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 ); configASSERT( pxLink->pxNextFreeBlock == NULL );

		if ((pxLink->xBlockSize & xBlockAllocatedBit) != 0) {
			if (pxLink->pxNextFreeBlock == NULL) {
				/* The block is being returned to the heap - it is no longer
				 allocated. */
				pxLink->xBlockSize &= ~xBlockAllocatedBit;

				vTaskSuspendAll();
				{
					/* Add this block to the list of free blocks. */
					xFreeBytesRemaining += pxLink->xBlockSize;
					traceFREE( pv, pxLink->xBlockSize );
					prvInsertBlockIntoFreeList(((BlockLink_t *) pxLink));
				}
				(void) xTaskResumeAll();
			} else {
				mtCOVERAGE_TEST_MARKER();
			}
		} else {
			mtCOVERAGE_TEST_MARKER();
		}
//		DBG_RAM_HEAP_INFO("ram_free:%p[%d]\n", pv , pxLink->xBlockSize);
	}
}

/*-----------------------------------------------------------*/
/* Add by Alfa 2015/02/04 -----------------------------------*/
static void (*ext_free)(void *p) = NULL;
//static
uint32_t ext_upper = 0;
//static
uint32_t ext_lower = 0;
void vPortSetExtFree(void (*free)(void *p), uint32_t upper, uint32_t lower) {
	ext_free = free;
	ext_upper = upper;
	ext_lower = lower;
}

void vPortFree(void *pv) {
	if (((uint32_t) pv >= ext_lower) && ((uint32_t) pv < ext_upper)) {
		// use external free function
		if (ext_free)
			ext_free(pv);
	} else
		__vPortFree(pv);
}

/*-----------------------------------------------------------*/

size_t xPortGetFreeHeapSize(void) {
	return xFreeBytesRemaining;
}
/*-----------------------------------------------------------*/

size_t xPortGetMinimumEverFreeHeapSize(void) {
	return xMinimumEverFreeBytesRemaining;
}
/*-----------------------------------------------------------*/

static void prvInsertBlockIntoFreeList(BlockLink_t *pxBlockToInsert) {
	BlockLink_t *pxIterator;
	uint8_t *puc;

	/* Iterate through the list until a block is found that has a higher address
	 than the block being inserted. */
	for (pxIterator = &xStart; pxIterator->pxNextFreeBlock < pxBlockToInsert;
			pxIterator = pxIterator->pxNextFreeBlock) {
		/* Nothing to do here, just iterate to the right position. */
	}

	/* Do the block being inserted, and the block it is being inserted after
	 make a contiguous block of memory? */
	puc = (uint8_t *) pxIterator;
	if ((puc + pxIterator->xBlockSize) == (uint8_t *) pxBlockToInsert) {
		pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;
		pxBlockToInsert = pxIterator;
	} else {
		mtCOVERAGE_TEST_MARKER();
	}

	/* Do the block being inserted, and the block it is being inserted before
	 make a contiguous block of memory? */
	puc = (uint8_t *) pxBlockToInsert;
	if ((puc + pxBlockToInsert->xBlockSize)
			== (uint8_t *) pxIterator->pxNextFreeBlock) {
		if (pxIterator->pxNextFreeBlock != pxEnd) {
			/* Form one big block from the two blocks. */
			pxBlockToInsert->xBlockSize +=
					pxIterator->pxNextFreeBlock->xBlockSize;
			pxBlockToInsert->pxNextFreeBlock =
					pxIterator->pxNextFreeBlock->pxNextFreeBlock;
		} else {
			pxBlockToInsert->pxNextFreeBlock = pxEnd;
		}
	} else {
		pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
	}

	/* If the block being inserted plugged a gab, so was merged with the block
	 before and the block after, then it's pxNextFreeBlock pointer will have
	 already been set, and should not be set here as that would make it point
	 to itself. */
	if (pxIterator != pxBlockToInsert) {
		pxIterator->pxNextFreeBlock = pxBlockToInsert;
	} else {
		mtCOVERAGE_TEST_MARKER();
	}
}
/*-----------------------------------------------------------*/
static void vPortDefineHeapRegions(const HeapRegion_t * const pxHeapRegions) {
	BlockLink_t *pxFirstFreeBlockInRegion = NULL, *pxPreviousFreeBlock;
	uint8_t *pucAlignedHeap;
	size_t xTotalRegionSize, xTotalHeapSize = 0;
	BaseType_t xDefinedRegions = 0;
	uint32_t ulAddress;
	const HeapRegion_t *pxHeapRegion;

#if defined(CONFIG_PLATFORM_8195A)
	/*
	 xHeapRegions[0].pucStartAddress = (uint8_t*)&__ram_heap1_start__;
	 xHeapRegions[0].xSizeInBytes = (u32)&__ram_heap1_end__ - (u32)xHeapRegions[0].pucStartAddress;
	 xHeapRegions[1].pucStartAddress = &ucHeap; // (uint8_t*)&__ram_heap2_start__;
	 xHeapRegions[1].xSizeInBytes = (u32)0x10070000 - (u32)xHeapRegions[1].pucStartAddress;
	 xHeapRegions[2].pucStartAddress = (uint8_t*)&__sdram_data_start__;
	 xHeapRegions[2].xSizeInBytes = (u32)0x30200000 - (u32)xHeapRegions[2].pucStartAddress;
	 */
#endif
	/* Can only call once! */
	configASSERT( pxEnd == NULL );

	pxHeapRegion = &(pxHeapRegions[xDefinedRegions]);

	uint8 chip_id = HalGetChipId();
	while (pxHeapRegion->xSizeInBytes > 0) {
		if (pxHeapRegion->pucStartAddress
				> 0x20000000 && chip_id >= CHIP_ID_8711AN && chip_id <= CHIP_ID_8711AF) {
//				pxHeapRegion->pucStartAddress = 0;
//				pxHeapRegion->xSizeInBytes = 0;
//				DBG_8195A("ChipID: %p !\n", chip_id);
		} else {
#if CONFIG_DEBUG_LOG > 2
			DBG_8195A("Init Heap Region: %p[%d]\n", pxHeapRegion->pucStartAddress, pxHeapRegion->xSizeInBytes);
#endif
#if CONFIG_DEBUG_LOG > 4
			rtl_memset(pxHeapRegion->pucStartAddress, 0, pxHeapRegion->xSizeInBytes);
#endif
			xTotalRegionSize = pxHeapRegion->xSizeInBytes;
			/* Ensure the heap region starts on a correctly aligned boundary. */
			ulAddress = (uint32_t) pxHeapRegion->pucStartAddress;
			if ((ulAddress & portBYTE_ALIGNMENT_MASK) != 0) {
				ulAddress += ( portBYTE_ALIGNMENT - 1);
				ulAddress &= ~portBYTE_ALIGNMENT_MASK;

				/* Adjust the size for the bytes lost to alignment. */
				xTotalRegionSize -= ulAddress
						- (uint32_t) pxHeapRegion->pucStartAddress;
			}

			pucAlignedHeap = (uint8_t *) ulAddress;

			/* Set xStart if it has not already been set. */
			if (xDefinedRegions == 0) {
				/* xStart is used to hold a pointer to the first item in the list of
				 free blocks.  The void cast is used to prevent compiler warnings. */
				xStart.pxNextFreeBlock = (BlockLink_t *) pucAlignedHeap;
				xStart.xBlockSize = (size_t) 0;
			} else {
				/* Should only get here if one region has already been added to the
				 heap. */
				configASSERT( pxEnd != NULL );

				/* Check blocks are passed in with increasing start addresses. */
				configASSERT( ulAddress > ( uint32_t ) pxEnd );
			}

			/* Remember the location of the end marker in the previous region, if
			 any. */
			pxPreviousFreeBlock = pxEnd;

			/* pxEnd is used to mark the end of the list of free blocks and is
			 inserted at the end of the region space. */
			ulAddress = ((uint32_t) pucAlignedHeap) + xTotalRegionSize;
			ulAddress -= uxHeapStructSize;
			ulAddress &= ~portBYTE_ALIGNMENT_MASK;
			pxEnd = (BlockLink_t *) ulAddress;
			pxEnd->xBlockSize = 0;
			pxEnd->pxNextFreeBlock = NULL;

			/* To start with there is a single free block in this region that is
			 sized to take up the entire heap region minus the space taken by the
			 free block structure. */
			pxFirstFreeBlockInRegion = (BlockLink_t *) pucAlignedHeap;
			pxFirstFreeBlockInRegion->xBlockSize = ulAddress
					- (uint32_t) pxFirstFreeBlockInRegion;
			pxFirstFreeBlockInRegion->pxNextFreeBlock = pxEnd;

			/* If this is not the first region that makes up the entire heap space
			 then link the previous region to this region. */
			if (pxPreviousFreeBlock != NULL) {
				pxPreviousFreeBlock->pxNextFreeBlock = pxFirstFreeBlockInRegion;
			}

			xTotalHeapSize += pxFirstFreeBlockInRegion->xBlockSize;
		}
		/* Move onto the next HeapRegion_t structure. */
		xDefinedRegions++;
		pxHeapRegion = &(pxHeapRegions[xDefinedRegions]);
	}

	xMinimumEverFreeBytesRemaining = xTotalHeapSize;
	xFreeBytesRemaining = xTotalHeapSize;

	/* Check something was actually defined before it is accessed. */
	configASSERT( xTotalHeapSize );

}

void* pvPortReAlloc(void *pv, size_t xWantedSize) {
	BlockLink_t *pxLink;

	if (((uint32_t) pv >= ext_lower) && ((uint32_t) pv < ext_upper)) {
		if (ext_free)
			ext_free(pv);
		pv = NULL;
	}

	unsigned char *puc = (unsigned char *) pv;

	if (pv) {
		if (!xWantedSize) {
			vPortFree(pv);
			return NULL;
		}

		void *newArea = pvPortMalloc(xWantedSize);
		if (newArea) {
			/* The memory being freed will have an xBlockLink structure immediately
			 before it. */
			puc -= uxHeapStructSize;

			/* This casting is to keep the compiler from issuing warnings. */
			pxLink = (void *) puc;

			int oldSize = (pxLink->xBlockSize & ~xBlockAllocatedBit) - uxHeapStructSize;
			int copySize = (oldSize < xWantedSize) ? oldSize : xWantedSize;
			rtl_memcpy(newArea, pv, copySize);

			vTaskSuspendAll();
			{
				/* Add this block to the list of free blocks. */
				pxLink->xBlockSize &= ~xBlockAllocatedBit;
				xFreeBytesRemaining += pxLink->xBlockSize;
				prvInsertBlockIntoFreeList(((BlockLink_t *) pxLink));
			}
			xTaskResumeAll();
			return newArea;
		}
	} else if (xWantedSize)
		return pvPortMalloc(xWantedSize);
	else
		return NULL;

	return NULL;
}

extern _LONG_CALL_ROM_ void *_memset(void *s, int c, SIZE_T n);

void *pvPortZalloc(size_t xWantedSize) {
	void * prt = pvPortMalloc(xWantedSize);
	if (prt)
		_memset(prt, 0, xWantedSize);
	return prt;
}

/*
 #ifdef  ARDUINO_SDK
 int vPortAddHeapRegion(uint8_t *addr, size_t size)
 {
 return 0;
 }
 #endif
 */
