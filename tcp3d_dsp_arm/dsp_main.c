/*
 *   dsp_client.c
 *
 *   DSP portion of Resource Manager ARM+DSP test that uses RPMSG and sockets to
 *   allow a DSP application to to request RM services from a RM Server running
 *   from Linux User-space.
 *
 *  ============================================================================
 *
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
 *
 */

/* Standard includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* XDC includes */
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/IHeap.h>

/* BIOS includes */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/family/c64p/Hwi.h>
#include <ti/sysbios/family/c64p/EventCombiner.h>
#include <ti/sysbios/family/c66/tci66xx/CpIntc.h>

/* IPC includes */
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/HeapBufMP.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/GateMP.h>
#include <ti/ipc/Ipc.h>
#include <ti/ipc/ListMP.h>
#include <ti/ipc/SharedRegion.h>

/* CSL includes */
#include <ti/csl/csl_chip.h>
#include <ti/csl/csl_psc.h>
#include <ti/csl/csl_pscAux.h>
#include <ti/csl/csl_chipAux.h>
#include <ti/csl/soc.h>
#include <ti/csl/cslr_tpcc.h>
#include <ti/csl/cslr_tcp3d_cfg.h>
#include <ti/csl/cslr_tcp3d_dma.h>
#include <ti/csl/cslr_tcp3d_dma_offsets.h>
#include <ti/csl/csl_tsc.h>
#include <ti/csl/csl_cacheAux.h>
#include <ti/csl/csl_xmcAux.h>

/* RM Includes */
#include <ti/drv/rm/rm.h>
#include <ti/drv/rm/rm_transport.h>
#include <ti/drv/rm/rm_services.h>

/* TCP3D Includes */
#include "sample.h"
#include "tcp3d_drv_sample.h"
#include "tcp3d_main.h"
#include "tcp3d_profile.h"

/**********************************************************************
 ************************** RM Test Symbols ***************************
 **********************************************************************/

/* Test will run on this core */
#define TEST_CORE                    0

/* Test FALSE */
#define RM_TEST_FALSE                0
/* Test TRUE */
#define RM_TEST_TRUE                 1

/* RM packet heap name */
#define MSGQ_HEAP_ID                 0

/* MessageQ Name for DSP RM Client */
#define CLIENT_MESSAGEQ_NAME         "RM_CLIENT"

/* Size of RM static allocation response queue.  Must be greater than number of APPROVED
 * static allocations */
#define MAX_STATIC_ALLOCATION_RESPS  5

/* Size of RM service response queue */
#define MAX_QUEUED_SERVICE_RESPONSES 10

/* Error checking macro */
#define ERROR_CHECK(checkVal, resultVal, rmInstName, printMsg)                   \
    if (resultVal != checkVal) {                                                 \
        char errorMsgToPrint[] = printMsg;                                       \
        System_printf("Error Core %d : %s : ", coreNum, rmInstName);             \
        System_printf("%s with error code : %d\n", errorMsgToPrint, resultVal);  \
        testErrors++;                                                            \
        System_abort("Test Failure\n");                                          \
    }

#define POSITIVE_PASS_CHECK(title, core, instName, resName, resStart, resLen, align, state, check)  \
    do {                                                                                            \
        int32_t start = resStart;                                                                   \
        int32_t alignment = align;                                                                  \
        char    titleMsg[] = title;                                                                 \
        System_printf ("Core %d : ---------------------------------------------------------\n",     \
                       core);                                                                       \
        System_printf ("Core %d : %s\n", core, titleMsg);                                           \
        System_printf ("Core %d : - Instance Name: %-32s       -\n", core, instName);               \
        System_printf ("Core %d : - Resource Name: %-32s       -\n", core, resName);                \
        if (start == RM_RESOURCE_BASE_UNSPECIFIED) {                                                \
            System_printf ("Core %d : - Start:         UNSPECIFIED                            -\n", \
                           core);                                                                   \
            System_printf ("Core %d : - Length:        %-16d                       -\n",            \
                           core, resLen);                                                           \
        }                                                                                           \
        else {                                                                                      \
            System_printf ("Core %d : - Start:         %-16d                       -\n",            \
                           core, resStart);                                                         \
            System_printf ("Core %d : - End:           %-16d                       -\n", core,      \
                           (start + resLen - 1));                                                   \
        }                                                                                           \
        if (alignment == RM_RESOURCE_ALIGNMENT_UNSPECIFIED) {                                       \
            System_printf ("Core %d : - Alignment:     UNSPECIFIED                            -\n", \
                           core);                                                                   \
        }                                                                                           \
        else {                                                                                      \
            System_printf ("Core %d : - Alignment:     %-16d                       -\n",            \
                           core, alignment);                                                        \
        }                                                                                           \
        System_printf ("Core %d : -                                                       -\n",     \
                       core);                                                                       \
        if (state == check) {                                                                       \
            System_printf ("Core %d : - PASSED                                                -\n", \
                           core);                                                                   \
        }                                                                                           \
        else {                                                                                      \
            System_printf ("Core %d : - FAILED - Denial: %-6d                               -\n",   \
                           core, state);                                                            \
            testErrors++;                                                                           \
        }                                                                                           \
        System_printf ("Core %d : ---------------------------------------------------------\n",     \
                       core);                                                                       \
        System_printf ("\n");                                                                       \
    } while(0);

#define NEGATIVE_PASS_CHECK(title, core, instName, resName, resStart, resLen, align, state, check)  \
    do {                                                                                            \
        int32_t start = resStart;                                                                   \
        int32_t alignment = align;                                                                  \
        char    titleMsg[] = title;                                                                 \
        System_printf ("Core %d : ---------------------------------------------------------\n",     \
                       core);                                                                       \
        System_printf ("Core %d : %s\n", core, titleMsg);                                           \
        System_printf ("Core %d : - Instance Name: %-32s       -\n", core, instName);               \
        System_printf ("Core %d : - Resource Name: %-32s       -\n", core, resName);                \
        if (start == RM_RESOURCE_BASE_UNSPECIFIED) {                                                \
            System_printf ("Core %d : - Start:         UNSPECIFIED                            -\n", \
                           core);                                                                   \
            System_printf ("Core %d : - Length:        %-16d                       -\n",            \
                           core, resLen);                                                           \
        }                                                                                           \
        else {                                                                                      \
            System_printf ("Core %d : - Start:         %-16d                       -\n",            \
                           core, resStart);                                                         \
            System_printf ("Core %d : - End:           %-16d                       -\n", core,      \
                           (start + resLen - 1));                                                   \
        }                                                                                           \
        if (alignment == RM_RESOURCE_ALIGNMENT_UNSPECIFIED) {                                       \
            System_printf ("Core %d : - Alignment:     UNSPECIFIED                            -\n", \
                           core);                                                                   \
        }                                                                                           \
        else {                                                                                      \
            System_printf ("Core %d : - Alignment:     %-16d                       -\n",            \
                           core, alignment);                                                        \
        }                                                                                           \
        System_printf ("Core %d : -                                                       -\n",     \
                       core);                                                                       \
        if (state != check) {                                                                       \
            System_printf ("Core %d : - PASSED - Denial: %-6d                               -\n",   \
                           core, state);                                                            \
        }                                                                                           \
        else {                                                                                      \
            System_printf ("Core %d : - FAILED - Expected Denial                              -\n", \
                           core);                                                                   \
            testErrors++;                                                                           \
        }                                                                                           \
        System_printf ("Core %d : ---------------------------------------------------------\n",     \
                       core);                                                                       \
        System_printf ("\n");                                                                       \
    } while(0);

#define STATUS_PASS_CHECK(title, core, instName, resName, resStart, resLen, refCnt, state, check, expectRefCnt) \
    do {                                                                                                        \
        int32_t start = resStart;                                                                               \
        char    titleMsg[] = title;                                                                             \
        System_printf ("Core %d : ---------------------------------------------------------\n",                 \
                       core);                                                                                   \
        System_printf ("Core %d : %s\n", core, titleMsg);                                                       \
        System_printf ("Core %d : - Instance Name: %-32s       -\n", core, instName);                           \
        System_printf ("Core %d : - Resource Name: %-32s       -\n", core, resName);                            \
        System_printf ("Core %d : - Start:         %-16d                       -\n",                            \
                           core, resStart);                                                                     \
        System_printf ("Core %d : - End:           %-16d                       -\n", core,                      \
                           (start + resLen - 1));                                                               \
        System_printf ("Core %d : - Expected Owner Count: %-16d                -\n",                            \
                       core, expectRefCnt);                                                                     \
        System_printf ("Core %d : - Returned Owner Count: %-16d                -\n",                            \
                       core, refCnt);                                                                           \
        System_printf ("Core %d : -                                                       -\n", core);          \
        if ((state == check) && (refCnt == expectRefCnt)) {                                                     \
            System_printf ("Core %d : - PASSED                                                -\n", core);      \
        }                                                                                                       \
        else {                                                                                                  \
            if (refCnt != expectRefCnt) {                                                                       \
                System_printf ("Core %d : - FAILED - Owner Count Mismatch                         -\n",         \
                               core);                                                                           \
            }                                                                                                   \
            else {                                                                                              \
                System_printf ("Core %d : - FAILED - Denial: %-6d                               -\n",           \
                               core, state);                                                                    \
            }                                                                                                   \
            testErrors++;                                                                                       \
        }                                                                                                       \
        System_printf ("Core %d : ---------------------------------------------------------\n",                 \
                       core);                                                                                   \
        System_printf ("\n");                                                                                   \
    } while(0);

/**********************************************************************
 ********************** RM Test Data Structures ***********************
 **********************************************************************/

/* IPC MessageQ RM packet encapsulation structure */
typedef struct {
    /* IPC MessageQ header (must be first element in structure) */
    MessageQ_MsgHeader msgQHeader;
    /* Pointer to RM packet */
    Rm_Packet          rmPkt;
    /* Phys Pointer to Ptr1 */
    void			   *phys_ptr1;
    int				   size1;
    void			   *phys_ptr2;
    int				   size2;
    void			   *phys_ptr3;
    int				   size3;
} MsgQ_RmPacket;

/**********************************************************************
 ********************** Extern Variables ******************************
 **********************************************************************/

/* Alloc and free OSAL variables */
extern uint32_t rmMallocCounter;
extern uint32_t rmFreeCounter;

/* RM test Static Policy provided to RM Client */
extern const char rmStaticPolicy[];

/**********************************************************************
 ********************** Global Variables ******************************
 **********************************************************************/

/* Core number */
uint16_t            coreNum;
/* Number of errors that occurred during the test */
uint16_t            testErrors;

/* Task to configure application transport code for RM */
Task_Handle         rmStartupTskHandle;
/* High priority task for receiving RM packets */
Task_Handle         rmReceiveTskHandle;
/* RM client delegate and client test task */
Task_Handle         rmClientTskHandle;

/* Handle for heap that RM packets will be allocated from */
HeapBufMP_Handle    rmPktHeapHandle = NULL;

/* Client instance name (must match with RM Global Resource List (GRL) and policies */
char                rmClientName[RM_NAME_MAX_CHARS] = "RM_Client";

/* Client MessageQ */
MessageQ_Handle     rmClientQ = NULL;

/* Linux MessageQ ID */
MessageQ_QueueId    linuxQueueId;

/* Client instance handles */
Rm_Handle           rmClientHandle = NULL;

/* Client instance service handles */
Rm_ServiceHandle   *rmClientServiceHandle = NULL;

/* Client from Server transport handle */
Rm_TransportHandle  clientFromServerTransportHandle;

/* Static allocation response queue */
Rm_ServiceRespInfo  staticResponseQueue[MAX_STATIC_ALLOCATION_RESPS];
/* Static allocation response queue index */
uint32_t            numStaticResponses;

/* RM response info queue used to store service responses received via the callback function */
Rm_ServiceRespInfo  responseInfoQueue[MAX_QUEUED_SERVICE_RESPONSES];

/* RM resource names (must match resource node names in GRL and policies */
char                resourceNameMemRegion[RM_NAME_MAX_CHARS]  = "memory-regions";
char                resourceNameAccumCh[RM_NAME_MAX_CHARS]    = "accumulator-ch";
char                resourceNameGpQ[RM_NAME_MAX_CHARS]        = "gp-queue";
char                resourceNameAifQ[RM_NAME_MAX_CHARS]       = "aif-queue";
char                resourceNameQosCluster[RM_NAME_MAX_CHARS] = "qos-cluster";
char                resourceNameAifRxCh[RM_NAME_MAX_CHARS]    = "aif-rx-ch";
char                resourceNameInfraQ[RM_NAME_MAX_CHARS]     = "infra-queue";

/* Test RM NameServer name */
char                nameServerNameFavQ[RM_NAME_MAX_CHARS]     = "My_Favorite_Queue";

/**********************************************************************
 * ************************ TCP3d Variables ***************************
 **********************************************************************/
#define START_CMD_PERIOD             1

volatile PROFILE_TAG    profileTag[PROF_TAG_LEN];
volatile UInt32         profileTagInd = 0;
/* to avoid hardcoding we can have a pattern similar to this
* 0x0C000000 - num_items
* 0x0C001000 - size of first buffer
* 0x0C002000 - size of second buffer
* 0x0C000000+4096*num_items - size of nth buffer
* 0x0C000000+4096*(num_items+1) - first buffer
* 0x0C000000+4096*(num_items+1)+sizeof(first_buffer)
* 0x0C000000+4096*(num_items+1)+sizeof(first_buffer)+sizeof(second_buffer)
* ....
*/
#define PTR1 0x0C000000
volatile uint32_t *arm_ptr1 = (void *)PTR1;
#define PTR2 0x0C001000
volatile uint32_t *arm_ptr2 = (void *)PTR2;
#define PTR3 0x0C012000
volatile uint32_t *arm_ptr3 = (void *)PTR3;
#define PTR4 0x0C013000
volatile uint32_t *arm_ptr4 = (void *)PTR4;

IHeap_Handle            dataHeap = NULL;
IHeap_Handle            drvHeap = NULL;

/**
 * EDMA3 LLD & TCP3D Driver Init/Deinit related variables
 */
UInt32                  testMaxCodeBlocks;
EDMA3_DRV_Handle        hEdma;
UInt32                  tpccNum;
UInt32                  tpccRegionUsed;
UInt32                  dspCoreID;
EDMA_CONFIG             edmaConfig[2];
UInt8                   instNum;
/* Flags used in ISR functions */
UInt32                  pingComplete, pongComplete;
UInt32                  pauseIntr = 0, l2pIntr = 0;
UInt32                  soldoutCntr = 0;
UInt32                  tcp3dEventCntr = 0;
UInt32                  tpccEvtCntr = 0;
UInt32                  rcvStartFlag = 0;
UInt32                  pauseIntFlag = 0;
UInt32                  afterIntrSoldout = 0, afterIntrPause = 0;
UInt32                  pendPauseCntr = 0;
Int32                   sendBlockCnt;
Int32                   rcvBlockCnt;
UInt32                  testCntr = 0;
UInt32                  testErrCntr = 0;
UInt32                  totErrCnt;
Char                    folderName[1000] = "";
Char                    *strMode[4] = {"3GPP(0)","LTE(1)","WIMAX(2)","WCDMA Split(3)"};
Char                    *strDBuf[2] = {"Disable(0)","Enable(1)"};

/* Driver configuration variables */
Tcp3d_Result            tcp3dResultSend = TCP3D_DRV_NO_ERR;
Tcp3d_Instance          *tcp3dDrvInst[2] = {NULL, NULL};
Tcp3d_Instance          *inst;
Tcp3d_Ctrl              drvCtrl;
Tcp3d_Sts               drvStatus;

/* Throughput calculation variables */
clock_t                 total_clock_end, total_clock_start;
UInt32                  test_cycles;
UInt32                  TotalBitsDecoded;
UInt32                   ThroughPut;

Semaphore_Handle        semRcvDone, semSendBlock, semSendWait, semRcvStart;
#pragma DATA_SECTION(codeBlockSet, ".main_mem")
cbTestDesc              codeBlockSet;

/**********************************************************************
 *************************** Test Functions ***************************
 **********************************************************************/

void tcp3dBeginMemAccess (void *ptr, uint32_t size)
{
    /* Invalidate L1D cache and wait until operation is complete.
     * Use this approach if L2 cache is not enabled */
    CACHE_invL1d (ptr, size, CACHE_FENCE_WAIT);

    /*  Cleanup the prefectch buffer also. */
    CSL_XMC_invalidatePrefetchBuffer();

    return;
}

void tcp3dEndMemAccess (void *ptr, uint32_t size)
{
    /* Writeback L1D cache and wait until operation is complete.
     * Use this approach if L2 cache is not enabled */
    CACHE_wbL1d (ptr, size, CACHE_FENCE_WAIT);

    return;
}

Void revt0ChCallback(Void)
{
    /* Increment the ISR counter */
    pauseIntr++;

    pauseIntFlag = 1;

    LOG_TIME_ISR(PROF_RESTART_PING, PROF_INST, TSCL);

    if ( sendBlockCnt >= codeBlockSet.maxNumCB )
        Semaphore_post(semSendWait);
    else
        Semaphore_post(semSendBlock);
}

Void revt1ChCallback(Void)
{
    /* Increment the ISR counter */
    pauseIntr++;

    pauseIntFlag = 2;

    LOG_TIME_ISR(PROF_RESTART_PONG, PROF_INST, TSCL);

    if ( sendBlockCnt >= codeBlockSet.maxNumCB )
        Semaphore_post(semSendWait);
    else
        Semaphore_post(semSendBlock);
}

Void tcp3dEventISR(UInt32 testEvtNum)
{
    tcp3dEventCntr++;
    tpccEvtCntr++;

    if ( sendBlockCnt >= codeBlockSet.maxNumCB )
        Semaphore_post(semSendWait);
    else
        Semaphore_post(semSendBlock);
}

Void registerTcp3dEvent(Void)
{
    static  UInt32 cookie = 0;
    Int     eventId = 0;    /* GEM event id */
    static  UInt32 mapDone = 0;
    UInt32  testEvt = getNotifyEventNum(instNum);
    UInt32  hostIntr = getHostIntrNum(dspCoreID);

    /* Disabling the global interrupts */
    cookie = Hwi_disable();

    /* Completion ISR Registration */
    CpIntc_dispatchPlug(testEvt, tcp3dEventISR, hostIntr, TRUE);
    if (!mapDone)
        CpIntc_mapSysIntToHostInt(0, testEvt, hostIntr);
    CpIntc_enableHostInt(0, hostIntr);
    eventId = CpIntc_getEventId(hostIntr);
    EventCombiner_dispatchPlug (eventId,
    		CpIntc_dispatch,hostIntr,TRUE);

    System_printf("\t\t testEvt : %d \n", testEvt);
    System_printf("\t\t hostIntr : %d \n", hostIntr);
    System_printf("\t\t eventId : %d \n", eventId);

    /* enable the 'global' switch */
    CpIntc_enableAllHostInts(0);

    mapDone = 1;

    /* Restore interrupts */
    Hwi_restore(cookie);
}

Void unregisterTcp3dEvent(Void)
{
    static UInt32 cookie = 0;
    Int eventId = 0;    /* GEM event id */
    UInt32  hostIntr = getHostIntrNum(dspCoreID);

    /* Disabling the global interrupts */
    cookie = Hwi_disable();

    /* Driver Completion ISR */
    CpIntc_disableHostInt(0, hostIntr);
    eventId = CpIntc_getEventId(hostIntr);
    EventCombiner_disableEvent(eventId);

    /* Restore interrupts */
    Hwi_restore(cookie);
}

Void soldOutAction(Void)
{
    /* clear flag */
    pauseIntFlag = 0;
    /* keep trying until successful */
    Semaphore_post(semSendBlock);
}

Void soldOutActionClear(Void)
{
	//nothing to do
}

Void allDeInit(Void)
{
    EDMA3_DRV_Result    edmaResult = EDMA3_DRV_SOK;

    /* Un-register the Notification Event for TCP3D */
    unregisterTcp3dEvent();

    /* Close all EDMA channels allocated for the test */
    System_printf("EDMA3 Channels freeing started...\n");

    /* Register call backs */
    EDMA3_DRV_unregisterTccCb(hEdma, edmaConfig[instNum].pingChRes[0].chNo);
    EDMA3_DRV_unregisterTccCb(hEdma, edmaConfig[instNum].pingChRes[1].chNo);
    EDMA3_DRV_unregisterTccCb(hEdma, edmaConfig[instNum].pongChRes[0].chNo);
    EDMA3_DRV_unregisterTccCb(hEdma, edmaConfig[instNum].pongChRes[1].chNo);

    /* Close channels */
    closeEdmaChannels(hEdma, instNum, &edmaConfig[instNum]);

    System_printf("EDMA3 Channels freeing complete\n");

    /* Deinit for TCP3D driver */
    System_printf("TCP3 Decoder Driver De-Initialization sequence started...\n");

    tcp3dSampleDeinit(drvHeap, instNum, tcp3dDrvInst[instNum]);

    System_printf("TCP3 Decoder Driver De-Initialization sequence complete\n");

    /* De-init EDMA3 */
    edmaResult = edma3deinit(tpccNum, hEdma);
    if (edmaResult != EDMA3_DRV_SOK)
    {
        System_printf("edma3deinit() FAILED, error code: %d\n", edmaResult);
    }
    else
    {
        System_printf("EDMA3 LLD De-Initialization complete\n");
    }
}

Void getMemoryStats(Void)
{
    Memory_Stats        memStats;

    Memory_getStats(drvHeap, &memStats);
    System_printf("\nHeap Usage/Staus\n");
    System_printf("    tcp3dDrvHeap : %d of %d free\n", memStats.totalFreeSize, memStats.totalSize);

    Memory_getStats(dataHeap, &memStats);
    System_printf("    tcp3dDataHeap : %d of %d free\n", memStats.totalFreeSize, memStats.totalSize);
}

Rm_Packet *transportAlloc(Rm_AppTransportHandle appTransport, uint32_t pktSize, Rm_PacketHandle *pktHandle)
{
    Rm_Packet     *rmPkt = NULL;
    MsgQ_RmPacket *rmMsg = NULL;

    /* Allocate a messageQ message for containing the RM packet */
    rmMsg = (MsgQ_RmPacket *)MessageQ_alloc(MSGQ_HEAP_ID, sizeof(MsgQ_RmPacket));
    if (rmMsg == NULL) {
        System_printf("Error Core %d : MessageQ_alloc failed to allocate message: %d\n", coreNum);
        testErrors++;
        *pktHandle = NULL;
        return(NULL);
    }
    else {
        rmPkt = &(rmMsg->rmPkt);
        rmPkt->pktLenBytes = pktSize;
        *pktHandle = (Rm_PacketHandle)rmMsg;
    }
    return (rmPkt);
}

void transportFree (MessageQ_Msg rmMsgQMsg)
{
    int32_t  status;

    status = MessageQ_free(rmMsgQMsg);
    if (status < 0) {
        System_printf("Error Core %d : MessageQ_free failed to free message: %d\n", coreNum, status);
        testErrors++;
    }
}

int32_t transportSend (Rm_AppTransportHandle appTransport, Rm_PacketHandle pktHandle)
{
    MessageQ_QueueId *remoteQueueId = (MessageQ_QueueId *)appTransport;
    MsgQ_RmPacket    *rmMsg = pktHandle;
    int32_t           status;

    /* Send the message to Linux */
    status = MessageQ_put(*remoteQueueId, (MessageQ_Msg)rmMsg);
    if (status < 0) {
        System_printf("Error Core %d : MessageQ_put failed to send message: %d\n", coreNum, status);
        testErrors++;
    }

    return (0);
}

Void rcvBlockTaskFunc(Void) {
    Int32           errCnt;

    cbDataDesc       *cbPtr;
    Int             idx, loopCnt;
    Int             fail = 0;
    UInt8           *ptr1, *ptr2;

    rcvBlockCnt = 0;
    totErrCnt = 0;

    while(1)
    {
        Semaphore_pend(semRcvStart, BIOS_WAIT_FOREVER);

        /* prints for send task are done here */
        if ( tcp3dResultSend == TCP3D_DRV_NO_ERR )
        {
            for ( loopCnt = 0; loopCnt < sendBlockCnt; loopCnt++ )
            {
                cbPtr   = codeBlockSet.cbData[loopCnt];
                System_printf("Send Task: Enqueued Block %d (Size: %d, SW0: %d)\n",
                                    loopCnt, cbPtr->blockSize,
                                    cbPtr->sw0LengthUsed);
            }
            System_printf("Send Task: Enqueued %d Blocks\n\n", sendBlockCnt);
        }
        else
        {
            System_printf("Send Task: Enqueued Blocks failed (tcp3dResultSend : %d)\n\n", tcp3dResultSend);
            System_exit(0);
        }

        while( rcvBlockCnt < codeBlockSet.maxNumCB )
        {
            /* Get the pointer to the Code Block Set */
            cbPtr   = codeBlockSet.cbData[rcvBlockCnt];

            /* Step 2: Verify All the outputs */
            fail = 0;
            /* Step 2.1: Hard Decisions Verification */
            ptr1 = (UInt8 *) cbPtr->refHD;
            ptr2 = (UInt8 *) cbPtr->outHD;
            arm_ptr1 = (uint32_t *) cbPtr->outHD;
	    	/* Invalidate out HD */
            CACHE_invL1d (cbPtr->outHD, cbPtr->blockSize>>3, CACHE_WAIT);

            errCnt = 0;
            for (idx = 0; idx < (cbPtr->blockSize>>3); ++idx)
            {
                if ( ptr1[idx] != ptr2[idx] )
                {
                    errCnt++;
                    System_printf("Block %d:%d HD mismatch %d != %d\n", rcvBlockCnt, idx, ptr1[idx], ptr2[idx]);//idx);
                }
            }
            System_printf(" Checked %d Values", cbPtr->blockSize>>3);
            if (errCnt)
            {
                fail++;
            }

            /* Step 2.2: Soft Decisions Verification */
            if (cbPtr->sdFlag)
            {
                if ( codeBlockSet.mode == TEST_MODE_SPLIT ) /* SPLIT MODE */
                    loopCnt = cbPtr->blockSize;
                else
                    loopCnt = (3*cbPtr->blockSize);

                /* Invalidate out SD */
                CACHE_invL1d (cbPtr->outSD, loopCnt, CACHE_WAIT);

                /* NOTE: Assumed that the Soft Decisions are in a single array */
                errCnt = 0;
                for (idx = 0; idx < loopCnt; ++idx)
                {
                    if ( cbPtr->refSD[idx] != cbPtr->outSD[idx] )
                    {
                        errCnt += 1;
                        System_printf("\tBlock Count %d, SD mismatch byte %d\n", rcvBlockCnt, idx);
                    }
                }

                if (errCnt)
                {
                    fail++;
                }
            } /* if (cbPtr->sdFlag) */

            /* Step 2.3: errCnt Registers Verification */
            if (cbPtr->stsFlag)
            {
                /* Invalidate out Sts */
                CACHE_invL1d (cbPtr->outSts, 12, CACHE_WAIT);

                errCnt = 0;
                for (idx = 0; idx < 3; ++idx)
                {
                    if ( cbPtr->refSts[idx] != cbPtr->outSts[idx] )
                    {
                        errCnt += 1;
                        System_printf("\tBlock Count %d, STS mismatch word %d\n", rcvBlockCnt, idx);
                    }
                }
                if (errCnt)
                {
                    fail++;
                }
            } /* if (cbPtr->stsFlag) */
            if (fail)
            {
                System_printf("Rcv task: Block %d FAILED\n", rcvBlockCnt);
                totErrCnt++;
            }
            else
            {
                System_printf("Rcv task: Block %d PASSED\n", rcvBlockCnt);
            }
            rcvBlockCnt++;
        }
        if(rcvBlockCnt >= codeBlockSet.maxNumCB)
        {
            break;
        }
    }

    System_printf("Rcv Task: COMPLETE - verified %d blocks\n", rcvBlockCnt);

    /* Prepare for next test, set by "tester task" */
    Semaphore_post(semRcvDone);
}

Void sndBlockTaskFunc(Void) {
    UInt8               notifyFlag;
    cbDataDesc          *cbPtr;
    static UInt32       cookie = 0;

    sendBlockCnt = 0;

    total_clock_start = TSCL;

    while(1)
    {
        /* Pending on Semaphore to run the loop */
        Semaphore_pend(semSendBlock, BIOS_WAIT_FOREVER);

        /* set TCP3D instance to use */
        inst = tcp3dDrvInst[instNum];

        /* Get pointer to the code block data structure */
        cbPtr = codeBlockSet.cbData[sendBlockCnt];

        /* Interrupt flag, used in Tcp3d_enqueueCodeBlock function */
        notifyFlag = 0;
        if ( sendBlockCnt >= (codeBlockSet.maxNumCB-2) )
        {
            notifyFlag = 1; /* Set for the last CB in each path (PING & PONG) */
        }

        /**
         * Prepare input configuration (IC) registers.
         */
        if ( TCP3D_DRV_INPUT_LIST_FULL == tcp3dResultSend )
        {
            /* IC prepare not required. Just clear soldout actions. */
            soldOutActionClear();
        }
        else
        {
        	/* Prepare only beta state registers */
        	prepareBetaStateICParams(cbPtr, cbPtr->mode);
        }
        checkBetaValues (cbPtr->inCfg);

        /* Disabling the global interrupts */
        cookie = Hwi_disable();

        tcp3dEndMemAccess(cbPtr->inCfg, cbPtr->sizeCFG);
        tcp3dEndMemAccess(cbPtr->inLLR, cbPtr->sizeLLR);

        tcp3dBeginMemAccess(cbPtr->outHD, cbPtr->sizeHD);
        if (cbPtr->sdFlag)
            tcp3dBeginMemAccess(cbPtr->outSD, cbPtr->sizeSD);
        if (cbPtr->stsFlag)
            tcp3dBeginMemAccess(cbPtr->outSts, cbPtr->sizeSTS);

        /* Restore interrupts */
        Hwi_restore(cookie);

        /**
         * WORKAROUND CODE:
         * This code works in line with the code in the second while loop
         * in the send task where check for completion is done.
         * Here we are setting the last byte in the outHD with some value when
         * the refHD has 0x00. This avoids any false completion of send task.
         */
        if ( sendBlockCnt >= (codeBlockSet.maxNumCB-2) )
        {
            /* Fill the last byte in outHD when refHD last byte is ZERO */
            uint8_t     *bytePtr1, *bytePtr2;
            uint32_t    byteSize;

            bytePtr1 = (UInt8 *) cbPtr->refHD;
            bytePtr2 = (UInt8 *) cbPtr->outHD;
            byteSize = (cbPtr->blockSize>>3);

            if ( bytePtr1[byteSize-1] == 0 )
            {
                bytePtr2[byteSize-1] = 0xde;
            }
        }


        /* Enqueue the Code block */
        tcp3dResultSend = Tcp3d_enqueueCodeBlock ( inst,
                                                    cbPtr->blockSize,
                                                    (UInt32 *)L2GLBMAP(dspCoreID, cbPtr->inCfg),
                                                    (Int8 *)L2GLBMAP(dspCoreID, cbPtr->inLLR),
                                                    cbPtr->llrOffset,
                                                    (UInt32 *)L2GLBMAP(dspCoreID, cbPtr->outHD),
                                                    (Int8 *)L2GLBMAP(dspCoreID, cbPtr->outSD),
                                                    cbPtr->sdOffset,
                                                    (UInt32 *)L2GLBMAP(dspCoreID, cbPtr->outSts),
                                                    notifyFlag); // 1 - GEN EVENT, 0 - NO EVENT


        /* Check for soldout case */
        if ( TCP3D_DRV_INPUT_LIST_FULL != tcp3dResultSend )
        {
            /* increment the block count */
            sendBlockCnt++;

            /* goto next block */
            Semaphore_post(semSendBlock);
        }
        else
        {
            /* increment soldout count */
            soldoutCntr++;

            soldOutAction(); /* take action */
        }

        /* Start the driver after START_CMD_PERIOD blocks */
        if ( sendBlockCnt == START_CMD_PERIOD )
        {
            if ( TCP3D_DRV_NO_ERR != Tcp3d_start(inst, TCP3D_DRV_START_AUTO) )
            {
                System_printf("Tcp3d_start function returned error (AUTO)\n");
                System_exit(0);
            }
        }

        /* Check for end of task and exit */
        if ( sendBlockCnt >= codeBlockSet.maxNumCB )
        {
            /* set flags first */
            pauseIntFlag = 0;
            l2pIntr = pauseIntr;
            pendPauseCntr = 0;
            /* Set interrupt flag PAUSE channel */
            drvCtrl.cmd = TCP3D_DRV_CLR_REVT_INT;
            Tcp3d_control(inst, &drvCtrl);

            /* Check to see if restart needed before exit */
            if ( TCP3D_DRV_NO_ERR != Tcp3d_start(inst, TCP3D_DRV_START_AUTO) )
            {
                System_printf("Tcp3d_start function returned error (AUTO)\n");
                System_exit(0);
            }

            /* Set interrupt flag PAUSE channel */
            drvCtrl.cmd = TCP3D_DRV_SET_REVT_INT;
            drvCtrl.intrFlag = TEST_INTR_ENABLE;   // enable
            Tcp3d_control(inst, &drvCtrl);

            /* out of enqueue loop */
            break;
        }
    } //end of while 1
    /**
     * Check for pending Pauses and waiting for the last block to be decoded
     */
    while ( 1 )
    {
        /* Pending on Semaphore to run the loop */
        Semaphore_pend(semSendWait, BIOS_WAIT_FOREVER);

        /* Received both the completion events, so exit send task */
        if ( tcp3dEventCntr >= 2 )
        {
            break;
        } else if ( tcp3dEventCntr == 1 )
        {
            /* one code block test case */
            if ( codeBlockSet.maxNumCB == 1 )
            {
                break;
            }
            else if ( codeBlockSet.mode == TEST_MODE_SPLIT )
            { /* missing one notificatin event - possible in split mode */
                /**
                * WORKAROUND CODE:
                * This is possibility in case of SPLIT mode, that one event is
                * lost when both ping and pong channels try to generate system
                * events at close proximity.
                * In this test bench we have enabled notification events for
                * the last two blocks, so checking the outHD & refHD last bytes
                * to confirm the decoding of these blocks are completed.
                */

                /* cbPtr for last two code blocks */
                cbDataDesc  *cbPtr1 = codeBlockSet.cbData[codeBlockSet.maxNumCB-2];
                cbDataDesc  *cbPtr2 = codeBlockSet.cbData[codeBlockSet.maxNumCB-1];
                uint8_t     *bytePtr11, *bytePtr12, *bytePtr21, *bytePtr22;
                uint32_t    size1, size2;

                bytePtr11 = (UInt8 *) cbPtr1->refHD;
                bytePtr12 = (UInt8 *) cbPtr1->outHD;
                bytePtr21 = (UInt8 *) cbPtr2->refHD;
                bytePtr22 = (UInt8 *) cbPtr2->outHD;
                size1 = (cbPtr1->blockSize>>3); /* in bytes */
                size2 = (cbPtr2->blockSize>>3); /* in bytes */

                /* check if last HD byte of last two blocks are completed */
                if ((bytePtr11[size1-1] == bytePtr12[size1-1]) &&
                    (bytePtr21[size2-1] == bytePtr22[size2-1]) )
                {
                    System_printf("Notification event missed (Race Condition)\n");
                    System_printf("Since the last two block decoding completed, completing send task\n");
                    System_printf("Block : %d\n", codeBlockSet.maxNumCB-2);
                    System_printf("\trefHD[%d] = 0x%x\t outHD[%d] = 0x%x\n", size1-1, bytePtr11[size1-1], size1-1, bytePtr12[size1-1]);
                    System_printf("Block : %d\n", codeBlockSet.maxNumCB-1);
                    System_printf("\trefHD[%d] = 0x%x\t outHD[%d] = 0x%x\n", size2-1, bytePtr21[size2-1], size2-1, bytePtr22[size2-1]);
                    break;
                }
            }
        }

        if ( TCP3D_DRV_NO_ERR != Tcp3d_start(inst, TCP3D_DRV_START_AUTO) )
        {
            System_printf("Tcp3d_start function returned error\n");
            System_exit(0);
        }

        /* keep trying until finding two end events */
        Semaphore_post(semSendWait);

        pendPauseCntr++;
    }

    /* Last code block decoded - Start the receive task */
    total_clock_end = TSCL;
    Semaphore_post(semRcvStart);
}

int tcp3d_init() {
    Tcp3d_Result        tcp3dResult = TCP3D_DRV_NO_ERR;
    EDMA3_DRV_Result    edmaResult = EDMA3_DRV_SOK;

    testMaxCodeBlocks   = 8; /* max possible used in init */

    CSL_Tcp3d_cfgRegs       *tcp3dCfgRegs = (CSL_Tcp3d_cfgRegs *) getTcp3dCfgRegsBase(instNum);

    /* Initialize EDMA3 first */
    hEdma = NULL;
    tpccNum = 2;
    tpccRegionUsed = 3;
    hEdma = edma3init ( tpccNum,
                        &edmaResult,
                        dspCoreID,
                        tpccRegionUsed);
    if (edmaResult != EDMA3_DRV_SOK)
    {
        System_printf("edma3init() FAILED, error code: %d\n", edmaResult);
    }
    else
    {
        System_printf("EDMA3 LLD Initialization complete (TPCC #%d, Region #%d)\n", tpccNum, tpccRegionUsed);
    }
    /* Allocate all EDMA channels required for TCP3D Driver */
    System_printf("EDMA3 Channels opening started...\n");

    /* Open channels for one instance */
    openEdmaChannels (hEdma, instNum, &edmaConfig[instNum]);

    /* Register call backs */
    EDMA3_DRV_registerTccCb(hEdma, edmaConfig[instNum].pingChRes[0].chNo, (EDMA3_RM_TccCallback)&revt0ChCallback, NULL);
    EDMA3_DRV_registerTccCb(hEdma, edmaConfig[instNum].pongChRes[0].chNo, (EDMA3_RM_TccCallback)&revt1ChCallback, NULL);

    /* Fill call back details */
    edmaConfig[instNum].pingChRes[0].cbFunc  = (EDMA3_RM_TccCallback)&revt0ChCallback;
    edmaConfig[instNum].pingChRes[0].cbData  = NULL;
    edmaConfig[instNum].pongChRes[0].cbFunc  = (EDMA3_RM_TccCallback)&revt1ChCallback;
    edmaConfig[instNum].pongChRes[0].cbData  = NULL;

    /**
     * Update the information to use with local EDMA ISR
     * (NOTE: This function must be called after the channels are opened)
     */
    updateAllocatedTccsLoc(&edmaConfig[instNum]);
    System_printf("EDMA3 Channels opening complete\n");
    System_printf("TCP3 Decoder Driver Initialization sequence started...\n");

    /***** Soft Reset the TCP3D for synchronization ******/
    LOG_TIME(PROF_SOFT_RESET, PROF_INST, TSCL);
    tcp3dCfgRegs->TCP3_SOFT_RESET = 1;
    tcp3dCfgRegs->TCP3_SOFT_RESET = 0;

    /* Initialize the TCP3D first */
    tcp3dDrvInst[instNum] = tcp3dSampleInit (drvHeap,
                                    instNum,
                                    testMaxCodeBlocks,
                                    codeBlockSet.mode,
                                    codeBlockSet.doubleBuffer,
                                    codeBlockSet.lteCrcSel,
                                    dspCoreID,
                                    hEdma,
                                    tpccRegionUsed,
                                    &edmaConfig[instNum],
                                    &tcp3dResult);

    System_printf("TCP3 Decoder Driver Initialization sequence complete\n");
    /* Register the Notification Event for TCP3D */
    registerTcp3dEvent();

    /* Set the global flags to default values */
    pingComplete = 0;
    pongComplete = 0;
    pauseIntr = 0;
    l2pIntr = 0;
    tcp3dEventCntr = 0;
    pauseIntFlag = 0;
    rcvStartFlag = 0;
    soldoutCntr = 0;
    afterIntrSoldout = 0;
    afterIntrPause = 0;
    pendPauseCntr = 0;

    return 0;
}

// We need to get this working. Let's spend tmr on it!
// getting all the includes and libraries in the cfg
// we'll remove the llrs and keep the hd for check on here
int tcp3d_test(void) {
    Int                 i;
    Task_Params         taskParams;
    Semaphore_Params    semParams;

    /* Set the one-time global test variables */
    testMaxCodeBlocks   = 86; /* max possible used in init */
    dspCoreID           = CSL_chipReadDNUM();
    instNum = getTcp3dInstNum(dspCoreID);

    /* Initialize the default Task parameters */
    Task_Params_init(&taskParams);

    /* Initialize the default Semaphore parameters */
    Semaphore_Params_init(&semParams);

    /* Crete the Binary Semaphore */
    semParams.mode = Semaphore_Mode_BINARY;
    semRcvDone = Semaphore_create(0, &semParams, NULL);

    /* Get the Heap handles - used when ever memory allocations are needed */
    //dataHeap = HeapMem_Handle_upCast(tcp3dDataHeap);
    dataHeap = (IHeap_Handle) SharedRegion_getHeap(0);
    drvHeap = HeapMem_Handle_upCast(tcp3dDrvHeap);

    /**
     * Create the Binary semaphores each time using the parameters set
     * outside the tester while loop.
     *
     * It was observed that at times the receive semaphore count was
     * non-zero after the first run and receive task was getting triggered
     * before posting from the ISR callback. So, the semaphores are created
     * for each test to work-around with the problem.
     */
    semSendBlock = Semaphore_create(0, &semParams, NULL);
    semSendWait = Semaphore_create(0, &semParams, NULL);
    semRcvStart = Semaphore_create(0, &semParams, NULL);

	// figure out what the taskparams does so we can see where tcp3d_start comes in
    Task_create((Task_FuncPtr)sndBlockTaskFunc, &taskParams, NULL);
    Task_create((Task_FuncPtr)rcvBlockTaskFunc, &taskParams, NULL);

    System_printf("\n******************************************************************\n");
    System_printf("\n----- TEST #0 STARTED ------\n");

    System_printf("\nReading test vector files started (including memory allocation)...\n");
    getTestSetCB(dataHeap, &codeBlockSet, folderName);
    System_printf("Reading test vector files complete\n");
    System_printf("\tPrepared %d code blocks in %s mode\n", codeBlockSet.maxNumCB, strMode[codeBlockSet.mode]);
    tcp3d_init();
    getMemoryStats();

    System_printf("\n----- TEST INITIALIZATION COMPLETE -----\n\n");
    for (i = 0; i < codeBlockSet.maxNumCB ;i++)
    {
        /* Prepare fixed IC registers using the inCfgParams of first block*/
        Tcp3d_prepFixedConfigRegs(codeBlockSet.cbData[i]->inCfgParams, codeBlockSet.cbData[i]->inCfg);

        /* Prepare block size dependent params */
        prepareBlockSizeDepICParams(codeBlockSet.cbData[i]);
    }
    /* Start the Send task first */
    Semaphore_post(semSendBlock);
    /* Wait for the Receive task to complete */
    // we get stuck here again. hmm
    Semaphore_pend(semRcvDone, BIOS_WAIT_FOREVER);
    /**
     * Test Profile Calculations
     *
     *                              (Total Bits)
     * Throughput (Mbps) = -----------------------------
     *                      (Total Time)*(10^-9)*(10^6)
     *
     */
    TotalBitsDecoded = 0;
    for (i = 0; i < codeBlockSet.maxNumCB; ++i)
    {
        TotalBitsDecoded += codeBlockSet.cbData[i]->blockSize;
    }

    test_cycles = (total_clock_end - total_clock_start);
    ThroughPut = TotalBitsDecoded*1000; // *1
    ThroughPut = (ThroughPut/test_cycles); // *1000 but can't print %f???

    /******** Free code blocks ********/
    System_printf("\nTest vectors memory freeing started...\n");
    //freeTestSetCB(dataHeap, &codeBlockSet);
    // For some reason the memory is not freeing
    // ti.sysbios.heaps.HeapMem: ERROR: line 353: assertion failure: A_invalidFree
    // possibly need to edit the .cfg file
    System_printf("Test vectors memory freeing complete\n");
    System_printf("\tFreed memory allocated for %d code blocks in %s mode\n", codeBlockSet.maxNumCB, strMode[codeBlockSet.mode]);

    System_printf("\n----- TEST DE-INITIALIZATION STARTED -----\n\n");
    allDeInit();
    getMemoryStats(); /* Heap Stats */
    System_printf("\n----- TEST DE-INITIALIZATION COMPLETE -----\n");

    if ( totErrCnt > 0 )
    {
        System_printf("\n----- TEST #%d FAILED -----\n", testCntr);
        testErrCntr++;
    }
    else
    {
        System_printf("\n----- TEST #%d PASSED -----\n", testCntr);
    }
    System_printf("\n+++++++++++++++++++++++ TEST #%d SUMMARY +++++++++++++++++++++++++\n", testCntr);
    System_printf("TCP3D Peripheral Configuration\n");
    System_printf("    Mode Tested                  : %s\n", strMode[codeBlockSet.mode]);
    System_printf("    Double Buffer Mode           : %s\n", strDBuf[codeBlockSet.doubleBuffer]);
    System_printf("Max code blocks (Input Capacity) : %d\n", testMaxCodeBlocks);
    System_printf("Code blocks sent for decoding    : %d\n", codeBlockSet.maxNumCB);
    System_printf("Call back counters               : %d - interrupts\n", pauseIntr);
    System_printf("                          (%d-SOLDOUT, %d-PAUSE, %d-PENDPAUSE)\n", afterIntrSoldout, afterIntrPause, pendPauseCntr);
    System_printf("Total Notificaiton Interrupts    : %d\n", tcp3dEventCntr);
    System_printf("Throughput Calculations\n");
    System_printf("    Total Bits Decoded           : %d\n", TotalBitsDecoded);
    System_printf("    Time Taken (in cycles)       : %d\n", test_cycles);
    System_printf("    Effective Throughput         : %d Mbps\n", ThroughPut);
    System_printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    System_printf("\n******************************************************************\n");

    /* Increment the test counter */
    testCntr++;

    /**
     * Delete the semaphores each time, so that there is no left over count.
     * See the explanation at the beginning of this loop where the create
     * semaphore calls are present.
     */
    Semaphore_delete(&semSendWait);
    Semaphore_delete(&semSendBlock);
    Semaphore_delete(&semRcvStart);

	/* All test status print */
	if(testErrCntr)
	{
		System_printf("!!! SOME TESTS FAILED !!!\n");
	}
	else
	{
		System_printf("!!! ALL TESTS PASSED !!!\n");
	}

	/* Remove all creations - to make graceful system exit */
	Semaphore_delete(&semRcvDone);

	// All done let's send message back to ARM
	*arm_ptr3 = 0;

	System_exit(0);
    return 0;
}

void transportReceive (void)
{
    int32_t          numPkts;
    MessageQ_Msg     rmMsg = NULL;
    Rm_Packet       *rmPkt = NULL;
    int32_t          status;
    uint32_t         i;
    Task_Params        taskParams;

    /* Check if any packets available */
    numPkts = (int32_t) MessageQ_count(rmClientQ);

    /* Process all available packets */
    for (i = 0; i < numPkts; i++) {
        status = (int32_t) MessageQ_get(rmClientQ, &rmMsg, MessageQ_FOREVER);
        if (rmMsg == NULL) {
            System_printf("Error Core %d : MessageQ_get failed, returning a NULL packet\n", coreNum);
            testErrors++;
        }

        // Worst fear coming true with the data
        // can't use both rm and tcp3d because of
        // tasks. Let's just do some core sync stuff
        // assign pointers to physical memory
        // wait for the values to change
        // do stuff. easy enough

        /* Extract the Rm_Packet from the RM msg */
        rmPkt = &(((MsgQ_RmPacket *)rmMsg)->rmPkt);

        /* Extract the Phys_ptr from the RM msg */
        //ptr1 = ((MsgQ_RmPacket *)rmMsg)->phys_ptr1;
        //size1 = ((MsgQ_RmPacket *)rmMsg)->size1;
        //ptr2 = ((MsgQ_RmPacket *)rmMsg)->phys_ptr2;
        //size2 = ((MsgQ_RmPacket *)rmMsg)->size2;
        //ptr3 = ((MsgQ_RmPacket *)rmMsg)->phys_ptr3;
        //size3 = ((MsgQ_RmPacket *)rmMsg)->size3;

        //memcpy(&block0_llrs, ptr2, size2);

        // works ok
        // Now to get 2 buffers and messages back to the ARM...
        //System_printf("\n Value Ptr1 is: %lx ", *(int *)ptr1);
        //System_printf("\n Value Ptr2: %d", *(int *)ptr2);
        //System_printf("\n Value Ptr3: %d", *(bool *)ptr3);
        //System_printf("\n Ptr1 is: %lx ", ptr1);
        //System_printf("\n Ptr2 is: %lx ", ptr2);
        //System_printf("\n Ptr3 is: %lx ", ptr3);
        //System_printf("\n block0_llrs: %d", *ptr2);
        //System_printf("\n block0_llrs: %d", *(ptr2+1));
        // Process the data on the TCP3d with a task. doesn't like tasks in tasks :(
       // tcp3d_test();
        //*(bool *)ptr3 = 0;
        //System_printf("\n Value Ptr3: %d", *(bool *)ptr3);

        /* Provide packet to RM for processing */
        if (status = Rm_receivePacket(clientFromServerTransportHandle, rmPkt)) {
            System_printf("Error Core %d : RM failed to process received packet: %d\n", coreNum, status);
            testErrors++;
        }

        /* Free RM packet buffer and  messageQ message */
        transportFree(rmMsg);
    }
}

void serviceCallback(Rm_ServiceRespInfo *serviceResponse)
{
    uint32_t qIndex = 0;

    /* Populate next free entry in the responseInfoQueue */
    while (responseInfoQueue[qIndex].serviceId != 0) {
        qIndex++;
        if (qIndex == MAX_QUEUED_SERVICE_RESPONSES) {
            qIndex = 0;
        }
    }

    /* Save the response in the response queue for the test task to pick up */
    memcpy((void *)&responseInfoQueue[qIndex], (void *)serviceResponse, sizeof(Rm_ServiceRespInfo));
}

/* Packets received via rpmsg port will issue callback vai transportReceive function */
void waitForResponse(Rm_ServiceRespInfo *respInfo)
{
    uint32_t qIndex = 0;

    if ((respInfo->serviceState == RM_SERVICE_PROCESSING) ||
        (respInfo->serviceState == RM_SERVICE_PENDING_SERVER_RESPONSE)) {
        /* Scan responseInfoQueue for the response received via the callback function */
        while((responseInfoQueue[qIndex].serviceId != respInfo->serviceId) ||
              (responseInfoQueue[qIndex].rmHandle != respInfo->rmHandle)) {
            qIndex++;
            if (qIndex == MAX_QUEUED_SERVICE_RESPONSES) {
                qIndex = 0;
            }

            /* Higher priority receive task will retrieve response */
        }

        memcpy((void *)respInfo, (void *)&responseInfoQueue[qIndex], sizeof(Rm_ServiceRespInfo));
        memset((void *)&responseInfoQueue[qIndex], 0, sizeof(Rm_ServiceRespInfo));
    }
}

void setRmRequest(Rm_ServiceReqInfo *reqInfo, Rm_ServiceType type, const char *resName, int32_t resBase,
                  uint32_t resLen, int32_t resAlign, const char *nsName, int setCallback, Rm_ServiceRespInfo *respInfo)
{
    memset((void *)reqInfo, 0, sizeof(Rm_ServiceReqInfo));
    reqInfo->type = type;
    reqInfo->resourceName = resName;
    reqInfo->resourceBase = resBase;
    reqInfo->resourceLength = resLen;
    reqInfo->resourceAlignment = resAlign;
    reqInfo->resourceNsName = nsName;
    if (setCallback) {
        reqInfo->callback.serviceCallback = serviceCallback;
    }
    memset((void *)respInfo, 0, sizeof(Rm_ServiceRespInfo));
}

void rmCleanupTsk(UArg arg0, UArg arg1)
{
    Rm_ServiceReqInfo  requestInfo;
    Rm_ServiceRespInfo responseInfo;
    int32_t            result;
    int32_t            finalMallocFree;

    /* Free all allocated resources */
    setRmRequest(&requestInfo, Rm_service_RESOURCE_FREE, resourceNameAccumCh,
                 0, 7, 0, NULL, RM_TEST_TRUE, &responseInfo);
    rmClientServiceHandle->Rm_serviceHandler(rmClientServiceHandle->rmHandle, &requestInfo, &responseInfo);
    waitForResponse(&responseInfo);
    POSITIVE_PASS_CHECK("-------------------- Resource Cleanup -------------------",
                        coreNum, rmClientName, responseInfo.resourceName,
                        responseInfo.resourceBase, responseInfo.resourceLength,
                        requestInfo.resourceAlignment, responseInfo.serviceState, RM_SERVICE_APPROVED);

    /* Cleanup all service ports, transport handles, RM instances, and IPC constructs */
    result = Rm_serviceCloseHandle(rmClientServiceHandle);
    ERROR_CHECK(RM_OK, result, rmClientName, "Service handle close failed");

    result = Rm_transportUnregister(clientFromServerTransportHandle);
    ERROR_CHECK(RM_OK, result, rmClientName, "Unregister of Server transport failed");

	result = MessageQ_delete(&rmClientQ);
    if (result < 0) {
        System_printf("Core %d : Error in MessageQ_delete [%d]\n", coreNum, result);
        testErrors++;
    }

    result = Rm_delete(rmClientHandle, RM_TEST_TRUE);
    ERROR_CHECK(RM_OK, result, rmClientName, "Instance delete failed");

    System_printf ("Core %d : ---------------------------------------------------------\n", coreNum);
    System_printf ("Core %d : ------------------ Memory Leak Check --------------------\n", coreNum);
    System_printf ("Core %d : -                       : malloc count   |   free count -\n", coreNum);
    System_printf ("Core %d : - Example Completion    :  %6d        |  %6d      -\n", coreNum,
                   rmMallocCounter, rmFreeCounter);
    finalMallocFree = rmMallocCounter - rmFreeCounter;
    if (finalMallocFree > 0) {
        System_printf ("Core %d : - FAILED - %6d unfreed mallocs                       -\n",
                       coreNum, finalMallocFree);
        testErrors++;
    }
    else if (finalMallocFree < 0) {
        System_printf ("Core %d : - FAILED - %6d more frees than mallocs               -\n",
                       coreNum, -finalMallocFree);
        testErrors++;
    }
    else {
        System_printf ("Core %d : - PASSED                                                -\n",
                       coreNum);
    }
    System_printf ("Core %d : ---------------------------------------------------------\n", coreNum);
    System_printf ("\n");

    System_printf ("Core %d : ---------------------------------------------------------\n", coreNum);
    System_printf ("Core %d : ------------------ Example Completion -------------------\n", coreNum);
    if (testErrors) {
        System_printf ("Core %d : - Test Errors: %-32d         -\n", coreNum, testErrors);
    }
    System_printf ("Core %d : ---------------------------------------------------------\n", coreNum);
    System_printf ("\n");
}

/* Receive task has priority of 2 so that it pre-empts the RM instance test tasks */
void rmReceiveTsk(UArg arg0, UArg arg1)
{
    while(1) {
        transportReceive();
        /* Sleep for 1ms so that the main test tasks can run */
        Task_sleep(1);
    }
}

void rmClientTsk(UArg arg0, UArg arg1)
{
    Rm_ServiceReqInfo  requestInfo;
    Rm_ServiceRespInfo responseInfo;
    Task_Params        taskParams;
    uint32_t           i, j;

    /* Create new NameServer object */

    /* Create the RM cleanup task. */
    System_printf("Core %d : Creating RM cleanup task...\n", coreNum);
    Task_Params_init (&taskParams);
    Task_create (rmCleanupTsk, &taskParams, NULL);
}

void rmStartupTsk(UArg arg0, UArg arg1)
{
    Task_Params      taskParams;
    Char             localQueueName[64];
    MessageQ_Msg     msg;
    Rm_TransportCfg  rmTransCfg;
    int32_t          rm_result;

    /* Construct a MessageQ name adorned with core name: */
    System_sprintf(localQueueName, "%s_%s", CLIENT_MESSAGEQ_NAME,
                   MultiProc_getName(MultiProc_self()));

    rmClientQ = MessageQ_create(localQueueName, NULL);
    if (rmClientQ == NULL) {
        System_abort("MessageQ_create failed\n");
    }

    if (coreNum == TEST_CORE) {
        System_printf("Awaiting sync message from host...\n");
        MessageQ_get(rmClientQ, &msg, MessageQ_FOREVER);

        linuxQueueId = MessageQ_getReplyQueue(msg);
        System_printf("\n\n ====== What is :");
        MessageQ_put(linuxQueueId, msg);

        /* Register the Server with the Client instance */
        rmTransCfg.rmHandle = rmClientHandle;
        rmTransCfg.appTransportHandle = (Rm_AppTransportHandle) &linuxQueueId;
        rmTransCfg.remoteInstType = Rm_instType_SERVER;
        rmTransCfg.transportCallouts.rmAllocPkt = transportAlloc;
        rmTransCfg.transportCallouts.rmSendPkt = transportSend;
        clientFromServerTransportHandle = Rm_transportRegister(&rmTransCfg, &rm_result);

        /* Create the RM receive task.  Assign higher priority than the test tasks so that
         * when they spin waiting for messages from other RM instances the receive task is
         * executed. */
        System_printf("Core %d : Creating RM receive task...\n", coreNum);
        Task_Params_init (&taskParams);
        taskParams.priority = 2;
        rmReceiveTskHandle = Task_create (rmReceiveTsk, &taskParams, NULL);

        System_printf("Core %d : Creating RM client task...\n", coreNum);
        Task_Params_init (&taskParams);
        taskParams.priority = 1;
        rmClientTskHandle = Task_create (rmClientTsk, &taskParams, NULL);
    }
}

int tcp3d_power() {
    /* TCP3D power domain is turned OFF by default.
     * It needs to be turned on before doing any TCP3D device register access.
     * This is not required for the simulator. */

    /* Set TCP3D Power domain to ON */
    CSL_PSC_enablePowerDomain (CSL_PSC_PD_TCP3D_01);

    /* Enable the clocks too for TCP3D */
    CSL_PSC_setModuleNextState (CSL_PSC_LPSC_TCP3D_0, PSC_MODSTATE_ENABLE);

    /* Start the state transition */
    CSL_PSC_startStateTransition (CSL_PSC_PD_TCP3D_01);

    /* Wait until the state transition process is completed. */
    while (!CSL_PSC_isStateTransitionDone (CSL_PSC_PD_TCP3D_01));

    /* Return TCP3D PSC status */
    if ((CSL_PSC_getPowerDomainState(CSL_PSC_PD_TCP3D_01) == PSC_PDSTATE_ON) &&
        (CSL_PSC_getModuleState (CSL_PSC_LPSC_TCP3D_0) == PSC_MODSTATE_ENABLE))
    {
        /* TCP3D ON. Ready for use */
        return 0;
    }
    else
    {
        /* SRIO Power on failed. Return error */
        return -1;
    }
}

int main(int argc, char *argv[])
{
    Task_Params        taskParams;
    Rm_InitCfg         rmInitCfg;
    Rm_ServiceReqInfo  requestInfo;
    Rm_ServiceRespInfo responseInfo;
    int32_t            result;

    // Tell DSP to halt
    *(volatile int *)arm_ptr3 = 1;
    *(volatile int *)arm_ptr4 = 1;

    System_printf ("*********************************************************\n");
    System_printf ("************ RM DSP+ARM DSP Client Testing **************\n");
    System_printf ("*********************************************************\n");

    System_printf ("RM Version : 0x%08x\nVersion String: %s\n", Rm_getVersion(), Rm_getVersionStr());

    coreNum = CSL_chipReadReg(CSL_CHIP_DNUM);

    if (coreNum == TEST_CORE) {
        testErrors = 0;

        /* Initialize the RM Client - RM must be initialized before anything else in the system */
        memset(&rmInitCfg, 0, sizeof(rmInitCfg));
        rmInitCfg.instName = rmClientName;
        rmInitCfg.instType = Rm_instType_CLIENT;
        rmInitCfg.instCfg.clientCfg.staticPolicy = (void *)rmStaticPolicy;
        rmClientHandle = Rm_init(&rmInitCfg, &result);
        ERROR_CHECK(RM_OK, result, rmClientName, "Initialization failed");

        System_printf("\n\nInitialized %s\n\n", rmClientName);

        /* Open Client service handle */
        rmClientServiceHandle = Rm_serviceOpenHandle(rmClientHandle, &result);
        ERROR_CHECK(RM_OK, result, rmClientName, "Service handle open failed");

        /* Enable time stamp counter */
        CSL_tscEnable();
        /* Enable L1D cache. Disable L2 caching for our tests. */
        CACHE_setL1DSize (CACHE_L1_MAXIM3); /* CACHE_L1_0KCACHE */
        CACHE_setL2Size (CACHE_0KCACHE);

        /* Turn ON the TCP3d */
        result = tcp3d_power();
        if (result == 0) {
        	System_printf(" tcp3d_power PASS\n");
        } else {
        	System_printf(" tcp3d_power FAIL\n");
        }
        int a;
        // seems to be enough time for the setup...
        // really don't like this approach oh well
        int b = 10000;
        int c = 10000;
        // wait until ARM is done setting up
        System_printf("waiting for ARM %d\n", *arm_ptr4);
        System_printf("\n Ptr4 is: %lx ", arm_ptr4);
        System_printf("\n Ptr4 Add is: %lx ", &arm_ptr4);
        if(1) {
        	for (a=0; a<b; a++) {
        		for (b=0; b<c; b++) {
        		}
        	}
        }
        System_printf(" Done waiting! \n");
        Task_Params_init(&taskParams);
        Task_create((Task_FuncPtr)tcp3d_test, &taskParams, NULL);
    }
    else {
        System_printf("Core %d : RM DSP+ARM Linux test not executing on this core\n", coreNum);
    }

    /* Create the RM startup task */
    //System_printf("Core %d : Creating RM startup task...\n", coreNum);
    //Task_Params_init (&taskParams);
    //rmStartupTskHandle = Task_create (rmStartupTsk, &taskParams, NULL);
    //Task_create((Task_FuncPtr)tcp3d_test, &taskParams, NULL);
    System_printf("Core %d : Starting BIOS...\n", coreNum);
    BIOS_start();

    return (0);
}
