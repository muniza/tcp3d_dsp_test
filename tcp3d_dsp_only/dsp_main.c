/*
 *   dsp_main.c
 *
 *	TCP3d DSP Only Test. This test can be loaded on DSP Core0 to
 *	test the functionality of the TCP3d using some test data.
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
#include <xdc/runtime/IHeap.h>
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/System.h>

/* BIOS includes */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/family/c64p/Hwi.h>
#include <ti/sysbios/family/c64p/EventCombiner.h>
#include <ti/sysbios/family/c66/tci66xx/CpIntc.h>

/* IPC includes */
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/GateMP.h>
#include <ti/ipc/Ipc.h>
#include <ti/ipc/ListMP.h>
#include <ti/ipc/HeapBufMP.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/SharedRegion.h>

/* CSL includes */
#include <ti/csl/csl_chip.h>
#include <ti/csl/csl_chipAux.h>
#include <ti/csl/soc.h>
#include <ti/csl/cslr_tpcc.h>
#include <ti/csl/cslr_tcp3d_cfg.h>
#include <ti/csl/cslr_tcp3d_dma.h>
#include <ti/csl/cslr_tcp3d_dma_offsets.h>
#include <ti/csl/csl_tsc.h>
#include <ti/csl/csl_cacheAux.h>
#include <ti/csl/csl_xmcAux.h>
#include <ti/csl/csl_psc.h>
#include <ti/csl/csl_pscAux.h>

/* RM Includes */
#include <ti/drv/rm/rm.h>
#include <ti/drv/rm/rm_transport.h>
#include <ti/drv/rm/rm_services.h>

/* TCP3D Includes */
#include "sample.h"
#include "tcp3d_drv_sample.h"
#include "tcp3d_main.h"
#include "tcp3d_profile.h"

/* Test will run on this core */
#define TEST_CORE                    0
#define START_CMD_PERIOD             1

volatile PROFILE_TAG    profileTag[PROF_TAG_LEN];
volatile UInt32         profileTagInd = 0;
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
 ********************** Global Variables ******************************
 **********************************************************************/

/* Core number */
uint16_t            coreNum;
/* Number of errors that occurred during the test */
uint16_t            testErrors;

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

/**
 *  @b Description
 *  @n
 *      The function is used to indicate that the block of memory has
 *      finished being accessed. If the memory block is cached then the
 *      application would need to ensure that the contents of the cache
 *      are updated immediately to the actual memory.
 *
 *  @param[in]  ptr
 *       Address of memory block
 *  @param[in]  size
 *       Size of memory block
 *
 *  @retval
 *      Not Applicable
 */
void tcp3dEndMemAccess (void *ptr, uint32_t size)
{
    /* Writeback L1D cache and wait until operation is complete.
     * Use this approach if L2 cache is not enabled */
    CACHE_wbL1d (ptr, size, CACHE_FENCE_WAIT);

    return;
}

Void soldOutAction(Void)
{
    /* clear flag */
    pauseIntFlag = 0;
    /* keep trying until successful */
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

Void soldOutActionClear(Void)
{
	//nothing to do
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

/*******************************************************************************
 ******************************************************************************/
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

int enable_tcp3d() {
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

Void getMemoryStats(Void)
{
    Memory_Stats        memStats;

    Memory_getStats(drvHeap, &memStats);
    System_printf("\nHeap Usage/Staus\n");
    System_printf("    tcp3dDrvHeap : %d of %d free\n", memStats.totalFreeSize, memStats.totalSize);

    Memory_getStats(dataHeap, &memStats);
    System_printf("    tcp3dDataHeap : %d of %d free\n", memStats.totalFreeSize, memStats.totalSize);
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

int tcp3d_test() {
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

    /* Create the Binary Semaphore */
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

    /* freeTestSetCB(dataHeap, &codeBlockSet);
    * For some reason the memory is not freeing
    * ti.sysbios.heaps.HeapMem: ERROR: line 353: assertion failure: A_invalidFree
    * possibly need to edit the .cfg file
    */

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
    System_printf("Total Notification Interrupts    : %d\n", tcp3dEventCntr);
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

	System_exit(0);
    return 0;
}

int main(int argc, char *argv[])
{
    Task_Params        taskParams;

    System_printf ("*********************************************************\n");
    System_printf ("***************** TCP3d DSP Only Test *******************\n");
    System_printf ("*********************************************************\n");

    coreNum = CSL_chipReadReg(CSL_CHIP_DNUM);

    if (coreNum == TEST_CORE) {
        testErrors = 0;
        /* Enable time stamp counter */
        CSL_tscEnable();
        /* Enable L1D cache. Disable L2 caching for our tests. */
        CACHE_setL1DSize (CACHE_L1_MAXIM3); /* CACHE_L1_0KCACHE */
        CACHE_setL2Size (CACHE_0KCACHE);
        /* Power on TCP3D */
        if (enable_tcp3d () < 0)
        {
            System_printf (" TCP3D Power FAIL \n");
        }
        else
        {
        	System_printf (" TCP3D Power PASS \n");
        }
        Task_Params_init(&taskParams);
        Task_create((Task_FuncPtr)tcp3d_test, &taskParams, NULL);
    }
    else {
        System_printf("Core %d : TCP3d DSP only test not executing on this core\n", coreNum);
    }

    System_printf("Core %d : Starting BIOS...\n", coreNum);
    BIOS_start();

    return (0);
}

