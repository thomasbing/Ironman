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

#ifndef __NVSTITCH_COMMON_H__
#define __NVSTITCH_COMMON_H__

#if defined(_WIN32) || defined(_WIN64)
#define NVSTITCHAPI __stdcall
#define NVSTITCHCALLBACK __stdcall
#if (_MSC_VER < 1800) //VS2013
#else
#include <stdint.h>
#endif
#else
#define NVSTITCHAPI
#define NVSTITCHCALLBACK
#endif

#define NVSTITCH_VERSION_MAJOR 1
#define NVSTITCH_VERSION_MINOR 1
#define NVSTITCH_VERSION_RELEASE 0
#define NVSTITCH_VERSION ((NVSTITCH_VERSION_MAJOR << 16) | (NVSTITCH_VERSION_MINOR << 8) | NVSTITCH_VERSION_RELEASE)

typedef enum
{
    NVSTITCH_SUCCESS = 0x00000000,
    NVSTITCH_ERROR_GENERAL,
    NVSTITCH_ERROR_BAD_STATE,
    NVSTITCH_ERROR_BAD_PARAMETER,
    NVSTITCH_ERROR_BAD_INDEX,
    NVSTITCH_ERROR_INVALID_VERSION,
    NVSTITCH_ERROR_OUT_OF_SYSTEM_MEMORY,
    NVSTITCH_ERROR_OUT_OF_VIDEO_MEMORY,
    NVSTITCH_ERROR_UNSUPPORTED_CONFIG,
    NVSTITCH_ERROR_INVALID_DEVICE,
    NVSTITCH_ERROR_NOT_IMPLEMENTED,
    NVSTITCH_ERROR_MISSING_FILE,
    NVSTITCH_ERROR_NULL_POINTER,
    NVSTITCH_ERROR_NOT_ALLOWED,
    NVSTITCH_ERROR_ENCODER_INIT_FAILED,
    NVSTITCH_ERROR_CUDA_ERROR,
    NVSTITCH_ERROR_MISSING_DEPENDENCY,
    NVSTITCH_ERROR_DECODER_INIT_FAILED,

    NVSTITCH_VIDEO_UNSUPPORTED_RIG = 0x00010000,

    NVSTITCH_CALIB_FAILED_CONVERGENCE = 0x00020000,
    NVSTITCH_CALIB_BAD_INPUT_IMAGE,
    NVSTITCH_CALIB_MISSING_INPUT_IMAGE,
    NVSTITCH_CALIB_BAD_CAMERA_PROPERTY_VALUE,
    NVSTITCH_CALIB_INSUFFICIENT_FEATURES,

    NVSTITCH_AUDIO_UNSUPPORTED_CODEC = 0x00030000,
    NVSTITCH_AUDIO_BUFFER_FULL,
    NVSTITCH_AUDIO_CONFIGURATION_COMMITTED,
    NVSTITCH_AUDIO_CONFIGURATION_NOT_COMMITTED,
    NVSTITCH_AUDIO_OUT_OF_DATA,
    NVSTITCH_AUDIO_INSUFFICIENT_OUTPUT_BUFFER,
    NVSTITCH_AUDIO_ALL_STREAMS_ENDED,
    NVSTITCH_AUDIO_CONFIGURATION_MISMATCH
} nvstitchResult;

//! Used to describe position in 3D space (rotation and translation)
typedef struct nvstitchPose_st
{
    float rotation[3 * 3];  //!< A 3x3 row major rotation matrix
                            //!< Coordinate system is X-right, Y-up, Z-in
    float translation[3];   //!< Translation in centimeters
}
nvstitchPose_t;

#endif
