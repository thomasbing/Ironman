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

#ifndef __NVSS_VIDEO_H__
#define __NVSS_VIDEO_H__

#include <stdint.h>
#include "nvstitch_common_video.h"

// Forward declaration for CUDA API
// CUstream and cudaStream_t are CUstream_st*
struct CUstream_st;

#ifdef __cplusplus
extern "C" {
#endif

// ==== Stitcher properties ====
typedef enum
{
    NVSS_STITCHER_FORMAT_RGBA8UI,

    NVSS_STITCHER_FORMAT_ENUM_SIZE = 0x7fffffff // Force int32_t
}
nvssVideoStitcherFormat;

//! Property set used to create a stitcher instance.
typedef struct nvssVideoStitcherProperties_st
{
    uint32_t version;                           //!< structure version; set to NVSS_VERSION
    nvssVideoStitcherFormat format;             //!< the color format
    nvstitchStitcherPipelineType pipeline;      //!< type of stitching - monoscopic or stereoscopic
    nvstitchStitcherQuality quality;            //!< stitching quality preset; affects output quality and execution time
    nvstitchPanoramaProjectionType projection;  //!< projection type of 360 degree panorama output
    uint32_t pano_width;                        //!< width of output panorama
    float stereo_ipd;                           //!< interpupilary distance for output panorama pair, defaults to 6.3 if set to less than zero
    float feather_width;                        //!< width of the blending region in pixels used for monoscopic stitching
    uint32_t num_gpus;                          //!< number of GPUs to use; must specify at least 1 device, for monoscopic only single GPU supported
    const int* ptr_gpus;                        //!< array of CUDA device indices, cannot be null
    int min_dist;                               //!< minimum required distance of objects from camera rig, in cm
}
nvssVideoStitcherProperties_t;

// ==== Stitcher instance ====
typedef struct nvssVideo_t* nvssVideoHandle;

// Manage lifetime of CoreVideo instance

// FUNCTION NAME:   nvssVideoCreateInstance
//
//! DESCRIPTION:    This function creates an instance from the input properties and returns a handle to the newly created instance.
//!                 The handle is used as a parameter in subsequent API calls.
//!
//! \param [in]     stitcher_props          Pointer to the set of stitcher properties
//! \param [in]     rig_props               Pointer to the set of camera rig properties
//! \param [out]    handle                  Pointer to the handle of the newly created stitcher instance
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_INVALID_VERSION
//! \retval ::NVSTITCH_ERROR_BAD_PARAMETER
//! \retval ::NVSTITCH_ERROR_OUT_OF_SYSTEM_MEMORY
//! \retval ::NVSTITCH_VIDEO_UNSUPPORTED_RIG
extern nvstitchResult NVSTITCHAPI nvssVideoCreateInstance(const nvssVideoStitcherProperties_t* stitcher_props,
                                                          const nvstitchVideoRigProperties_t* rig_props,
                                                          nvssVideoHandle* handle);

// FUNCTION NAME:   nvssVideoCreateInstanceWithMapping
//
//! DESCRIPTION:    This function creates an instance from the input properties and camera projection maps.
//!                 A handle to the newly created instance is returned via the stitcher parameter.
//!                 The handle is used as a parameter in subsequent API calls. 
//!
//! \param [in]     stitcher_props          Pointer to the set of stitcher properties
//! \param [in]     cam_mappings            Pointer to the structure containing projection maps 
//! \param [out]    handle                  Pointer to the handle of the newly created stitcher instance
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_INVALID_VERSION
//! \retval ::NVSTITCH_ERROR_BAD_PARAMETER
//! \retval ::NVSTITCH_ERROR_OUT_OF_SYSTEM_MEMORY
extern nvstitchResult NVSTITCHAPI nvssVideoCreateInstanceWithMapping(const nvssVideoStitcherProperties_t* stitcher_props,
                                                                     const nvstitchCameraMapping_t* cam_mappings,
                                                                     nvssVideoHandle* handle);

// FUNCTION NAME:   nvssVideoDestroyInstance
//
//! DESCRIPTION:    This function destroys an instance. The input nvssVideoHandle parameter is not modified, but it becomes invalid after this call executes. 
//!
//! \param [in]     handle                  Stitcher instance handle to be destroyed
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
extern nvstitchResult NVSTITCHAPI nvssVideoDestroyInstance(nvssVideoHandle handle);

// Get input device memory pointer, stream, and device

// FUNCTION NAME:   nvssVideoGetInputBuffer
//
//! DESCRIPTION:    This function populates the image buffer structure with the address in device memory of the input buffer for the given camera index. 
//!                 The input data should be copied to here with cudaMemcpy. 
//!
//! \param [in]     handle                  Stitcher instance handle
//! \param [in]     camera                  Camera index
//! \param [out]    buffer                  Pointer to the output buffer structure 
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_BAD_PARAMETER
//! \retval ::NVSTITCH_ERROR_BAD_STATE
extern nvstitchResult NVSTITCHAPI nvssVideoGetInputBuffer(nvssVideoHandle handle, uint32_t camera,
                                                          nvstitchImageBuffer_t* buffer);

// FUNCTION NAME:   nvssVideoGetInputStream
//
//! DESCRIPTION:    This function returns the CUDA stream which should be used when writing to the input buffer in order to ensure correct synchronization. 
//!                 Alternatively, the default stream can be used when writing to the input buffer, but this can lead to performance loss.
//!
//! \param [in]     handle                  Stitcher instance handle
//! \param [in]     camera                  Camera index
//! \param [out]    stream                  Pointer to the output CUDA stream
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_BAD_PARAMETER
//! \retval ::NVSTITCH_ERROR_BAD_STATE
extern nvstitchResult NVSTITCHAPI nvssVideoGetInputStream(nvssVideoHandle handle, uint32_t camera,
                                                          struct CUstream_st** stream);

// FUNCTION NAME:   nvssVideoGetInputDevice
//
//! DESCRIPTION:    This function returns the device on which the input buffer of the specified camera is stored. 
//!                 The return value is a pointer to the device index, as associated in CUDA device enumeration. 
//!
//! \param [in]     handle                  Stitcher instance handle
//! \param [in]     camera                  Camera index
//! \param [out]    device                  Pointer to the device on which the input buffer is stored 
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_BAD_PARAMETER
//! \retval ::NVSTITCH_ERROR_BAD_STATE
extern nvstitchResult NVSTITCHAPI nvssVideoGetInputDevice(nvssVideoHandle handle, uint32_t camera, int* device);

// Get output device memory pointer, stream, and device

// FUNCTION NAME:   nvssVideoGetOutputBuffer
//
//! DESCRIPTION:    This function populates the image buffer structure with the address in device memory of the output buffer 
//!                 for the given eye: left or right eye for stereoscopic, or mono for monoscopic. 
//!                 The output data should be copied from here with cudaMemcpy().  
//!
//! \param [in]     handle                  Stitcher instance handle
//! \param [in]     eye                     Eye index (left, right, mono) 
//! \param [out]    buffer                  Pointer to the output buffer structure 
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_BAD_PARAMETER
//! \retval ::NVSTITCH_ERROR_BAD_STATE
extern nvstitchResult NVSTITCHAPI nvssVideoGetOutputBuffer(nvssVideoHandle handle, nvstitchEye eye,
                                                           nvstitchImageBuffer_t* buffer);

// FUNCTION NAME:   nvssVideoGetOutputStream
//
//! DESCRIPTION:    This function returns the CUDA stream which should be used when reading the output buffer in order to ensure correct synchronization. 
//!                 Alternatively, the default stream can be used when reading the output buffer, but that can lead to performance loss. 
//!
//! \param [in]     handle                  Stitcher instance handle
//! \param [in]     eye                     Eye index (left, right, mono) 
//! \param [out]    stream                  Pointer to the output CUDA stream
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_BAD_PARAMETER
//! \retval ::NVSTITCH_ERROR_BAD_STATE
extern nvstitchResult NVSTITCHAPI nvssVideoGetOutputStream(nvssVideoHandle handle, nvstitchEye eye,
                                                           struct CUstream_st** stream);

// FUNCTION NAME:   nvssVideoGetOutputDevice
//
//! DESCRIPTION:    This function returns the device on which the output buffer is stored. 
//!                 The return value is a pointer to the device index, as associated in CUDA device enumeration. 
//!
//! \param [in]     handle                  Stitcher instance handle
//! \param [in]     eye                     Eye index (left, right, mono) 
//! \param [out]    device                  Pointer to the output device
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_BAD_PARAMETER
//! \retval ::NVSTITCH_ERROR_BAD_STATE
extern nvstitchResult NVSTITCHAPI nvssVideoGetOutputDevice(nvssVideoHandle handle, nvstitchEye eye, int* device);

// Run the stitcher (asynchronous)

// FUNCTION NAME:   nvssVideoStitch
//
//! DESCRIPTION:    This function stitches the current content of the input buffers and stores the result in output buffers accessible through nvssVideoGetOutputBuffer(). 
//!                 The call is asynchronous, meaning that the function returns before the output is ready. 
//!                 CUDA calls using the default stream or the stream returned by nvssVideoGetOutputStream() will synchronize with the output. 
//!
//! \param [in]     handle                  Stitcher instance handle
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_BAD_STATE
extern nvstitchResult NVSTITCHAPI nvssVideoStitch(nvssVideoHandle handle);

// FUNCTION NAME:   nvssVideoGetLastErrorMessage
//
//! DESCRIPTION:    After a result other than NVSTITCH_SUCCESS is received, call this to get additional information about the error.
//!                 This string is ephemeral, and may change or become invalid after another nvstitch call,
//!                 so a copy should be made if persistence is desired.
//!
//! \param [out]    errorString             A place to store a pointer to the error string.
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
nvstitchResult NVSTITCHAPI nvssVideoGetLastErrorMessage(const char** errorString);

// FUNCTION NAME:   nvssVideoGetErrorString
//
//! DESCRIPTION:    This function returns a string describing the nvstitch error code passed in as a parameter.
//!
//! \param [in]     error                   An nvstitchResult value for which the string description will be returned.
//!
//! \retval ::This returns an array of null-terminated characters describing the input error code
const char* NVSTITCHAPI nvssVideoGetErrorString(nvstitchResult error);

#ifdef __cplusplus
}
#endif

#endif // __NVSS_VIDEO_H__
