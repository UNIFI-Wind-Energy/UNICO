/*
 * UNICO_types.h
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "UNICO".
 *
 * Model version              : 10.58
 * Simulink Coder version : 9.9 (R2023a) 19-Nov-2022
 * C source code generated on : Tue Jan  9 12:19:13 2024
 *
 * Target selection: discon.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: 32-bit Generic
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_UNICO_types_h_
#define RTW_HEADER_UNICO_types_h_
#include "rtwtypes.h"

/* Custom Type definition for MATLAB Function: '<S6>/Extract CP-TSR Table' */
#include <stdio.h>
#include <stdio.h>
#ifndef struct_emxArray_real_T_513x1
#define struct_emxArray_real_T_513x1

struct emxArray_real_T_513x1
{
  real_T data[513];
  int32_T size[2];
};

#endif                                 /* struct_emxArray_real_T_513x1 */

#ifndef typedef_emxArray_real_T_513x1_UNICO_T
#define typedef_emxArray_real_T_513x1_UNICO_T

typedef struct emxArray_real_T_513x1 emxArray_real_T_513x1_UNICO_T;

#endif                               /* typedef_emxArray_real_T_513x1_UNICO_T */

#ifndef struct_emxArray_char_T
#define struct_emxArray_char_T

struct emxArray_char_T
{
  char_T *data;
  int32_T *size;
  int32_T allocatedSize;
  int32_T numDimensions;
  boolean_T canFreeData;
};

#endif                                 /* struct_emxArray_char_T */

#ifndef typedef_emxArray_char_T_UNICO_T
#define typedef_emxArray_char_T_UNICO_T

typedef struct emxArray_char_T emxArray_char_T_UNICO_T;

#endif                                 /* typedef_emxArray_char_T_UNICO_T */

#ifndef struct_emxArray_boolean_T
#define struct_emxArray_boolean_T

struct emxArray_boolean_T
{
  boolean_T *data;
  int32_T *size;
  int32_T allocatedSize;
  int32_T numDimensions;
  boolean_T canFreeData;
};

#endif                                 /* struct_emxArray_boolean_T */

#ifndef typedef_emxArray_boolean_T_UNICO_T
#define typedef_emxArray_boolean_T_UNICO_T

typedef struct emxArray_boolean_T emxArray_boolean_T_UNICO_T;

#endif                                 /* typedef_emxArray_boolean_T_UNICO_T */

/* Forward declaration for rtModel */
typedef struct tag_RTM_UNICO_T RT_MODEL_UNICO_T;

#endif                                 /* RTW_HEADER_UNICO_types_h_ */
