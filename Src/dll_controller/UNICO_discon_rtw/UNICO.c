/*
 * UNICO.c
 *
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * Code generation for model "UNICO".
 *
 * Model version              : 10.55
 * Simulink Coder version : 9.9 (R2023a) 19-Nov-2022
 * C source code generated on : Wed Dec 27 09:30:55 2023
 *
 * Target selection: discon.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: 32-bit Generic
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "UNICO.h"
#include "rtwtypes.h"
#include <stdio.h>
#include "UNICO_types.h"
#include <stddef.h>
#include <string.h>
#include "rt_nonfinite.h"
#include "UNICO_private.h"
#include <math.h>
#include <stdlib.h>

/* Block signals (default storage) */
B_UNICO_T UNICO_B;

/* Block states (default storage) */
DW_UNICO_T UNICO_DW;

/* External inputs (root inport signals with default storage) */
ExtU_UNICO_T UNICO_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_UNICO_T UNICO_Y;

/* Real-time model */
static RT_MODEL_UNICO_T UNICO_M_;
RT_MODEL_UNICO_T *const UNICO_M = &UNICO_M_;

/* Forward declaration for local functions */
static void UNICO_emxInit_char_T(emxArray_char_T_UNICO_T **pEmxArray, int32_T
  numDimensions);
static void UNICO_emxEnsureCapacity_char_T(emxArray_char_T_UNICO_T *emxArray,
  int32_T oldNumel);
static void UNICO_string_strtrim(const emxArray_char_T_UNICO_T *obj_Value,
  emxArray_char_T_UNICO_T *y_Value);
static int8_T UNICO_filedata_j(void);
static void UNICO_emxFree_char_T(emxArray_char_T_UNICO_T **pEmxArray);
static int8_T UNICO_cfopen_a(const emxArray_char_T_UNICO_T *cfilename, const
  char_T *cpermission);
static FILE *UNICO_fileManager_k(real_T varargin_1);
static void UNICO_fgetl(real_T fileID, emxArray_char_T_UNICO_T *out);
static void UNICO_emxInit_boolean_T(emxArray_boolean_T_UNICO_T **pEmxArray,
  int32_T numDimensions);
static void UNICO_emxFree_boolean_T(emxArray_boolean_T_UNICO_T **pEmxArray);
static int8_T UNICO_filedata(void);
static int8_T UNICO_cfopen(const char_T *cfilename, const char_T *cpermission);
static real_T UNICO_fileManager(void);
static void UNICO_fileManager_l(real_T varargin_1, FILE **f, boolean_T *a);
static int32_T UNICO_cfclose(real_T fid);
static void UNI_emxEnsureCapacity_boolean_T(emxArray_boolean_T_UNICO_T *emxArray,
  int32_T oldNumel);
static void UNICO_copysign(emxArray_char_T_UNICO_T *s1, int32_T *idx, const
  emxArray_char_T_UNICO_T *s, int32_T *k, int32_T n, boolean_T *foundsign,
  boolean_T *success);
static boolean_T UNICO_isUnitImag(const emxArray_char_T_UNICO_T *s, int32_T k,
  int32_T n);
static void UNICO_readNonFinite(const emxArray_char_T_UNICO_T *s, int32_T *k,
  int32_T n, boolean_T *b_finite, real_T *fv);
static boolean_T UNICO_copydigits(emxArray_char_T_UNICO_T *s1, int32_T *idx,
  const emxArray_char_T_UNICO_T *s, int32_T *k, int32_T n, boolean_T allowpoint);
static boolean_T UNICO_copyexponent(emxArray_char_T_UNICO_T *s1, int32_T *idx,
  const emxArray_char_T_UNICO_T *s, int32_T *k, int32_T n);
static void UNICO_readfloat(emxArray_char_T_UNICO_T *s1, int32_T *idx, const
  emxArray_char_T_UNICO_T *s, int32_T *k, int32_T n, boolean_T *isimag,
  boolean_T *b_finite, real_T *nfv, boolean_T *foundsign, boolean_T *success);
static creal_T UNICO_str2double(const emxArray_char_T_UNICO_T *s);
static void UNICO_emxInit_char_T(emxArray_char_T_UNICO_T **pEmxArray, int32_T
  numDimensions)
{
  emxArray_char_T_UNICO_T *emxArray;
  int32_T i;
  *pEmxArray = (emxArray_char_T_UNICO_T *)malloc(sizeof(emxArray_char_T_UNICO_T));
  emxArray = *pEmxArray;
  emxArray->data = (char_T *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int32_T *)malloc(sizeof(int32_T) * (uint32_T)numDimensions);
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

static void UNICO_emxEnsureCapacity_char_T(emxArray_char_T_UNICO_T *emxArray,
  int32_T oldNumel)
{
  int32_T i;
  int32_T newNumel;
  void *newData;
  if (oldNumel < 0) {
    oldNumel = 0;
  }

  newNumel = 1;
  for (i = 0; i < emxArray->numDimensions; i++) {
    newNumel *= emxArray->size[i];
  }

  if (newNumel > emxArray->allocatedSize) {
    i = emxArray->allocatedSize;
    if (i < 16) {
      i = 16;
    }

    while (i < newNumel) {
      if (i > 1073741823) {
        i = MAX_int32_T;
      } else {
        i <<= 1;
      }
    }

    newData = calloc((uint32_T)i, sizeof(char_T));
    if (emxArray->data != NULL) {
      memcpy(newData, emxArray->data, sizeof(char_T) * (uint32_T)oldNumel);
      if (emxArray->canFreeData) {
        free(emxArray->data);
      }
    }

    emxArray->data = (char_T *)newData;
    emxArray->allocatedSize = i;
    emxArray->canFreeData = true;
  }
}

/* Function for MATLAB Function: '<S6>/Extract CP-TSR Table' */
static void UNICO_string_strtrim(const emxArray_char_T_UNICO_T *obj_Value,
  emxArray_char_T_UNICO_T *y_Value)
{
  int32_T b_j1;
  int32_T j2;
  static const boolean_T d[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  int32_T i;
  b_j1 = 0;
  while ((b_j1 + 1 <= obj_Value->size[1]) && d[(uint8_T)obj_Value->data[b_j1] &
         127] && (!(obj_Value->data[b_j1] == '\x00'))) {
    b_j1++;
  }

  j2 = obj_Value->size[1] - 1;
  while ((j2 + 1 > 0) && d[(uint8_T)obj_Value->data[j2] & 127] &&
         (!(obj_Value->data[j2] == '\x00'))) {
    j2--;
  }

  if (b_j1 + 1 > j2 + 1) {
    b_j1 = 0;
    j2 = -1;
  }

  i = y_Value->size[0] * y_Value->size[1];
  y_Value->size[0] = 1;
  j2 -= b_j1;
  y_Value->size[1] = j2 + 1;
  UNICO_emxEnsureCapacity_char_T(y_Value, i);
  for (i = 0; i <= j2; i++) {
    y_Value->data[i] = obj_Value->data[b_j1 + i];
  }
}

/* Function for MATLAB Function: '<S6>/Extract CP-TSR Table' */
static int8_T UNICO_filedata_j(void)
{
  int32_T k;
  int8_T f;
  boolean_T exitg1;
  f = 0;
  k = 1;
  exitg1 = false;
  while ((!exitg1) && (k - 1 < 20)) {
    if (UNICO_DW.eml_openfiles[(int8_T)k - 1] == NULL) {
      f = (int8_T)k;
      exitg1 = true;
    } else {
      k++;
    }
  }

  return f;
}

static void UNICO_emxFree_char_T(emxArray_char_T_UNICO_T **pEmxArray)
{
  if (*pEmxArray != (emxArray_char_T_UNICO_T *)NULL) {
    if (((*pEmxArray)->data != (char_T *)NULL) && (*pEmxArray)->canFreeData) {
      free((*pEmxArray)->data);
    }

    free((*pEmxArray)->size);
    free(*pEmxArray);
    *pEmxArray = (emxArray_char_T_UNICO_T *)NULL;
  }
}

/* Function for MATLAB Function: '<S6>/Extract CP-TSR Table' */
static int8_T UNICO_cfopen_a(const emxArray_char_T_UNICO_T *cfilename, const
  char_T *cpermission)
{
  FILE *filestar;
  emxArray_char_T_UNICO_T *ccfilename;
  int32_T loop_ub;
  int8_T fileid;
  int8_T j;
  fileid = -1;
  j = UNICO_filedata_j();
  if (j >= 1) {
    UNICO_emxInit_char_T(&ccfilename, 2);
    loop_ub = ccfilename->size[0] * ccfilename->size[1];
    ccfilename->size[0] = 1;
    ccfilename->size[1] = cfilename->size[1] + 1;
    UNICO_emxEnsureCapacity_char_T(ccfilename, loop_ub);
    loop_ub = cfilename->size[1];
    if (loop_ub - 1 >= 0) {
      memcpy(&ccfilename->data[0], &cfilename->data[0], (uint32_T)loop_ub *
             sizeof(char_T));
    }

    ccfilename->data[cfilename->size[1]] = '\x00';
    filestar = fopen(&ccfilename->data[0], cpermission);
    UNICO_emxFree_char_T(&ccfilename);
    if (filestar != NULL) {
      UNICO_DW.eml_openfiles[j - 1] = filestar;
      loop_ub = j + 2;
      if (j + 2 > 127) {
        loop_ub = 127;
      }

      fileid = (int8_T)loop_ub;
    }
  }

  return fileid;
}

/* Function for MATLAB Function: '<S6>/Extract CP-TSR Table' */
static FILE *UNICO_fileManager_k(real_T varargin_1)
{
  FILE *f;
  int8_T fileid;
  fileid = (int8_T)varargin_1;
  if (varargin_1 != (int8_T)varargin_1) {
    fileid = -1;
  }

  if (fileid >= 3) {
    f = UNICO_DW.eml_openfiles[fileid - 3];
  } else if (fileid == 0) {
    f = stdin;
  } else if (fileid == 1) {
    f = stdout;
  } else if (fileid == 2) {
    f = stderr;
  } else {
    f = NULL;
  }

  return f;
}

/* Function for MATLAB Function: '<S6>/Extract CP-TSR Table' */
static void UNICO_fgetl(real_T fileID, emxArray_char_T_UNICO_T *out)
{
  FILE *b_filestar;
  FILE *filestar;
  char *cOut;
  int b_wherefrom;
  int st;
  emxArray_char_T_UNICO_T *outBuff;
  int32_T b_i;
  int32_T carriageReturnAt;
  int32_T exitg1;
  int32_T i;
  int32_T idx;
  int32_T reachedEndOfFile;
  int32_T tmp_1;
  char_T ReadBuff[1024];
  char_T tmp_0;
  uint8_T buf;
  boolean_T exitg2;
  boolean_T fileEndAfterCarriageReturn;
  boolean_T newLineAfterCarriageReturn;
  boolean_T readNewline;
  boolean_T tmp;
  filestar = UNICO_fileManager_k(fileID);
  tmp = ((!(fileID != 0.0)) || (!(fileID != 1.0)) || (!(fileID != 2.0)));
  if (tmp) {
    filestar = NULL;
  }

  UNICO_emxInit_char_T(&outBuff, 2);
  outBuff->size[0] = 1;
  outBuff->size[1] = 0;
  if (!(filestar == NULL)) {
    do {
      exitg1 = 0;
      cOut = fgets(&ReadBuff[0], 1024, filestar);
      readNewline = false;
      b_filestar = UNICO_fileManager_k(fileID);
      if (b_filestar == NULL) {
        reachedEndOfFile = 0;
      } else {
        st = feof(b_filestar);
        reachedEndOfFile = ((int32_T)st != 0);
      }

      if (cOut == NULL) {
        exitg1 = 1;
      } else {
        idx = 1;
        carriageReturnAt = 0;
        if (reachedEndOfFile != 0) {
          b_i = 0;
          exitg2 = false;
          while ((!exitg2) && (b_i < 1024)) {
            if (ReadBuff[b_i] == '\x00') {
              idx = b_i + 1;
              exitg2 = true;
            } else {
              if ((carriageReturnAt == 0) && (ReadBuff[b_i] == '\r')) {
                carriageReturnAt = b_i + 1;
              }

              b_i++;
            }
          }

          if (ReadBuff[idx - 1] == '\x00') {
            idx--;
          }
        } else {
          b_i = 0;
          exitg2 = false;
          while ((!exitg2) && (b_i < 1025)) {
            if (b_i + 1 > 1024) {
              idx = 1023;
              exitg2 = true;
            } else if (ReadBuff[b_i] == '\n') {
              idx = b_i + 1;
              exitg2 = true;
            } else {
              if ((carriageReturnAt == 0) && (ReadBuff[b_i] == '\r')) {
                carriageReturnAt = b_i + 1;
              }

              b_i++;
            }
          }

          readNewline = (ReadBuff[idx - 1] == '\n');
        }

        if ((carriageReturnAt > 0) && (carriageReturnAt < 1024)) {
          newLineAfterCarriageReturn = (ReadBuff[carriageReturnAt] == '\n');
          if ((reachedEndOfFile != 0) && (ReadBuff[carriageReturnAt] == '\x00'))
          {
            fileEndAfterCarriageReturn = true;
          } else {
            fileEndAfterCarriageReturn = false;
          }
        } else {
          newLineAfterCarriageReturn = false;
          fileEndAfterCarriageReturn = false;
        }

        if ((carriageReturnAt == 0) || newLineAfterCarriageReturn ||
            fileEndAfterCarriageReturn) {
          if (idx < 1) {
            i = -1;
            idx = -1;
          } else {
            i = idx - 1;
            idx--;
          }

          b_i = i + 1;
          i = outBuff->size[1];
          tmp_1 = outBuff->size[0] * outBuff->size[1];
          outBuff->size[1] = (idx + outBuff->size[1]) + 1;
          UNICO_emxEnsureCapacity_char_T(outBuff, tmp_1);
          for (idx = 0; idx < b_i; idx++) {
            outBuff->data[i + idx] = ReadBuff[idx];
          }
        } else {
          b_i = outBuff->size[1];
          i = outBuff->size[0] * outBuff->size[1];
          outBuff->size[1] += carriageReturnAt;
          UNICO_emxEnsureCapacity_char_T(outBuff, i);
          for (i = 0; i < carriageReturnAt; i++) {
            outBuff->data[b_i + i] = ReadBuff[i];
          }

          b_wherefrom = SEEK_CUR;
          b_filestar = UNICO_fileManager_k(fileID);
          if (tmp) {
            b_filestar = NULL;
          }

          if (!(b_filestar == NULL)) {
            fseek(b_filestar, (long int)(carriageReturnAt - idx), b_wherefrom);
          }
        }

        if (readNewline || (reachedEndOfFile != 0) || (carriageReturnAt > 0)) {
          exitg1 = 1;
        }
      }
    } while (exitg1 == 0);

    filestar = UNICO_fileManager_k(fileID);
    if (filestar == NULL) {
      reachedEndOfFile = 0;
    } else {
      st = feof(filestar);
      reachedEndOfFile = ((int32_T)st != 0);
    }

    if (reachedEndOfFile == 0) {
      filestar = UNICO_fileManager_k(fileID);
      if (tmp) {
        filestar = NULL;
      }

      if (!(filestar == NULL)) {
        fread(&buf, sizeof(uint8_T), (size_t)1, filestar);
      }

      filestar = UNICO_fileManager_k(fileID);
      if (!(filestar == NULL)) {
        st = feof(filestar);
        reachedEndOfFile = ((int32_T)st != 0);
      }

      if (reachedEndOfFile == 0) {
        b_wherefrom = SEEK_CUR;
        filestar = UNICO_fileManager_k(fileID);
        if (tmp) {
          filestar = NULL;
        }

        if (!(filestar == NULL)) {
          fseek(filestar, (long int)-1.0, b_wherefrom);
        }
      }
    }
  }

  if (outBuff->size[1] == 0) {
    out->size[0] = 1;
    out->size[1] = 0;
  } else {
    tmp_0 = outBuff->data[outBuff->size[1] - 1];
    if (tmp_0 == '\n') {
      if ((outBuff->size[1] > 1) && (outBuff->data[outBuff->size[1] - 2] == '\r'))
      {
        if (outBuff->size[1] - 2 < 1) {
          idx = -1;
        } else {
          idx = outBuff->size[1] - 3;
        }

        i = out->size[0] * out->size[1];
        out->size[0] = 1;
        out->size[1] = idx + 1;
        UNICO_emxEnsureCapacity_char_T(out, i);
        if (idx >= 0) {
          memcpy(&out->data[0], &outBuff->data[0], (uint32_T)(idx + 1) * sizeof
                 (char_T));
        }
      } else {
        if (outBuff->size[1] - 1 < 1) {
          idx = -1;
        } else {
          idx = outBuff->size[1] - 2;
        }

        i = out->size[0] * out->size[1];
        out->size[0] = 1;
        out->size[1] = idx + 1;
        UNICO_emxEnsureCapacity_char_T(out, i);
        if (idx >= 0) {
          memcpy(&out->data[0], &outBuff->data[0], (uint32_T)(idx + 1) * sizeof
                 (char_T));
        }
      }
    } else if (tmp_0 == '\r') {
      if (outBuff->size[1] - 1 < 1) {
        idx = -1;
      } else {
        idx = outBuff->size[1] - 2;
      }

      i = out->size[0] * out->size[1];
      out->size[0] = 1;
      out->size[1] = idx + 1;
      UNICO_emxEnsureCapacity_char_T(out, i);
      if (idx >= 0) {
        memcpy(&out->data[0], &outBuff->data[0], (uint32_T)(idx + 1) * sizeof
               (char_T));
      }
    } else {
      i = out->size[0] * out->size[1];
      out->size[0] = 1;
      out->size[1] = outBuff->size[1];
      UNICO_emxEnsureCapacity_char_T(out, i);
      idx = outBuff->size[1];
      memcpy(&out->data[0], &outBuff->data[0], (uint32_T)idx * sizeof(char_T));
    }
  }

  UNICO_emxFree_char_T(&outBuff);
}

static void UNICO_emxInit_boolean_T(emxArray_boolean_T_UNICO_T **pEmxArray,
  int32_T numDimensions)
{
  emxArray_boolean_T_UNICO_T *emxArray;
  int32_T i;
  *pEmxArray = (emxArray_boolean_T_UNICO_T *)malloc(sizeof
    (emxArray_boolean_T_UNICO_T));
  emxArray = *pEmxArray;
  emxArray->data = (boolean_T *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int32_T *)malloc(sizeof(int32_T) * (uint32_T)numDimensions);
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

static void UNICO_emxFree_boolean_T(emxArray_boolean_T_UNICO_T **pEmxArray)
{
  if (*pEmxArray != (emxArray_boolean_T_UNICO_T *)NULL) {
    if (((*pEmxArray)->data != (boolean_T *)NULL) && (*pEmxArray)->canFreeData)
    {
      free((*pEmxArray)->data);
    }

    free((*pEmxArray)->size);
    free(*pEmxArray);
    *pEmxArray = (emxArray_boolean_T_UNICO_T *)NULL;
  }
}

real_T rt_powd_snf(real_T u0, real_T u1)
{
  real_T tmp;
  real_T tmp_0;
  real_T y;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = (rtNaN);
  } else {
    tmp = fabs(u0);
    tmp_0 = fabs(u1);
    if (rtIsInf(u1)) {
      if (tmp == 1.0) {
        y = 1.0;
      } else if (tmp > 1.0) {
        if (u1 > 0.0) {
          y = (rtInf);
        } else {
          y = 0.0;
        }
      } else if (u1 > 0.0) {
        y = 0.0;
      } else {
        y = (rtInf);
      }
    } else if (tmp_0 == 0.0) {
      y = 1.0;
    } else if (tmp_0 == 1.0) {
      if (u1 > 0.0) {
        y = u0;
      } else {
        y = 1.0 / u0;
      }
    } else if (u1 == 2.0) {
      y = u0 * u0;
    } else if ((u1 == 0.5) && (u0 >= 0.0)) {
      y = sqrt(u0);
    } else if ((u0 < 0.0) && (u1 > floor(u1))) {
      y = (rtNaN);
    } else {
      y = pow(u0, u1);
    }
  }

  return y;
}

/* Function for MATLAB Function: '<S4>/Echo1' */
static int8_T UNICO_filedata(void)
{
  int32_T k;
  int8_T f;
  boolean_T exitg1;
  f = 0;
  k = 1;
  exitg1 = false;
  while ((!exitg1) && (k - 1 < 20)) {
    if (UNICO_DW.eml_openfiles_d[(int8_T)k - 1] == NULL) {
      f = (int8_T)k;
      exitg1 = true;
    } else {
      k++;
    }
  }

  return f;
}

/* Function for MATLAB Function: '<S4>/Echo1' */
static int8_T UNICO_cfopen(const char_T *cfilename, const char_T *cpermission)
{
  FILE *filestar;
  int32_T tmp;
  int8_T fileid;
  int8_T j;
  fileid = -1;
  j = UNICO_filedata();
  if (j >= 1) {
    filestar = fopen(cfilename, cpermission);
    if (filestar != NULL) {
      UNICO_DW.eml_autoflush[j - 1] = true;
      UNICO_DW.eml_openfiles_d[j - 1] = filestar;
      tmp = j + 2;
      if (j + 2 > 127) {
        tmp = 127;
      }

      fileid = (int8_T)tmp;
    }
  }

  return fileid;
}

/* Function for MATLAB Function: '<S4>/Echo1' */
static real_T UNICO_fileManager(void)
{
  int8_T fileid;
  fileid = UNICO_cfopen("UNICO_echo.txt", "wb");
  return fileid;
}

/* Function for MATLAB Function: '<S4>/Echo1' */
static void UNICO_fileManager_l(real_T varargin_1, FILE **f, boolean_T *a)
{
  int8_T fileid;
  fileid = (int8_T)varargin_1;
  if (((int8_T)varargin_1 > 22) || ((int8_T)varargin_1 < 0) || (varargin_1 !=
       (int8_T)varargin_1)) {
    fileid = -1;
  }

  if (fileid >= 3) {
    *f = UNICO_DW.eml_openfiles_d[fileid - 3];
    *a = UNICO_DW.eml_autoflush[fileid - 3];
  } else if (fileid == 0) {
    *f = stdin;
    *a = true;
  } else if (fileid == 1) {
    *f = stdout;
    *a = true;
  } else if (fileid == 2) {
    *f = stderr;
    *a = true;
  } else {
    *f = NULL;
    *a = true;
  }
}

/* Function for MATLAB Function: '<S4>/Echo1' */
static int32_T UNICO_cfclose(real_T fid)
{
  FILE *filestar;
  int32_T cst;
  int32_T st;
  int8_T b_fileid;
  int8_T fileid;
  st = -1;
  fileid = (int8_T)fid;
  if (((int8_T)fid > 22) || ((int8_T)fid < 0) || (fid != (int8_T)fid)) {
    fileid = -1;
  }

  b_fileid = fileid;
  if (fileid < 0) {
    b_fileid = -1;
  }

  if (b_fileid >= 3) {
    filestar = UNICO_DW.eml_openfiles_d[b_fileid - 3];
  } else if (b_fileid == 0) {
    filestar = stdin;
  } else if (b_fileid == 1) {
    filestar = stdout;
  } else if (b_fileid == 2) {
    filestar = stderr;
  } else {
    filestar = NULL;
  }

  if ((filestar != NULL) && (fileid >= 3)) {
    cst = fclose(filestar);
    if (cst == 0) {
      st = 0;
      UNICO_DW.eml_openfiles_d[fileid - 3] = NULL;
      UNICO_DW.eml_autoflush[fileid - 3] = true;
    }
  }

  return st;
}

static void UNI_emxEnsureCapacity_boolean_T(emxArray_boolean_T_UNICO_T *emxArray,
  int32_T oldNumel)
{
  int32_T i;
  int32_T newNumel;
  void *newData;
  if (oldNumel < 0) {
    oldNumel = 0;
  }

  newNumel = 1;
  for (i = 0; i < emxArray->numDimensions; i++) {
    newNumel *= emxArray->size[i];
  }

  if (newNumel > emxArray->allocatedSize) {
    i = emxArray->allocatedSize;
    if (i < 16) {
      i = 16;
    }

    while (i < newNumel) {
      if (i > 1073741823) {
        i = MAX_int32_T;
      } else {
        i <<= 1;
      }
    }

    newData = calloc((uint32_T)i, sizeof(boolean_T));
    if (emxArray->data != NULL) {
      memcpy(newData, emxArray->data, sizeof(boolean_T) * (uint32_T)oldNumel);
      if (emxArray->canFreeData) {
        free(emxArray->data);
      }
    }

    emxArray->data = (boolean_T *)newData;
    emxArray->allocatedSize = i;
    emxArray->canFreeData = true;
  }
}

/* Function for MATLAB Function: '<S6>/Extract CP-TSR Table' */
static void UNICO_copysign(emxArray_char_T_UNICO_T *s1, int32_T *idx, const
  emxArray_char_T_UNICO_T *s, int32_T *k, int32_T n, boolean_T *foundsign,
  boolean_T *success)
{
  char_T tmp;
  boolean_T isneg;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  isneg = false;
  *foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (*k <= n)) {
    tmp = s->data[*k - 1];
    if (tmp == '-') {
      isneg = !isneg;
      *foundsign = true;
      (*k)++;
    } else if (tmp == ',') {
      (*k)++;
    } else if (tmp == '+') {
      *foundsign = true;
      (*k)++;
    } else if (!b[(uint8_T)tmp & 127]) {
      exitg1 = true;
    } else {
      (*k)++;
    }
  }

  *success = (*k <= n);
  if ((*success) && isneg) {
    if ((*idx >= 2) && (s1->data[*idx - 2] == '-')) {
      s1->data[*idx - 2] = ' ';
    } else {
      s1->data[*idx - 1] = '-';
      (*idx)++;
    }
  }
}

/* Function for MATLAB Function: '<S6>/Extract CP-TSR Table' */
static boolean_T UNICO_isUnitImag(const emxArray_char_T_UNICO_T *s, int32_T k,
  int32_T n)
{
  int32_T b_k;
  int32_T j;
  char_T c[3];
  char_T tmp;
  boolean_T p;
  p = false;
  if (k <= n) {
    tmp = s->data[k - 1];
    if (tmp == 'j') {
      p = true;
    } else if (tmp == 'i') {
      if (k >= n - 1) {
        p = true;
      } else {
        b_k = k;
        for (j = 0; j < 3; j++) {
          c[j] = '\x00';
          while ((b_k <= n) && (s->data[b_k - 1] == ',')) {
            b_k++;
          }

          if (b_k <= n) {
            c[j] = s->data[b_k - 1];
          }

          b_k++;
        }

        if ((((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n'))
             && ((c[2] == 'F') || (c[2] == 'f'))) || (((c[0] == 'N') || (c[0] ==
               'n')) && ((c[1] == 'A') || (c[1] == 'a')) && ((c[2] == 'N') ||
              (c[2] == 'n')))) {
        } else {
          p = true;
        }
      }
    }
  }

  return p;
}

/* Function for MATLAB Function: '<S6>/Extract CP-TSR Table' */
static void UNICO_readNonFinite(const emxArray_char_T_UNICO_T *s, int32_T *k,
  int32_T n, boolean_T *b_finite, real_T *fv)
{
  int32_T j;
  int32_T ksaved;
  char_T c[3];
  ksaved = *k;
  for (j = 0; j < 3; j++) {
    c[j] = '\x00';
    while ((*k <= n) && (s->data[*k - 1] == ',')) {
      (*k)++;
    }

    if (*k <= n) {
      c[j] = s->data[*k - 1];
    }

    (*k)++;
  }

  if (((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n')) &&
      ((c[2] == 'F') || (c[2] == 'f'))) {
    *b_finite = false;
    *fv = (rtInf);
  } else if (((c[0] == 'N') || (c[0] == 'n')) && ((c[1] == 'A') || (c[1] == 'a'))
             && ((c[2] == 'N') || (c[2] == 'n'))) {
    *b_finite = false;
    *fv = (rtNaN);
  } else {
    *b_finite = true;
    *fv = 0.0;
    *k = ksaved;
  }
}

/* Function for MATLAB Function: '<S6>/Extract CP-TSR Table' */
static boolean_T UNICO_copydigits(emxArray_char_T_UNICO_T *s1, int32_T *idx,
  const emxArray_char_T_UNICO_T *s, int32_T *k, int32_T n, boolean_T allowpoint)
{
  char_T tmp;
  boolean_T exitg1;
  boolean_T haspoint;
  boolean_T success;
  success = (*k <= n);
  haspoint = false;
  exitg1 = false;
  while ((!exitg1) && (success && (*k <= n))) {
    tmp = s->data[*k - 1];
    if ((tmp >= '0') && (tmp <= '9')) {
      s1->data[*idx - 1] = tmp;
      (*idx)++;
      (*k)++;
    } else if (tmp == '.') {
      success = (allowpoint && (!haspoint));
      if (success) {
        s1->data[*idx - 1] = '.';
        (*idx)++;
        haspoint = true;
      }

      (*k)++;
    } else if (tmp == ',') {
      (*k)++;
    } else {
      exitg1 = true;
    }
  }

  return success;
}

/* Function for MATLAB Function: '<S6>/Extract CP-TSR Table' */
static boolean_T UNICO_copyexponent(emxArray_char_T_UNICO_T *s1, int32_T *idx,
  const emxArray_char_T_UNICO_T *s, int32_T *k, int32_T n)
{
  int32_T b_k;
  int32_T kexp;
  char_T tmp;
  boolean_T b_success;
  boolean_T success;
  success = true;
  if (*k <= n) {
    tmp = s->data[*k - 1];
    if ((tmp == 'E') || (tmp == 'e')) {
      s1->data[*idx - 1] = 'e';
      (*idx)++;
      (*k)++;
      while ((*k <= n) && (s->data[*k - 1] == ',')) {
        (*k)++;
      }

      if (*k <= n) {
        tmp = s->data[*k - 1];
        if (tmp == '-') {
          s1->data[*idx - 1] = '-';
          (*idx)++;
          (*k)++;
        } else if (tmp == '+') {
          (*k)++;
        }
      }

      kexp = *k;
      b_k = *k;
      b_success = UNICO_copydigits(s1, idx, s, &b_k, n, false);
      *k = b_k;
      if ((!b_success) || (b_k <= kexp)) {
        success = false;
      }
    }
  }

  return success;
}

/* Function for MATLAB Function: '<S6>/Extract CP-TSR Table' */
static void UNICO_readfloat(emxArray_char_T_UNICO_T *s1, int32_T *idx, const
  emxArray_char_T_UNICO_T *s, int32_T *k, int32_T n, boolean_T *isimag,
  boolean_T *b_finite, real_T *nfv, boolean_T *foundsign, boolean_T *success)
{
  int32_T b_idx;
  int32_T b_k;
  char_T tmp;
  boolean_T a__3;
  static const boolean_T b[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  *isimag = false;
  *b_finite = true;
  *nfv = 0.0;
  b_idx = *idx;
  b_k = *k;
  UNICO_copysign(s1, &b_idx, s, &b_k, n, foundsign, success);
  *idx = b_idx;
  *k = b_k;
  if (*success) {
    if (UNICO_isUnitImag(s, b_k, n)) {
      *isimag = true;
      *k = b_k + 1;
      exitg1 = false;
      while ((!exitg1) && (*k <= n)) {
        tmp = s->data[*k - 1];
        if (b[(uint8_T)tmp & 127] || (tmp == '\x00') || (tmp == ',')) {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= n) && (s->data[*k - 1] == '*')) {
        b_k = *k + 1;
        UNICO_copysign(s1, &b_idx, s, &b_k, n, &a__3, success);
        *idx = b_idx;
        if (*success) {
          if (UNICO_isUnitImag(s, b_k, n)) {
            *success = false;
          } else {
            UNICO_readNonFinite(s, &b_k, n, b_finite, nfv);
            if (*b_finite) {
              *success = UNICO_copydigits(s1, idx, s, &b_k, n, true);
              if (*success) {
                *success = UNICO_copyexponent(s1, idx, s, &b_k, n);
              }
            } else if ((b_idx >= 2) && (s1->data[b_idx - 2] == '-')) {
              *idx = b_idx - 1;
              s1->data[b_idx - 2] = ' ';
              *nfv = -*nfv;
            }

            exitg1 = false;
            while ((!exitg1) && (b_k <= n)) {
              tmp = s->data[b_k - 1];
              if (b[(uint8_T)tmp & 127] || (tmp == '\x00') || (tmp == ',')) {
                b_k++;
              } else {
                exitg1 = true;
              }
            }

            if ((b_k <= n) && (s->data[b_k - 1] == '*')) {
              b_k++;
              exitg1 = false;
              while ((!exitg1) && (b_k <= n)) {
                tmp = s->data[b_k - 1];
                if (b[(uint8_T)tmp & 127] || (tmp == '\x00') || (tmp == ',')) {
                  b_k++;
                } else {
                  exitg1 = true;
                }
              }
            }

            if (b_k <= n) {
              tmp = s->data[b_k - 1];
              if ((tmp == 'i') || (tmp == 'j')) {
                b_k++;
              }
            }
          }

          exitg1 = false;
          while ((!exitg1) && (b_k <= n)) {
            tmp = s->data[b_k - 1];
            if (b[(uint8_T)tmp & 127] || (tmp == '\x00') || (tmp == ',')) {
              b_k++;
            } else {
              exitg1 = true;
            }
          }
        }

        *k = b_k;
      } else {
        s1->data[b_idx - 1] = '1';
        *idx = b_idx + 1;
      }
    } else {
      UNICO_readNonFinite(s, &b_k, n, b_finite, nfv);
      *k = b_k;
      if (*b_finite) {
        *success = UNICO_copydigits(s1, idx, s, k, n, true);
        if (*success) {
          *success = UNICO_copyexponent(s1, idx, s, k, n);
        }
      } else if ((b_idx >= 2) && (s1->data[b_idx - 2] == '-')) {
        *idx = b_idx - 1;
        s1->data[b_idx - 2] = ' ';
        *nfv = -*nfv;
      }

      exitg1 = false;
      while ((!exitg1) && (*k <= n)) {
        tmp = s->data[*k - 1];
        if (b[(uint8_T)tmp & 127] || (tmp == '\x00') || (tmp == ',')) {
          (*k)++;
        } else {
          exitg1 = true;
        }
      }

      if ((*k <= n) && (s->data[*k - 1] == '*')) {
        (*k)++;
        exitg1 = false;
        while ((!exitg1) && (*k <= n)) {
          tmp = s->data[*k - 1];
          if (b[(uint8_T)tmp & 127] || (tmp == '\x00') || (tmp == ',')) {
            (*k)++;
          } else {
            exitg1 = true;
          }
        }
      }

      if (*k <= n) {
        tmp = s->data[*k - 1];
        if ((tmp == 'i') || (tmp == 'j')) {
          (*k)++;
          *isimag = true;
        }
      }
    }

    exitg1 = false;
    while ((!exitg1) && (*k <= n)) {
      tmp = s->data[*k - 1];
      if (b[(uint8_T)tmp & 127] || (tmp == '\x00') || (tmp == ',')) {
        (*k)++;
      } else {
        exitg1 = true;
      }
    }
  }
}

/* Function for MATLAB Function: '<S6>/Extract CP-TSR Table' */
static creal_T UNICO_str2double(const emxArray_char_T_UNICO_T *s)
{
  emxArray_char_T_UNICO_T *s1;
  creal_T x;
  real_T b_scanned1;
  real_T scanned1;
  real_T scanned2;
  int32_T idx;
  int32_T k;
  int32_T ntoread;
  char_T tmp;
  boolean_T a__1;
  boolean_T c_success;
  boolean_T foundsign;
  boolean_T isfinite1;
  boolean_T isimag1;
  boolean_T success;
  static const boolean_T c[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  boolean_T exitg1;
  x.re = (rtNaN);
  x.im = 0.0;
  if (s->size[1] >= 1) {
    UNICO_emxInit_char_T(&s1, 2);
    ntoread = 0;
    k = 1;
    exitg1 = false;
    while ((!exitg1) && (k <= s->size[1])) {
      tmp = s->data[k - 1];
      if (c[(uint8_T)tmp & 127] || (tmp == '\x00')) {
        k++;
      } else {
        exitg1 = true;
      }
    }

    idx = s1->size[0] * s1->size[1];
    s1->size[0] = 1;
    s1->size[1] = s->size[1] + 2;
    UNICO_emxEnsureCapacity_char_T(s1, idx);
    idx = s->size[1];
    if (idx + 1 >= 0) {
      memset(&s1->data[0], 0, (uint32_T)(idx + 2) * sizeof(char_T));
    }

    idx = 1;
    UNICO_readfloat(s1, &idx, s, &k, s->size[1], &isimag1, &isfinite1, &scanned1,
                    &a__1, &success);
    if (isfinite1) {
      ntoread = 1;
    }

    if (success && (k <= s->size[1])) {
      s1->data[idx - 1] = ' ';
      idx++;
      UNICO_readfloat(s1, &idx, s, &k, s->size[1], &a__1, &success, &scanned2,
                      &foundsign, &c_success);
      if (success) {
        ntoread++;
      }

      if (c_success && (k > s->size[1]) && (((boolean_T)(isimag1 ^ a__1)) &&
           foundsign)) {
        success = true;
      } else {
        success = false;
      }
    } else {
      scanned2 = 0.0;
    }

    if (success) {
      s1->data[idx - 1] = '\x00';
      if (ntoread == 2) {
        ntoread = sscanf(&s1->data[0], "%lf %lf", &scanned1, &scanned2);
        if (ntoread != 2) {
          scanned1 = (rtNaN);
          scanned2 = (rtNaN);
        }
      } else if (ntoread == 1) {
        ntoread = sscanf(&s1->data[0], "%lf", &b_scanned1);
        if (isfinite1) {
          if (ntoread == 1) {
            scanned1 = b_scanned1;
          } else {
            scanned1 = (rtNaN);
          }
        } else if (ntoread == 1) {
          scanned2 = b_scanned1;
        } else {
          scanned2 = (rtNaN);
        }
      }

      if (isimag1) {
        x.re = scanned2;
        x.im = scanned1;
      } else {
        x.re = scanned1;
        x.im = scanned2;
      }
    }

    UNICO_emxFree_char_T(&s1);
  }

  return x;
}

/* Model output function */
static void UNICO_output(void)
{
  /* local block i/o variables */
  real_T rtb_Sum1;
  real_T rtb_Switch;
  real_T rtb_Switch2;
  real_T rtb_Switch_f;
  FILE *filestar;
  int st;
  emxArray_boolean_T_UNICO_T *idxCp;
  emxArray_char_T_UNICO_T *aux_Value;
  emxArray_char_T_UNICO_T *tline;
  creal_T x;
  real_T TSR[100];
  real_T pitch[100];
  real_T count;
  real_T count_line;
  real_T rtb_GenSpeedRef;
  real_T rtb_GenTq;
  real_T rtb_Product1;
  real_T rtb_Product2;
  real_T rtb_Product2_a;
  real_T rtb_Product2_b;
  real_T rtb_SpeedErr;
  real_T rtb_Sum;
  real_T rtb_Sum2;
  real_T rtb_Sum3;
  real_T rtb_Sum_e;
  real_T rtb_Sum_k;
  real_T rtb_Switch1_o;
  real_T rtb_Switch_g;
  real_T rtb_ki;
  real_T rtb_kp;
  real_T rtb_tau;
  real_T rtb_tau_h;
  real_T rtb_tau_j;
  real_T rtb_y;
  real_T xtmp;
  int32_T b_j;
  int32_T endPos;
  int32_T l;
  int32_T loop_ub;
  int32_T low_ip1;
  int32_T mid_i;
  int32_T x_tmp;
  uint32_T count_val;
  char_T *tmp;
  int8_T ii_data[100];
  int8_T fileid;
  boolean_T autoflush;
  static const boolean_T s[128] = { false, false, false, false, false, false,
    false, false, false, true, true, true, true, true, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    true, true, true, true, true, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false };

  emxArray_char_T_UNICO_T *fileid_0;
  emxArray_char_T_UNICO_T *tmp_0;
  int32_T exitg2;
  boolean_T exitg1;
  UNICO_emxInit_char_T(&tline, 2);

  /* Outputs for Atomic SubSystem: '<S3>/Extract Signals1' */
  /* Gain: '<S17>/tau' incorporates:
   *  Gain: '<S9>/Gain'
   *  Inport: '<Root>/dWindSpeedFLPF'
   */
  rtb_tau_j = 6.2831853071795862 * UNICO_U.dWindSpeedFLPF * 0.004;

  /* Product: '<S18>/Product2' incorporates:
   *  Bias: '<S18>/Bias'
   */
  rtb_Product2_b = 1.0 / (rtb_tau_j + 1.0) * rtb_tau_j;

  /* Sum: '<S17>/Sum' incorporates:
   *  Inport: '<Root>/Wind_Speed'
   *  UnitDelay: '<S17>/Unit Delay'
   */
  rtb_Sum_k = UNICO_U.Wind_Speed + UNICO_DW.UnitDelay_DSTATE_e;

  /* Sum: '<S17>/Sum1' incorporates:
   *  Bias: '<S18>/Bias1'
   *  Gain: '<S18>/Gain'
   *  Inport: '<Root>/Wind_Speed'
   *  Product: '<S17>/Product4'
   */
  UNICO_B.Sum1_o = ((-2.0) * rtb_Product2_b + 1.0) * rtb_Sum_k +
    UNICO_U.Wind_Speed;

  /* Gain: '<S19>/tau' incorporates:
   *  Gain: '<S9>/Gain1'
   *  Inport: '<Root>/dGenSpeedFLPF'
   */
  rtb_tau_h = 6.2831853071795862 * UNICO_U.dGenSpeedFLPF * 0.004;

  /* Product: '<S20>/Product2' incorporates:
   *  Bias: '<S20>/Bias'
   */
  rtb_Product2_a = 1.0 / (rtb_tau_h + 1.0) * rtb_tau_h;

  /* Sum: '<S19>/Sum' incorporates:
   *  Inport: '<Root>/Generator_Speed'
   *  UnitDelay: '<S19>/Unit Delay'
   */
  rtb_Sum_e = UNICO_U.Generator_Speed + UNICO_DW.UnitDelay_DSTATE_o;

  /* Sum: '<S19>/Sum1' incorporates:
   *  Bias: '<S20>/Bias1'
   *  Gain: '<S20>/Gain'
   *  Inport: '<Root>/Generator_Speed'
   *  Product: '<S19>/Product4'
   */
  UNICO_B.Sum1_i = ((-2.0) * rtb_Product2_a + 1.0) * rtb_Sum_e +
    UNICO_U.Generator_Speed;

  /* Switch: '<S9>/Switch' incorporates:
   *  Inport: '<Root>/Wind_Speed'
   *  Inport: '<Root>/dWindSpeedFSw'
   *  Product: '<S17>/Product1'
   */
  if (UNICO_U.dWindSpeedFSw != 0.0) {
    rtb_Switch_g = rtb_Product2_b * rtb_Sum_k;
  } else {
    rtb_Switch_g = UNICO_U.Wind_Speed;
  }

  /* End of Switch: '<S9>/Switch' */

  /* Switch: '<S9>/Switch1' incorporates:
   *  Inport: '<Root>/Generator_Speed'
   *  Inport: '<Root>/dGenSpeedFSw'
   *  Product: '<S19>/Product1'
   */
  if (UNICO_U.dGenSpeedFSw != 0.0) {
    rtb_Switch1_o = rtb_Product2_a * rtb_Sum_e;
  } else {
    rtb_Switch1_o = UNICO_U.Generator_Speed;
  }

  /* End of Switch: '<S9>/Switch1' */
  /* End of Outputs for SubSystem: '<S3>/Extract Signals1' */

  /* Sum: '<S14>/Sum3' incorporates:
   *  Delay: '<S14>/Delay One Step2'
   *  Inport: '<Root>/SimTime'
   */
  rtb_Sum3 = UNICO_U.SimTime - UNICO_DW.DelayOneStep2_DSTATE;

  /* Outputs for Atomic SubSystem: '<S2>/Read tables' */
  /* MATLAB Function: '<S6>/Extract CP-TSR Table' incorporates:
   *  Inport: '<Root>/Init'
   *  Inport: '<Root>/dRgn3Mode'
   *  Inport: '<Root>/dTablePath'
   */
  /* MATLAB Function 'Input_parameters/Read tables/Extract CP-TSR Table': '<S8>:1' */
  /* '<S8>:1:116' */
  if ((UNICO_U.Init == 0.0) && (UNICO_U.dRgn3Mode == 0.0)) {
    /* '<S8>:1:3' */
    endPos = (int32_T)strlen(&UNICO_U.dTablePath[0]);
    mid_i = tline->size[0] * tline->size[1];
    tline->size[0] = 1;
    tline->size[1] = endPos;
    UNICO_emxEnsureCapacity_char_T(tline, mid_i);
    tmp = &tline->data[0];
    strncpy(tmp, &UNICO_U.dTablePath[0], (uint32_T)endPos);

    /* '<S8>:1:5' */
    endPos = 0;
    UNICO_emxInit_char_T(&aux_Value, 2);
    exitg1 = false;
    while ((!exitg1) && (endPos <= tline->size[1] - 1)) {
      if (tline->data[endPos] == '!') {
        if (endPos < 1) {
          loop_ub = -1;
        } else {
          loop_ub = endPos - 1;
        }

        x_tmp = aux_Value->size[0] * aux_Value->size[1];
        aux_Value->size[0] = 1;
        aux_Value->size[1] = loop_ub + 1;
        UNICO_emxEnsureCapacity_char_T(aux_Value, x_tmp);
        for (x_tmp = 0; x_tmp <= loop_ub; x_tmp++) {
          aux_Value->data[x_tmp] = tline->data[x_tmp];
        }

        exitg1 = true;
      } else {
        endPos++;
      }
    }

    /* '<S8>:1:6' */
    /* '<S8>:1:8' */
    UNICO_emxInit_char_T(&tmp_0, 2);
    UNICO_string_strtrim(aux_Value, tmp_0);
    fileid = UNICO_cfopen_a(tmp_0, "rb");
    UNICO_emxFree_char_T(&tmp_0);

    /* '<S8>:1:15' */
    UNICO_emxInit_char_T(&fileid_0, 2);
    UNICO_fgetl((real_T)fileid, fileid_0);
    UNICO_emxFree_char_T(&fileid_0);

    /* '<S8>:1:19' */
    count = 0.0;

    /* '<S8>:1:20' */
    count_line = 1.0;

    /* '<S8>:1:21' */
    memset(&TSR[0], 0, 100U * sizeof(real_T));

    /* '<S8>:1:22' */
    memset(&UNICO_B.Cp[0], 0, 10000U * sizeof(real_T));

    /* '<S8>:1:23' */
    memset(&pitch[0], 0, 100U * sizeof(real_T));
    UNICO_emxInit_boolean_T(&idxCp, 2);
    do {
      exitg2 = 0;
      filestar = UNICO_fileManager_k((real_T)fileid);
      if (filestar == NULL) {
        low_ip1 = 0;
      } else {
        st = feof(filestar);
        low_ip1 = ((int32_T)st != 0);
      }

      if (low_ip1 == 0) {
        /* '<S8>:1:25' */
        /* '<S8>:1:27' */
        UNICO_fgetl((real_T)fileid, tline);
        if (tline->size[1] == 0) {
          /* '<S8>:1:30' */
          count++;

          /* '<S8>:1:31' */
          count_line = 1.0;
        } else {
          autoflush = false;
          if (tline->data[0] == '#') {
            autoflush = true;
          }

          if (autoflush) {
            /* '<S8>:1:36' */
            count++;

            /* '<S8>:1:37' */
            count_line = 1.0;
          } else if (count == 3.0) {
            /* '<S8>:1:41' */
            /* '<S8>:1:42' */
            x_tmp = idxCp->size[0] * idxCp->size[1];
            idxCp->size[0] = 1;
            idxCp->size[1] = tline->size[1];
            UNI_emxEnsureCapacity_boolean_T(idxCp, x_tmp);
            low_ip1 = tline->size[1];
            for (endPos = 0; endPos < low_ip1; endPos++) {
              idxCp->data[endPos] = s[(uint8_T)tline->data[endPos] & 127];
            }

            /* '<S8>:1:43' */
            count_val = 1U;

            /* '<S8>:1:44' */
            b_j = 0;
            low_ip1 = tline->size[1];

            /* '<S8>:1:45' */
            for (endPos = 0; endPos < low_ip1; endPos++) {
              if (idxCp->data[endPos]) {
                /* '<S8>:1:46' */
                if (!idxCp->data[endPos - 1]) {
                  /* '<S8>:1:47' */
                  if (b_j + 1 > endPos) {
                    mid_i = 0;
                    l = 0;
                  } else {
                    mid_i = b_j;
                    l = endPos;
                  }

                  /* '<S8>:1:50' */
                  /* '<S8>:1:51' */
                  x_tmp = aux_Value->size[0] * aux_Value->size[1];
                  aux_Value->size[0] = 1;
                  loop_ub = l - mid_i;
                  aux_Value->size[1] = loop_ub;
                  UNICO_emxEnsureCapacity_char_T(aux_Value, x_tmp);
                  for (x_tmp = 0; x_tmp < loop_ub; x_tmp++) {
                    aux_Value->data[x_tmp] = tline->data[mid_i + x_tmp];
                  }

                  x = UNICO_str2double(aux_Value);
                  pitch[(int32_T)count_val - 1] = x.re;

                  /* '<S8>:1:52' */
                  count_val++;
                } else {
                  /* '<S8>:1:54' */
                  b_j = endPos;
                }
              }
            }
          } else if (count == 4.0) {
            /* '<S8>:1:63' */
            /* '<S8>:1:65' */
            x_tmp = idxCp->size[0] * idxCp->size[1];
            idxCp->size[0] = 1;
            idxCp->size[1] = tline->size[1];
            UNI_emxEnsureCapacity_boolean_T(idxCp, x_tmp);
            low_ip1 = tline->size[1];
            for (endPos = 0; endPos < low_ip1; endPos++) {
              idxCp->data[endPos] = s[(uint8_T)tline->data[endPos] & 127];
            }

            /* '<S8>:1:66' */
            count_val = 1U;

            /* '<S8>:1:67' */
            b_j = 0;
            low_ip1 = tline->size[1];

            /* '<S8>:1:68' */
            for (endPos = 0; endPos < low_ip1; endPos++) {
              if (idxCp->data[endPos]) {
                /* '<S8>:1:69' */
                if (!idxCp->data[endPos - 1]) {
                  /* '<S8>:1:70' */
                  if (b_j + 1 > endPos) {
                    mid_i = 0;
                    l = 0;
                  } else {
                    mid_i = b_j;
                    l = endPos;
                  }

                  /* '<S8>:1:73' */
                  /* '<S8>:1:74' */
                  x_tmp = aux_Value->size[0] * aux_Value->size[1];
                  aux_Value->size[0] = 1;
                  loop_ub = l - mid_i;
                  aux_Value->size[1] = loop_ub;
                  UNICO_emxEnsureCapacity_char_T(aux_Value, x_tmp);
                  for (x_tmp = 0; x_tmp < loop_ub; x_tmp++) {
                    aux_Value->data[x_tmp] = tline->data[mid_i + x_tmp];
                  }

                  x = UNICO_str2double(aux_Value);
                  TSR[(int32_T)count_val - 1] = x.re;

                  /* '<S8>:1:75' */
                  count_val++;
                } else {
                  /* '<S8>:1:77' */
                  b_j = endPos;
                }
              }
            }
          } else if (count == 8.0) {
            /* '<S8>:1:85' */
            /* '<S8>:1:88' */
            x_tmp = idxCp->size[0] * idxCp->size[1];
            idxCp->size[0] = 1;
            idxCp->size[1] = tline->size[1];
            UNI_emxEnsureCapacity_boolean_T(idxCp, x_tmp);
            low_ip1 = tline->size[1];
            for (endPos = 0; endPos < low_ip1; endPos++) {
              idxCp->data[endPos] = s[(uint8_T)tline->data[endPos] & 127];
            }

            /* '<S8>:1:89' */
            count_val = 1U;

            /* '<S8>:1:90' */
            b_j = 0;
            mid_i = tline->size[1];

            /* '<S8>:1:91' */
            for (endPos = 0; endPos < mid_i; endPos++) {
              if (idxCp->data[endPos]) {
                /* '<S8>:1:92' */
                if (!idxCp->data[endPos - 1]) {
                  /* '<S8>:1:93' */
                  if (b_j + 1 > endPos) {
                    l = 0;
                    low_ip1 = 0;
                  } else {
                    l = b_j;
                    low_ip1 = endPos;
                  }

                  /* '<S8>:1:96' */
                  /* '<S8>:1:97' */
                  x_tmp = aux_Value->size[0] * aux_Value->size[1];
                  aux_Value->size[0] = 1;
                  loop_ub = low_ip1 - l;
                  aux_Value->size[1] = loop_ub;
                  UNICO_emxEnsureCapacity_char_T(aux_Value, x_tmp);
                  for (x_tmp = 0; x_tmp < loop_ub; x_tmp++) {
                    aux_Value->data[x_tmp] = tline->data[l + x_tmp];
                  }

                  x = UNICO_str2double(aux_Value);
                  UNICO_B.Cp[((int32_T)count_line + 100 * ((int32_T)count_val -
                    1)) - 1] = x.re;

                  /* '<S8>:1:98' */
                  count_val++;
                } else {
                  /* '<S8>:1:100' */
                  b_j = endPos;
                }
              }
            }

            /* '<S8>:1:106' */
            count_line++;
          }
        }
      } else {
        exitg2 = 1;
      }
    } while (exitg2 == 0);

    UNICO_emxFree_boolean_T(&idxCp);
    UNICO_emxFree_char_T(&aux_Value);

    /* '<S8>:1:115' */
    endPos = 0;
    b_j = 0;
    exitg1 = false;
    while ((!exitg1) && (b_j < 100)) {
      if (pitch[b_j] == UNICO_U.dFPitch) {
        endPos++;
        ii_data[endPos - 1] = (int8_T)(b_j + 1);
        if (endPos >= 100) {
          exitg1 = true;
        } else {
          b_j++;
        }
      } else {
        b_j++;
      }
    }

    if (endPos < 1) {
      endPos = 0;
    }

    for (x_tmp = 0; x_tmp < endPos; x_tmp++) {
      pitch[x_tmp] = ii_data[x_tmp];
    }

    /* '<S8>:1:116' */
    /* '<S8>:1:121' */
    x_tmp = ((int32_T)pitch[0] - 1) * 100;
    if (!rtIsNaN(UNICO_B.Cp[x_tmp])) {
      b_j = 1;
    } else {
      b_j = 0;
      endPos = 2;
      exitg1 = false;
      while ((!exitg1) && (endPos < 101)) {
        if (!rtIsNaN(UNICO_B.Cp[(x_tmp + endPos) - 1])) {
          b_j = endPos;
          exitg1 = true;
        } else {
          endPos++;
        }
      }
    }

    if (b_j != 0) {
      count = UNICO_B.Cp[(x_tmp + b_j) - 1];
      low_ip1 = b_j;
      for (endPos = b_j + 1; endPos < 101; endPos++) {
        count_line = UNICO_B.Cp[(x_tmp + endPos) - 1];
        if (count < count_line) {
          count = count_line;
          low_ip1 = endPos;
        }
      }
    }

    /* '<S8>:1:124' */
    /* '<S8>:1:125' */
    /* '<S8>:1:127' */
    /* '<S8>:1:129' */
    memcpy(&pitch[0], &TSR[0], (uint32_T)low_ip1 * sizeof(real_T));
    for (endPos = 0; endPos < low_ip1; endPos++) {
      TSR[endPos] = UNICO_B.Cp[x_tmp + endPos];
    }

    /* '<S8>:1:132' */
    memcpy(&UNICO_B.tmp_data[0], &pitch[0], (uint32_T)low_ip1 * sizeof(real_T));
    UNICO_DW.SFunction_DIMS3[0] = low_ip1;
    UNICO_DW.SFunction_DIMS3[1] = 1;

    /* '<S8>:1:133' */
    UNICO_DW.SFunction_DIMS2[0] = low_ip1;
    UNICO_DW.SFunction_DIMS2[1] = 1;
    memcpy(&UNICO_B.TSRSgn[0], &UNICO_B.tmp_data[0], (uint32_T)low_ip1 * sizeof
           (real_T));
    memcpy(&UNICO_B.CpInvSgn[0], &TSR[0], (uint32_T)low_ip1 * sizeof(real_T));
  } else {
    /* '<S8>:1:137' */
    UNICO_B.TSRSgn[0] = 0.0;

    /* '<S8>:1:139' */
    UNICO_DW.SFunction_DIMS3[0] = 1;
    UNICO_DW.SFunction_DIMS2[0] = 1;
    UNICO_DW.SFunction_DIMS3[1] = 1;
    UNICO_DW.SFunction_DIMS2[1] = 1;
    UNICO_B.CpInvSgn[0] = 0.0;
  }

  /* End of MATLAB Function: '<S6>/Extract CP-TSR Table' */

  /* Switch: '<S6>/Switch' incorporates:
   *  Inport: '<Root>/Init'
   */
  if (UNICO_U.Init != 0.0) {
    /* Switch: '<S6>/Switch' incorporates:
     *  Constant: '<S6>/Constant'
     */
    rtb_Switch_f = 0.0;
  } else {
    /* Switch: '<S6>/Switch' incorporates:
     *  Constant: '<S6>/Constant1'
     */
    rtb_Switch_f = 1.0;
  }

  /* End of Switch: '<S6>/Switch' */

  /* Outputs for Enabled SubSystem: '<S6>/Enabled Subsystem' incorporates:
   *  EnablePort: '<S7>/Enable'
   */
  if (rtb_Switch_f > 0.0) {
    if (!UNICO_DW.EnabledSubsystem_MODE) {
      /* FinalizeDims for SignalConversion generated from: '<S7>/CpInvSgn' */
      UNICO_DW.CpInvSgn_DIMS1[0] = UNICO_DW.SFunction_DIMS2[0];

      /* FinalizeDims for SignalConversion generated from: '<S7>/TSRSgn' */
      UNICO_DW.TSRSgn_DIMS1[0] = UNICO_DW.SFunction_DIMS3[0];

      /* FinalizeDims for SignalConversion generated from: '<S7>/CpInvSgn' */
      UNICO_DW.CpInvSgn_DIMS1[1] = 1;

      /* FinalizeDims for SignalConversion generated from: '<S7>/TSRSgn' */
      UNICO_DW.TSRSgn_DIMS1[1] = 1;
      UNICO_DW.EnabledSubsystem_MODE = true;
    }

    /* SignalConversion generated from: '<S7>/CpInvSgn' */
    loop_ub = UNICO_DW.SFunction_DIMS2[0];
    memcpy(&UNICO_B.CpInvSgn_g[0], &UNICO_B.CpInvSgn[0], (uint32_T)loop_ub *
           sizeof(real_T));

    /* SignalConversion generated from: '<S7>/TSRSgn' */
    loop_ub = UNICO_DW.SFunction_DIMS3[0];
    memcpy(&UNICO_B.TSRSgn_j[0], &UNICO_B.TSRSgn[0], (uint32_T)loop_ub * sizeof
           (real_T));
  } else {
    UNICO_DW.EnabledSubsystem_MODE = false;
  }

  /* End of Outputs for SubSystem: '<S6>/Enabled Subsystem' */
  /* End of Outputs for SubSystem: '<S2>/Read tables' */

  /* MATLAB Function: '<S12>/WT_Controller' incorporates:
   *  Inport: '<Root>/dCpMax'
   *  Inport: '<Root>/dGenEff'
   *  Inport: '<Root>/dMRgn3Lin'
   *  Inport: '<Root>/dMinOMSpd'
   *  Inport: '<Root>/dRgn2Mode'
   *  Inport: '<Root>/dRgn3Mode'
   *  Inport: '<Root>/dRhoAir'
   *  Inport: '<Root>/dRotorRadius'
   *  Inport: '<Root>/dRtGenSpd'
   *  Inport: '<Root>/dRtPwr'
   *  Inport: '<Root>/dRtWSpd'
   *  Inport: '<Root>/dTSRopt'
   *  Inport: '<Root>/dVLin'
   *  SignalConversion generated from: '<S7>/CpInvSgn'
   *  SignalConversion generated from: '<S7>/TSRSgn'
   */
  /* MATLAB Function 'UNICO CONTROLLER/Reference Speed Determination/WT_Controller': '<S26>:1' */
  /* '<S26>:1:4' */
  /* '<S26>:1:5' */
  /* '<S26>:1:6' */
  /* '<S26>:1:7' */
  /* '<S26>:1:8' */
  /* '<S26>:1:9' */
  /* '<S26>:1:10' */
  /* '<S26>:1:11' */
  /* '<S26>:1:12' */
  /* '<S26>:1:13' */
  /* '<S26>:1:14' */
  /* '<S26>:1:15' */
  /* '<S26>:1:16' */
  /* '<S26>:1:17' */
  /* '<S26>:1:18' */
  /* '<S26>:1:23' */
  if (rtb_Switch_g <= UNICO_U.dRtWSpd) {
    /* '<S26>:1:26' */
    if (UNICO_U.dRgn2Mode == 0.0) {
      /* '<S26>:1:27' */
      /* '<S26>:1:28' */
      count = rtb_Switch_g * UNICO_U.dTSRopt / UNICO_U.dRotorRadius;
      if ((UNICO_U.dMinOMSpd >= count) || rtIsNaN(count)) {
        count = UNICO_U.dMinOMSpd;
      }

      if ((count <= UNICO_U.dRtGenSpd) || rtIsNaN(UNICO_U.dRtGenSpd)) {
        rtb_GenSpeedRef = count;
      } else {
        rtb_GenSpeedRef = UNICO_U.dRtGenSpd;
      }

      /* '<S26>:1:32' */
    } else if ((UNICO_U.dRtGenSpd >= UNICO_U.dMinOMSpd) || rtIsNaN
               (UNICO_U.dMinOMSpd)) {
      rtb_GenSpeedRef = UNICO_U.dRtGenSpd;
    } else {
      rtb_GenSpeedRef = UNICO_U.dMinOMSpd;
    }
  } else if (UNICO_U.dRgn3Mode == 0.0) {
    /* '<S26>:1:39' */
    /* '<S26>:1:41' */
    /* '<S26>:1:42' */
    count = UNICO_U.dRtPwr * UNICO_U.dGenEff / (0.5 * UNICO_U.dRhoAir *
      3.1415926535897931 * (UNICO_U.dRotorRadius * UNICO_U.dRotorRadius) *
      rt_powd_snf(rtb_Switch_g, 3.0));
    if ((count <= 0.0) || rtIsNaN(count)) {
      count = 0.0;
    }

    if ((!(count <= UNICO_U.dCpMax)) && (!rtIsNaN(UNICO_U.dCpMax))) {
      count = UNICO_U.dCpMax;
    }

    /* '<S26>:1:43' */
    mid_i = UNICO_DW.CpInvSgn_DIMS1[0];
    loop_ub = UNICO_DW.CpInvSgn_DIMS1[0];
    if (loop_ub - 1 >= 0) {
      memcpy(&UNICO_B.tmp_data[0], &UNICO_B.CpInvSgn_g[0], (uint32_T)loop_ub *
             sizeof(real_T));
    }

    loop_ub = UNICO_DW.TSRSgn_DIMS1[0];
    if (loop_ub - 1 >= 0) {
      memcpy(&UNICO_B.y_data[0], &UNICO_B.TSRSgn_j[0], (uint32_T)loop_ub *
             sizeof(real_T));
    }

    b_j = UNICO_DW.CpInvSgn_DIMS1[0] - 1;
    count_line = (rtNaN);
    low_ip1 = 0;
    do {
      exitg2 = 0;
      if (low_ip1 <= UNICO_DW.CpInvSgn_DIMS1[0] - 1) {
        if (rtIsNaN(UNICO_B.CpInvSgn_g[low_ip1])) {
          exitg2 = 1;
        } else {
          low_ip1++;
        }
      } else {
        if (UNICO_B.CpInvSgn_g[1] < UNICO_B.CpInvSgn_g[0]) {
          low_ip1 = (b_j + 1) >> 1;
          for (endPos = 0; endPos < low_ip1; endPos++) {
            xtmp = UNICO_B.tmp_data[endPos];
            x_tmp = b_j - endPos;
            UNICO_B.tmp_data[endPos] = UNICO_B.tmp_data[x_tmp];
            UNICO_B.tmp_data[x_tmp] = xtmp;
          }

          if ((UNICO_DW.TSRSgn_DIMS1[0] != 0) && (UNICO_DW.TSRSgn_DIMS1[0] > 1))
          {
            b_j = UNICO_DW.TSRSgn_DIMS1[0] - 1;
            low_ip1 = UNICO_DW.TSRSgn_DIMS1[0] >> 1;
            for (endPos = 0; endPos < low_ip1; endPos++) {
              xtmp = UNICO_B.y_data[endPos];
              x_tmp = b_j - endPos;
              UNICO_B.y_data[endPos] = UNICO_B.y_data[x_tmp];
              UNICO_B.y_data[x_tmp] = xtmp;
            }
          }
        }

        if ((!(count > UNICO_B.tmp_data[mid_i - 1])) && (!(count <
              UNICO_B.tmp_data[0]))) {
          endPos = mid_i;
          b_j = 1;
          low_ip1 = 2;
          while (endPos > low_ip1) {
            mid_i = (b_j >> 1) + (endPos >> 1);
            if (((b_j & 1) == 1) && ((endPos & 1) == 1)) {
              mid_i++;
            }

            if (count >= UNICO_B.tmp_data[mid_i - 1]) {
              b_j = mid_i;
              low_ip1 = mid_i + 1;
            } else {
              endPos = mid_i;
            }
          }

          count_line = UNICO_B.tmp_data[b_j - 1];
          count = (count - count_line) / (UNICO_B.tmp_data[b_j] - count_line);
          if (count == 0.0) {
            count_line = UNICO_B.y_data[b_j - 1];
          } else if (count == 1.0) {
            count_line = UNICO_B.y_data[b_j];
          } else {
            count_line = UNICO_B.y_data[b_j - 1];
            if (!(count_line == UNICO_B.y_data[b_j])) {
              count_line = (1.0 - count) * count_line + count *
                UNICO_B.y_data[b_j];
            }
          }
        }

        exitg2 = 1;
      }
    } while (exitg2 == 0);

    /* '<S26>:1:44' */
    rtb_GenSpeedRef = count_line * rtb_Switch_g / UNICO_U.dRotorRadius;
  } else if (rtb_Switch_g < UNICO_U.dVLin) {
    /* '<S26>:1:48' */
    /* '<S26>:1:49' */
    rtb_GenSpeedRef = UNICO_U.dRtGenSpd;
  } else {
    /* '<S26>:1:51' */
    /* '<S26>:1:52' */
    count = (rtb_Switch_g - UNICO_U.dVLin) * UNICO_U.dMRgn3Lin +
      UNICO_U.dRtGenSpd;
    if ((count >= UNICO_U.dMinOMSpd) || rtIsNaN(UNICO_U.dMinOMSpd)) {
      rtb_GenSpeedRef = count;
    } else {
      rtb_GenSpeedRef = UNICO_U.dMinOMSpd;
    }
  }

  /* End of MATLAB Function: '<S12>/WT_Controller' */

  /* Gain: '<S27>/tau' incorporates:
   *  Gain: '<S12>/Gain1'
   *  Inport: '<Root>/dRefSpeedFLPF'
   */
  /* '<S26>:1:58' */
  rtb_tau = 6.2831853071795862 * UNICO_U.dRefSpeedFLPF * 0.004;

  /* Product: '<S28>/Product2' incorporates:
   *  Bias: '<S28>/Bias'
   */
  rtb_Product2 = 1.0 / (rtb_tau + 1.0) * rtb_tau;

  /* Sum: '<S27>/Sum' incorporates:
   *  UnitDelay: '<S27>/Unit Delay'
   */
  rtb_Sum = rtb_GenSpeedRef + UNICO_DW.UnitDelay_DSTATE;

  /* Switch: '<S12>/Switch1' incorporates:
   *  Inport: '<Root>/dRefSpeedFSw'
   *  Product: '<S27>/Product1'
   */
  if (UNICO_U.dRefSpeedFSw != 0.0) {
    count = rtb_Product2 * rtb_Sum;
  } else {
    count = rtb_GenSpeedRef;
  }

  /* Sum: '<S10>/Sum' incorporates:
   *  Switch: '<S12>/Switch1'
   */
  rtb_SpeedErr = count - rtb_Switch1_o;

  /* MATLAB Function: '<S10>/Gain scheduling' incorporates:
   *  Inport: '<Root>/dKiAr'
   *  Inport: '<Root>/dKiBr'
   *  Inport: '<Root>/dKpAr'
   *  Inport: '<Root>/dKpBr'
   *  Inport: '<Root>/dOMSpdSwitch'
   *  Inport: '<Root>/dRgn2Mode'
   *  Inport: '<Root>/dRtGenSpd'
   *  Inport: '<Root>/dRtWSpd'
   */
  /* MATLAB Function 'UNICO CONTROLLER/PI Controller1/Gain scheduling': '<S21>:1' */
  /* '<S21>:1:4' */
  /* '<S21>:1:5' */
  /* '<S21>:1:6' */
  /* '<S21>:1:7' */
  if (UNICO_U.dRgn2Mode == 0.0) {
    /* '<S21>:1:11' */
    if ((rtb_Switch_g < UNICO_U.dRtWSpd) && (rtb_Switch1_o < UNICO_U.dRtGenSpd))
    {
      /* '<S21>:1:13' */
      if (rtb_Switch1_o <= UNICO_U.dOMSpdSwitch) {
        /* '<S21>:1:16' */
        /* '<S21>:1:17' */
        rtb_kp = UNICO_U.dKpBr;

        /* '<S21>:1:18' */
        rtb_ki = UNICO_U.dKiBr;
      } else if ((rtb_Switch1_o > UNICO_U.dOMSpdSwitch) && (rtb_Switch1_o <
                  UNICO_U.dRtGenSpd)) {
        /* '<S21>:1:20' */
        /* '<S21>:1:23' */
        count = UNICO_U.dRtGenSpd - UNICO_U.dOMSpdSwitch;
        count_line = rtb_Switch1_o - UNICO_U.dOMSpdSwitch;
        rtb_kp = (UNICO_U.dKpAr - UNICO_U.dKpBr) / count * count_line +
          UNICO_U.dKpBr;

        /* '<S21>:1:24' */
        rtb_ki = (UNICO_U.dKiAr - UNICO_U.dKiBr) / count * count_line +
          UNICO_U.dKiBr;
      } else {
        /* '<S21>:1:28' */
        rtb_kp = UNICO_U.dKpAr;

        /* '<S21>:1:29' */
        rtb_ki = UNICO_U.dKiAr;
      }
    } else {
      /* '<S21>:1:36' */
      rtb_kp = UNICO_U.dKpAr;

      /* '<S21>:1:37' */
      rtb_ki = UNICO_U.dKiAr;
    }
  } else {
    /* '<S21>:1:42' */
    rtb_kp = UNICO_U.dKpAr;

    /* '<S21>:1:43' */
    rtb_ki = UNICO_U.dKiAr;
  }

  /* End of MATLAB Function: '<S10>/Gain scheduling' */

  /* Sum: '<S10>/Sum1' incorporates:
   *  Delay: '<S10>/Delay One Step1'
   *  Product: '<S10>/Product'
   *  Product: '<S10>/Product2'
   */
  rtb_Sum1 = rtb_Sum3 * rtb_SpeedErr * rtb_ki + UNICO_DW.DelayOneStep1_DSTATE;

  /* Switch: '<S24>/Switch' incorporates:
   *  Inport: '<Root>/dMinTq'
   *  RelationalOperator: '<S24>/UpperRelop'
   */
  if (rtb_Sum1 < UNICO_U.dMinTq) {
    /* Switch: '<S24>/Switch' */
    rtb_Switch = UNICO_U.dMinTq;
  } else {
    /* Switch: '<S24>/Switch' */
    rtb_Switch = rtb_Sum1;
  }

  /* End of Switch: '<S24>/Switch' */

  /* Switch: '<S24>/Switch2' incorporates:
   *  Inport: '<Root>/dRtTq'
   *  RelationalOperator: '<S24>/LowerRelop1'
   */
  if (rtb_Sum1 > UNICO_U.dRtTq) {
    /* Switch: '<S24>/Switch2' */
    rtb_Switch2 = UNICO_U.dRtTq;
  } else {
    /* Switch: '<S24>/Switch2' */
    rtb_Switch2 = rtb_Switch;
  }

  /* End of Switch: '<S24>/Switch2' */

  /* Product: '<S10>/Product1' */
  rtb_Product1 = rtb_SpeedErr * rtb_kp;

  /* MATLAB Function: '<S10>/Initialize integral term' incorporates:
   *  Delay: '<S10>/Delay One Step4'
   *  Inport: '<Root>/dCpMax'
   *  Inport: '<Root>/dKSwitch'
   *  Inport: '<Root>/dMinOMSpdSwitch'
   *  Inport: '<Root>/dOMSpdSwitch'
   *  Inport: '<Root>/dRgn2K'
   *  Inport: '<Root>/dRgn2Mode'
   *  Inport: '<Root>/dRhoAir'
   *  Inport: '<Root>/dRotorRadius'
   *  Inport: '<Root>/dTSRopt'
   */
  UNICO_B.PI_int = rtb_Switch2;

  /* MATLAB Function 'UNICO CONTROLLER/PI Controller1/Initialize integral term': '<S22>:1' */
  /* '<S22>:1:5' */
  /* '<S22>:1:6' */
  /* '<S22>:1:7' */
  /* '<S22>:1:9' */
  /* '<S22>:1:10' */
  /* '<S22>:1:11' */
  /* '<S22>:1:12' */
  /* '<S22>:1:13' */
  /* '<S22>:1:14' */
  /* '<S22>:1:15' */
  /* '<S22>:1:17' */
  if (UNICO_U.dRgn2Mode == 1.0) {
    /* '<S22>:1:20' */
    if (UNICO_U.dKSwitch == 1.0) {
      /* '<S22>:1:22' */
      /* '<S22>:1:23' */
      count = UNICO_U.dRgn2K;
    } else {
      /* '<S22>:1:25' */
      count = 0.5 * UNICO_U.dRhoAir * 3.1415926535897931 * rt_powd_snf
        (UNICO_U.dRotorRadius, 5.0) * UNICO_U.dCpMax / rt_powd_snf
        (UNICO_U.dTSRopt, 3.0);
    }

    if ((rtb_Switch1_o < UNICO_U.dOMSpdSwitch) && (rtb_Switch_g <=
         UNICO_U.dRtWSpd)) {
      /* '<S22>:1:27' */
      if (rtb_Switch1_o < UNICO_U.dMinOMSpdSwitch) {
        /* '<S22>:1:29' */
        /* '<S22>:1:32' */
        UNICO_B.check = 0.0;
        if (UNICO_DW.DelayOneStep4_DSTATE == 2.0) {
          /* '<S22>:1:33' */
          /* '<S22>:1:35' */
          /* '<S22>:1:36' */
          UNICO_B.PI_int = rtb_Switch1_o * rtb_Switch1_o * count - rtb_Product1;
        }
      } else {
        /* '<S22>:1:45' */
        UNICO_B.check = 2.0;
      }
    } else if ((rtb_Switch1_o < UNICO_U.dRtGenSpd) && (rtb_Switch_g <=
                UNICO_U.dRtWSpd)) {
      /* '<S22>:1:49' */
      /* '<S22>:1:52' */
      UNICO_B.check = 1.0;
      if (UNICO_DW.DelayOneStep4_DSTATE == 2.0) {
        /* '<S22>:1:54' */
        /* '<S22>:1:56' */
        /* '<S22>:1:57' */
        UNICO_B.PI_int = rtb_Switch1_o * rtb_Switch1_o * count - rtb_Product1;
      }
    } else {
      /* '<S22>:1:75' */
      UNICO_B.check = 3.0;
    }
  } else {
    /* '<S22>:1:82' */
    UNICO_B.check = 0.0;
  }

  /* End of MATLAB Function: '<S10>/Initialize integral term' */

  /* Sum: '<S10>/Sum2' */
  rtb_Sum2 = UNICO_B.PI_int + rtb_Product1;

  /* MATLAB Function: '<S10>/Saturate  torque' incorporates:
   *  Inport: '<Root>/SimTime'
   *  Inport: '<Root>/dMaxTq'
   *  Inport: '<Root>/dMinTq'
   */
  /* MATLAB Function 'UNICO CONTROLLER/PI Controller1/Saturate  torque': '<S23>:1' */
  /* '<S23>:1:4' */
  /* '<S23>:1:5' */
  if (UNICO_U.SimTime < 20.0) {
    /* '<S23>:1:7' */
    /* '<S23>:1:8' */
    rtb_y = rtb_Sum2;
  } else if (rtb_Sum2 > UNICO_U.dMaxTq) {
    /* '<S23>:1:11' */
    /* '<S23>:1:14' */
    rtb_y = UNICO_U.dMaxTq;
  } else if (rtb_Sum2 < UNICO_U.dMinTq) {
    /* '<S23>:1:16' */
    /* '<S23>:1:18' */
    rtb_y = UNICO_U.dMinTq;
  } else {
    /* '<S23>:1:22' */
    rtb_y = rtb_Sum2;
  }

  /* End of MATLAB Function: '<S10>/Saturate  torque' */

  /* MATLAB Function: '<S13>/Region 2 Switch Mode' incorporates:
   *  Inport: '<Root>/dCpMax'
   *  Inport: '<Root>/dKSwitch'
   *  Inport: '<Root>/dMinOMSpd'
   *  Inport: '<Root>/dMinOMSpdSwitch'
   *  Inport: '<Root>/dOMSpdSwitch'
   *  Inport: '<Root>/dRgn2K'
   *  Inport: '<Root>/dRgn2Mode'
   *  Inport: '<Root>/dRhoAir'
   *  Inport: '<Root>/dRotorRadius'
   *  Inport: '<Root>/dRtGenSpd'
   *  Inport: '<Root>/dTSRopt'
   */
  /* MATLAB Function 'UNICO CONTROLLER/Region Switch /Region 2 Switch Mode': '<S29>:1' */
  /* '<S29>:1:4' */
  /* '<S29>:1:5' */
  /* '<S29>:1:6' */
  /* '<S29>:1:7' */
  /* '<S29>:1:8' */
  /* '<S29>:1:9' */
  /* '<S29>:1:10' */
  /* '<S29>:1:11' */
  /* '<S29>:1:12' */
  /* '<S29>:1:13' */
  /* '<S29>:1:14' */
  /* '<S29>:1:15' */
  if (UNICO_U.dKSwitch == 1.0) {
    /* '<S29>:1:22' */
    /* '<S29>:1:23' */
    count = UNICO_U.dRgn2K;
  } else {
    /* '<S29>:1:25' */
    count = 0.5 * UNICO_U.dRhoAir * 3.1415926535897931 * rt_powd_snf
      (UNICO_U.dRotorRadius, 5.0) * UNICO_U.dCpMax / rt_powd_snf(UNICO_U.dTSRopt,
      3.0);
  }

  /* '<S29>:1:28' */
  count_line = UNICO_U.dOMSpdSwitch * UNICO_U.dOMSpdSwitch * count;

  /* '<S29>:1:30' */
  xtmp = UNICO_U.dMinOMSpdSwitch * UNICO_U.dMinOMSpdSwitch * count;
  if (UNICO_U.dRgn2Mode == 1.0) {
    /* '<S29>:1:33' */
    if ((rtb_Switch1_o < UNICO_U.dOMSpdSwitch) && (rtb_Switch_g <=
         UNICO_U.dRtWSpd)) {
      /* '<S29>:1:36' */
      /* '<S29>:1:38' */
      rtb_GenTq = rtb_Switch1_o * rtb_Switch1_o * count;
      if ((rtb_Switch1_o < UNICO_U.dMinOMSpdSwitch) && (rtb_Switch1_o >
           UNICO_U.dMinOMSpd)) {
        /* '<S29>:1:40' */
        /* '<S29>:1:43' */
        rtb_GenTq = (rtb_y - xtmp) / (UNICO_U.dMinOMSpd -
          UNICO_U.dMinOMSpdSwitch) * (rtb_Switch1_o - UNICO_U.dMinOMSpdSwitch) +
          xtmp;
      } else if (rtb_Switch1_o < UNICO_U.dMinOMSpd) {
        /* '<S29>:1:45' */
        /* '<S29>:1:48' */
        rtb_GenTq = rtb_y;
      }
    } else if ((rtb_Switch1_o < UNICO_U.dRtGenSpd) && (rtb_Switch_g <=
                UNICO_U.dRtWSpd)) {
      /* '<S29>:1:51' */
      /* '<S29>:1:53' */
      rtb_GenTq = (rtb_y - count_line) / (UNICO_U.dRtGenSpd -
        UNICO_U.dOMSpdSwitch) * (rtb_Switch1_o - UNICO_U.dOMSpdSwitch) +
        count_line;
    } else {
      /* '<S29>:1:56' */
      rtb_GenTq = rtb_y;
    }
  } else {
    /* '<S29>:1:60' */
    rtb_GenTq = rtb_y;
  }

  /* End of MATLAB Function: '<S13>/Region 2 Switch Mode' */

  /* Outport: '<Root>/Generator_Torque' incorporates:
   *  MATLAB Function: '<S3>/Rate limiter'
   */
  UNICO_Y.Generator_Torque = rtb_GenTq;

  /* MATLAB Function: '<S3>/Rate limiter' incorporates:
   *  Delay: '<S3>/Delay One Step3'
   *  Inport: '<Root>/SimTime'
   *  Inport: '<Root>/dMaxRat'
   */
  /* MATLAB Function 'UNICO CONTROLLER/Rate limiter': '<S11>:1' */
  if (UNICO_U.SimTime > 20.0) {
    /* '<S11>:1:4' */
    /* '<S11>:1:6' */
    /* '<S11>:1:8' */
    count = (rtb_GenTq - UNICO_DW.DelayOneStep3_DSTATE) / rtb_Sum3;
    if (count > UNICO_U.dMaxRat) {
      /* Outport: '<Root>/Generator_Torque' */
      /* '<S11>:1:10' */
      /* '<S11>:1:12' */
      UNICO_Y.Generator_Torque = UNICO_U.dMaxRat * rtb_Sum3 +
        UNICO_DW.DelayOneStep3_DSTATE;
    } else if (count < -UNICO_U.dMaxRat) {
      /* Outport: '<Root>/Generator_Torque' */
      /* '<S11>:1:14' */
      /* '<S11>:1:16' */
      UNICO_Y.Generator_Torque = -UNICO_U.dMaxRat * rtb_Sum3 +
        UNICO_DW.DelayOneStep3_DSTATE;
    }
  }

  /* Sum: '<S27>/Sum1' incorporates:
   *  Bias: '<S28>/Bias1'
   *  Gain: '<S28>/Gain'
   *  Product: '<S27>/Product4'
   */
  /* '<S11>:1:24' */
  UNICO_B.Sum1 = ((-2.0) * rtb_Product2 + 1.0) * rtb_Sum + rtb_GenSpeedRef;

  /* Outport: '<Root>/Blade1_Pitch_Angle' incorporates:
   *  Inport: '<Root>/Measured_Pitch'
   */
  UNICO_Y.Blade1_Pitch_Angle = UNICO_U.Measured_Pitch;

  /* Outport: '<Root>/Blade2_Pitch_Angle' incorporates:
   *  Inport: '<Root>/Measured_Pitch'
   */
  UNICO_Y.Blade2_Pitch_Angle = UNICO_U.Measured_Pitch;

  /* Outport: '<Root>/Blade3_Pitch_Angle' incorporates:
   *  Inport: '<Root>/Measured_Pitch'
   */
  UNICO_Y.Blade3_Pitch_Angle = UNICO_U.Measured_Pitch;

  /* Outport: '<Root>/Collective_Pitch_Angle' incorporates:
   *  Inport: '<Root>/Measured_Pitch'
   */
  UNICO_Y.Collective_Pitch_Angle = UNICO_U.Measured_Pitch;

  /* Outputs for Enabled SubSystem: '<S1>/Enabled Subsystem' incorporates:
   *  EnablePort: '<S4>/Enable'
   */
  /* Inport: '<Root>/dEcho' */
  if (UNICO_U.dEcho > 0.0) {
    /* MATLAB Function: '<S4>/Echo1' incorporates:
     *  Inport: '<Root>/Init'
     *  Inport: '<Root>/dCpMax'
     *  Inport: '<Root>/dGenEff'
     *  Inport: '<Root>/dGenSpeedFLPF'
     *  Inport: '<Root>/dGenSpeedFSw'
     *  Inport: '<Root>/dKSwitch'
     *  Inport: '<Root>/dKiAr'
     *  Inport: '<Root>/dKiBr'
     *  Inport: '<Root>/dKpAr'
     *  Inport: '<Root>/dKpBr'
     *  Inport: '<Root>/dMRgn3Lin'
     *  Inport: '<Root>/dMaxRat'
     *  Inport: '<Root>/dMaxTq'
     *  Inport: '<Root>/dMinOMSpd'
     *  Inport: '<Root>/dMinOMSpdSwitch'
     *  Inport: '<Root>/dMinTq'
     *  Inport: '<Root>/dOMSpdSwitch'
     *  Inport: '<Root>/dRefSpeedFLPF'
     *  Inport: '<Root>/dRefSpeedFSw'
     *  Inport: '<Root>/dRgn2K'
     *  Inport: '<Root>/dRgn2Mode'
     *  Inport: '<Root>/dRgn3Mode'
     *  Inport: '<Root>/dRhoAir'
     *  Inport: '<Root>/dRotorRadius'
     *  Inport: '<Root>/dRtGenSpd'
     *  Inport: '<Root>/dRtPwr'
     *  Inport: '<Root>/dRtTq'
     *  Inport: '<Root>/dRtWSpd'
     *  Inport: '<Root>/dTSRopt'
     *  Inport: '<Root>/dVLin'
     *  Inport: '<Root>/dWindSpeedFLPF'
     *  Inport: '<Root>/dWindSpeedFSw'
     */
    /* MATLAB Function 'Echo print/Enabled Subsystem/Echo1': '<S5>:1' */
    /* '<S5>:1:3' */
    /* '<S5>:1:4' */
    /* '<S5>:1:5' */
    /* '<S5>:1:6' */
    /* '<S5>:1:7' */
    /* '<S5>:1:8' */
    /* '<S5>:1:9' */
    /* '<S5>:1:10' */
    /* '<S5>:1:11' */
    /* '<S5>:1:12' */
    /* '<S5>:1:13' */
    /* '<S5>:1:14' */
    /* '<S5>:1:15' */
    /* '<S5>:1:16' */
    /* '<S5>:1:17' */
    /* '<S5>:1:18' */
    /* '<S5>:1:19' */
    /* '<S5>:1:20' */
    /* '<S5>:1:21' */
    /* '<S5>:1:22' */
    /* '<S5>:1:23' */
    /* '<S5>:1:24' */
    /* '<S5>:1:25' */
    /* '<S5>:1:26' */
    /* '<S5>:1:27' */
    /* '<S5>:1:28' */
    if (UNICO_U.Init == 0.0) {
      /* '<S5>:1:33' */
      /* '<S5>:1:35' */
      count = UNICO_fileManager();

      /* '<S5>:1:36' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "UNICO CONTROLLER SIMULATION INPUT \n");
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:37' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Rotor radius [m]:                           %f \n",
                UNICO_U.dRotorRadius);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:38' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Rated wind speed [m/s]:                     %f \n",
                UNICO_U.dRtWSpd);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:39' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Rated Torque [Nm]:                          %f \n",
                UNICO_U.dRtTq);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:40' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Rated power [W]:                            %f \n",
                UNICO_U.dRtPwr);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:41' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Rated generator speed [rad/s]:              %f \n",
                UNICO_U.dRtGenSpd);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:42' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Generator efficiency [-]:                   %f \n",
                UNICO_U.dGenEff);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:43' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Minimum Torque [Nm]:                        %f \n",
                UNICO_U.dMinTq);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:44' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Maximum generator torque [Nm]:              %f \n",
                UNICO_U.dMaxTq);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:45' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Maximum generator torque rate [Nm/s]:       %f \n",
                UNICO_U.dMaxRat);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:46' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Minumum generator speed [rad/s]:            %f \n",
                UNICO_U.dMinOMSpd);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:47' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Switch between Rgn1.5 and 2 [rad/s]:        %f \n",
                UNICO_U.dMinOMSpdSwitch);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:48' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Region 2 control [-]:                      %f \n",
                UNICO_U.dRgn2Mode);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:49' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Switch for user defined K [-]:              %f \n",
                UNICO_U.dKSwitch);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:50' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "User defined K value [Nms^2/rad^2]:         %f \n",
                UNICO_U.dRgn2K);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:51' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Switch between Rgn2 and 2.5 [rad/s]:        %f \n",
                UNICO_U.dOMSpdSwitch);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:52' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Optimum TSR [-]:                            %f \n",
                UNICO_U.dTSRopt);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:53' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Maximum Cp value [-]:                       %f \n",
                UNICO_U.dCpMax);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:54' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Region 3 control [-]:                       %f \n",
                UNICO_U.dRgn3Mode);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:55' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Angular coefficient linear method []:       %f \n",
                UNICO_U.dMRgn3Lin);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:56' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Start reduction of ref velocity [m/s]:      %f \n",
                UNICO_U.dVLin);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:57' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Above rated PI Controller Kp [Nms/rad]:     %f \n",
                UNICO_U.dKpAr);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:58' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Above rated PI Controller Ki [Nm/rad]:      %f \n",
                UNICO_U.dKiAr);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:59' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Below rated PI Controller Kp [Nms/rad]:     %f \n",
                UNICO_U.dKpBr);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:60' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Below rated PI Controller Ki [Nm/rad]:      %f \n",
                UNICO_U.dKiBr);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:61' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Switch Wind speed filter [-]:               %f \n",
                UNICO_U.dWindSpeedFSw);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:62' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Wind speed lowpass frequency[Hz]:           %f \n",
                UNICO_U.dWindSpeedFLPF);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:63' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Switch Generator speed filter [-]:          %f \n",
                UNICO_U.dGenSpeedFSw);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:64' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Generator speed lowpass frequency [Hz]:     %f \n",
                UNICO_U.dGenSpeedFLPF);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:65' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Switch reference speed filter [-]:          %f \n",
                UNICO_U.dRefSpeedFSw);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:66' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Reference speed lowpass frequency [Hz]:     %f \n",
                UNICO_U.dRefSpeedFLPF);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:67' */
      UNICO_fileManager_l(count, &filestar, &autoflush);
      if (!(filestar == NULL)) {
        fprintf(filestar, "Air density [kg/m^3]:                       %f \n",
                UNICO_U.dRhoAir);
        if (autoflush) {
          fflush(filestar);
        }
      }

      /* '<S5>:1:68' */
      UNICO_cfclose(count);
    }

    /* End of MATLAB Function: '<S4>/Echo1' */
  }

  /* End of Inport: '<Root>/dEcho' */
  /* End of Outputs for SubSystem: '<S1>/Enabled Subsystem' */
  UNICO_emxFree_char_T(&tline);
}

/* Model update function */
static void UNICO_update(void)
{
  /* Update for Delay: '<S3>/Delay One Step3' incorporates:
   *  Outport: '<Root>/Generator_Torque'
   */
  UNICO_DW.DelayOneStep3_DSTATE = UNICO_Y.Generator_Torque;

  /* Update for Atomic SubSystem: '<S3>/Extract Signals1' */
  /* Update for UnitDelay: '<S17>/Unit Delay' */
  UNICO_DW.UnitDelay_DSTATE_e = UNICO_B.Sum1_o;

  /* Update for UnitDelay: '<S19>/Unit Delay' */
  UNICO_DW.UnitDelay_DSTATE_o = UNICO_B.Sum1_i;

  /* End of Update for SubSystem: '<S3>/Extract Signals1' */

  /* Update for Delay: '<S10>/Delay One Step4' */
  UNICO_DW.DelayOneStep4_DSTATE = UNICO_B.check;

  /* Update for Delay: '<S14>/Delay One Step2' incorporates:
   *  Inport: '<Root>/SimTime'
   */
  UNICO_DW.DelayOneStep2_DSTATE = UNICO_U.SimTime;

  /* Update for UnitDelay: '<S27>/Unit Delay' */
  UNICO_DW.UnitDelay_DSTATE = UNICO_B.Sum1;

  /* Update for Delay: '<S10>/Delay One Step1' */
  UNICO_DW.DelayOneStep1_DSTATE = UNICO_B.PI_int;

  /* Update absolute time for base rate */
  /* The "clockTick0" counts the number of times the code of this task has
   * been executed. The absolute time is the multiplication of "clockTick0"
   * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
   * overflow during the application lifespan selected.
   * Timer of this task consists of two 32 bit unsigned integers.
   * The two integers represent the low bits Timing.clockTick0 and the high bits
   * Timing.clockTickH0. When the low bit overflows to 0, the high bits increment.
   */
  if (!(++UNICO_M->Timing.clockTick0)) {
    ++UNICO_M->Timing.clockTickH0;
  }

  UNICO_M->Timing.t[0] = UNICO_M->Timing.clockTick0 * UNICO_M->Timing.stepSize0
    + UNICO_M->Timing.clockTickH0 * UNICO_M->Timing.stepSize0 * 4294967296.0;
}

/* Model initialize function */
static void UNICO_initialize(void)
{
  /* ConstCode for Outport: '<Root>/Log3' */
  UNICO_Y.Log3 = 0.0;

  /* ConstCode for Outport: '<Root>/Log4' */
  UNICO_Y.Log4 = 0.0;

  /* ConstCode for Outport: '<Root>/Log5' */
  UNICO_Y.Log5 = 0.0;

  /* ConstCode for Outport: '<Root>/Log6' */
  UNICO_Y.Log6 = 0.0;

  /* ConstCode for Outport: '<Root>/Log7' */
  UNICO_Y.Log7 = 0.0;

  /* ConstCode for Outport: '<Root>/Log10' */
  UNICO_Y.Log10 = 0.0;

  /* ConstCode for Outport: '<Root>/Log8' */
  UNICO_Y.Log8 = 0.0;

  /* ConstCode for Outport: '<Root>/Log20' */
  UNICO_Y.Log20 = 0.0;

  /* ConstCode for Outport: '<Root>/Log12' */
  UNICO_Y.Log12 = 0.0;

  /* ConstCode for Outport: '<Root>/Log17' */
  UNICO_Y.Log17 = 0.0;

  /* ConstCode for Outport: '<Root>/Log18' */
  UNICO_Y.Log18 = 0.0;

  /* ConstCode for Outport: '<Root>/Log19' */
  UNICO_Y.Log19 = 0.0;

  /* ConstCode for Outport: '<Root>/Log13' */
  UNICO_Y.Log13 = 0.0;

  /* ConstCode for Outport: '<Root>/Log14' */
  UNICO_Y.Log14 = 0.0;

  /* ConstCode for Outport: '<Root>/Log15' */
  UNICO_Y.Log15 = 0.0;

  /* ConstCode for Outport: '<Root>/Log1' */
  UNICO_Y.Log1 = 0.0;

  /* ConstCode for Outport: '<Root>/Log16' */
  UNICO_Y.Log16 = 0.0;

  /* ConstCode for Outport: '<Root>/Log2' */
  UNICO_Y.Log2 = 0.0;

  /* ConstCode for Outport: '<Root>/Log9' */
  UNICO_Y.Log9 = 0.0;

  /* ConstCode for Outport: '<Root>/Log11' */
  UNICO_Y.Log11 = 0.0;

  /* ConstCode for Outport: '<Root>/Yaw_Rate' */
  UNICO_Y.Yaw_Rate = 0.0;

  {
    int32_T i;

    /* InitializeConditions for Delay: '<S3>/Delay One Step3' */
    UNICO_DW.DelayOneStep3_DSTATE = 0.0;

    /* InitializeConditions for Delay: '<S10>/Delay One Step4' */
    UNICO_DW.DelayOneStep4_DSTATE = 0.0;

    /* InitializeConditions for Delay: '<S14>/Delay One Step2' */
    UNICO_DW.DelayOneStep2_DSTATE = 0.0;

    /* InitializeConditions for UnitDelay: '<S27>/Unit Delay' */
    UNICO_DW.UnitDelay_DSTATE = 0.0;

    /* InitializeConditions for Delay: '<S10>/Delay One Step1' */
    UNICO_DW.DelayOneStep1_DSTATE = 0.0;

    /* SystemInitialize for Atomic SubSystem: '<S3>/Extract Signals1' */
    /* InitializeConditions for UnitDelay: '<S17>/Unit Delay' */
    UNICO_DW.UnitDelay_DSTATE_e = 0.0;

    /* InitializeConditions for UnitDelay: '<S19>/Unit Delay' */
    UNICO_DW.UnitDelay_DSTATE_o = 0.0;

    /* End of SystemInitialize for SubSystem: '<S3>/Extract Signals1' */

    /* SystemInitialize for Atomic SubSystem: '<S2>/Read tables' */
    /* SystemInitialize for MATLAB Function: '<S6>/Extract CP-TSR Table' */
    for (i = 0; i < 20; i++) {
      UNICO_DW.eml_openfiles[i] = NULL;
    }

    /* End of SystemInitialize for MATLAB Function: '<S6>/Extract CP-TSR Table' */

    /* SystemInitialize for Enabled SubSystem: '<S6>/Enabled Subsystem' */
    /* SystemInitialize for Outport: '<S7>/CpInv' */
    UNICO_DW.CpInvSgn_DIMS1[0] = 513;
    UNICO_DW.CpInvSgn_DIMS1[1] = 1;
    for (i = 0; i < 513; i++) {
      /* SystemInitialize for SignalConversion generated from: '<S7>/CpInvSgn' incorporates:
       *  Outport: '<S7>/CpInv'
       */
      UNICO_B.CpInvSgn_g[i] = 0.0;

      /* SystemInitialize for SignalConversion generated from: '<S7>/TSRSgn' incorporates:
       *  Outport: '<S7>/TSR'
       */
      UNICO_B.TSRSgn_j[i] = 0.0;
    }

    /* SystemInitialize for Outport: '<S7>/TSR' */
    UNICO_DW.TSRSgn_DIMS1[0] = 513;
    UNICO_DW.TSRSgn_DIMS1[1] = 1;

    /* End of SystemInitialize for SubSystem: '<S6>/Enabled Subsystem' */
    /* End of SystemInitialize for SubSystem: '<S2>/Read tables' */

    /* SystemInitialize for Enabled SubSystem: '<S1>/Enabled Subsystem' */
    /* SystemInitialize for MATLAB Function: '<S4>/Echo1' */
    for (i = 0; i < 20; i++) {
      UNICO_DW.eml_autoflush[i] = false;
    }

    for (i = 0; i < 20; i++) {
      UNICO_DW.eml_openfiles_d[i] = NULL;
    }

    /* End of SystemInitialize for MATLAB Function: '<S4>/Echo1' */
    /* End of SystemInitialize for SubSystem: '<S1>/Enabled Subsystem' */
  }
}

/* Model terminate function */
static void UNICO_terminate(void)
{
  /* (no terminate code required) */
}

/*========================================================================*
 * Start of Classic call interface                                        *
 *========================================================================*/
void MdlOutputs(int_T tid)
{
  UNICO_output();
  UNUSED_PARAMETER(tid);
}

void MdlUpdate(int_T tid)
{
  UNICO_update();
  UNUSED_PARAMETER(tid);
}

void MdlInitializeSizes(void)
{
}

void MdlInitializeSampleTimes(void)
{
}

void MdlInitialize(void)
{
}

void MdlStart(void)
{
  UNICO_initialize();
}

void MdlTerminate(void)
{
  UNICO_terminate();
}

/* Registration function */
RT_MODEL_UNICO_T *UNICO(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize real-time model */
  (void) memset((void *)UNICO_M, 0,
                sizeof(RT_MODEL_UNICO_T));

  /* Initialize timing info */
  {
    int_T *mdlTsMap = UNICO_M->Timing.sampleTimeTaskIDArray;
    mdlTsMap[0] = 0;

    /* polyspace +2 MISRA2012:D4.1 [Justified:Low] "UNICO_M points to
       static memory which is guaranteed to be non-NULL" */
    UNICO_M->Timing.sampleTimeTaskIDPtr = (&mdlTsMap[0]);
    UNICO_M->Timing.sampleTimes = (&UNICO_M->Timing.sampleTimesArray[0]);
    UNICO_M->Timing.offsetTimes = (&UNICO_M->Timing.offsetTimesArray[0]);

    /* task periods */
    UNICO_M->Timing.sampleTimes[0] = (0.008);

    /* task offsets */
    UNICO_M->Timing.offsetTimes[0] = (0.0);
  }

  rtmSetTPtr(UNICO_M, &UNICO_M->Timing.tArray[0]);

  {
    int_T *mdlSampleHits = UNICO_M->Timing.sampleHitArray;
    mdlSampleHits[0] = 1;
    UNICO_M->Timing.sampleHits = (&mdlSampleHits[0]);
  }

  rtmSetTFinal(UNICO_M, -1);
  UNICO_M->Timing.stepSize0 = 0.008;

  /* Setup for data logging */
  {
    static RTWLogInfo rt_DataLoggingInfo;
    rt_DataLoggingInfo.loggingInterval = (NULL);
    UNICO_M->rtwLogInfo = &rt_DataLoggingInfo;
  }

  /* Setup for data logging */
  {
    rtliSetLogXSignalInfo(UNICO_M->rtwLogInfo, (NULL));
    rtliSetLogXSignalPtrs(UNICO_M->rtwLogInfo, (NULL));
    rtliSetLogT(UNICO_M->rtwLogInfo, "");
    rtliSetLogX(UNICO_M->rtwLogInfo, "");
    rtliSetLogXFinal(UNICO_M->rtwLogInfo, "");
    rtliSetLogVarNameModifier(UNICO_M->rtwLogInfo, "rt_");
    rtliSetLogFormat(UNICO_M->rtwLogInfo, 0);
    rtliSetLogMaxRows(UNICO_M->rtwLogInfo, 1000);
    rtliSetLogDecimation(UNICO_M->rtwLogInfo, 1);
    rtliSetLogY(UNICO_M->rtwLogInfo, "");
    rtliSetLogYSignalInfo(UNICO_M->rtwLogInfo, (NULL));
    rtliSetLogYSignalPtrs(UNICO_M->rtwLogInfo, (NULL));
  }

  UNICO_M->solverInfoPtr = (&UNICO_M->solverInfo);
  UNICO_M->Timing.stepSize = (0.008);
  rtsiSetFixedStepSize(&UNICO_M->solverInfo, 0.008);
  rtsiSetSolverMode(&UNICO_M->solverInfo, SOLVER_MODE_SINGLETASKING);

  /* block I/O */
  UNICO_M->blockIO = ((void *) &UNICO_B);
  (void) memset(((void *) &UNICO_B), 0,
                sizeof(B_UNICO_T));

  /* states (dwork) */
  UNICO_M->dwork = ((void *) &UNICO_DW);
  (void) memset((void *)&UNICO_DW, 0,
                sizeof(DW_UNICO_T));

  /* external inputs */
  UNICO_M->inputs = (((void*)&UNICO_U));
  (void)memset(&UNICO_U, 0, sizeof(ExtU_UNICO_T));
  (void) memset((void *)&UNICO_U, 0,
                sizeof(ExtU_UNICO_T));

  /* external outputs */
  UNICO_M->outputs = (&UNICO_Y);
  (void)memset(&UNICO_Y, 0, sizeof(ExtY_UNICO_T));

  /* Initialize Sizes */
  UNICO_M->Sizes.numContStates = (0);  /* Number of continuous states */
  UNICO_M->Sizes.numY = (26);          /* Number of model outputs */
  UNICO_M->Sizes.numU = (57);          /* Number of model inputs */
  UNICO_M->Sizes.sysDirFeedThru = (1); /* The model is direct feedthrough */
  UNICO_M->Sizes.numSampTimes = (1);   /* Number of sample times */
  UNICO_M->Sizes.numBlocks = (116);    /* Number of blocks */
  UNICO_M->Sizes.numBlockIO = (12);    /* Number of block outputs */
  return UNICO_M;
}

/*========================================================================*
 * End of Classic call interface                                          *
 *========================================================================*/
