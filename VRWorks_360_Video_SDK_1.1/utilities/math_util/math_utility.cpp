/*
* Copyright 1993-2017 NVIDIA Corporation.  All rights reserved.
*
* NOTICE TO LICENSEE:
*
* This source code and/or documentation ("Licensed Deliverables") are
* subject to NVIDIA intellectual property rights under U.S. and
* international Copyright laws.
*
* These Licensed Deliverables contained herein is PROPRIETARY and
* CONFIDENTIAL to NVIDIA and is being provided under the terms and
* conditions of a form of NVIDIA software license agreement by and
* between NVIDIA and Licensee ("License Agreement") or electronically
* accepted by Licensee.  Notwithstanding any terms or conditions to
* the contrary in the License Agreement, reproduction or disclosure
* of the Licensed Deliverables to any third party without the express
* written consent of NVIDIA is prohibited.
*
* NOTWITHSTANDING ANY TERMS OR CONDITIONS TO THE CONTRARY IN THE
* LICENSE AGREEMENT, NVIDIA MAKES NO REPRESENTATION ABOUT THE
* SUITABILITY OF THESE LICENSED DELIVERABLES FOR ANY PURPOSE.  IT IS
* PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.
* NVIDIA DISCLAIMS ALL WARRANTIES WITH REGARD TO THESE LICENSED
* DELIVERABLES, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY,
* NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
* NOTWITHSTANDING ANY TERMS OR CONDITIONS TO THE CONTRARY IN THE
* LICENSE AGREEMENT, IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY
* SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, OR ANY
* DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
* WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
* ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
* OF THESE LICENSED DELIVERABLES.
*
* U.S. Government End Users.  These Licensed Deliverables are a
* "commercial item" as that term is defined at 48 C.F.R. 2.101 (OCT
* 1995), consisting of "commercial computer software" and "commercial
* computer software documentation" as such terms are used in 48
* C.F.R. 12.212 (SEPT 1995) and is provided to the U.S. Government
* only as a commercial end item.  Consistent with 48 C.F.R.12.212 and
* 48 C.F.R. 227.7202-1 through 227.7202-4 (JUNE 1995), all
* U.S. Government End Users acquire the Licensed Deliverables with
* only those rights set forth herein.
*
* Any use of the Licensed Deliverables in individual and commercial
* software must include, in the user documentation and internal
* comments to the code, the above Disclaimer and U.S. Government End
* Users Notice.
*/

#include <vector>
#include <algorithm>

#define _USE_MATH_DEFINES
#include <math.h>
#include "math_utility.h"

namespace math_util
{
  // Functions in header

  void convertPrincipalPointTopLeftToCenterOffset(
    int width, int height,
    float topLeftX, float topLeftY,
    float* centerX, float* centerY)
  {
    *centerX = topLeftX - (width  - 1) * .5f;
    *centerY = topLeftY - (height - 1) * .5f;
  }

  void convertPrincipalPointCenterToTopLeftOffset(
    int width, int height,
    float centerX, float centerY,
    float* topLeftX, float* topLeftY
  )
  {
    *topLeftX = centerX + (width  - 1) * .5f;
    *topLeftY = centerY + (height - 1) * .5f;
  }

  bool getFocalLengthEstimateFromFov(
    int width, int height,
    float hfov, float vfov,
    nvstitchDistortionType distType,
    float validPixelRadius,
    float& focalLength
  )
  {
    if (distType == nvstitchDistortionType::NVSTITCH_DISTORTION_TYPE_BROWN)
    {
      if (hfov)
      {
        focalLength = (width * 0.5f) / atan(hfov * 0.5f * M_PI / 180.0f);
        return true;
      }
      else if (vfov)
      {
        focalLength = (height * 0.5f) / atan(vfov * 0.5f * M_PI / 180.0f);
        return true;
      }
      else
        return false;
    }
    else if (distType == nvstitchDistortionType::NVSTITCH_DISTORTION_TYPE_FISHEYE)
    {
      if (hfov)
      {
        const double radius = std::min(validPixelRadius, width * 0.5f);
        focalLength = (radius / (hfov * 0.5f * M_PI / 180.0f));
        return true;
      }
      else if (vfov)
      {
        const double radius = std::min(validPixelRadius, height * 0.5f);
        focalLength = (radius / (vfov * 0.5f * M_PI / 180.0f));
        return true;
      }
      else
        return false;
    }
    return false;
  }

  void setMatrixToIdentity(float mat[9])
  {
    mat[1] = mat[2] = mat[3] = mat[5] = mat[6] = mat[7] = 0.0f;
    mat[0] = mat[4] = mat[8] = 1.0f;
  }

  void matrixTranspose(const float mat[9], float matTranspose[9])
  {
    float tmp[9] = {
      mat[0], mat[3], mat[6],
      mat[1], mat[4], mat[7],
      mat[2], mat[5], mat[8]
    };
    memcpy(matTranspose, tmp, sizeof(tmp));
  }


  /* The following transforms have the Y-axis pointing up, the X-axis to the right, and the Z-axis pointing inward.
   * Yaw   rotates rightward around the Y-axis.
   * Pitch rotates  upward   around the X-axis.
   * Roll  rotates clockwise around the Z-axis.
   */

  #define SET_TRANSFORM(T, t0, t1, t2, t3, t4, t5, t6, t7, t8) \
    do { T[0] = t0; T[1] = t1; T[2] = t2; T[3] = t3; T[4] = t4; T[5] = t5; T[6] = t6; T[7] = t7; T[8] = t8; } while(0)
 

  float* transformVector3(const float T[9], const float vin[3], float vout[3])
  {
    vout[0] = (float)((double)T[0] * vin[0] + T[3] * vin[1] + T[6] * vin[2]);
    vout[1] = (float)((double)T[1] * vin[0] + T[4] * vin[1] + T[7] * vin[2]);
    vout[2] = (float)((double)T[2] * vin[0] + T[5] * vin[1] + T[8] * vin[2]);
    return vout;
  }

  float* inverseTransformVector3(const float T[9], const float vin[3], float vout[3])
  {
    vout[0] = (float)((double)T[0] * vin[0] + T[1] * vin[1] + T[2] * vin[2]);
    vout[1] = (float)((double)T[3] * vin[0] + T[4] * vin[1] + T[5] * vin[2]);
    vout[2] = (float)((double)T[6] * vin[0] + T[7] * vin[1] + T[8] * vin[2]);
    return vout;
  }

  float* concatenateTransformations(const float T1[9], const float T2[9], float T12[9])
  {
    transformVector3(T2, T1+0, T12+0);
    transformVector3(T2, T1+3, T12+3);
    transformVector3(T2, T1+6, T12+6);
    return T12;
  }

  float* yawTransform(float yaw, float T[9])
  {
    const float c = cosf(yaw);
    const float s = sinf(yaw);
    SET_TRANSFORM(T, c, 0, s, 0, 1, 0, -s, 0, c);
    return T;
  }

  float* pitchTransform(float pitch, float T[9])
  {
    const float c = cosf(pitch);
    const float s = sinf(pitch);
    SET_TRANSFORM(T, 1, 0, 0, 0, c, s, 0, -s, c);
    return T;
  }

  float* rollTransform(float roll, float T[9])
  {
    const float c = cosf(roll);
    const float s = sinf(roll);
    SET_TRANSFORM(T, c, -s, 0, s, c, 0, 0, 0, 1);
    return T;
  }

  float* cameraToWorldTransform(float yaw, float pitch, float roll, float T[9])
  {
    float Y[9], P[9], R[9], S[9];
    return concatenateTransformations(concatenateTransformations(rollTransform(roll, R), pitchTransform(pitch, P), S), yawTransform(yaw, Y), T);
  }

  float* worldToCameraTransform(float yaw, float pitch, float roll, float T[9])
  {
    float Y[9], P[9], R[9], S[9];
    return concatenateTransformations(concatenateTransformations(yawTransform(-yaw, Y), pitchTransform(-pitch, P), S), rollTransform(-roll, R), T);
  }

  void cameraToWorldAngles(const float T[9], float ypr[3])
  {
    float r = hypotf(T[1], T[4]);
    if (r)
    {
      ypr[2] = atan2f(-T[1], T[4]);
      ypr[0] = atan2f(-T[6], T[8]);
      ypr[1] = atan2f(-T[7], r);
    }
    else
    {
      ypr[2] = 0.;
      ypr[1] = asinf(T[7]);
      ypr[0] = atan2f(T[2], T[0]);
    }
  }

  void worldToCameraAngles(const float T[9], float ypr[3])
  {
    float r = hypotf(T[3], T[4]);
    if (r)
    {
      ypr[2] = atan2f(-T[3], T[4]);
      ypr[0] = atan2f(-T[2], T[8]);
      ypr[1] = atan2f(-T[5], r);
    }
    else
    {
      ypr[2] = 0.;
      ypr[1] = asinf(T[5]);
      ypr[0] = atan2f(T[6], T[0]);
    }
  }

  float* convertVectorYupToYdown(const float fr[3], float to[3])
  {
    float v[3] = { fr[0], -fr[1], -fr[2] };
    memcpy(to, v, sizeof(v));
    return to;
  }

  float* convertVectorYdownToYup(const float fr[3], float to[3])
  {
    float v[3] = { fr[0], -fr[1], -fr[2] };
    memcpy(to, v, sizeof(v));
    return to;
  }

  float* convertVectorYupToZup(const float fr[3], float to[3])
  {
    float v[3] = { fr[0], -fr[2], fr[1] };
    memcpy(to, v, sizeof(v));
    return to;
  }

  float* convertVectorZupToYup(const float fr[3], float to[3])
  {
    float v[3] = { fr[0], fr[2], -fr[1] };
    memcpy(to, v, sizeof(v));
    return to;
  }

  float* convertVectorYdownToZup(const float fr[3], float to[3])
  {
    float v[3] = { fr[0], fr[2], -fr[1] };
    memcpy(to, v, sizeof(v));
    return to;
  }

  float* convertVectorZupToYdown(const float fr[3], float to[3])
  {
    float v[3] = { fr[0], -fr[2], fr[1] };
    memcpy(to, v, sizeof(v));
    return to;
  }


  float* convertTransformYupToYdown(const float fr[9], float to[9])
  {
    float M[9] = { +fr[0], -fr[1], -fr[2], -fr[3], +fr[4], +fr[5], -fr[6], +fr[7], +fr[8] };
    memcpy(to, M, sizeof(M));
    return to;
  }

  float* convertTransformYdownToYup(const float fr[9], float to[9])
  {
    float M[9] = { +fr[0], -fr[1], -fr[2], -fr[3], +fr[4], +fr[5], -fr[6], +fr[7], +fr[8] };
    memcpy(to, M, sizeof(M));
    return to;
  }

  float* convertTransformYupToZup(const float fr[9], float to[9])
  {
    float M[9] = { +fr[0], -fr[2], +fr[1], -fr[6], +fr[8], -fr[7], +fr[3], -fr[5], +fr[4] };
    memcpy(to, M, sizeof(M));
    return to;
  }

  float* convertTransformZupToYup(const float fr[9], float to[9])
  {
    float M[9] = { +fr[0], +fr[2], -fr[1], +fr[6], +fr[8], -fr[7], -fr[3], -fr[5], +fr[4] };
    memcpy(to, M, sizeof(M));
    return to;
  }

  float* convertTransformYdownToZup(const float fr[9], float to[9])
  {
    float M[9] = { +fr[0], +fr[2], -fr[1], +fr[6], +fr[8], -fr[7], -fr[3], -fr[5], +fr[4] };
    memcpy(to, M, sizeof(M));
    return to;
  }

  float* convertTransformZupToYdown(const float fr[9], float to[9])
  {
    float M[9] = { +fr[0], -fr[2], +fr[1], -fr[6], +fr[8], -fr[7], +fr[3], -fr[5], +fr[4] };
    memcpy(to, M, sizeof(M));
    return to;
  }

  float degToRad(float deg)
  {
    return (float)((deg * M_PI) / 180.0);
  }
}

