/*
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#include <stdio.h>
#include <string.h>

#include "tcp3d_main.h"

#define READ_BINARY_TEST_VECTORS    0 /* Read .bin test vector files */

UInt32                  morePrints = 0;
extern Char             *strMode[4];
extern Char             testvectFolderBase[];

/* Changing the test files into arrays */
Int32 number_of_blocks[] = {1};
Int32 block0_cfgreg[] = {1,0,5504,64,0,0,0,1,0,1,1,8,14,1,0,1,0,4,2,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24};
Int32 block0_tail_llrs[] = {-7,20,22,-23,16,16,23,12,-1,23,-23,13};
extern const Int32 block0_hard_dec[];
extern const Int32 block0_llrs[];

/*******************************************************************************
 ******************************************************************************/
/**
 * @brief   Bit reverse in 32-bit word
 */
UInt32 bitr(UInt32 src)
{
    UInt32 a,c;
    UInt32 i;
    UInt32 sa;

    a = src;
    c = 0;
    for (i = 0,sa=31; i < 16; i++,sa-=2) {
      c |= (a & (0x1 << i)) << sa;
    }
    for (i = 16,sa=1; i < 32; i++,sa+=2) {
      c |= (a & (0x1 << i)) >> (sa);
    }
    return c;
}

/*******************************************************************************
 ******************************************************************************/
/**
 * @brief   Swap bytes in 32-bit word
 */
UInt32 swapBytes(UInt32 src)
{
    UInt32 a,c;

    a = src;
    c = (a & 0x000000ff) << 24;
    c |= (a & 0x0000ff00) << 8;
    c |= (a & 0x00ff0000) >> 8;
    c |= (a & 0xff000000) >> 24;
    return c;
}

/*******************************************************************************
 ******************************************************************************/
Int getTestSetCB(IHeap_Handle dataHeap, cbTestDesc *cbTestSet, Char *testFolder)
{
    cbDataDesc      *cbPtr;
    cbConfig        tempCbConfig;
    Int             i, j, cbCnt;
    Int32           numBytes;

    Int             cbCfgStrSize=(sizeof(cbConfig)>>2);
    Int32           *tmp32 = (Int32 *) &tempCbConfig;
                            /* Note: Temporary pointer used for writing
                                into the tempCbConfig Structure memory */
    Int32           stmp[3];
    UInt32          inSize;
    UInt32          tmp;

    cbCnt = 0;
    cbTestSet->maxNumCB = number_of_blocks[0];

    /* Allocate memory for code blocks */
    numBytes = cbTestSet->maxNumCB * sizeof(cbDataDesc *);
    cbTestSet->cbData = (cbDataDesc **) Memory_alloc(dataHeap, numBytes, 64, NULL);

    /* Load code blocks */
    for(cbCnt=0; cbCnt < cbTestSet->maxNumCB; cbCnt++)
    {
        /* Allocate memory for code block set and update the local pointer */
        cbTestSet->cbData[cbCnt] = (cbDataDesc *) Memory_alloc ( dataHeap,
                                                sizeof(cbDataDesc),
                                                64,
                                                NULL);
        cbPtr = cbTestSet->cbData[cbCnt];
        if ( cbPtr == NULL )
        {
            System_printf("\tMemory allocation failed for Code Block Set %d!\n", cbCnt);
        }

        /* =================
         *
         * block0_cfgreg.dat
         *
         */
        for(i=0;i<cbCfgStrSize;i++)
        {
            tmp32[i] = block0_cfgreg[i]; /* tmp32 points to tempCbConfig address */
        }
        /* Set the Code Block specific values from Config Read */
        cbPtr->blockSize     = tempCbConfig.NumInfoBits;
        cbPtr->mode          = tempCbConfig.mode_sel;
        cbPtr->crcInitVal    = tempCbConfig.lte_crc_init_sel;
        cbPtr->sw0LengthUsed = tempCbConfig.SW0_length;

        /* Set the code block set value with the first one block config */
        if (cbCnt == 0)
        {
            cbTestSet->mode       = cbPtr->mode;
            cbTestSet->lteCrcSel  = cbPtr->crcInitVal;
        }

        /* Check if the mode is different from the first one in the group */
        /* Force the mode value to be same as first one */
        if ( cbPtr->mode != cbTestSet->mode )
        {
            if ( !((cbPtr->mode+cbTestSet->mode) % 3) )
            {
                System_printf("\tBlock = %d, Mode changed from %s to %s\n", cbCnt,
                                                    strMode[cbPtr->mode],
                                                    strMode[cbTestSet->mode]);
                cbPtr->mode = cbTestSet->mode;
            }
            else
            {
                System_exit(0);
            }
        }

        /* Check for LTE CRC Init Value Change */
        if ( ( cbPtr->mode == TEST_MODE_LTE ) &&
             ( cbPtr->crcInitVal != cbTestSet->lteCrcSel ) )
        {
            System_printf("\tLTE CRC Initial Value is different\n");
            System_printf("\tSet Value is %d\n", cbTestSet->lteCrcSel);
            System_exit(0);
        }

        /* Allocate Memory for Input LLR data */
        if ( cbPtr->mode == TEST_MODE_SPLIT )
        {   /* 3GPP - Split Mode */
            cbPtr->llrOffset   = COMPUTE_KOUT(cbPtr->blockSize);
        }
        else
        {
            cbPtr->llrOffset   = cbPtr->blockSize;
        }
        cbPtr->sizeLLR = 3 * cbPtr->llrOffset; /* three streams (sys, par0, par1) */
        cbPtr->inLLR = (Int8 *) Memory_alloc(dataHeap, cbPtr->sizeLLR, 64, NULL);
        if(cbPtr->inLLR == NULL)
        {
            System_printf("\tMemory allocation failed !!! (LLR)\n");
            System_exit(0);
        }

        /* Prepare Input LLR streams */
        /* This mode value is set with the value read from cfgreg file */
        if ( cbPtr->mode == TEST_MODE_SPLIT ) /* 3GPP - split mode */
        {
            /* set loop count for LLR file read */
            inSize  = cbPtr->blockSize*3;

            for(i=0;i<inSize;i+=3)
            {
            	stmp[0] = block0_llrs[i];
            	stmp[1] = block0_llrs[i+1];
            	stmp[2] = block0_llrs[i+2];
                cbPtr->inLLR[i]                     = (Int8) stmp[0];
                cbPtr->inLLR[i+cbPtr->llrOffset]    = (Int8) stmp[1];
                cbPtr->inLLR[i+cbPtr->llrOffset<<1] = (Int8) stmp[2];
            }
            for (i = 0; i < 6; i++)
            {
            	stmp[0] = block0_tail_llrs[i];
            	stmp[1] = block0_tail_llrs[6+i];
                cbPtr->tailBits[i]      = (Int8) stmp[0];
                cbPtr->tailBits[6+i]    = (Int8) stmp[1];
            }
        }
        else if ( cbPtr->mode == TEST_MODE_LTE )  /* LTE mode */
        {
            for(i=0;i<cbPtr->blockSize;i++)
            {
            	j = 3*i;
            	stmp[0] = block0_llrs[j];
            	stmp[1] = block0_llrs[j+1];
               	stmp[2] = block0_llrs[j+2];
                cbPtr->inLLR[i]                      = (Int8) stmp[0];
                cbPtr->inLLR[i+cbPtr->llrOffset]  = (Int8) stmp[1];
                cbPtr->inLLR[i+2*cbPtr->llrOffset]= (Int8) stmp[2];
            }
            for (i = 0; i < 6; i++)
            {
            	stmp[0] = block0_tail_llrs[i];
            	stmp[1] = block0_tail_llrs[6+i];
                cbPtr->tailBits[i] = (Int8) stmp[0];
                cbPtr->tailBits[6+i] = (Int8) stmp[1];
            }
        }
        else if ( cbPtr->mode == TEST_MODE_WIMAX )  /* WIMAX mode */
        {
            for(i=0;i<cbPtr->blockSize;i++)
            {
            	j = 3*i;
            	stmp[0] = block0_llrs[j];
            	stmp[1] = block0_llrs[j+1];
            	stmp[2] = block0_llrs[j+2];
                cbPtr->inLLR[i]                      = (Int8) stmp[0];
                cbPtr->inLLR[i+cbPtr->llrOffset]  = (Int8) stmp[1];
                cbPtr->inLLR[i+2*cbPtr->llrOffset]= (Int8) stmp[2];
            }
        }

        /* Allocate Memory for Ouput & Reference Hard Decisions */
        cbPtr->sizeHD = COMPUTE_HD_BYTE_SIZE(cbPtr->blockSize);
        cbPtr->outHD = (UInt32 *) Memory_calloc(dataHeap,
                                                    cbPtr->sizeHD,
                                                    64,
                                                    NULL);
        if(cbPtr->outHD == NULL)
        {
            System_printf("\tMemory allocation failed !!! (Out HD)\n");
            System_exit(0);
        }
        cbPtr->refHD = (UInt32 *) Memory_calloc(dataHeap,
                                                    cbPtr->sizeHD,
                                                    64,
                                                    NULL);
        if(cbPtr->refHD == NULL)
        {
            System_printf("\tMemory allocation failed !!! (Ref HD)\n");
            System_exit(0);
        }

        /* Prepare Reference Hard Decisions */
        for(i=0;i<(cbPtr->sizeHD>>2);i++)
        {
        	tmp = block0_hard_dec[i];
#ifdef _BIG_ENDIAN
            if (!tempCbConfig.out_order_sel)
            {
                cbPtr->refHD[i] = bitr(tmp);
            }
            else
            {
                cbPtr->refHD[i] = tmp;
            }
#else
            if (!tempCbConfig.out_order_sel)
            {
                cbPtr->refHD[i] = tmp;
            }
            else
            {
                cbPtr->refHD[i] = bitr(tmp);
            }
#endif
        }

        /* Allocate Memory for Interleaver Table */
        /* Interleaver flag is cleared - no external interleaver table */
        cbPtr->interFlag = 0;
        cbPtr->sizeINTER = 0;
        cbPtr->inInter = NULL;

        /* Allocate Memory for Ouput & Reference Soft Decisions */
        cbPtr->sdFlag = 0;
        cbPtr->sizeSD = 0;
        cbPtr->sdOffset = 0;
        cbPtr->outSD = NULL;
        cbPtr->refSD = NULL;
        if ( tempCbConfig.soft_out_flag_en )
        {
            cbPtr->sdFlag = 1;

            if ( cbPtr->mode == TEST_MODE_SPLIT )  /* SPLIT MODE */
            {
                cbPtr->sizeSD      = cbPtr->blockSize;
                cbPtr->sdOffset    = NULL;
            }
            else
            {
                cbPtr->sizeSD      = 3 * cbPtr->blockSize;
                cbPtr->sdOffset    = cbPtr->blockSize;
            }
            cbPtr->outSD = (Int8 *) Memory_calloc(dataHeap, cbPtr->sizeSD, 64, NULL);
            if(cbPtr->outSD == NULL)
            {
                System_printf("\tMemory allocation failed !!! (Out SD)\n");
                System_exit(0);
            }
            cbPtr->refSD = (Int8 *) Memory_calloc(dataHeap, cbPtr->sizeSD, 64, NULL);
            if(cbPtr->refSD == NULL)
            {
                System_printf("\tMemory allocation failed !!! (Ref SD)\n");
                System_exit(0);
            }

            /* Prepare Reference Soft Decisions */
            for (i = 0; i < cbPtr->blockSize; i++)
            {
                cbPtr->refSD[i]          = (Int8) stmp[0];
                if (cbPtr->sdOffset)
                {
                    cbPtr->refSD[i+cbPtr->sdOffset]   = (Int8) stmp[1];
                    cbPtr->refSD[i+2*cbPtr->sdOffset] = (Int8) stmp[2];
                }
            }
        }
        /* Allocate Memory for Ouput & Reference Status Registers */
        cbPtr->stsFlag = 0;
        cbPtr->sizeSTS = 0;
        cbPtr->outSts = NULL;
        cbPtr->refSts = NULL;
        if ( tempCbConfig.out_flag_en )
        {
            cbPtr->stsFlag = 1;
            /* allocate outSts memory */
            cbPtr->sizeSTS = 3 * sizeof(UInt32);
            cbPtr->outSts = (UInt32 *) Memory_calloc(dataHeap, cbPtr->sizeSTS, 64, NULL);
            if(cbPtr->outSts == NULL)
            {
                System_printf("\tMemory allocation failed !!! (Out STS)\n");
                System_exit(0);
            }
            cbPtr->refSts = (UInt32 *) Memory_calloc(dataHeap, cbPtr->sizeSTS, 64, NULL);
            if(cbPtr->refSts == NULL)
            {
                System_printf("\tMemory allocation failed !!! (Ref STS)\n");
                System_exit(0);
            }
        }
        /* Allocate Memory for Input Config Registers */
        cbPtr->sizeCFG = 15 * sizeof(UInt32);
        cbPtr->inCfg = (UInt32 *) Memory_calloc(dataHeap, cbPtr->sizeCFG, 64, NULL);
        if(cbPtr->inCfg == NULL)
        {
            System_printf("\tMemory allocation failed !!! (CFG)\n");
            System_exit(0);
        }

        /* Allocate Memory for Tcp3d_InCfgParams structure */
        cbPtr->inCfgParams = (Tcp3d_InCfgParams *) Memory_alloc(dataHeap,
                                                sizeof(Tcp3d_InCfgParams),
                                                64,
                                                NULL);
        if (cbPtr->inCfgParams == NULL)
        {
            System_printf("Memory allocation failed !!! (inCfgParams)\n");
            System_exit(0);
        }
        /* Update the input config params structure */
        fillICParams(cbPtr->inCfgParams, &tempCbConfig);

        if ( (morePrints) && ((cbCnt+1) % 5) == 0 )
            System_printf("\tCode block prepared : %d \n", cbCnt+1);

    } /* for(cbCnt=0; cbCnt < cbTestSet->maxNumCB; cbCnt++) */

    /* Set the double buffer value based on mode value */
    if ( ( cbTestSet->mode == TEST_MODE_SINGLE ) || ( cbTestSet->mode == TEST_MODE_SPLIT ) )
        cbTestSet->doubleBuffer = 0; //CSL_TCP3D_CFG_TCP3_MODE_IN_MEM_DB_EN_DISABLE
    else
        cbTestSet->doubleBuffer = 1; //CSL_TCP3D_CFG_TCP3_MODE_IN_MEM_DB_EN_ENABLE

    return (cbCnt);
}

/*******************************************************************************
 ******************************************************************************/
Void freeTestSetCB(IHeap_Handle dataHeap, cbTestDesc *cbTestSet)
{
    Int32           i;

    /* Free memory allocated for Code Block sets */
    for(i=0; i< cbTestSet->maxNumCB; i++)
    {
        Memory_free(dataHeap, cbTestSet->cbData[i]->inCfg, cbTestSet->cbData[i]->sizeCFG);
        Memory_free(dataHeap, cbTestSet->cbData[i]->inLLR, cbTestSet->cbData[i]->sizeLLR);
        Memory_free(dataHeap, cbTestSet->cbData[i]->outHD, cbTestSet->cbData[i]->sizeHD);
        Memory_free(dataHeap, cbTestSet->cbData[i]->refHD, cbTestSet->cbData[i]->sizeHD);
        if ( cbTestSet->cbData[i]->sdFlag )
        {
            Memory_free(dataHeap, cbTestSet->cbData[i]->outSD, cbTestSet->cbData[i]->sizeSD);
            Memory_free(dataHeap, cbTestSet->cbData[i]->refSD, cbTestSet->cbData[i]->sizeSD);
        }
        if ( cbTestSet->cbData[i]->stsFlag )
        {
            Memory_free(dataHeap, cbTestSet->cbData[i]->outSts, cbTestSet->cbData[i]->sizeSTS);
            Memory_free(dataHeap, cbTestSet->cbData[i]->refSts, cbTestSet->cbData[i]->sizeSTS);
        }
        if ( cbTestSet->cbData[i]->interFlag )
        {
            Memory_free(dataHeap, cbTestSet->cbData[i]->inInter, cbTestSet->cbData[i]->sizeINTER);
        }
        Memory_free(dataHeap, cbTestSet->cbData[i]->inCfgParams, sizeof(Tcp3d_InCfgParams));
        Memory_free(dataHeap, cbTestSet->cbData[i], sizeof(cbDataDesc));
    }
    Memory_free(dataHeap, cbTestSet->cbData, cbTestSet->maxNumCB * sizeof(cbDataDesc *));
}

/* end of file */
