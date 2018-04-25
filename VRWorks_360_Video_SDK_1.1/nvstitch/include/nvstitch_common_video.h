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

#ifndef __NVSTITCH_COMMON_VIDEO_H__
#define __NVSTITCH_COMMON_VIDEO_H__

#include "nvstitch_common.h"

//! Camera type, with regards to its role in stereo stitching pipeline
typedef enum
{
    NVSTITCH_CAMERA_LAYOUT_EQUATORIAL = 0,          //!< Equatorial ring camera (default)
    NVSTITCH_CAMERA_LAYOUT_GENERAL,                 //!< Other cameras, not used for stereo stitching as of now

    NVSTITCH_CAMERA_LAYOUT_ENUM_SIZE  = 0x7fffffff // Force int32_t
}
nvstitchCameraLayout;

//! Lens distortion model, determines how the distortion coefficients in nvstitchCameraProperties_t are interpreted
typedef enum
{
    NVSTITCH_DISTORTION_TYPE_FISHEYE = 0,               //!< General fisheye model, described using 4 radial (k0, k1, k2, k3) distortion coefficients 
    NVSTITCH_DISTORTION_TYPE_BROWN,                     //!< Brown model, described using 3 radial (k0, k1, k4) and 2 tangential (k2, k3) distortion coefficients 

    NVSTITCH_DISTORTION_TYPE_ENUM_SIZE  = 0x7fffffff    // Force int32_t
}
nvstitchDistortionType;

//! Denotes which camera properties were set by user
//! The set_flags field in nvstitchCameraProperties_t contains a set of nvstitchCameraSetPropertyFlag values
//! Calibration assumes default values for all properties that are not set
//! All properties need to be set in order to use the camera for stitching
typedef enum
{
    CAMERA_PRINCIPAL_POINT_SET = 1 << 0,
    CAMERA_DISTORTION_TYPE_SET = 1 << 1,
    CAMERA_DISTORTION_COEFFICIENTS_SET = 1 << 2,
    CAMERA_FISHEYE_RADIUS_SET = 1 << 3,
    CAMERA_ROTATION_SET = 1 << 4,
    CAMERA_TRANSLATION_SET = 1 << 5,
    CAMERA_ALL_SET = (1 << 6) - 1,

    CAMERA_SET_FLAGS_ENUM_SIZE  = 0x7fffffff // Force int32_t
} nvstitchCameraSetPropertyFlag;

//! Video stitching quality presets, specifying the trade-off between quality and performance
typedef enum
{
    NVSTITCH_STITCHER_QUALITY_HIGH,                     //!< High quality/low performance option, applicable to offline stitching
    NVSTITCH_STITCHER_QUALITY_MEDIUM,                   //!< Balanced quality/performance option (real-time stitching with high-end systems)
    NVSTITCH_STITCHER_QUALITY_LOW,                      //!< Lower quality/high performance option, applicable to real-time stitching with a larger range of systems

    NVSTITCH_STITCHER_QUALITY_ENUM_SIZE = 0x7fffffff    // Force int32_t
}
nvstitchStitcherQuality;

//! Contains camera information, including its intrinsics and extrinsics
typedef struct nvstitchCameraProperties_st
{
    uint32_t version;                       //!< Structure version
    nvstitchCameraLayout camera_layout;     //!< Camera type, based on its orientation
    struct { uint32_t x, y; } image_size;   //!< Input resolution
    struct { float x, y; } principal_point; //!< Principal point (center of projection) of the image, offset from top left corner
    float focal_length;                     //!< Focal length of the lens, in pixels (per radian)
    nvstitchDistortionType distortion_type; //!< Distortion model of the lens
    float distortion_coefficients[5];       //!< Distortion coefficients of the lens
                                            //!< For fisheye distortion, this is 4 radial (k0, k1, k2, k3) distortion coefficients 
                                            //!< For brown distortion, this is 3 radial (k0, k1, k4) and 2 tangential (k2, k3) distortion coefficients 
    float fisheye_radius;                   //!< Radius of the fisheye circle, in pixels
    nvstitchPose_t extrinsics;              //!< Orientation of the camera, with translation relative to the center of the rig and using Y-up coordinate system
    uint32_t set_flags;                     //!< Reflects which fields in the structure are set by the user
}
nvstitchCameraProperties_t;

//! Camera rig information, including its position and properties of each camera
//! Used in nvstitchCreateVideoRigInstance and nvstitchGetVideoRigProperties
typedef struct nvstitchVideoRigProperties_st
{
    uint32_t version;                   //!< Structure version
    float rig_diameter;                 //!< Diameter of the camera ring, in cm
                                        //!< if set to 0 in input, default value will be used
    float rotation[3 * 3];              //!< Not yet supported
    uint32_t num_cameras;               //!< Number of cameras in the rig
    nvstitchCameraProperties_t* cameras;//!< Array of camera properties, needs to have num_cameras elements
}
nvstitchVideoRigProperties_t;

//! Structure describing a camera projection map
typedef struct nvstitchCameraMappingMatrix_st
{
    struct { uint32_t x, y; } map_offset;   //!< Location of the top left corner of the projection map in the output 360 panorama
    struct { uint32_t x, y; } map_size;     //!< Size of the projection map, in pixels
    struct { uint32_t x, y; } input_size;   //!< Size of the camera input, in pixels
    nvstitchCameraLayout layout;            //!< Camera type, based on its orientation
    float* matrix;                          //!< Projection map, containing an array of pairs of floating point coordinates mapping the camera input into equirectangular panorama
}
nvstitchCameraMappingMatrix_t;

//! Structure containing all projection maps for a given rig
typedef struct nvstitchCameraMapping_st
{
    uint32_t version;                               //!< Structure version
    uint32_t num_cameras;                           //!< Number of cameras in the rig
    struct { uint32_t x, y; } reference_resolution; //!< Equirectangluar panorama resolution for which the projection maps are defined
    float rig_diameter;                             //!< Estimated rig diameter, in centimeters
    nvstitchCameraMappingMatrix_t* matrices;        //!< Array of projection maps, needs to have num_cameras elements
}
nvstitchCameraMapping_t;

// ==== Utility ====

typedef enum
{
    NVSTITCH_EYE_LEFT = 0,  //!< Left eye omnidirectional panorama
    NVSTITCH_EYE_RIGHT = 1, //!< Right eye omnidirectional panorama
    NVSTITCH_EYE_MONO = 2   //!< Buffer containing a panorama suitable for mono viewing, in case of monoscopic stitching pipeline
}
nvstitchEye;

//! Pitch-linear image buffer in device memory
typedef struct nvstitchImageBuffer_st
{
    void* dev_ptr;      //!< Device pointer to the buffer
    size_t pitch;       //!< Pitch in bytes
    size_t row_bytes;   //!< Width in bytes
    size_t width;       //!< Width in pixels
    size_t height;      //!< Buffer height
}
nvstitchImageBuffer_t;

//! Used in nvstitchStitcherProperties_t and nvssVideoStitcherProperties_t to specify output panorama format
typedef enum
{
    NVSTITCH_PANORAMA_PROJECTION_EQUIRECTANGULAR = 0,   //!< Equirectangular panorama in 2:1 ratio (e.g. 3840x1920)

    NVSTITCH_PANORAMA_PROJECTION_ENUM_SIZE = 0x7fffffff    // Force int32_t
}
nvstitchPanoramaProjectionType;

//! Used in nvstitchVideoPipelineOptions_t and nvssVideoStitcherProperties_t to specify the stitching algorithm
typedef enum
{
    NVSTITCH_STITCHER_PIPELINE_NONE = 0,                //!< No stitching (no image output)
    NVSTITCH_STITCHER_PIPELINE_MONO,                    //!< Monoscopic stitching
    NVSTITCH_STITCHER_PIPELINE_STEREO,                  //!< Omnidirectional stereoscopic stitching

    NVSTITCH_STITCHER_PIPELINE_ENUM_SIZE = 0x7fffffff   // Force int32_t
}
nvstitchStitcherPipelineType;

#endif