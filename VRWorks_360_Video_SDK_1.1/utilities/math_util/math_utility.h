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

/** @file math_utility.h */

#ifndef MATH_UTILITY_H
#define MATH_UTILITY_H

#include "nvstitch_common.h"
#include "nvstitch_common_video.h"

namespace math_util
{
  //
  void convertPrincipalPointTopLeftToCenterOffset(
    int width, int height,
    float topLeftX, float topLeftY,
    float* centerX, float* centerY);

  void convertPrincipalPointCenterToTopLeftOffset(
    int width, int height,
    float centerX, float centerY,
    float* topLeftX, float* topLeftY);

  // Gets the focal length estimate from FOV
  bool getFocalLengthEstimateFromFov(
    int width, int height,
    float hfov, float vfov,
    nvstitchDistortionType distType,
    float validPixelRadius,
    float& focalLength
  );


  /********************************************************************************
   * Conversion of transforms specified in one basis to another.
   ********************************************************************************/
 
   /** Convert a transform specified in the Y-up basis to one specified in the Y-down basis.
   * @param[in]   fr  the "from" transform.
   * @param[out]  to  the  "to"  transform. This may be the same as fr.
   * @return      the to transform.
   */
  float* convertTransformYupToYdown(const float fr[9], float to[9]);

  /** Convert a transform specified in the Y-down basis to one specified in the Y-up basis.
   * @param[in]   fr  the "from" transform.
   * @param[out]  to  the  "to"  transform. This may be the same as fr. 
   * @return      the to transform.
   */
  float* convertTransformYdownToYup(const float fr[9], float to[9]);

  /** Convert a transform specified in the Y-up basis to one specified in the Z-up basis.
   * @param[in]   fr  the "from" transform.
   * @param[out]  to  the  "to"  transform. This may be the same as fr. 
   * @return      the to transform.
   */
  float* convertTransformYupToZup(const float fr[9], float to[9]);

  /** Convert a transform specified in the Z-up basis to one specified in the Y-up basis.
   * @param[in]   fr  the "from" transform.
   * @param[out]  to  the  "to"  transform. This may be the same as fr. 
   * @return      the to transform.
   */
  float* convertTransformZupToYup(const float fr[9], float to[9]);

  /** Convert a transform specified in the Z-up basis to one specified in the Y-down basis.
   * @param[in]   fr  the "from" transform.
   * @param[out]  to  the  "to"  transform. This may be the same as fr. 
   * @return      the to transform.
   */
  float* convertTransformZupToYdown(const float fr[9], float to[9]);

  /** Convert a transform specified in the Y-down basis to one specified in the Z-up basis.
   * @param[in]   fr  the "from" transform.
   * @param[out]  to  the  "to"  transform. This may be the same as fr. 
   * @return      the to transform.
   */
  float* convertTransformYdownToZup(const float fr[9], float to[9]);


  /********************************************************************************
   * Conversion of vectors specified in one basis to another.
   ********************************************************************************/
 
  /** Convert a vector from the Y-up basis to the Y-down basis.
   * @param[in]   fr  the "from" vector.
   * @param[out]  to  the  "to"  vector. This may be the same as fr. 
   * @return      the to vector.
   */
  float* convertVectorYupToYdown(const float fr[3], float to[3]);
  
  /** Convert a vector from the Y-down basis to the Y-up basis.
   * @param[in]   fr  the "from" vector.
   * @param[out]  to  the  "to"  vector. This may be the same as fr. 
   * @return      the to vector.
   */
  float* convertVectorYdownToYup(const float fr[3], float to[3]);
  
  /** Convert a vector from the Y-up basis to the Z-up basis.
   * @param[in]   fr  the "from" vector.
   * @param[out]  to  the  "to"  vector. This may be the same as fr. 
   * @return      the to vector.
   */
  float* convertVectorYupToZup  (const float fr[3], float to[3]);
  
  /** Convert a vector from the Z-up basis to the Y-up basis.
   * @param[in]   fr  the "from" vector.
   * @param[out]  to  the  "to"  vector. This may be the same as fr. 
   * @return      the to vector.
   */
  float* convertVectorZupToYup  (const float fr[3], float to[3]);
  
  /** Convert a vector from the Y-down basis to the Z-up basis.
   * @param[in]   fr  the "from" vector.
   * @param[out]  to  the  "to"  vector. This may be the same as fr. 
   * @return      the to vector.
   */
  float* convertVectorYdownToZup(const float fr[3], float to[3]);
  
  /** Convert a vector from the Z-up basis to the Y-down basis.
   * @param[in]   fr  the "from" vector.
   * @param[out]  to  the  "to"  vector. This may be the same as fr. 
   * @return      the to vector.
   */
  float* convertVectorZupToYdown(const float fr[3], float to[3]);


  /** Set the given transform to the identity.
   * @param[out]  mat the matrix to be set.
   */
  void setMatrixToIdentity(float mat[9]);

  /** Transpose a 3x3 matrix. Since the matrix represents an orthogonal transform, this inverts the transform.
   * This works in-place.
   * @param[in]   mat           the input matrix.
   * @param[out]  matTranspose  the output matrix.
   */
  void matrixTranspose(const float mat[9], float matTranspose[9]);

  float degToRad(float deg);


 

/********************************************************************************
 ********************************************************************************
 ** The following transforms have
 **    the Y-axis pointing up,
 **    the X-axis to the right, an
 **    the Z-axis pointing inward.
 ** The Euler angles, given in radians, are specified as
 **    Yaw   rotates rightward around the Y-axis.
 **    Pitch rotates  upward   around the X-axis.
 **    Roll  rotates clockwise around the Z-axis.
 ** Transforms are specified by their action on vectors, independently of matrices.
 ********************************************************************************
 ********************************************************************************/

 

/** Transform a vector.
 * w0 = T0 * v0 + T3 * v1 + T6 * v2
 * w1 = T1 * v0 + T4 * v1 + T7 * v2
 * w2 = T2 * v0 + T5 * v1 + T8 * v2
 * @param[in]   T     the transformation.
 * @param[in]   vin   the input vector.
 * @param[out]  vout  the output vector.
 * @return      the transformed vector.
 * @note This does not work in-place.
 */
float* transformVector3(const float T[9], const float vin[3], float vout[3]);


/** Transform a vector by the inverse of the specified orthogonal transformation.
 * w0 = T0 * v0 + T3 * v1 + T6 * v2
 * w1 = T1 * v0 + T4 * v1 + T7 * v2
 * w2 = T2 * v0 + T5 * v1 + T8 * v2
 * @param[in]   T     the transformation.
 * @param[in]   vin   the input vector.
 * @param[out]  vout  the output vector.
 * @return      the transformed vector.
 * @note this does not work if the transformation is not orthogonal (i.e. only rotation & reflections)
 * @note This does not work in-place.
 */
float* inverseTransformVector3(const float T[9], const float vin[3], float vout[3]);


/** Concatenate two transformations so that T1 comes first and T2 comes second.
 * @param[in]   T1  the first transformation.
 * @param[in]   T2  the second transformation.
 * @param[out]  T12 the concatenation of the first transformation with the second.
 * @return      the concatenated transformation.
 * @note This does not work in-place.
 */
float* concatenateTransformations(const float T1[9], const float T2[9], float T12[9]);


/** Rotate about the Y-up axis from X to Z.
 * @param[in] yaw   the yaw angle, in radians.
 * @param[out]  T   the yaw transformation.
 * @return      the yaw transformation.
 */
float* yawTransform(float yaw, float T[9]);


/** Rotate about the X-right axis from Y to Z.
 * @param[in]   pitch the pitch angle, in radians.
 * @param[out]  T     the pitch transformation.
 * @return      the pitch transformation.
 */
float* pitchTransform(float pitch, float T[9]);


/** Rotate about the Z-in axis from Y to X.
 * @param[in]   roll  the roll angle, in radians.
 * @param[out]  T     the roll transformation.
 * @return      the roll transformation.
 */
float* rollTransform(float roll, float T[9]);


/** Construct the transform taking the camera into the world.
 * @param[in]   yaw   the yaw   angle, in radians.
 * @param[in]   pitch the pitch angle, in radians.
 * @param[in]   roll  the roll  angle, in radians.
 * @param[out]  T     the concatenated transformation.
 * @return      the camera-to-world transformation.
 */
float* cameraToWorldTransform(float yaw, float pitch, float roll, float T[9]);


/** Construct the transform taking the world into the camera.
 * @param[in]   yaw   the yaw   angle, in radians.
 * @param[in]   pitch the pitch angle, in radians.
 * @param[in]   roll  the roll  angle, in radians.
 * @param[out]  T     the concatenated transformation.
 * @return      the world-to-camera transformation.
 */
float* worldToCameraTransform(float yaw, float pitch, float roll, float T[9]);


/** Convert a camera-to-world transform into yaw-pitch-roll angles.
 * @param[in]   T   the transform.
 * @param[out]  ypr the resultant yaw-pitch-roll angles.
 */
void cameraToWorldAngles(const float T[9], float ypr[3]);


/** Convert a world-to-camera transform into yaw-pitch-roll angles.
 * @param[in]   T   the transform.
 * @param[out]  ypr the resultant yaw-pitch-roll angles.
 */
void worldToCameraAngles(const float T[9], float ypr[3]);



}

#endif
