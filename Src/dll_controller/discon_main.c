/*
 * Copyright 1994-2012 The MathWorks, Inc.
 *
 * File    : classic_main.c
 *
 * Abstract:
 *      A Generic "Real-Time (single tasking or pseudo-multitasking,
 *      statically allocated data)" main that runs under most
 *      operating systems.
 *
 *      This file may be a useful starting point when targeting a new
 *      processor or microcontroller.
 *
 *
 * Compiler specified defines:
 *	RT              - Required.
 *      MODEL=modelname - Required.
 *	NUMST=#         - Required. Number of sample times.
 *	NCSTATES=#      - Required. Number of continuous states.
 *      TID01EQ=1 or 0  - Optional. Only define to 1 if sample time task
 *                        id's 0 and 1 have equal rates.
 *      MULTITASKING    - Optional. (use MT for a synonym).
 *	SAVEFILE        - Optional (non-quoted) name of .mat file to create.
 *			  Default is <MODEL>.mat
 */


/* 

Modified from https://github.com/TUDelft-DataDrivenControl/DISCON_Simulink/tree/master/Simulink_64bit

Copyright(c) 2018 Delft Center for Systems& Control, Data - Driven Control(TU Delft)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this softwareand associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be included in all
copies or substantial portions of the Software.

*/
#define _CRT_SECURE_NO_WARNINGS
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
FILE* pFile; //Added by JW to read the init file

typedef struct {
    /* Torque controller gains (GS on generator speed) */
    int nKpBr; float* KpBr; float* GS_KpBr;
    int nKiBr; float* KiBr; float* GS_KiBr;
    int nKpAr; float* KpAr; float* GS_KpAr;
    int nKiAr; float* KiAr; float* GS_KiAr;
    /* Cp/TSR 1D curve (derived from Cp matrix) */
    int nCpTSR; float* Cp_lut; float* TSR_lut;
    /* Pitch PI gains (GS on pitch angle) */
    int nPC_GS; float* PC_GS_angles; float* PC_GS_Kp; float* PC_GS_Ki;
    /* Peak shaving */
    int nPS; float* PS_WindSpeeds; float* PS_BldPitchMin;
    /* EKF system poles */
    int nWE_FOPoles; float* WE_FOPoles_v; float* WE_FOPoles;
    /* Cp performance matrix (for TBE/EKF) */
    int Cp_nPitch; int Cp_nTSR;
    float* Cp_v_pitch; float* Cp_v_TSR; float* Cp_matrix;
} ControllerLUTs;

static ControllerLUTs ctrlLUTs = { 0 };

float interpolateLUT(float* X, float* Y, int n, float Xq) {
    if (n <= 0) return 0.0f;
    if (Xq <= X[0]) return Y[0];
    if (Xq >= X[n - 1]) return Y[n - 1];
    for (int i = 0; i < n - 1; i++) {
        if (Xq >= X[i] && Xq <= X[i + 1]) {
            float t = (Xq - X[i]) / (X[i + 1] - X[i]);
            return Y[i] + t * (Y[i + 1] - Y[i]);
        }
    }
    return Y[n - 1];
}

float interpolate2D(float* Xvec, int nX, float* Yvec, int nY, float* matrix, float Xq, float Yq) {
    /* Bilinear interpolation on matrix[nY][nX] with Xvec=pitch, Yvec=TSR */
    int ix = 0, iy = 0;
    float tx, ty, c00, c10, c01, c11;

    /* Clamp X */
    if (Xq <= Xvec[0]) { ix = 0; tx = 0.0f; }
    else if (Xq >= Xvec[nX - 1]) { ix = nX - 2; tx = 1.0f; }
    else {
        for (int i = 0; i < nX - 1; i++) {
            if (Xq >= Xvec[i] && Xq <= Xvec[i + 1]) {
                ix = i; tx = (Xq - Xvec[i]) / (Xvec[i + 1] - Xvec[i]); break;
            }
        }
    }
    /* Clamp Y */
    if (Yq <= Yvec[0]) { iy = 0; ty = 0.0f; }
    else if (Yq >= Yvec[nY - 1]) { iy = nY - 2; ty = 1.0f; }
    else {
        for (int i = 0; i < nY - 1; i++) {
            if (Yq >= Yvec[i] && Yq <= Yvec[i + 1]) {
                iy = i; ty = (Yq - Yvec[i]) / (Yvec[i + 1] - Yvec[i]); break;
            }
        }
    }
    /* matrix is stored row-major: matrix[iy * nX + ix] */
    c00 = matrix[iy * nX + ix];
    c10 = matrix[iy * nX + ix + 1];
    c01 = matrix[(iy + 1) * nX + ix];
    c11 = matrix[(iy + 1) * nX + ix + 1];

    return c00 * (1 - tx) * (1 - ty) + c10 * tx * (1 - ty) +
        c01 * (1 - tx) * ty + c11 * tx * ty;
}

void ExtractCpTSR_from_matrix(float fixedPitch_deg) {
    int nTSR = ctrlLUTs.Cp_nTSR;
    int nPitch = ctrlLUTs.Cp_nPitch;

    /* NOTE: this function is now called every control step (see
     * calcOutputController) so that the Cp(TSR) inversion used for dTSR_R3
     * always matches the CURRENT pitch, instead of the single dFPitch value
     * read once at startup. With a dynamic beta(U) feedforward schedule the
     * pitch is no longer constant, so re-deriving this curve only once at
     * init would leave dTSR_R3 (and therefore omega_ref in Region 3) based on
     * a stale, wrong-pitch curve as soon as the wind/pitch moves away from
     * the initial point.
     *
     * The working/output buffers are allocated ONCE (first call) and reused
     * on every subsequent call, so calling this every step does not leak
     * memory in the real-time loop. */
    static float* cp_tmp = NULL;
    static float* tsr_tmp = NULL;
    if (cp_tmp == NULL) {
        cp_tmp = (float*)malloc(nTSR * sizeof(float));
        tsr_tmp = (float*)malloc(nTSR * sizeof(float));
    }
    if (ctrlLUTs.TSR_lut == NULL) {
        /* nTSR is the maximum possible length of the rising-branch curve */
        ctrlLUTs.TSR_lut = (float*)malloc(nTSR * sizeof(float));
        ctrlLUTs.Cp_lut = (float*)malloc(nTSR * sizeof(float));
    }

    for (int i = 0; i < nTSR; i++) {
        /* Interpolate Cp at fixed pitch for this TSR */
        cp_tmp[i] = interpolateLUT(ctrlLUTs.Cp_v_pitch,
            &ctrlLUTs.Cp_matrix[i * nPitch],
            nPitch, fixedPitch_deg);
        tsr_tmp[i] = ctrlLUTs.Cp_v_TSR[i];
    }

    /* Find index of maximum Cp */
    int imax = 0;
    for (int i = 1; i < nTSR; i++)
        if (cp_tmp[i] > cp_tmp[imax]) imax = i;

    /* Save only the rising portion 0..imax (buffers already sized for the
     * worst case nTSR, so this never overflows) */
    ctrlLUTs.nCpTSR = imax + 1;
    for (int i = 0; i <= imax; i++) {
        ctrlLUTs.TSR_lut[i] = tsr_tmp[i];
        ctrlLUTs.Cp_lut[i] = cp_tmp[i];
    }
}

void ExtractCpTSR_from_matrix_max(void) {
    /* Variable pitch mode: for each TSR, find max Cp across all pitch angles */
    int nTSR = ctrlLUTs.Cp_nTSR;
    int nPitch = ctrlLUTs.Cp_nPitch;
    float* cp_tmp = (float*)malloc(nTSR * sizeof(float));
    float* tsr_tmp = (float*)malloc(nTSR * sizeof(float));

    for (int i = 0; i < nTSR; i++) {
        float cpmax = -1e30f;
        for (int j = 0; j < nPitch; j++) {
            float val = ctrlLUTs.Cp_matrix[i * nPitch + j];
            if (val > cpmax) cpmax = val;
        }
        cp_tmp[i] = cpmax;
        tsr_tmp[i] = ctrlLUTs.Cp_v_TSR[i];
    }

    int imax = 0;
    for (int i = 1; i < nTSR; i++)
        if (cp_tmp[i] > cp_tmp[imax]) imax = i;

    ctrlLUTs.nCpTSR = imax + 1;
    ctrlLUTs.TSR_lut = (float*)malloc((imax + 1) * sizeof(float));
    ctrlLUTs.Cp_lut = (float*)malloc((imax + 1) * sizeof(float));
    for (int i = 0; i <= imax; i++) {
        ctrlLUTs.TSR_lut[i] = tsr_tmp[i];
        ctrlLUTs.Cp_lut[i] = cp_tmp[i];
    }

    free(tsr_tmp);
    free(cp_tmp);
}

void skipLine(FILE* fp) {
    int c;
    while ((c = fgetc(fp)) != '\n' && c != EOF);
}

#include "rtwtypes.h"
# include "rtmodel.h"
#include "rt_sim.h"
#include "rt_logging.h"
#ifdef UseMMIDataLogging
#include "rt_logging_mmi.h"
#endif
#include "ext_work.h"
/*=========*
 * Defines *
 *=========*/
#ifndef TRUE
#define FALSE (0)
#define TRUE  (1)
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE  1
#endif
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS  0
#endif
#define QUOTE1(name) #name
#define QUOTE(name) QUOTE1(name)
#ifndef RT
# error "must define RT"
#endif
#ifndef MODEL
# error "must define MODEL"
#endif
#ifndef NUMST
# error "must define number of sample times, NUMST"
#endif
#ifndef NCSTATES
# error "must define NCSTATES"
#endif
#ifndef SAVEFILE
# define MATFILE2(file) #file ".mat"
# define MATFILE1(file) MATFILE2(file)
# define MATFILE MATFILE1(MODEL)
#else
# define MATFILE QUOTE(SAVEFILE)
#endif
#define RUN_FOREVER -1.0
#define EXPAND_CONCAT(name1,name2) name1 ## name2
#define CONCAT(name1,name2) EXPAND_CONCAT(name1,name2)
#define RT_MODEL            CONCAT(MODEL,_rtModel)
#define EXPAND_CONCAT3(name1,name2,name3) name1 ## _ ## name2 ## . ## name3
#define CONCAT3(name1,name2,name3) EXPAND_CONCAT3(name1,name2,name3)
#define SIG_MODEL(suffix,name)     CONCAT3(MODEL,suffix,name)
#define NINT(a) ((a) >= 0.0 ? (int)((a)+0.5) : (int)((a)-0.5))
#define MIN(a,b) ((a)>(b)?(b):(a))



 /*====================*
  * External functions *
  *====================*/
#ifdef __cplusplus

extern "C" {

#endif

    extern RT_MODEL* MODEL(void);

    extern void MdlInitializeSizes(void);
    extern void MdlInitializeSampleTimes(void);
    extern void MdlStart(void);
    extern void MdlOutputs(int_T tid);
    extern void MdlUpdate(int_T tid);
    extern void MdlTerminate(void);

    extern void __declspec(dllexport) __cdecl DISCON(float* avrSwap, int* aviFail, char* accInfile, char* avcOutname, char* avcMsg);

#ifdef __cplusplus

}
#endif

#if NCSTATES > 0
#ifdef __cplusplus

extern "C" {

#endif
    extern void rt_ODECreateIntegrationData(RTWSolverInfo* si);
    extern void rt_ODEUpdateContinuousStates(RTWSolverInfo* si);
#ifdef __cplusplus

}
#endif

# define rt_CreateIntegrationData(S) \
    rt_ODECreateIntegrationData(rtmGetRTWSolverInfo(S));
# define rt_UpdateContinuousStates(S) \
    rt_ODEUpdateContinuousStates(rtmGetRTWSolverInfo(S));
# else
# define rt_CreateIntegrationData(S)  \
      rtsiSetSolverName(rtmGetRTWSolverInfo(S),"FixedStepDiscrete");
# define rt_UpdateContinuousStates(S) /* Do Nothing */
#endif


/*==================================*
 * Global data local to this module *
 *==================================*/

static struct {
    int_T    stopExecutionFlag;
    int_T    isrOverrun;
    int_T    overrunFlags[NUMST];
    int_T    eventFlags[NUMST];
    const    char_T* errmsg;
} GBLbuf;


#ifdef EXT_MODE
#  define rtExtModeSingleTaskUpload(S)                          \
   {                                                            \
        int stIdx;                                              \
        rtExtModeUploadCheckTrigger(rtmGetNumSampleTimes(S));   \
        for (stIdx=0; stIdx<NUMST; stIdx++) {                   \
            if (rtmIsSampleHit(S, stIdx, 0 /*unused*/)) {       \
                rtExtModeUpload(stIdx,rtmGetTaskTime(S,stIdx)); \
            }                                                   \
        }                                                       \
   }
#else
#  define rtExtModeSingleTaskUpload(S) /* Do nothing */
#endif

/*=================*
 * Local functions *
 *=================*/
 /* Function: initiateController ===========================================
  *
  * Abstract:
  *      Initialize the controller of the compiled Matlab Simulink block.
  */


static RT_MODEL* S;
int initiateController(char* errorMsg) {
    (void)errorMsg;
    const char* status;

    /****************************
     * Initialize global memory *
     ****************************/
    (void)memset(&GBLbuf, 0, sizeof(GBLbuf));

    /************************
     * Initialize the model *
     ************************/

    S = MODEL();
    if (rtmGetErrorStatus(S) != NULL) {
        (void)fprintf(stderr, "Error during model registration: %s\n",
            rtmGetErrorStatus(S));
        exit(EXIT_FAILURE);
    }
    rtmSetTFinal(S, RUN_FOREVER);

    MdlInitializeSizes();
    MdlInitializeSampleTimes();

    status = rt_SimInitTimingEngine(rtmGetNumSampleTimes(S),
        rtmGetStepSize(S),
        rtmGetSampleTimePtr(S),
        rtmGetOffsetTimePtr(S),
        rtmGetSampleHitPtr(S),
        rtmGetSampleTimeTaskIDPtr(S),
        rtmGetTStart(S),
        &rtmGetSimTimeStep(S),
        &rtmGetTimingData(S));

    if (status != NULL) {
        (void)fprintf(stderr,
            "Failed to initialize sample time engine: %s\n", status);
        exit(EXIT_FAILURE);
    }
    rt_CreateIntegrationData(S);

#ifdef UseMMIDataLogging
    rt_FillStateSigInfoFromMMI(rtmGetRTWLogInfo(S), &rtmGetErrorStatus(S));
#endif
    GBLbuf.errmsg = rt_StartDataLogging(rtmGetRTWLogInfo(S),
        rtmGetTFinal(S),
        rtmGetStepSize(S),
        &rtmGetErrorStatus(S));
    if (GBLbuf.errmsg != NULL) {
        (void)fprintf(stderr, "Error starting data logging: %s\n", GBLbuf.errmsg);
        return(EXIT_FAILURE);
    }

    rtExtModeCheckInit(rtmGetNumSampleTimes(S));
    rtExtModeWaitForStartPkt(rtmGetRTWExtModeInfo(S),
        rtmGetNumSampleTimes(S),
        (boolean_T*)&rtmGetStopRequested(S));

    (void)printf("\n** Starting UNICO controller **\n");

    MdlStart();
    if (rtmGetErrorStatus(S) != NULL) {
        GBLbuf.stopExecutionFlag = 1;
    }

    return 0;
}  /* end initiateController */

#if !defined(MULTITASKING)  /* SINGLETASKING */
int calcOutputController(
    float rUserVar1, float rUserVar2, float rUserVar3, float rUserVar4, float rUserVar5,
    float rUserVar6, float rUserVar7, float rUserVar8, float rUserVar9, float rUserVar10,
    float rUserVar11, float rUserVar12, float rUserVar13, float rUserVar14, float rUserVar15,
    float rUserVar16, float rUserVar17, float rUserVar18, float rUserVar19, float rUserVar20,
    float rUserVar21, float rUserVar22, float rUserVar23, float rUserVar24, float rUserVar25,
    float rUserVar26, float rUserVar27, float rUserVar28, float rUserVar29, float rUserVar30,
    float rUserVar31, float rUserVar32, float rUserVar33, float rUserVar34, float rUserVar35,
    float rUserVar36, float rUserVar37,
    float rUserVar38, float rUserVar39, float rUserVar40,
    float rUserVar41, float rUserVar42, float rUserVar43, float rUserVar44, float rUserVar45,
    float rUserVar46, float rUserVar47,
    float rUserVar48, float rUserVar49, float rUserVar50, float rUserVar51, float rUserVar52,
    float rUserVar53, float rUserVar54, float rUserVar55,
    float rUserVar56, float rUserVar57,
    float rUserVar58, float rUserVar59, float rUserVar60, float rUserVar61, float rUserVar62,
    float rUserVar63, float rUserVar64,
    float rUserVar65, float rUserVar66, float rUserVar67, float rUserVar68, float rUserVar69,
    float rUserVar70, float rUserVar71, float rUserVar72, float rUserVar73, float rUserVar74,
    float rUserVar75,
    float rUserVar76, float rUserVar77, float rUserVar78,
    float rUserVar79, float rUserVar80,
    float rUserVar81,
    float rTime, float rInit, float rSample, float rGeneratorSpeed, float rWindSpeed, float rRatedSpeed,
    float rBelowRatedPitch, float rForeAftTower, float rSideTower,
    float rRotorAzimuth, float rOP1RootMoment, float rOP2RootMoment,
    float rOP3RootMoment, float rIP1RootMoment, float rIP2RootMoment,
    float rIP3RootMoment, float rMeasuredPitch, float rMeasuredTorque, float rShaftTorque,
    float rModeGain, float rYawError, float rYawBearingRate, float rElectricalPower,
    float* rTorqueDemand, float* rBlade1Pitch,
    float* rBlade2Pitch, float* rBlade3Pitch, float* rPitchDemand, char* errorMsg, float* rYawRate,
    float* rLog1, float* rLog2, float* rLog3, float* rLog4, float* rLog5,
    float* rLog6, float* rLog7, float* rLog8, float* rLog9, float* rLog10,
    float* rLog11, float* rLog12, float* rLog13, float* rLog14, float* rLog15,
    float* rLog16, float* rLog17, float* rLog18, float* rLog19, float* rLog20) {
    real_T tnext;

    SIG_MODEL(U, SimTime) = rTime;
    SIG_MODEL(U, Generator_Speed) = rGeneratorSpeed;
    SIG_MODEL(U, Wind_Speed) = rWindSpeed;
    SIG_MODEL(U, Com_dt) = rSample;
    SIG_MODEL(U, Rated_Speed) = rRatedSpeed;
    SIG_MODEL(U, Below_Rated_Pitch_Angle) = rBelowRatedPitch;
    SIG_MODEL(U, Fore_Aft_Tower_Accel) = rForeAftTower;
    SIG_MODEL(U, Sidewards_Tower_Accel) = rSideTower;
    SIG_MODEL(U, Measured_Pitch) = rMeasuredPitch;
    SIG_MODEL(U, Measured_Torque) = rMeasuredTorque;
    SIG_MODEL(U, Shaft_Torque) = rShaftTorque;
    SIG_MODEL(U, Mode_Gain) = rModeGain;
    SIG_MODEL(U, Rotor_Azimuth_Angle) = rRotorAzimuth;
    SIG_MODEL(U, Blade1_OP_Root_Moment) = rOP1RootMoment;
    SIG_MODEL(U, Blade2_OP_Root_Moment) = rOP2RootMoment;
    SIG_MODEL(U, Blade3_OP_Root_Moment) = rOP3RootMoment;
    SIG_MODEL(U, Blade1_IP_Root_Moment) = rIP1RootMoment;
    SIG_MODEL(U, Blade2_IP_Root_Moment) = rIP2RootMoment;
    SIG_MODEL(U, Blade3_IP_Root_Moment) = rIP3RootMoment;

    SIG_MODEL(U, Init) = rInit;
    SIG_MODEL(U, dEcho) = rUserVar1;
    SIG_MODEL(U, dRotorRadius) = rUserVar2;
    SIG_MODEL(U, dRtWSpd) = rUserVar3;
    SIG_MODEL(U, dRtTq) = rUserVar4;
    SIG_MODEL(U, dRtPwr) = rUserVar5;
    SIG_MODEL(U, dRtGenSpd) = rUserVar6;
    SIG_MODEL(U, dGenEff) = rUserVar7;
    SIG_MODEL(U, dMinTq) = rUserVar8;
    SIG_MODEL(U, dMaxTq) = rUserVar9;
    SIG_MODEL(U, dMaxRat) = rUserVar10;
    SIG_MODEL(U, dMinOMSpd) = rUserVar11;
    SIG_MODEL(U, dRgn2Mode) = rUserVar12;
    SIG_MODEL(U, dKSwitch) = rUserVar13;
    SIG_MODEL(U, dRgn2K) = rUserVar14;
    SIG_MODEL(U, dOMSpdSwitch) = rUserVar15;
    SIG_MODEL(U, dMinOMSpdSwitch) = rUserVar16;
    SIG_MODEL(U, dTSRopt) = rUserVar17;
    SIG_MODEL(U, dCpMax) = rUserVar18;
    SIG_MODEL(U, dRgn3Mode) = rUserVar19;
    SIG_MODEL(U, dMRgn3Lin) = rUserVar20;
    SIG_MODEL(U, dVLin) = rUserVar21;
    SIG_MODEL(U, dWindSpeedFSw) = rUserVar22;
    SIG_MODEL(U, dWindSpeedFLPF) = rUserVar23;
    SIG_MODEL(U, dGenSpeedFSw) = rUserVar24;
    SIG_MODEL(U, dGenSpeedFLPF) = rUserVar25;
    SIG_MODEL(U, dRefSpeedFSw) = rUserVar26;
    SIG_MODEL(U, dRefSpeedFLPF) = rUserVar27;
    SIG_MODEL(U, dRhoAir) = rUserVar28;
    SIG_MODEL(U, dFPitch) = rUserVar29;
    SIG_MODEL(U, Architecture_Selector) = rUserVar30;
    SIG_MODEL(U, dZeros_Damping) = rUserVar31;
    SIG_MODEL(U, dPoles_Damping) = rUserVar32;
    SIG_MODEL(U, dBlades_Number) = rUserVar33;
    SIG_MODEL(U, dBlades_Height) = rUserVar34;
    SIG_MODEL(U, dIntTermSwitch) = rUserVar35;
    SIG_MODEL(U, dRotInertia) = rUserVar36;
    SIG_MODEL(U, dAWSwitch) = rUserVar37;
    /* --- NUOVI PARAMETRI --- */
    SIG_MODEL(U, Darrieus_Selector) = rUserVar38;
    SIG_MODEL(U, Rotor_Area) = rUserVar39;
    SIG_MODEL(U, GearBox_Ratio) = rUserVar40;
    SIG_MODEL(U, Pitch_LPF_Sw) = rUserVar41;
    SIG_MODEL(U, Pitch_F_LPF) = rUserVar42;
    SIG_MODEL(U, Torque_in_LPF_Sw) = rUserVar43;
    SIG_MODEL(U, Torque_in_F_LPF)  = rUserVar44;
    SIG_MODEL(U, Power_LPF_Sw) = rUserVar45;
    SIG_MODEL(U, Power_F_LPF) = rUserVar46;
    SIG_MODEL(U, LPF_StartingTime) = rUserVar47;
    SIG_MODEL(U, SPS_Mode) = rUserVar48;
    SIG_MODEL(U, SPS_Kvs) = rUserVar49;
    SIG_MODEL(U, SPS_Kpc) = rUserVar50;
    SIG_MODEL(U, SPS_Abbas_Factor) = rUserVar51;
    SIG_MODEL(U, PS_F_LPF) = rUserVar52;
    SIG_MODEL(U, PS_LPF_Sw) = rUserVar53;
    SIG_MODEL(U, SPS_LPF_Sw) = rUserVar54;
    SIG_MODEL(U, SPS_F_LPF) = rUserVar55;
    SIG_MODEL(U, Pitch_Rate) = rUserVar56;
    SIG_MODEL(U, Max_Pitch) = rUserVar57;
    SIG_MODEL(U, WSE_Mode) = rUserVar58;
    SIG_MODEL(U, Process_Noise_Q) = rUserVar59;
    SIG_MODEL(U, Measurement_Noise_R) = rUserVar60;
    SIG_MODEL(U, TBE_F_LPF) = rUserVar61;
    SIG_MODEL(U, EKF_F_LPF) = rUserVar62;
    SIG_MODEL(U, Rm) = rUserVar63;
    SIG_MODEL(U, Ti) = rUserVar64;
    SIG_MODEL(U, WindSpeed_PreWarp) = rUserVar65;
    SIG_MODEL(U, GenSpeed_PreWarp)  = rUserVar66;
    SIG_MODEL(U, RefSpeed_PreWarp)  = rUserVar67;
    SIG_MODEL(U, Pitch_PreWarp)     = rUserVar68;
    SIG_MODEL(U, Torque_in_PreWarp) = rUserVar69;
    SIG_MODEL(U, Torque_out_LPF_Sw) = rUserVar76;
    SIG_MODEL(U, Torque_out_F_LPF)  = rUserVar77;
    SIG_MODEL(U, Torque_out_PreWarp) = rUserVar78;
    SIG_MODEL(U, Power_PreWarp)     = rUserVar70;
    SIG_MODEL(U, PS_PreWarp)        = rUserVar71;
    SIG_MODEL(U, SPS_PreWarp)       = rUserVar72;
    SIG_MODEL(U, TBE_PreWarp)       = rUserVar73;
    SIG_MODEL(U, EKF_PreWarp)       = rUserVar74;
    SIG_MODEL(U, Pitch_Ctrl_Sw)     = rUserVar75;
    SIG_MODEL(U, NF_StartingTime)  = rUserVar79;
    SIG_MODEL(U, NF_Switch)        = rUserVar80;
    SIG_MODEL(U, omega_stallmin)   = rUserVar81;

    /* --- TORQUE CONTROLLER GAINS (interpolated from LUTs) --- */
    SIG_MODEL(U, dKpBr) = interpolateLUT(ctrlLUTs.GS_KpBr, ctrlLUTs.KpBr, ctrlLUTs.nKpBr, rGeneratorSpeed);
    SIG_MODEL(U, dKiBr) = interpolateLUT(ctrlLUTs.GS_KiBr, ctrlLUTs.KiBr, ctrlLUTs.nKiBr, rGeneratorSpeed);
    SIG_MODEL(U, dKpAr) = interpolateLUT(ctrlLUTs.GS_KpAr, ctrlLUTs.KpAr, ctrlLUTs.nKpAr, rGeneratorSpeed);
    SIG_MODEL(U, dKiAr) = interpolateLUT(ctrlLUTs.GS_KiAr, ctrlLUTs.KiAr, ctrlLUTs.nKiAr, rGeneratorSpeed);

    /* --- PITCH PI GAINS (interpolated on measured pitch) --- */
    SIG_MODEL(U, PC_Kp) = interpolateLUT(ctrlLUTs.PC_GS_angles, ctrlLUTs.PC_GS_Kp, ctrlLUTs.nPC_GS, rMeasuredPitch);
    SIG_MODEL(U, PC_Ki) = interpolateLUT(ctrlLUTs.PC_GS_angles, ctrlLUTs.PC_GS_Ki, ctrlLUTs.nPC_GS, rMeasuredPitch);

    /* --- PEAK SHAVING (interpolated on wind speed) --- */
    SIG_MODEL(U, Min_Pitch) = interpolateLUT(ctrlLUTs.PS_WindSpeeds, ctrlLUTs.PS_BldPitchMin, ctrlLUTs.nPS, rWindSpeed);

    /* --- EKF SYSTEM POLES (interpolated on vh from model) --- */
    SIG_MODEL(U, A_op) = interpolateLUT(ctrlLUTs.WE_FOPoles_v, ctrlLUTs.WE_FOPoles, ctrlLUTs.nWE_FOPoles, SIG_MODEL(Y, vh));

    /* --- CP 2D INTERPOLATION (for TBE/EKF, using vh and vw from model) --- */
    {
        float vh_ekf = SIG_MODEL(Y, vh);
        float vw_ekf = SIG_MODEL(Y, vw);
        float pitch_deg = rMeasuredPitch * 180.0f / 3.14159265f;
        float lambda_opt = 0.0f;
        float lambda_tau = 0.0f;
        if (vh_ekf > 1.0f)
            lambda_opt = rGeneratorSpeed * rUserVar2 / vh_ekf;
        if (vw_ekf > 1.0f)
            lambda_tau = rGeneratorSpeed * rUserVar2 / vw_ekf;

        SIG_MODEL(U, Cp_opt_in) = interpolate2D(
            ctrlLUTs.Cp_v_pitch, ctrlLUTs.Cp_nPitch,
            ctrlLUTs.Cp_v_TSR, ctrlLUTs.Cp_nTSR,
            ctrlLUTs.Cp_matrix,
            pitch_deg, lambda_opt);

        SIG_MODEL(U, Cp_2_in) = interpolate2D(
            ctrlLUTs.Cp_v_pitch, ctrlLUTs.Cp_nPitch,
            ctrlLUTs.Cp_v_TSR, ctrlLUTs.Cp_nTSR,
            ctrlLUTs.Cp_matrix,
            pitch_deg, lambda_tau);

        /* --- Re-derive the Cp(TSR) inversion curve at the CURRENT pitch ---
         * With the dynamic beta(U) LUT the pitch changes with wind speed, so
         * this can no longer be computed once at dFPitch during init: it must
         * track pitch_deg every step, otherwise dTSR_R3 (-> omega_ref in
         * Region 3) is derived from the wrong pitch as soon as beta moves
         * away from its initial value. */
        ExtractCpTSR_from_matrix(pitch_deg);
    }

    /* --- CP/TSR 1D for Region 3: invert the Cp computed by WT_Controller (1-step delayed) --- */
    SIG_MODEL(U, dTSR_R3) = interpolateLUT(ctrlLUTs.Cp_lut, ctrlLUTs.TSR_lut,
        ctrlLUTs.nCpTSR, SIG_MODEL(Y, Cp));

    SIG_MODEL(U, YawError) = rYawError;
    SIG_MODEL(U, YawBearingRate) = rYawBearingRate;
    SIG_MODEL(U, ElectricalPower) = rElectricalPower;

    /***********************************************
     * Check and see if base step time is too fast *
     ***********************************************/
    if (GBLbuf.isrOverrun++) {
        GBLbuf.stopExecutionFlag = 1;
        return EXIT_FAILURE;
    }
    /***********************************************
     * Check and see if error status has been set  *
     ***********************************************/
    if (rtmGetErrorStatus(S) != NULL) {
        GBLbuf.stopExecutionFlag = 1;
        return EXIT_FAILURE;
    }
    /* enable interrupts here */
    tnext = rt_SimGetNextSampleHit();
    rtsiSetSolverStopTime(rtmGetRTWSolverInfo(S), tnext);
    MdlOutputs(0);
    rtExtModeSingleTaskUpload(S);
    GBLbuf.errmsg = rt_UpdateTXYLogVars(rtmGetRTWLogInfo(S), rtmGetTPtr(S));
    if (GBLbuf.errmsg != NULL) {
        GBLbuf.stopExecutionFlag = 1;
        return EXIT_FAILURE;
    }
    MdlUpdate(0);
    rt_SimUpdateDiscreteTaskSampleHits(rtmGetNumSampleTimes(S),
        rtmGetTimingData(S), rtmGetSampleHitPtr(S), rtmGetTPtr(S));
    if (rtmGetSampleTime(S, 0) == CONTINUOUS_SAMPLE_TIME) {
        rt_UpdateContinuousStates(S);
    }
    GBLbuf.isrOverrun--;
    rtExtModeCheckEndTrigger();

    rTorqueDemand[0] = SIG_MODEL(Y, Generator_Torque);
    rBlade1Pitch[0] = SIG_MODEL(Y, Blade1_Pitch_Angle);
    rBlade2Pitch[0] = SIG_MODEL(Y, Blade2_Pitch_Angle);
    rBlade3Pitch[0] = SIG_MODEL(Y, Blade3_Pitch_Angle);
    rPitchDemand[0] = SIG_MODEL(Y, Collective_Pitch_Angle);
    rYawRate[0] = SIG_MODEL(Y, Yaw_Rate);
    rLog1[0] = SIG_MODEL(Y, Log1);
    rLog2[0] = SIG_MODEL(Y, Log2);
    rLog3[0] = SIG_MODEL(Y, Log3);
    rLog4[0] = SIG_MODEL(Y, Log4);
    rLog5[0] = SIG_MODEL(Y, Log5);
    rLog6[0] = SIG_MODEL(Y, Log6);
    rLog7[0] = SIG_MODEL(Y, Log7);
    rLog8[0] = SIG_MODEL(Y, Log8);
    rLog9[0] = SIG_MODEL(Y, Log9);
    rLog10[0] = SIG_MODEL(Y, Log10);
    rLog11[0] = SIG_MODEL(Y, Log11);
    rLog12[0] = SIG_MODEL(Y, Log12);
    rLog13[0] = SIG_MODEL(Y, Log13);
    rLog14[0] = SIG_MODEL(Y, Log14);
    rLog15[0] = SIG_MODEL(Y, Log15);
    rLog16[0] = SIG_MODEL(Y, Log16);
    rLog17[0] = SIG_MODEL(Y, Log17);
    rLog18[0] = SIG_MODEL(Y, Log18);
    rLog19[0] = SIG_MODEL(Y, Log19);
    rLog20[0] = SIG_MODEL(Y, Log20);
    return 0;
}  /* end calcOutputController SINGLETASKING */

#else /* MULTITASKING */
# if TID01EQ == 1
#  define FIRST_TID 1
# else
#  define FIRST_TID 0
# endif

int calcOutputController(
    float rUserVar1, float rUserVar2, float rUserVar3, float rUserVar4, float rUserVar5,
    float rUserVar6, float rUserVar7, float rUserVar8, float rUserVar9, float rUserVar10,
    float rUserVar11, float rUserVar12, float rUserVar13, float rUserVar14, float rUserVar15,
    float rUserVar16, float rUserVar17, float rUserVar18, float rUserVar19, float rUserVar20,
    float rUserVar21, float rUserVar22, float rUserVar23, float rUserVar24, float rUserVar25,
    float rUserVar26, float rUserVar27, float rUserVar28, float rUserVar29, float rUserVar30,
    float rUserVar31, float rUserVar32, float rUserVar33, float rUserVar34, float rUserVar35,
    float rUserVar36, float rUserVar37,
    float rUserVar38, float rUserVar39, float rUserVar40,
    float rUserVar41, float rUserVar42, float rUserVar43, float rUserVar44, float rUserVar45,
    float rUserVar46, float rUserVar47,
    float rUserVar48, float rUserVar49, float rUserVar50, float rUserVar51, float rUserVar52,
    float rUserVar53, float rUserVar54, float rUserVar55,
    float rUserVar56, float rUserVar57,
    float rUserVar58, float rUserVar59, float rUserVar60, float rUserVar61, float rUserVar62,
    float rUserVar63, float rUserVar64,
    float rUserVar65, float rUserVar66, float rUserVar67, float rUserVar68, float rUserVar69,
    float rUserVar70, float rUserVar71, float rUserVar72, float rUserVar73, float rUserVar74,
    float rUserVar75,
    float rUserVar76, float rUserVar77, float rUserVar78,
    float rUserVar79, float rUserVar80,
    float rUserVar81,
    float rTime, float rInit, float rSample, float rGeneratorSpeed, float rWindSpeed, float rRatedSpeed,
    float rBelowRatedPitch, float rForeAftTower, float rSideTower,
    float rRotorAzimuth, float rOP1RootMoment, float rOP2RootMoment,
    float rOP3RootMoment, float rIP1RootMoment, float rIP2RootMoment,
    float rIP3RootMoment, float rMeasuredPitch, float rMeasuredTorque, float rShaftTorque,
    float rModeGain, float rYawError, float rYawBearingRate, float rElectricalPower,
    float* rTorqueDemand, float* rBlade1Pitch,
    float* rBlade2Pitch, float* rBlade3Pitch, float* rPitchDemand, char* errorMsg, float* rYawRate,
    float* rLog1, float* rLog2, float* rLog3, float* rLog4, float* rLog5,
    float* rLog6, float* rLog7, float* rLog8, float* rLog9, float* rLog10,
    float* rLog11, float* rLog12, float* rLog13, float* rLog14, float* rLog15,
    float* rLog16, float* rLog17, float* rLog18, float* rLog19, float* rLog20) {
    int_T  i;
    real_T tnext;
    int_T* sampleHit = rtmGetSampleHitPtr(S);

    SIG_MODEL(U, SimTime) = rTime;
    SIG_MODEL(U, Generator_Speed) = rGeneratorSpeed;
    SIG_MODEL(U, Wind_Speed) = rWindSpeed;
    SIG_MODEL(U, Com_dt) = rSample;
    SIG_MODEL(U, Rated_Speed) = rRatedSpeed;
    SIG_MODEL(U, Below_Rated_Pitch_Angle) = rBelowRatedPitch;
    SIG_MODEL(U, Fore_Aft_Tower_Accel) = rForeAftTower;
    SIG_MODEL(U, Sidewards_Tower_Accel) = rSideTower;
    SIG_MODEL(U, Measured_Pitch) = rMeasuredPitch;
    SIG_MODEL(U, Measured_Torque) = rMeasuredTorque;
    SIG_MODEL(U, Shaft_Torque) = rShaftTorque;
    SIG_MODEL(U, Mode_Gain) = rModeGain;
    SIG_MODEL(U, Rotor_Azimuth_Angle) = rRotorAzimuth;
    SIG_MODEL(U, Blade1_OP_Root_Moment) = rOP1RootMoment;
    SIG_MODEL(U, Blade2_OP_Root_Moment) = rOP2RootMoment;
    SIG_MODEL(U, Blade3_OP_Root_Moment) = rOP3RootMoment;
    SIG_MODEL(U, Blade1_IP_Root_Moment) = rIP1RootMoment;
    SIG_MODEL(U, Blade2_IP_Root_Moment) = rIP2RootMoment;
    SIG_MODEL(U, Blade3_IP_Root_Moment) = rIP3RootMoment;

    SIG_MODEL(U, Init) = rInit;
    SIG_MODEL(U, dEcho) = rUserVar1;
    SIG_MODEL(U, dRotorRadius) = rUserVar2;
    SIG_MODEL(U, dRtWSpd) = rUserVar3;
    SIG_MODEL(U, dRtTq) = rUserVar4;
    SIG_MODEL(U, dRtPwr) = rUserVar5;
    SIG_MODEL(U, dRtGenSpd) = rUserVar6;
    SIG_MODEL(U, dGenEff) = rUserVar7;
    SIG_MODEL(U, dMinTq) = rUserVar8;
    SIG_MODEL(U, dMaxTq) = rUserVar9;
    SIG_MODEL(U, dMaxRat) = rUserVar10;
    SIG_MODEL(U, dMinOMSpd) = rUserVar11;
    SIG_MODEL(U, dRgn2Mode) = rUserVar12;
    SIG_MODEL(U, dKSwitch) = rUserVar13;
    SIG_MODEL(U, dRgn2K) = rUserVar14;
    SIG_MODEL(U, dOMSpdSwitch) = rUserVar15;
    SIG_MODEL(U, dMinOMSpdSwitch) = rUserVar16;
    SIG_MODEL(U, dTSRopt) = rUserVar17;
    SIG_MODEL(U, dCpMax) = rUserVar18;
    SIG_MODEL(U, dRgn3Mode) = rUserVar19;
    SIG_MODEL(U, dMRgn3Lin) = rUserVar20;
    SIG_MODEL(U, dVLin) = rUserVar21;
    SIG_MODEL(U, dWindSpeedFSw) = rUserVar22;
    SIG_MODEL(U, dWindSpeedFLPF) = rUserVar23;
    SIG_MODEL(U, dGenSpeedFSw) = rUserVar24;
    SIG_MODEL(U, dGenSpeedFLPF) = rUserVar25;
    SIG_MODEL(U, dRefSpeedFSw) = rUserVar26;
    SIG_MODEL(U, dRefSpeedFLPF) = rUserVar27;
    SIG_MODEL(U, dRhoAir) = rUserVar28;
    SIG_MODEL(U, dFPitch) = rUserVar29;
    SIG_MODEL(U, Architecture_Selector) = rUserVar30;
    SIG_MODEL(U, dZeros_Damping) = rUserVar31;
    SIG_MODEL(U, dPoles_Damping) = rUserVar32;
    SIG_MODEL(U, dBlades_Number) = rUserVar33;
    SIG_MODEL(U, dBlades_Height) = rUserVar34;
    SIG_MODEL(U, dIntTermSwitch) = rUserVar35;
    SIG_MODEL(U, dRotInertia) = rUserVar36;
    SIG_MODEL(U, dAWSwitch) = rUserVar37;
    /* --- NUOVI PARAMETRI --- */
    SIG_MODEL(U, Darrieus_Selector) = rUserVar38;
    SIG_MODEL(U, Rotor_Area) = rUserVar39;
    SIG_MODEL(U, GearBox_Ratio) = rUserVar40;
    SIG_MODEL(U, Pitch_LPF_Sw) = rUserVar41;
    SIG_MODEL(U, Pitch_F_LPF) = rUserVar42;
    SIG_MODEL(U, Torque_in_LPF_Sw) = rUserVar43;
    SIG_MODEL(U, Torque_in_F_LPF)  = rUserVar44;
    SIG_MODEL(U, Power_LPF_Sw) = rUserVar45;
    SIG_MODEL(U, Power_F_LPF) = rUserVar46;
    SIG_MODEL(U, LPF_StartingTime) = rUserVar47;
    SIG_MODEL(U, SPS_Mode) = rUserVar48;
    SIG_MODEL(U, SPS_Kvs) = rUserVar49;
    SIG_MODEL(U, SPS_Kpc) = rUserVar50;
    SIG_MODEL(U, SPS_Abbas_Factor) = rUserVar51;
    SIG_MODEL(U, PS_F_LPF) = rUserVar52;
    SIG_MODEL(U, PS_LPF_Sw) = rUserVar53;
    SIG_MODEL(U, SPS_LPF_Sw) = rUserVar54;
    SIG_MODEL(U, SPS_F_LPF) = rUserVar55;
    SIG_MODEL(U, Pitch_Rate) = rUserVar56;
    SIG_MODEL(U, Max_Pitch) = rUserVar57;
    SIG_MODEL(U, WSE_Mode) = rUserVar58;
    SIG_MODEL(U, Process_Noise_Q) = rUserVar59;
    SIG_MODEL(U, Measurement_Noise_R) = rUserVar60;
    SIG_MODEL(U, TBE_F_LPF) = rUserVar61;
    SIG_MODEL(U, EKF_F_LPF) = rUserVar62;
    SIG_MODEL(U, Rm) = rUserVar63;
    SIG_MODEL(U, Ti) = rUserVar64;
    SIG_MODEL(U, WindSpeed_PreWarp) = rUserVar65;
    SIG_MODEL(U, GenSpeed_PreWarp)  = rUserVar66;
    SIG_MODEL(U, RefSpeed_PreWarp)  = rUserVar67;
    SIG_MODEL(U, Pitch_PreWarp)     = rUserVar68;
    SIG_MODEL(U, Torque_in_PreWarp) = rUserVar69;
    SIG_MODEL(U, Torque_out_LPF_Sw) = rUserVar76;
    SIG_MODEL(U, Torque_out_F_LPF)  = rUserVar77;
    SIG_MODEL(U, Torque_out_PreWarp) = rUserVar78;
    SIG_MODEL(U, Power_PreWarp)     = rUserVar70;
    SIG_MODEL(U, PS_PreWarp)        = rUserVar71;
    SIG_MODEL(U, SPS_PreWarp)       = rUserVar72;
    SIG_MODEL(U, TBE_PreWarp)       = rUserVar73;
    SIG_MODEL(U, EKF_PreWarp)       = rUserVar74;
    SIG_MODEL(U, Pitch_Ctrl_Sw)     = rUserVar75;
    SIG_MODEL(U, NF_StartingTime)  = rUserVar79;
    SIG_MODEL(U, NF_Switch)        = rUserVar80;
    SIG_MODEL(U, omega_stallmin)   = rUserVar81;

    /* --- TORQUE CONTROLLER GAINS (interpolated from LUTs) --- */
    SIG_MODEL(U, dKpBr) = interpolateLUT(ctrlLUTs.GS_KpBr, ctrlLUTs.KpBr, ctrlLUTs.nKpBr, rGeneratorSpeed);
    SIG_MODEL(U, dKiBr) = interpolateLUT(ctrlLUTs.GS_KiBr, ctrlLUTs.KiBr, ctrlLUTs.nKiBr, rGeneratorSpeed);
    SIG_MODEL(U, dKpAr) = interpolateLUT(ctrlLUTs.GS_KpAr, ctrlLUTs.KpAr, ctrlLUTs.nKpAr, rGeneratorSpeed);
    SIG_MODEL(U, dKiAr) = interpolateLUT(ctrlLUTs.GS_KiAr, ctrlLUTs.KiAr, ctrlLUTs.nKiAr, rGeneratorSpeed);

    /* --- PITCH PI GAINS (interpolated on measured pitch) --- */
    SIG_MODEL(U, PC_Kp) = interpolateLUT(ctrlLUTs.PC_GS_angles, ctrlLUTs.PC_GS_Kp, ctrlLUTs.nPC_GS, rMeasuredPitch);
    SIG_MODEL(U, PC_Ki) = interpolateLUT(ctrlLUTs.PC_GS_angles, ctrlLUTs.PC_GS_Ki, ctrlLUTs.nPC_GS, rMeasuredPitch);

    /* --- PEAK SHAVING (interpolated on wind speed) --- */
    SIG_MODEL(U, Min_Pitch) = interpolateLUT(ctrlLUTs.PS_WindSpeeds, ctrlLUTs.PS_BldPitchMin, ctrlLUTs.nPS, rWindSpeed);

    /* --- EKF SYSTEM POLES (interpolated on vh from model) --- */
    SIG_MODEL(U, A_op) = interpolateLUT(ctrlLUTs.WE_FOPoles_v, ctrlLUTs.WE_FOPoles, ctrlLUTs.nWE_FOPoles, SIG_MODEL(Y, vh));

    /* --- CP 2D INTERPOLATION (for TBE/EKF, using vh and vw from model) --- */
    {
        float vh_ekf = SIG_MODEL(Y, vh);
        float vw_ekf = SIG_MODEL(Y, vw);
        float pitch_deg = rMeasuredPitch * 180.0f / 3.14159265f;
        float lambda_opt = 0.0f;
        float lambda_tau = 0.0f;
        if (vh_ekf > 1.0f)
            lambda_opt = rGeneratorSpeed * rUserVar2 / vh_ekf;
        if (vw_ekf > 1.0f)
            lambda_tau = rGeneratorSpeed * rUserVar2 / vw_ekf;

        SIG_MODEL(U, Cp_opt_in) = interpolate2D(
            ctrlLUTs.Cp_v_pitch, ctrlLUTs.Cp_nPitch,
            ctrlLUTs.Cp_v_TSR, ctrlLUTs.Cp_nTSR,
            ctrlLUTs.Cp_matrix,
            pitch_deg, lambda_opt);

        SIG_MODEL(U, Cp_2_in) = interpolate2D(
            ctrlLUTs.Cp_v_pitch, ctrlLUTs.Cp_nPitch,
            ctrlLUTs.Cp_v_TSR, ctrlLUTs.Cp_nTSR,
            ctrlLUTs.Cp_matrix,
            pitch_deg, lambda_tau);

        /* --- Re-derive the Cp(TSR) inversion curve at the CURRENT pitch ---
         * See note in the SINGLETASKING branch above: with the dynamic
         * beta(U) LUT, pitch_deg changes every step, so this curve must be
         * refreshed here too instead of relying on the static init-time one. */
        ExtractCpTSR_from_matrix(pitch_deg);
    }

    /* --- CP/TSR 1D for Region 3: invert the Cp computed by WT_Controller (1-step delayed) --- */
    SIG_MODEL(U, dTSR_R3) = interpolateLUT(ctrlLUTs.Cp_lut, ctrlLUTs.TSR_lut,
        ctrlLUTs.nCpTSR, SIG_MODEL(Y, Cp));

    /***********************************************
     * Check and see if error status has been set  *
     ***********************************************/
    if (rtmGetErrorStatus(S) != NULL) {
        GBLbuf.stopExecutionFlag = 1;
        return EXIT_FAILURE;
    }
    /* enable interrupts here */

    /***********************************************
     * Update discrete events                      *
     ***********************************************/
    tnext = rt_SimUpdateDiscreteEvents(rtmGetNumSampleTimes(S),
        rtmGetTimingData(S),
        rtmGetSampleHitPtr(S),
        rtmGetPerTaskSampleHitsPtr(S));
    rtsiSetSolverStopTime(rtmGetRTWSolverInfo(S), tnext);
    for (i = FIRST_TID + 1; i < NUMST; i++) {
        if (sampleHit[i] && GBLbuf.eventFlags[i]++) {
            GBLbuf.isrOverrun--;
            GBLbuf.overrunFlags[i]++;    /* Are we sampling too fast for */
            GBLbuf.stopExecutionFlag = 1;  /*   sample time "i"?           */
            return EXIT_FAILURE;
        }
    }
    /*******************************************
     * Step the model for the base sample time *
     *******************************************/
    MdlOutputs(FIRST_TID);

    rtExtModeUploadCheckTrigger(rtmGetNumSampleTimes(S));
    rtExtModeUpload(FIRST_TID, rtmGetTaskTime(S, FIRST_TID));

    GBLbuf.errmsg = rt_UpdateTXYLogVars(rtmGetRTWLogInfo(S),
        rtmGetTPtr(S));
    if (GBLbuf.errmsg != NULL) {
        GBLbuf.stopExecutionFlag = 1;
        return EXIT_FAILURE;
    }

    MdlUpdate(FIRST_TID);

    if (rtmGetSampleTime(S, 0) == CONTINUOUS_SAMPLE_TIME) {
        rt_UpdateContinuousStates(S);
    }
    else {
        rt_SimUpdateDiscreteTaskTime(rtmGetTPtr(S),
            rtmGetTimingData(S), 0);
    }

#if FIRST_TID == 1
    rt_SimUpdateDiscreteTaskTime(rtmGetTPtr(S),
        rtmGetTimingData(S), 1);
#endif

    /************************************************************************
      * Model step complete for base sample time, now it is okay to          *
      * re-interrupt this ISR.                                               *
      ************************************************************************/
    GBLbuf.isrOverrun--;
    /*********************************************
     * Step the model for any other sample times *
     *********************************************/
    for (i = FIRST_TID + 1; i < NUMST; i++) {
        /* If task "i" is running, don't run any lower priority task */
        if (GBLbuf.overrunFlags[i]) return EXIT_FAILURE;
        if (GBLbuf.eventFlags[i]) {
            GBLbuf.overrunFlags[i]++;
            MdlOutputs(i);

            rtExtModeUpload(i, rtmGetTaskTime(S, i));
            MdlUpdate(i);
            rt_SimUpdateDiscreteTaskTime(rtmGetTPtr(S),
                rtmGetTimingData(S), i);
            /* Indicate task complete for sample time "i" */
            GBLbuf.overrunFlags[i]--;
            GBLbuf.eventFlags[i]--;
        }
    }
    rtExtModeCheckEndTrigger();

    rTorqueDemand[0] = SIG_MODEL(Y, Generator_Torque);
    /*(void)printf("\n** %f **\n", rTorqueDemand[0]);*/
    rBlade1Pitch[0] = SIG_MODEL(Y, Blade1_Pitch_Angle);
    rBlade2Pitch[0] = SIG_MODEL(Y, Blade2_Pitch_Angle);
    rBlade3Pitch[0] = SIG_MODEL(Y, Blade3_Pitch_Angle);
    rPitchDemand[0] = SIG_MODEL(Y, Collective_Pitch_Angle);
    rYawRate[0] = SIG_MODEL(Y, Yaw_Rate);
    rLog1[0] = SIG_MODEL(Y, Log1);
    rLog2[0] = SIG_MODEL(Y, Log2);
    rLog3[0] = SIG_MODEL(Y, Log3);
    rLog4[0] = SIG_MODEL(Y, Log4);
    rLog5[0] = SIG_MODEL(Y, Log5);
    rLog6[0] = SIG_MODEL(Y, Log6);
    rLog7[0] = SIG_MODEL(Y, Log7);
    rLog8[0] = SIG_MODEL(Y, Log8);
    rLog9[0] = SIG_MODEL(Y, Log9);
    rLog10[0] = SIG_MODEL(Y, Log10);
    rLog11[0] = SIG_MODEL(Y, Log11);
    rLog12[0] = SIG_MODEL(Y, Log12);
    rLog13[0] = SIG_MODEL(Y, Log13);
    rLog14[0] = SIG_MODEL(Y, Log14);
    rLog15[0] = SIG_MODEL(Y, Log15);
    rLog16[0] = SIG_MODEL(Y, Log16);
    rLog17[0] = SIG_MODEL(Y, Log17);
    rLog18[0] = SIG_MODEL(Y, Log18);
    rLog19[0] = SIG_MODEL(Y, Log19);
    rLog20[0] = SIG_MODEL(Y, Log20);
    return 0;
}  /* end calcOutputController */
#endif /* MULTITASKING */
/* Function: performCleanup ===============================================
 *
 * Abstract:
 *      Execute model on a generic target such as a workstation.
 */
int performCleanup(char* errorMsg) {

#ifdef UseMMIDataLogging
    rt_CleanUpForStateLogWithMMI(rtmGetRTWLogInfo(S));
#endif
    rt_StopDataLogging(MATFILE, rtmGetRTWLogInfo(S));

    rtExtModeShutdown(rtmGetNumSampleTimes(S));

    if (GBLbuf.errmsg) {
        (void)fprintf(stderr, "%s\n", GBLbuf.errmsg);
        exit(EXIT_FAILURE);
    }

    if (rtmGetErrorStatus(S) != NULL) {
        (void)fprintf(stderr, "ErrorStatus set: \"%s\"\n", rtmGetErrorStatus(S));
        exit(EXIT_FAILURE);
    }

    if (GBLbuf.isrOverrun) {
        (void)fprintf(stderr,
            "%s: ISR overrun - base sampling rate is too fast\n",
            QUOTE(MODEL));
        exit(EXIT_FAILURE);
    }

#ifdef MULTITASKING
    else {
        int_T i;
        for (i = 1; i < NUMST; i++) {
            if (GBLbuf.overrunFlags[i]) {
                (void)fprintf(stderr,
                    "%s ISR overrun - sampling rate is too fast for "
                    "sample time index %d\n", QUOTE(MODEL), i);
                exit(EXIT_FAILURE);
            }
        }
    }
#endif

    sprintf(errorMsg, "** Stopping the controller **");
    MdlTerminate();

    return 0;
}  /* end performCleanup */
static void displayUsage(void)
{
    (void)printf("usage: %s -tf <finaltime> -w -port <TCPport>\n", QUOTE(MODEL));
    (void)printf("arguments:\n");
    (void)printf("  -tf <finaltime> - overrides final time specified in "
        "Simulink (inf for no limit).\n");
    (void)printf("  -w              - waits for Simulink to start model "
        "in External Mode.\n");
    (void)printf("  -port <TCPport> - overrides 17725 default port in "
        "External Mode, valid range 256 to 65535.\n");
}

/* Helper: parse N floats from a long line (skipping comment after '!') */
static int parseFloatRow(FILE* f, float* arr, int n) {
    char buf[4096];
    if (!fgets(buf, sizeof(buf), f)) return 0;
    /* truncate at '!' comment */
    char* bang = strchr(buf, '!');
    if (bang) *bang = '\0';
    char* p = buf;
    for (int i = 0; i < n; i++) {
        char* end;
        arr[i] = (float)strtod(p, &end);
        if (p == end) return 0;
        p = end;
    }
    return 1;
}

/* This function reads all parameters and LUTs from discon.in */
float* SetParams(float* avrSwap)
{
    char mystring[4096];
    int iStatus;
    int n;
    iStatus = NINT(avrSwap[0]);
    if (iStatus == 0)
    {
        pFile = fopen("discon.in", "r");
        if (pFile == NULL) { avrSwap[128] = 1; }
        else {
            fgets(mystring, 200, pFile);                                /* [1]  HEADER */
            fgets(mystring, 200, pFile); avrSwap[130] = atof(mystring); /* [2]  dEcho */
            fgets(mystring, 200, pFile);                                /* [3]  empty */
            fgets(mystring, 200, pFile);                                /* [4]  #GEOMETRY DATA */
            fgets(mystring, 200, pFile); avrSwap[163] = atof(mystring); /* [5]  Architecture_Selector */
            fgets(mystring, 200, pFile); avrSwap[175] = atof(mystring); /* [6]  Darrieus_Selector */
            fgets(mystring, 200, pFile); avrSwap[176] = atof(mystring); /* [7]  Rotor_Area */
            fgets(mystring, 200, pFile); avrSwap[166] = atof(mystring); /* [8]  dBlades_Number */
            fgets(mystring, 200, pFile); avrSwap[131] = atof(mystring); /* [9]  dRotorRadius */
            fgets(mystring, 200, pFile); avrSwap[167] = atof(mystring); /* [10] dBlades_Height */
            fgets(mystring, 200, pFile); avrSwap[173] = atof(mystring); /* [11] dRotInertia */
            fgets(mystring, 200, pFile); avrSwap[161] = atof(mystring); /* [12] dRhoAir */
            fgets(mystring, 200, pFile);                                /* [13] empty */
            fgets(mystring, 200, pFile);                                /* [14] #WIND TURBINE SPECS */
            fgets(mystring, 200, pFile); avrSwap[132] = atof(mystring); /* [15] dRtWSpd */
            fgets(mystring, 200, pFile); avrSwap[133] = atof(mystring); /* [16] dRtTq */
            fgets(mystring, 200, pFile); avrSwap[134] = atof(mystring); /* [17] dRtPwr */
            fgets(mystring, 200, pFile); avrSwap[147] = atof(mystring); /* [18] dCpMax */
            fgets(mystring, 200, pFile); avrSwap[146] = atof(mystring); /* [19] dTSRopt */
            fgets(mystring, 200, pFile);                                /* [20] empty */
            fgets(mystring, 200, pFile);                                /* [21] #GENERATOR SPECS */
            fgets(mystring, 200, pFile); avrSwap[135] = atof(mystring); /* [22] dRtGenSpd */
            fgets(mystring, 200, pFile); avrSwap[136] = atof(mystring); /* [23] dGenEff */
            fgets(mystring, 200, pFile); avrSwap[137] = atof(mystring); /* [24] dMinTq */
            fgets(mystring, 200, pFile); avrSwap[138] = atof(mystring); /* [25] dMaxTq */
            fgets(mystring, 200, pFile); avrSwap[139] = atof(mystring); /* [26] dMaxRat */
            fgets(mystring, 200, pFile); avrSwap[140] = atof(mystring); /* [27] dMinOMSpd */
            fgets(mystring, 200, pFile); avrSwap[219] = atof(mystring); /* [28] dMinStallSpd */
            fgets(mystring, 200, pFile); avrSwap[177] = atof(mystring); /* [29] GearBox_Ratio */
            fgets(mystring, 200, pFile);                                /* [29] empty */
            fgets(mystring, 200, pFile);                                /* [30] #CONTROL MODE */
            fgets(mystring, 200, pFile); avrSwap[141] = atof(mystring); /* [31] dRgn2Mode */
            fgets(mystring, 200, pFile); avrSwap[142] = atof(mystring); /* [32] dKSwitch */
            fgets(mystring, 200, pFile); avrSwap[143] = atof(mystring); /* [33] dRgn2K */
            fgets(mystring, 200, pFile); avrSwap[144] = atof(mystring); /* [34] dOMSpdSwitch */
            fgets(mystring, 200, pFile); avrSwap[145] = atof(mystring); /* [35] dMinOMSpdSwitch */
            fgets(mystring, 200, pFile); avrSwap[148] = atof(mystring); /* [36] dRgn3Mode */
            fgets(mystring, 200, pFile); avrSwap[149] = atof(mystring); /* [37] dMRgn3Lin */
            fgets(mystring, 200, pFile); avrSwap[150] = atof(mystring); /* [38] dVLin */
            fgets(mystring, 200, pFile);                                /* [39] empty */
            fgets(mystring, 200, pFile);                                /* [40] #DATA FILTERS */
            fgets(mystring, 200, pFile); avrSwap[155] = atof(mystring); /* [41] dWindSpeedFSw */
            fgets(mystring, 200, pFile); avrSwap[156] = atof(mystring); /* [42] dWindSpeedFLPF */
            fgets(mystring, 200, pFile); avrSwap[203] = atof(mystring); /* [43] WindSpeed_PreWarp */
            fgets(mystring, 200, pFile); avrSwap[157] = atof(mystring); /* [44] dGenSpeedFSw */
            fgets(mystring, 200, pFile); avrSwap[158] = atof(mystring); /* [45] dGenSpeedFLPF */
            fgets(mystring, 200, pFile); avrSwap[204] = atof(mystring); /* [46] GenSpeed_PreWarp */
            fgets(mystring, 200, pFile); avrSwap[159] = atof(mystring); /* [47] dRefSpeedFSw */
            fgets(mystring, 200, pFile); avrSwap[160] = atof(mystring); /* [48] RefSpeedFLPF */
            fgets(mystring, 200, pFile); avrSwap[205] = atof(mystring); /* [49] RefSpeed_PreWarp */
            fgets(mystring, 200, pFile); avrSwap[162] = atof(mystring); /* [50] dFPitch */
            fgets(mystring, 200, pFile); avrSwap[213] = atof(mystring); /* [51] Pitch_Ctrl_Sw */
            fgets(mystring, 200, pFile); avrSwap[178] = atof(mystring); /* [52] Pitch_LPF_Sw */
            fgets(mystring, 200, pFile); avrSwap[179] = atof(mystring); /* [52] Pitch_F_LPF */
            fgets(mystring, 200, pFile); avrSwap[206] = atof(mystring); /* [53] Pitch_PreWarp */
            fgets(mystring, 200, pFile); avrSwap[181] = atof(mystring); /* [54] Torque_in_LPF_Sw */
            fgets(mystring, 200, pFile); avrSwap[182] = atof(mystring); /* [55] Torque_in_F_LPF */
            fgets(mystring, 200, pFile); avrSwap[207] = atof(mystring); /* [56] Torque_in_PreWarp */
            fgets(mystring, 200, pFile); avrSwap[214] = atof(mystring); /* [57] Torque_out_LPF_Sw */
            fgets(mystring, 200, pFile); avrSwap[215] = atof(mystring); /* [58] Torque_out_F_LPF */
            fgets(mystring, 200, pFile); avrSwap[216] = atof(mystring); /* [59] Torque_out_PreWarp */
            fgets(mystring, 200, pFile); avrSwap[183] = atof(mystring); /* [57] Power_LPF_Sw */
            fgets(mystring, 200, pFile); avrSwap[184] = atof(mystring); /* [58] Power_F_LPF */
            fgets(mystring, 200, pFile); avrSwap[208] = atof(mystring); /* [59] Power_PreWarp */
            fgets(mystring, 200, pFile); avrSwap[185] = atof(mystring); /* [60] LPF_StartingTime */
            fgets(mystring, 200, pFile); avrSwap[217] = atof(mystring); /* [61] NF_StartingTime */
            fgets(mystring, 200, pFile); avrSwap[218] = atof(mystring); /* [62] NF_Switch */
            fgets(mystring, 200, pFile); avrSwap[164] = atof(mystring); /* [63] dZeros_Damping */
            fgets(mystring, 200, pFile); avrSwap[165] = atof(mystring); /* [64] dPoles_Damping */
            fgets(mystring, 200, pFile); avrSwap[174] = atof(mystring); /* [63] dAWSwitch */
            fgets(mystring, 200, pFile); avrSwap[168] = atof(mystring); /* [64] dIntTermSwitch */
            fgets(mystring, 200, pFile);                                /* [65] empty */
            fgets(mystring, 200, pFile);                                /* [66] #SET POINT SMOOTHER */
            fgets(mystring, 200, pFile); avrSwap[186] = atof(mystring); /* [67] SPS_Mode */
            fgets(mystring, 200, pFile); avrSwap[187] = atof(mystring); /* [68] SPS_Kvs */
            fgets(mystring, 200, pFile); avrSwap[188] = atof(mystring); /* [69] SPS_Kpc */
            fgets(mystring, 200, pFile); avrSwap[189] = atof(mystring); /* [70] SPS_Abbas_Factor */
            fgets(mystring, 200, pFile); avrSwap[190] = atof(mystring); /* [71] PS_F_LPF */
            fgets(mystring, 200, pFile); avrSwap[209] = atof(mystring); /* [72] PS_PreWarp */
            fgets(mystring, 200, pFile); avrSwap[191] = atof(mystring); /* [73] PS_LPF_Sw */
            fgets(mystring, 200, pFile); avrSwap[192] = atof(mystring); /* [74] SPS_LPF_Sw */
            fgets(mystring, 200, pFile); avrSwap[193] = atof(mystring); /* [75] SPS_F_LPF */
            fgets(mystring, 200, pFile); avrSwap[210] = atof(mystring); /* [76] SPS_PreWarp */
            fgets(mystring, 200, pFile);                                /* [77] empty */
            fgets(mystring, 200, pFile);                                /* [78] #PITCH CONTROL INPUTS */
            fgets(mystring, 200, pFile); avrSwap[194] = atof(mystring); /* [79] Pitch_Rate */
            fgets(mystring, 200, pFile); avrSwap[195] = atof(mystring); /* [80] Max_Pitch */
            fgets(mystring, 200, pFile);                                /* [81] empty */
            fgets(mystring, 200, pFile);                                /* [82] #WINDSPEED ESTIMATOR */
            fgets(mystring, 200, pFile); avrSwap[196] = atof(mystring); /* [83] WSE_Mode */
            fgets(mystring, 200, pFile); avrSwap[197] = atof(mystring); /* [84] Process_Noise_Q */
            fgets(mystring, 200, pFile); avrSwap[198] = atof(mystring); /* [85] Measurement_Noise_R */
            fgets(mystring, 200, pFile); avrSwap[199] = atof(mystring); /* [86] TBE_F_LPF */
            fgets(mystring, 200, pFile); avrSwap[211] = atof(mystring); /* [87] TBE_PreWarp */
            fgets(mystring, 200, pFile); avrSwap[200] = atof(mystring); /* [88] EKF_F_LPF */
            fgets(mystring, 200, pFile); avrSwap[212] = atof(mystring); /* [89] EKF_PreWarp */
            fgets(mystring, 200, pFile); avrSwap[201] = atof(mystring); /* [90] Rm */
            fgets(mystring, 200, pFile); avrSwap[202] = atof(mystring); /* [91] Ti */
            fgets(mystring, 200, pFile);                                /* [82] empty */
            fgets(mystring, 200, pFile);                                /* [83] TORQUE CONTROLLER GAINS */
            
            fgets(mystring, 200, pFile); sscanf(mystring, "%d", &n);
            ctrlLUTs.nKpBr = n;
            ctrlLUTs.GS_KpBr = (float*)malloc(n * sizeof(float));
            ctrlLUTs.KpBr = (float*)malloc(n * sizeof(float));
            for (int i = 0; i < n; i++) {
                fgets(mystring, 200, pFile);
                sscanf(mystring, "%f %f", &ctrlLUTs.GS_KpBr[i], &ctrlLUTs.KpBr[i]);
            }
            /* --- KiBr --- */
            fgets(mystring, 200, pFile); sscanf(mystring, "%d", &n);
            ctrlLUTs.nKiBr = n;
            ctrlLUTs.GS_KiBr = (float*)malloc(n * sizeof(float));
            ctrlLUTs.KiBr = (float*)malloc(n * sizeof(float));
            for (int i = 0; i < n; i++) {
                fgets(mystring, 200, pFile);
                sscanf(mystring, "%f %f", &ctrlLUTs.GS_KiBr[i], &ctrlLUTs.KiBr[i]);
            }
            /* --- KpAr --- */
            fgets(mystring, 200, pFile); sscanf(mystring, "%d", &n);
            ctrlLUTs.nKpAr = n;
            ctrlLUTs.GS_KpAr = (float*)malloc(n * sizeof(float));
            ctrlLUTs.KpAr = (float*)malloc(n * sizeof(float));
            for (int i = 0; i < n; i++) {
                fgets(mystring, 200, pFile);
                sscanf(mystring, "%f %f", &ctrlLUTs.GS_KpAr[i], &ctrlLUTs.KpAr[i]);
            }
            /* --- KiAr --- */
            fgets(mystring, 200, pFile); sscanf(mystring, "%d", &n);
            ctrlLUTs.nKiAr = n;
            ctrlLUTs.GS_KiAr = (float*)malloc(n * sizeof(float));
            ctrlLUTs.KiAr = (float*)malloc(n * sizeof(float));
            for (int i = 0; i < n; i++) {
                fgets(mystring, 200, pFile);
                sscanf(mystring, "%f %f", &ctrlLUTs.GS_KiAr[i], &ctrlLUTs.KiAr[i]);
            }

            /* ===== PITCH PI GAINS (inline, ROSCO-style: n, then 3 rows of n floats) ===== */
            fgets(mystring, 4096, pFile);                               /* #PITCH PI GAINS */
            fgets(mystring, 200, pFile); sscanf(mystring, "%d", &n);    /* PC_GS_n */
            ctrlLUTs.nPC_GS = n;
            ctrlLUTs.PC_GS_angles = (float*)malloc(n * sizeof(float));
            ctrlLUTs.PC_GS_Kp = (float*)malloc(n * sizeof(float));
            ctrlLUTs.PC_GS_Ki = (float*)malloc(n * sizeof(float));
            parseFloatRow(pFile, ctrlLUTs.PC_GS_angles, n);            /* PC_GS_angles */
            parseFloatRow(pFile, ctrlLUTs.PC_GS_Kp, n);                /* PC_GS_Kp */
            parseFloatRow(pFile, ctrlLUTs.PC_GS_Ki, n);                /* PC_GS_Ki */

            /* ===== PEAK SHAVING (inline: n, then 2 rows of n floats) ===== */
            fgets(mystring, 4096, pFile);                               /* #PEAK SHAVING */
            fgets(mystring, 200, pFile); sscanf(mystring, "%d", &n);    /* PS_n */
            ctrlLUTs.nPS = n;
            ctrlLUTs.PS_WindSpeeds = (float*)malloc(n * sizeof(float));
            ctrlLUTs.PS_BldPitchMin = (float*)malloc(n * sizeof(float));
            parseFloatRow(pFile, ctrlLUTs.PS_WindSpeeds, n);            /* PS_WindSpeeds */
            parseFloatRow(pFile, ctrlLUTs.PS_BldPitchMin, n);           /* PS_BldPitchMin */

            /* ===== EKF SYSTEM POLES (inline: n, then 2 rows of n floats) ===== */
            fgets(mystring, 4096, pFile);                               /* #EKF SYSTEM POLES */
            fgets(mystring, 200, pFile); sscanf(mystring, "%d", &n);    /* WE_FOPoles_n */
            ctrlLUTs.nWE_FOPoles = n;
            ctrlLUTs.WE_FOPoles_v = (float*)malloc(n * sizeof(float));
            ctrlLUTs.WE_FOPoles = (float*)malloc(n * sizeof(float));
            parseFloatRow(pFile, ctrlLUTs.WE_FOPoles_v, n);             /* WE_FOPoles_v */
            parseFloatRow(pFile, ctrlLUTs.WE_FOPoles, n);               /* WE_FOPoles */

            /* ===== CP PERFORMANCE TABLE (inline: nPitch nTSR, pitch row, TSR row, then nTSR rows of nPitch floats) ===== */
            fgets(mystring, 4096, pFile);                               /* #CP PERFORMANCE TABLE */
            {
                int nPitch, nTSR;
                fgets(mystring, 200, pFile);
                sscanf(mystring, "%d %d", &nPitch, &nTSR);             /* Cp_nPitch Cp_nTSR */
                ctrlLUTs.Cp_nPitch = nPitch;
                ctrlLUTs.Cp_nTSR = nTSR;
                ctrlLUTs.Cp_v_pitch = (float*)malloc(nPitch * sizeof(float));
                ctrlLUTs.Cp_v_TSR = (float*)malloc(nTSR * sizeof(float));
                ctrlLUTs.Cp_matrix = (float*)malloc(nTSR * nPitch * sizeof(float));
                parseFloatRow(pFile, ctrlLUTs.Cp_v_pitch, nPitch);      /* Cp_v_pitch */
                parseFloatRow(pFile, ctrlLUTs.Cp_v_TSR, nTSR);          /* Cp_v_TSR */
                for (int i = 0; i < nTSR; i++) {
                    parseFloatRow(pFile, &ctrlLUTs.Cp_matrix[i * nPitch], nPitch);  /* Cp row i */
                }
            }

            /* Derive 1D Cp/TSR curve from 2D matrix (dFPitch in degrees) */
            ExtractCpTSR_from_matrix(avrSwap[162]);

            fclose(pFile);
        }
    }
    return(avrSwap);
}


/*===================*
 * Visible functions *
 *===================*/


/* Function: main =============================================================
 *
 * Abstract:
 *      Execute model on a generic target such as a workstation.
 */
void __declspec(dllexport) __cdecl DISCON(float* avrSwap, int* aviFail, char* accInfile, char* avcOutname, char* avcMsg)
{
    int iStatus, iFirstLog;
    char errorMsg[257], OutName[1025];// inFile[257]; 
    float rTime, rSample, rGeneratorSpeed, rWindSpeed, rRatedSpeed, rBelowRatedPitch,
        rRotorAzimuth, rOP1RootMoment, rOP2RootMoment, rOP3RootMoment, rIP1RootMoment, rIP2RootMoment, rIP3RootMoment,
        rForeAftTower, rSideTower, rMeasuredPitch, rMeasuredTorque, rShaftTorque,
        rModeGain, rInit, rYawError, rYawBearingRate, rElectricalPower;
    static float rUserVar1, rUserVar2, rUserVar3, rUserVar4, rUserVar5,
        rUserVar6, rUserVar7, rUserVar8, rUserVar9, rUserVar10,
        rUserVar11, rUserVar12, rUserVar13, rUserVar14, rUserVar15,
        rUserVar16, rUserVar17, rUserVar18, rUserVar19, rUserVar20,
        rUserVar21, rUserVar22, rUserVar23, rUserVar24, rUserVar25,
        rUserVar26, rUserVar27, rUserVar28, rUserVar29, rUserVar30,
        rUserVar31, rUserVar32, rUserVar33, rUserVar34, rUserVar35,
        rUserVar36, rUserVar37,
        rUserVar38, rUserVar39, rUserVar40,
        rUserVar41, rUserVar42, rUserVar43, rUserVar44, rUserVar45,
        rUserVar46, rUserVar47,
        rUserVar48, rUserVar49, rUserVar50, rUserVar51, rUserVar52,
        rUserVar53, rUserVar54, rUserVar55,
        rUserVar56, rUserVar57,
        rUserVar58, rUserVar59, rUserVar60, rUserVar61, rUserVar62,
        rUserVar63, rUserVar64,
        rUserVar65, rUserVar66, rUserVar67, rUserVar68, rUserVar69,
        rUserVar70, rUserVar71, rUserVar72, rUserVar73, rUserVar74,
        rUserVar75,
        rUserVar76, rUserVar77, rUserVar78,
        rUserVar79, rUserVar80,
        rUserVar81;
    static float rTorqueDemand, rPitchDemand, rBlade1Pitch, rBlade2Pitch,
        rBlade3Pitch, rYawRate, rLog1, rLog2, rLog3, rLog4, rLog5, rLog6, rLog7, rLog8, rLog9, rLog10, rLog11, rLog12, rLog13, rLog14, rLog15, rLog16, rLog17, rLog18, rLog19, rLog20;
    static char mystring[200];

    /* Take local copies of strings */
    //memcpy(inFile, accInfile, NINT(avrSwap[49]));
    //inFile[NINT(avrSwap[49])+1] = '\0';
    //memcpy(outName, avcOutname, NINT(avrSwap[50]));
    //outName[NINT(avrSwap[50])+1] = '\0';

    /* Set message to blank */
    memset(errorMsg, ' ', 257);

    /*(void)printf("\n** %s **\n", path2table);*/

    /* Set constants JW turned this on, see function just above this call*/
    SetParams(avrSwap); /*PF disable this call for Labview's sake*/

    /* Load variables from Bladed (See Appendix A) */
    iStatus = NINT(avrSwap[0]);
    rInit = avrSwap[0];
    rTime = avrSwap[1];
    rSample = avrSwap[2];
    rMeasuredPitch = avrSwap[3];
    rBelowRatedPitch = avrSwap[4];
    rModeGain = avrSwap[15];
    rRatedSpeed = avrSwap[18];
    rGeneratorSpeed = avrSwap[19];
    rWindSpeed = avrSwap[26];
    rMeasuredTorque = avrSwap[22];
    rShaftTorque = avrSwap[108];
    rOP1RootMoment = avrSwap[29];
    rOP2RootMoment = avrSwap[30];
    rOP3RootMoment = avrSwap[31];
    rIP1RootMoment = avrSwap[68];
    rIP2RootMoment = avrSwap[69];
    rIP3RootMoment = avrSwap[70];
    rForeAftTower = avrSwap[52];
    rSideTower = avrSwap[53];
    rRotorAzimuth = avrSwap[59];
    rYawError = avrSwap[23];
    rYawBearingRate = avrSwap[180];
    rElectricalPower = avrSwap[14];

    if (iStatus == 0) {
        rUserVar1 = avrSwap[130];  /* dEcho */
        rUserVar2 = avrSwap[131];  /* dRotorRadius */
        rUserVar3 = avrSwap[132];  /* dRtWSpd */
        rUserVar4 = avrSwap[133];  /* dRtTq */
        rUserVar5 = avrSwap[134];  /* dRtPwr */
        rUserVar6 = avrSwap[135];  /* dRtGenSpd */
        rUserVar7 = avrSwap[136];  /* dGenEff */
        rUserVar8 = avrSwap[137];  /* dMinTq */
        rUserVar9 = avrSwap[138];  /* dMaxTq */
        rUserVar10 = avrSwap[139]; /* dMaxRat */
        rUserVar11 = avrSwap[140]; /* dMinOMSpd */
        rUserVar12 = avrSwap[141]; /* dRgn2Mode */
        rUserVar13 = avrSwap[142]; /* dKSwitch */
        rUserVar14 = avrSwap[143]; /* dRgn2K */
        rUserVar15 = avrSwap[144]; /* dOMSpdSwitch */
        rUserVar16 = avrSwap[145]; /* dMinOMSpdSwitch */
        rUserVar17 = avrSwap[146]; /* dTSRopt */
        rUserVar18 = avrSwap[147]; /* dCpMax */
        rUserVar19 = avrSwap[148]; /* dRgn3Mode */
        rUserVar20 = avrSwap[149]; /* dMRgn3Lin */
        rUserVar21 = avrSwap[150]; /* dVLin */
        rUserVar22 = avrSwap[155]; /* dWindSpeedFSw */
        rUserVar23 = avrSwap[156]; /* dWindSpeedFLPF */
        rUserVar24 = avrSwap[157]; /* dGenSpeedFSw */
        rUserVar25 = avrSwap[158]; /* dGenSpeedFLPF */
        rUserVar26 = avrSwap[159]; /* dRefSpeedFSw */
        rUserVar27 = avrSwap[160]; /* dRefSpeedFLPF */
        rUserVar28 = avrSwap[161]; /* dRhoAir */
        rUserVar29 = avrSwap[162]; /* dFPitch */
        rUserVar30 = avrSwap[163]; /* Architecture_Selector */
        rUserVar31 = avrSwap[164]; /* dZeros_Damping */
        rUserVar32 = avrSwap[165]; /* dPoles_Damping */
        rUserVar33 = avrSwap[166]; /* dBlades_Number */
        rUserVar34 = avrSwap[167]; /* dBlades_Height */
        rUserVar35 = avrSwap[168]; /* dIntTermSwitch */
        rUserVar36 = avrSwap[173]; /* dRotInertia */
        rUserVar37 = avrSwap[174]; /* dAWSwitch */
        /* --- NUOVI PARAMETRI --- */
        rUserVar38 = avrSwap[175]; /* Darrieus_Selector */
        rUserVar39 = avrSwap[176]; /* Rotor_Area */
        rUserVar40 = avrSwap[177]; /* GearBox_Ratio */
        rUserVar41 = avrSwap[178]; /* Pitch_LPF_Sw */
        rUserVar42 = avrSwap[179]; /* Pitch_F_LPF */
        rUserVar43 = avrSwap[181]; /* Torque_in_LPF_Sw */
        rUserVar44 = avrSwap[182]; /* Torque_in_F_LPF */
        rUserVar45 = avrSwap[183]; /* Power_LPF_Sw */
        rUserVar46 = avrSwap[184]; /* Power_F_LPF */
        rUserVar47 = avrSwap[185]; /* LPF_StartingTime */
        rUserVar48 = avrSwap[186]; /* SPS_Mode */
        rUserVar49 = avrSwap[187]; /* SPS_Kvs */
        rUserVar50 = avrSwap[188]; /* SPS_Kpc */
        rUserVar51 = avrSwap[189]; /* SPS_Abbas_Factor */
        rUserVar52 = avrSwap[190]; /* PS_F_LPF */
        rUserVar53 = avrSwap[191]; /* PS_LPF_Sw */
        rUserVar54 = avrSwap[192]; /* SPS_LPF_Sw */
        rUserVar55 = avrSwap[193]; /* SPS_F_LPF */
        rUserVar56 = avrSwap[194]; /* Pitch_Rate */
        rUserVar57 = avrSwap[195]; /* Max_Pitch */
        rUserVar58 = avrSwap[196]; /* WSE_Mode */
        rUserVar59 = avrSwap[197]; /* Process_Noise_Q */
        rUserVar60 = avrSwap[198]; /* Measurement_Noise_R */
        rUserVar61 = avrSwap[199]; /* TBE_F_LPF */
        rUserVar62 = avrSwap[200]; /* EKF_F_LPF */
        rUserVar63 = avrSwap[201]; /* Rm */
        rUserVar64 = avrSwap[202]; /* Ti */
        rUserVar65 = avrSwap[203]; /* WindSpeed_PreWarp */
        rUserVar66 = avrSwap[204]; /* GenSpeed_PreWarp */
        rUserVar67 = avrSwap[205]; /* RefSpeed_PreWarp */
        rUserVar68 = avrSwap[206]; /* Pitch_PreWarp */
        rUserVar69 = avrSwap[207]; /* Torque_in_PreWarp */
        rUserVar70 = avrSwap[208]; /* Power_PreWarp */
        rUserVar71 = avrSwap[209]; /* PS_PreWarp */
        rUserVar72 = avrSwap[210]; /* SPS_PreWarp */
        rUserVar73 = avrSwap[211]; /* TBE_PreWarp */
        rUserVar74 = avrSwap[212]; /* EKF_PreWarp */
        rUserVar75 = avrSwap[213]; /* Pitch_Ctrl_Sw */
        rUserVar76 = avrSwap[214]; /* Torque_out_LPF_Sw */
        rUserVar77 = avrSwap[215]; /* Torque_out_F_LPF */
        rUserVar78 = avrSwap[216]; /* Torque_out_PreWarp */
        rUserVar79 = avrSwap[217]; /* NF_StartingTime */
        rUserVar80 = avrSwap[218]; /* NF_Switch */
        rUserVar81 = avrSwap[219]; /* dMinStallSpd */

        pFile = fopen("discon.in", "r");
        if (pFile == NULL) {}
        else
        {
            char skipbuf[4096];
            int nskip;
            fgets(mystring, 200, pFile); /* [1]  HEADER */
            fgets(mystring, 200, pFile); /* [2]  dEcho */
            fgets(mystring, 200, pFile); /* [3]  empty */
            fgets(mystring, 200, pFile); /* [4]  #GEOMETRY DATA */
            fgets(mystring, 200, pFile); /* [5]  Architecture_Selector */
            fgets(mystring, 200, pFile); /* [6]  Darrieus_Selector */
            fgets(mystring, 200, pFile); /* [7]  Rotor_Area */
            fgets(mystring, 200, pFile); /* [8]  dBlades_Number */
            fgets(mystring, 200, pFile); /* [9]  dRotorRadius */
            fgets(mystring, 200, pFile); /* [10] dBlades_Height */
            fgets(mystring, 200, pFile); /* [11] dRotInertia */
            fgets(mystring, 200, pFile); /* [12] dRhoAir */
            fgets(mystring, 200, pFile); /* [13] empty */
            fgets(mystring, 200, pFile); /* [14] #WIND TURBINE SPECS */
            fgets(mystring, 200, pFile); /* [15] dRtWSpd */
            fgets(mystring, 200, pFile); /* [16] dRtTq */
            fgets(mystring, 200, pFile); /* [17] dRtPwr */
            fgets(mystring, 200, pFile); /* [18] dCpMax */
            fgets(mystring, 200, pFile); /* [19] dTSRopt */
            fgets(mystring, 200, pFile); /* [20] empty */
            fgets(mystring, 200, pFile); /* [21] #GENERATOR SPECS */
            fgets(mystring, 200, pFile); /* [22] dRtGenSpd */
            fgets(mystring, 200, pFile); /* [23] dGenEff */
            fgets(mystring, 200, pFile); /* [24] dMinTq */
            fgets(mystring, 200, pFile); /* [25] dMaxTq */
            fgets(mystring, 200, pFile); /* [26] dMaxRat */
            fgets(mystring, 200, pFile); /* [27] dMinOMSpd */
            fgets(mystring, 200, pFile); /* [28] dMinStallSpd */
            fgets(mystring, 200, pFile); /* [29] GearBox_Ratio */
            fgets(mystring, 200, pFile); /* [29] empty */
            fgets(mystring, 200, pFile); /* [30] #CONTROL MODE */
            fgets(mystring, 200, pFile); /* [31] dRgn2Mode */
            fgets(mystring, 200, pFile); /* [32] dKSwitch */
            fgets(mystring, 200, pFile); /* [33] dRgn2K */
            fgets(mystring, 200, pFile); /* [34] dOMSpdSwitch */
            fgets(mystring, 200, pFile); /* [35] dMinOMSpdSwitch */
            fgets(mystring, 200, pFile); /* [36] dRgn3Mode */
            fgets(mystring, 200, pFile); /* [37] dMRgn3Lin */
            fgets(mystring, 200, pFile); /* [38] dVLin */
            fgets(mystring, 200, pFile); /* [39] empty */
            fgets(mystring, 200, pFile); /* [40] #DATA FILTERS */
            fgets(mystring, 200, pFile); /* [41] dWindSpeedFSw */
            fgets(mystring, 200, pFile); /* [42] dWindSpeedFLPF */
            fgets(mystring, 200, pFile); /* [43] WindSpeed_PreWarp */
            fgets(mystring, 200, pFile); /* [44] dGenSpeedFSw */
            fgets(mystring, 200, pFile); /* [45] dGenSpeedFLPF */
            fgets(mystring, 200, pFile); /* [46] GenSpeed_PreWarp */
            fgets(mystring, 200, pFile); /* [47] dRefSpeedFSw */
            fgets(mystring, 200, pFile); /* [48] RefSpeedFLPF */
            fgets(mystring, 200, pFile); /* [49] RefSpeed_PreWarp */
            fgets(mystring, 200, pFile); /* [50] dFPitch */
            fgets(mystring, 200, pFile); /* [51] Pitch_Ctrl_Sw */
            fgets(mystring, 200, pFile); /* [52] Pitch_LPF_Sw */
            fgets(mystring, 200, pFile); /* [52] Pitch_F_LPF */
            fgets(mystring, 200, pFile); /* [53] Pitch_PreWarp */
            fgets(mystring, 200, pFile); /* [54] Torque_in_LPF_Sw */
            fgets(mystring, 200, pFile); /* [55] Torque_in_F_LPF */
            fgets(mystring, 200, pFile); /* [56] Torque_in_PreWarp */
            fgets(mystring, 200, pFile); /* [57] Torque_out_LPF_Sw */
            fgets(mystring, 200, pFile); /* [58] Torque_out_F_LPF */
            fgets(mystring, 200, pFile); /* [59] Torque_out_PreWarp */
            fgets(mystring, 200, pFile); /* [57] Power_LPF_Sw */
            fgets(mystring, 200, pFile); /* [58] Power_F_LPF */
            fgets(mystring, 200, pFile); /* [59] Power_PreWarp */
            fgets(mystring, 200, pFile); /* [60] LPF_StartingTime */
            fgets(mystring, 200, pFile); /* [61] NF_StartingTime */
            fgets(mystring, 200, pFile); /* [62] NF_Switch */
            fgets(mystring, 200, pFile); /* [63] dZeros_Damping */
            fgets(mystring, 200, pFile); /* [64] dPoles_Damping */
            fgets(mystring, 200, pFile); /* [65] dAWSwitch */
            fgets(mystring, 200, pFile); /* [66] dIntTermSwitch */
            fgets(mystring, 200, pFile); /* [65] empty */
            fgets(mystring, 200, pFile); /* [66] #SET POINT SMOOTHER */
            fgets(mystring, 200, pFile); /* [67] SPS_Mode */
            fgets(mystring, 200, pFile); /* [68] SPS_Kvs */
            fgets(mystring, 200, pFile); /* [69] SPS_Kpc */
            fgets(mystring, 200, pFile); /* [70] SPS_Abbas_Factor */
            fgets(mystring, 200, pFile); /* [71] PS_F_LPF */
            fgets(mystring, 200, pFile); /* [72] PS_PreWarp */
            fgets(mystring, 200, pFile); /* [73] PS_LPF_Sw */
            fgets(mystring, 200, pFile); /* [74] SPS_LPF_Sw */
            fgets(mystring, 200, pFile); /* [75] SPS_F_LPF */
            fgets(mystring, 200, pFile); /* [76] SPS_PreWarp */
            fgets(mystring, 200, pFile); /* [77] empty */
            fgets(mystring, 200, pFile); /* [78] #PITCH CONTROL INPUTS */
            fgets(mystring, 200, pFile); /* [79] Pitch_Rate */
            fgets(mystring, 200, pFile); /* [80] Max_Pitch */
            fgets(mystring, 200, pFile); /* [81] empty */
            fgets(mystring, 200, pFile); /* [82] #WINDSPEED ESTIMATOR */
            fgets(mystring, 200, pFile); /* [83] WSE_Mode */
            fgets(mystring, 200, pFile); /* [84] Process_Noise_Q */
            fgets(mystring, 200, pFile); /* [85] Measurement_Noise_R */
            fgets(mystring, 200, pFile); /* [86] TBE_F_LPF */
            fgets(mystring, 200, pFile); /* [87] TBE_PreWarp */
            fgets(mystring, 200, pFile); /* [88] EKF_F_LPF */
            fgets(mystring, 200, pFile); /* [89] EKF_PreWarp */
            fgets(mystring, 200, pFile); /* [90] Rm */
            fgets(mystring, 200, pFile); /* [91] Ti */
            /* LUT sections already loaded by SetParams - just skip to end */
            fclose(pFile);
        }
    }
    else
    {
        /* LUTs already loaded at iStatus==0, nothing to do */
    }
    /* determine iStatus */
    aviFail[0] = 0;
    if (iStatus == 0) {
        /* Initialize Controller */
        aviFail[0] = initiateController(errorMsg);
        aviFail[0] = calcOutputController(
            rUserVar1, rUserVar2, rUserVar3, rUserVar4, rUserVar5,
            rUserVar6, rUserVar7, rUserVar8, rUserVar9, rUserVar10,
            rUserVar11, rUserVar12, rUserVar13, rUserVar14, rUserVar15,
            rUserVar16, rUserVar17, rUserVar18, rUserVar19, rUserVar20,
            rUserVar21, rUserVar22, rUserVar23, rUserVar24, rUserVar25,
            rUserVar26, rUserVar27, rUserVar28, rUserVar29, rUserVar30,
            rUserVar31, rUserVar32, rUserVar33, rUserVar34, rUserVar35,
            rUserVar36, rUserVar37,
            rUserVar38, rUserVar39, rUserVar40,
            rUserVar41, rUserVar42, rUserVar43, rUserVar44, rUserVar45,
            rUserVar46, rUserVar47,
            rUserVar48, rUserVar49, rUserVar50, rUserVar51, rUserVar52,
            rUserVar53, rUserVar54, rUserVar55,
            rUserVar56, rUserVar57,
            rUserVar58, rUserVar59, rUserVar60, rUserVar61, rUserVar62,
            rUserVar63, rUserVar64,
            rUserVar65, rUserVar66, rUserVar67, rUserVar68, rUserVar69,
            rUserVar70, rUserVar71, rUserVar72, rUserVar73, rUserVar74,
            rUserVar75,
            rUserVar76, rUserVar77, rUserVar78,
            rUserVar79, rUserVar80,
            rUserVar81,
            rTime, rInit, rSample, rGeneratorSpeed, rWindSpeed, rRatedSpeed,
            rBelowRatedPitch, rForeAftTower, rSideTower,
            rRotorAzimuth, rOP1RootMoment, rOP2RootMoment,
            rOP3RootMoment, rIP1RootMoment, rIP2RootMoment,
            rIP3RootMoment, rMeasuredPitch, rMeasuredTorque, rShaftTorque,
            rModeGain, rYawError, rYawBearingRate, rElectricalPower,
            &rTorqueDemand, &rBlade1Pitch, &rBlade2Pitch, &rBlade3Pitch,
            &rPitchDemand, errorMsg, &rYawRate,
            &rLog1, &rLog2, &rLog3, &rLog4, &rLog5,
            &rLog6, &rLog7, &rLog8, &rLog9, &rLog10,
            &rLog11, &rLog12, &rLog13, &rLog14, &rLog15,
            &rLog16, &rLog17, &rLog18, &rLog19, &rLog20);
        rPitchDemand = rMeasuredPitch;
        rTorqueDemand = rMeasuredTorque;
        sprintf(errorMsg, "Controller initialization complete");
    }
    else if (iStatus >= 0) {
        aviFail[0] = calcOutputController(
            rUserVar1, rUserVar2, rUserVar3, rUserVar4, rUserVar5,
            rUserVar6, rUserVar7, rUserVar8, rUserVar9, rUserVar10,
            rUserVar11, rUserVar12, rUserVar13, rUserVar14, rUserVar15,
            rUserVar16, rUserVar17, rUserVar18, rUserVar19, rUserVar20,
            rUserVar21, rUserVar22, rUserVar23, rUserVar24, rUserVar25,
            rUserVar26, rUserVar27, rUserVar28, rUserVar29, rUserVar30,
            rUserVar31, rUserVar32, rUserVar33, rUserVar34, rUserVar35,
            rUserVar36, rUserVar37,
            rUserVar38, rUserVar39, rUserVar40,
            rUserVar41, rUserVar42, rUserVar43, rUserVar44, rUserVar45,
            rUserVar46, rUserVar47,
            rUserVar48, rUserVar49, rUserVar50, rUserVar51, rUserVar52,
            rUserVar53, rUserVar54, rUserVar55,
            rUserVar56, rUserVar57,
            rUserVar58, rUserVar59, rUserVar60, rUserVar61, rUserVar62,
            rUserVar63, rUserVar64,
            rUserVar65, rUserVar66, rUserVar67, rUserVar68, rUserVar69,
            rUserVar70, rUserVar71, rUserVar72, rUserVar73, rUserVar74,
            rUserVar75,
            rUserVar76, rUserVar77, rUserVar78,
            rUserVar79, rUserVar80,
            rUserVar81,
            rTime, rInit, rSample, rGeneratorSpeed, rWindSpeed, rRatedSpeed,
            rBelowRatedPitch, rForeAftTower, rSideTower,
            rRotorAzimuth, rOP1RootMoment, rOP2RootMoment,
            rOP3RootMoment, rIP1RootMoment, rIP2RootMoment,
            rIP3RootMoment, rMeasuredPitch, rMeasuredTorque, rShaftTorque,
            rModeGain, rYawError, rYawBearingRate, rElectricalPower,
            &rTorqueDemand, &rBlade1Pitch, &rBlade2Pitch, &rBlade3Pitch,
            &rPitchDemand, errorMsg, &rYawRate,
            &rLog1, &rLog2, &rLog3, &rLog4, &rLog5,
            &rLog6, &rLog7, &rLog8, &rLog9, &rLog10,
            &rLog11, &rLog12, &rLog13, &rLog14, &rLog15,
            &rLog16, &rLog17, &rLog18, &rLog19, &rLog20);
    }
    else if (iStatus == -1) {
        aviFail[0] = calcOutputController(
            rUserVar1, rUserVar2, rUserVar3, rUserVar4, rUserVar5,
            rUserVar6, rUserVar7, rUserVar8, rUserVar9, rUserVar10,
            rUserVar11, rUserVar12, rUserVar13, rUserVar14, rUserVar15,
            rUserVar16, rUserVar17, rUserVar18, rUserVar19, rUserVar20,
            rUserVar21, rUserVar22, rUserVar23, rUserVar24, rUserVar25,
            rUserVar26, rUserVar27, rUserVar28, rUserVar29, rUserVar30,
            rUserVar31, rUserVar32, rUserVar33, rUserVar34, rUserVar35,
            rUserVar36, rUserVar37,
            rUserVar38, rUserVar39, rUserVar40,
            rUserVar41, rUserVar42, rUserVar43, rUserVar44, rUserVar45,
            rUserVar46, rUserVar47,
            rUserVar48, rUserVar49, rUserVar50, rUserVar51, rUserVar52,
            rUserVar53, rUserVar54, rUserVar55,
            rUserVar56, rUserVar57,
            rUserVar58, rUserVar59, rUserVar60, rUserVar61, rUserVar62,
            rUserVar63, rUserVar64,
            rUserVar65, rUserVar66, rUserVar67, rUserVar68, rUserVar69,
            rUserVar70, rUserVar71, rUserVar72, rUserVar73, rUserVar74,
            rUserVar75,
            rUserVar76, rUserVar77, rUserVar78,
            rUserVar79, rUserVar80,
            rUserVar81,
            rTime, rInit, rSample, rGeneratorSpeed, rWindSpeed, rRatedSpeed,
            rBelowRatedPitch, rForeAftTower, rSideTower,
            rRotorAzimuth, rOP1RootMoment, rOP2RootMoment,
            rOP3RootMoment, rIP1RootMoment, rIP2RootMoment,
            rIP3RootMoment, rMeasuredPitch, rMeasuredTorque, rShaftTorque,
            rModeGain, rYawError, rYawBearingRate, rElectricalPower,
            &rTorqueDemand, &rBlade1Pitch, &rBlade2Pitch, &rBlade3Pitch,
            &rPitchDemand, errorMsg, &rYawRate,
            &rLog1, &rLog2, &rLog3, &rLog4, &rLog5,
            &rLog6, &rLog7, &rLog8, &rLog9, &rLog10,
            &rLog11, &rLog12, &rLog13, &rLog14, &rLog15,
            &rLog16, &rLog17, &rLog18, &rLog19, &rLog20);
        aviFail[0] = performCleanup(errorMsg);
    }
    else {
        aviFail[0] = -1;
        sprintf(errorMsg, "iStatus is not recognized: %d", iStatus);
    }
    /* Store variables to Bladed (See Appendix A) */
    avrSwap[9] = 0; /* Pitch Angle */
    avrSwap[27] = 1; /* Individual Pitch control */
    avrSwap[34] = 1; /* Generator contactor status */
    avrSwap[35] = 0; /* Shaft brake status: 0=off */
    avrSwap[40] = 0; /* Demanded yaw actuator torque */
    avrSwap[41] = rBlade1Pitch;  /* Blade 1 pitch angle demand */
    avrSwap[42] = rBlade2Pitch;  /* Blade 2 pitch angle demand */
    avrSwap[43] = rBlade3Pitch;  /* Blade 3 pitch angle demand */
    avrSwap[44] = rPitchDemand;  /* Pitch angle demand CPC*/
    avrSwap[46] = rTorqueDemand; /* Generator torque demand */
    avrSwap[47] = rYawRate; /* Demanded nacelle yaw rate */
    avrSwap[54] = 0; /* Pitch override */
    avrSwap[55] = 0; /* Torque override */
    avrSwap[71] = 0; /* Generator start-up resistance */
    avrSwap[78] = 1; /* Request for loads: 0=none */
    avrSwap[79] = 0; /* Variable slip current status */
    avrSwap[80] = 0; /* Variable slip current demand */
    /*  (void)printf("\n** %f **\n", avrSwap[46]); */
     // To read the log variables in bladed (JW)
    avrSwap[64] = 0; /* Number of variables returned for logging */
    iFirstLog = NINT(avrSwap[62]) - 1;
    strcpy(OutName, "Log1:-;Log2:-;Log3:-;Log4:-;Log5:-;Log6:-;Log7:-;Log8:-;Log9:-;Log10:-;Log11:-;Log12:-;Log13:-;Log14:-;Log15:-;Log16:-;Log17:-;Log18:-;Log19:-;Log20:-;");
    avrSwap[iFirstLog] = rLog1;
    avrSwap[iFirstLog + 1] = rLog2;
    avrSwap[iFirstLog + 2] = rLog3;
    avrSwap[iFirstLog + 3] = rLog4;
    avrSwap[iFirstLog + 4] = rLog5;
    avrSwap[iFirstLog + 5] = rLog6;
    avrSwap[iFirstLog + 6] = rLog7;
    avrSwap[iFirstLog + 7] = rLog8;
    avrSwap[iFirstLog + 8] = rLog9;
    avrSwap[iFirstLog + 9] = rLog10;
    avrSwap[iFirstLog + 10] = rLog11;
    avrSwap[iFirstLog + 11] = rLog12;
    avrSwap[iFirstLog + 12] = rLog13;
    avrSwap[iFirstLog + 13] = rLog14;
    avrSwap[iFirstLog + 14] = rLog15;
    avrSwap[iFirstLog + 15] = rLog16;
    avrSwap[iFirstLog + 16] = rLog17;
    avrSwap[iFirstLog + 17] = rLog18;
    avrSwap[iFirstLog + 18] = rLog19;
    avrSwap[iFirstLog + 19] = rLog20;
    //Return strings
    memcpy(avcOutname, OutName, NINT(avrSwap[63]));
    memcpy(avcMsg, errorMsg, MIN(256, NINT(avrSwap[48])));
    return;
}
/* end DISCON */
/* EOF: discon_main.c */
