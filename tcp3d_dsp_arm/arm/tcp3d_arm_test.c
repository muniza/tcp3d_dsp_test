/*
 * Copyright (c) 2012-2013, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== tcp3d_arm_test.c ========
 *
 *  Creates buffers using CMEM, DSP runs the TCP3d, and ARM checks the data
 */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* IPC Headers */
#include <ti/ipc/Std.h>
#include <ti/ipc/Ipc.h>
#include <ti/ipc/MessageQ.h>

/* Socket Includes */
#include "sockutils.h"
#include "sockrmmsg.h"

/* RM includes */
#include <ti/drv/rm/rm_transport.h>

/* CMEM includes */
#include <ti/cmem.h>

/* TCP3d Test Includes */
#include "tcp3d_data.h"

/* App defines:  Must match on remote proc side: */
#define HEAPID               0u
#define CLIENT_MESSAGEQ_NAME "RM_CLIENT"
#define SERVER_MESSAGEQ_NAME "RM_SERVER"

#define PROC_ID_DEFAULT      1     /* Host is zero, remote cores start at 1 */

// TCP3d Buffers to pass to DSP
extern const int block0_hard_dec[];
extern const int block0_llrs[];

void tcp3d_check(void *ptr1) {
	/* compare with the block0_hard_dec[]
	* something is wrong with the first 3 values. Seems okay on the DSP though so something
	* happened in the copy?
	*/

	int i;
	for (i=0; i < sizeof(block0_hard_dec)/4; ++i) {
		if ( *(int *)(ptr1+4*i) != block0_hard_dec[i] )
		{
			printf(" Block 0:%d HD mismatch %d != %d\n", i, *(int *)(ptr1+4*i), block0_hard_dec[i]);//idx);
		}
	}
	printf(" Ptr1:0 %#lx \n", *(int *)ptr1);
	printf(" Ptr1:1 %#lx \n", *(int *)(ptr1+4));
	printf(" Ptr1:2 %#lx \n", *(int *)(ptr1+8));
	printf(" Done checks %d\n", i);
}

void cmem_start() {
	int poolid;
	int size1;
	int size2;
	bool size3;
	bool size4;
	void * volatile ptr1;
	void * volatile ptr2;
	void * volatile ptr3;
	void * volatile ptr4;
	off_t * phys_ptr1;
	off_t * phys_ptr2;
	off_t * phys_ptr3;
	off_t * phys_ptr4;
	CMEM_AllocParams  params;

    /* Setup CMEM */
	params.type = CMEM_HEAP;//CMEM_POOL;
	params.alignment = 32;
	params.flags = CMEM_NONCACHED;

	if(CMEM_init() == 0) {
		printf("\n CMEM_init() PASS \n");
	} else {
		printf("\n CMEM_init() FAIL \n");
	}

	size1 = sizeof(block0_hard_dec);
	size2 = sizeof(block0_llrs);
	size3 = sizeof(bool);
	size4 = sizeof(bool);
	printf("\n Hard Decision Size: %d", size1);
	printf("\n LLRs Size: %d", size2);
	printf("\n Pin Check Size: %d",size3);

	ptr1 = CMEM_alloc(size1, &params);
	ptr2 = CMEM_alloc(size2, &params);
	ptr3 = CMEM_alloc(size3, &params);
	ptr4 = CMEM_alloc(size4, &params);
	printf("\n Virtual Ptr1: %#lx", ptr1);
	printf("\n Virtual Ptr2: %#lx", ptr2);
	printf("\n Virtual Ptr3: %#lx", ptr3);
	printf("\n Virtual Ptr3: %#lx", ptr4);

	/*
	 *  We'll need to change this for when the GNU Radio comes
	 *  but test to ensure transport works
	 */
	memcpy(ptr2, &block0_llrs, size2);
	*(volatile int *)ptr3 = 1;

	printf("\n Value at VP1 is: %#lx", *(int *)ptr1);
	printf("\n Value at VP2 is: %d", *(int *)ptr2);
	printf("\n Value at VP3 is: %d", *(volatile int *)ptr3);
	printf("\n Value at VP4 is: %d", *(volatile int *)ptr4);
	phys_ptr1 = CMEM_getPhys(ptr1);
	phys_ptr2 = CMEM_getPhys(ptr2);
	phys_ptr3 = CMEM_getPhys(ptr3);
	phys_ptr4 = CMEM_getPhys(ptr4);
	printf("\n Phys Ptr1 is: %#lx", phys_ptr1);
	printf("\n Phys Ptr2 is: %#lx", phys_ptr2);
	printf("\n Phys Ptr3 is: %#lx", phys_ptr3);
	printf("\n Phys Ptr4 is: %#lx", phys_ptr4);

	/*
	 * Busy-wait set for DSP when done test. DSP ack's but ARM Caches the data
	 * therefore doesn't detect the change. How to non-cache?
	 */
	int i = 0;
	int j = 2;
	while(*(volatile int *)ptr3 == 1) {
		for (i=0; i<j; i++);
	}
	printf("\nYay, done waiting!\n");
	tcp3d_check(ptr1);
}
int main (int argc, char ** argv)
{
    Int32 status = 0;
    UInt16 procId = PROC_ID_DEFAULT;

    printf("\n -----------------------\n ");
    printf(" ---- HERE WE ARE ---- \n");
    /* Parse Args: */
    switch (argc) {
        case 1:
           /* use defaults */
           break;
        case 2:
           procId = atoi(argv[2]);
           break;
        default:
           printf("Usage: %s [<ProcId>]\n", argv[0]);
           printf("\tDefaults: ProcId: %d\n", PROC_ID_DEFAULT);
           exit(0);
    }

    cmem_start();

    return(0);   
}
