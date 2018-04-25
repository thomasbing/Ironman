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

#ifndef __NVSTITCH_H__
#define __NVSTITCH_H__

#include "nvstitch_common_video.h"
#include "nvstitch_common_audio.h"

#ifdef __cplusplus
extern "C" {
#endif


// FUNCTION NAME:   nvstitchGetErrorString
//
//! DESCRIPTION:    Returns a string describing the nvstitch error code passed in as a parameter
//!
//! \param [in]     error   An nvstitchResult value for which the string description will be returned
//!
//! \return array of null terminated characters describing the input error code
const char* NVSTITCHAPI nvstitchGetErrorString(nvstitchResult error);

// ==== Video Rig ====

//! Video rig instance handle. Used to reference the camera rig after creation
typedef struct _nvstitchVideoRigInstance_t* nvstitchVideoRigHandle;

// FUNCTION NAME:   nvstitchCreateVideoRigInstance
//
//! DESCRIPTION:    Creates a video rig instance from the input properties and returns a handle to the newly created rig.
//!                 The handle is used in all API calls that take video rig as an input parameter
//!
//! \param [in]     properties          Pointer to the set of properties describing the rig, including the intrinsics and extrinsics of each camera
//! \param [out]    video_rig_instance  Pointer to the handle of the newly created rig
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_INVALID_VERSION
//! \retval ::NVSTITCH_ERROR_OUT_OF_SYSTEM_MEMORY
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
nvstitchResult NVSTITCHAPI nvstitchCreateVideoRigInstance(const nvstitchVideoRigProperties_t* properties,
                                                          nvstitchVideoRigHandle* video_rig_instance);

// FUNCTION NAME:   nvstitchDestroyVideoRigInstance
//
//! DESCRIPTION:    Destroys a video rig instance. The input nvstitchVideoRigHandle parameter is not modified,
//!                 but it becomes invalid after this call executes
//!
//! \param [in]     video_rig_instance  Video rig handle to be destroyed
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
nvstitchResult NVSTITCHAPI nvstitchDestroyVideoRigInstance(nvstitchVideoRigHandle video_rig_instance);

// FUNCTION NAME:   nvstitchGetVideoRigCameraProperties
//
//! DESCRIPTION:    Copies the properties of the camera with given index into the location referenced by the output camera parameter.
//!                 Camera indexes are based on the cameras array in the nvstitchVideoRigProperties_t struct.
//!
//! \param [in]     video_rig_instance  Video rig handle
//! \param [in]     index               Camera index in the video rig
//! \param [out]    camera              Pointer to the destination camera properties structure
//!
//! \retval ::NVSTITCH_SUCCESSO
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_BAD_INDEX
nvstitchResult NVSTITCHAPI nvstitchGetVideoRigCameraProperties(const nvstitchVideoRigHandle video_rig_instance,
                                                               const uint32_t index,
                                                               nvstitchCameraProperties_t* camera);
// FUNCTION NAME:   nvstitchGetVideoRigProperties
//
//! DESCRIPTION:    Copies the properties of the video rig into the location referenced by the output video_rig_properties parameter.
//!                 This includes properties of all cameras in the rig.
//!
//! \param [in]     video_rig_instance      Video rig handle
//! \param [out]    video_rig_properties    Pointer to the destination video rig properties structure
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
nvstitchResult NVSTITCHAPI nvstitchGetVideoRigProperties(const nvstitchVideoRigHandle video_rig_instance,
                                                         nvstitchVideoRigProperties_t* video_rig_properties);

//! Used in nvstitchVideoRigHints_t to specify hint type
typedef enum
{
    NVSTITCH_RIG_TYPE_RING = 0,

    NVSTITCH_RIG_TYPE_ENUM_SIZE = 0x7fffffff // Force int32_t
}
nvstitchVideoRigType;

//! Used to generate video rigs with estimated properties based on hints
typedef struct nvstitchVideoRigHints_st
{
    nvstitchVideoRigType rig_type; //!< Type of the rig hints in the hints union
    union
    {
        struct
        {
            uint32_t num_cameras;                   //!< Number of cameras in the rig
            float rig_diameter;                     //!< Estimated rig diameter, in centimeters.
            struct { uint32_t x, y; } image_size;   //!< resolution of the cameras in the rig  in pixels (assuming homogeneous rig).
            struct { float x, y; } fov;             //!< Field of view in degrees, corresponding to the image_size. Either one or both must be > 0.
            float roll;                             //!< The roll angle in degrees, increasing clockwise, typically { 0, -90, , +90 }.
        } ring;
    } hints;
}
nvstitchVideoRigHints_t;

// FUNCTION NAME:   nvstitchCreateVideoRigFromHints
//
//! DESCRIPTION:    Creates a video rig instance from the input hints. The resulting rig is a generic estimate,
//!                 but can be used as calibration input to compute the exact rig parameters.
//!
//! \param [in]     hints   Pointer to the input video rig hints that contain the estimates to be used for creating the rig
//! \param [out]    rig     Pointer to the handle of the newly created rig
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_CALIB_BAD_CAMERA_PROPERTY_VALUE
nvstitchResult NVSTITCHAPI nvstitchCreateVideoRigFromHints(const nvstitchVideoRigHints_t* hints,
                                                           nvstitchVideoRigHandle* rig);

// ==== Audio Rig ====

//! Audio rig instance handle. Used to reference the audio source rig after creation
typedef struct _nvstitchAudioRigInstance_t* nvstitchAudioRigHandle;

// FUNCTION NAME:   nvstitchCreateAudioRigInstance
//
//! DESCRIPTION:    Creates an audio rig instance from the input properties and returns a handle to the newly created rig.
//!                 The handle is used in all API calls that take audio rig as an input parameter
//!
//! \param [in]     properties          Pointer to the set of properties describing the rig, including the properties of each audio source
//! \param [out]    video_rig_instance  Pointer to the handle of the newly created rig
//! \param [out]    perSourceStatus     Array of results per audio source, currently not used
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_INVALID_VERSION
//! \retval ::NVSTITCH_ERROR_OUT_OF_SYSTEM_MEMORY
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
nvstitchResult NVSTITCHAPI nvstitchCreateAudioRigInstance(const nvstitchAudioRigProperties_t* properties,
                                                          nvstitchAudioRigHandle* audio_rig_instance,
                                                          nvstitchResult *perSourceStatus); 

// FUNCTION NAME:   nvstitchDestroyAudioRigInstance
//
//! DESCRIPTION:    Destroys an audio rig instance. The input nvstitchAudioRigHandle parameter is not modified,
//!                 but it becomes invalid after this call executes
//!
//! \param [in] audio_rig_instance  Audio rig handle to be destroyed
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
nvstitchResult NVSTITCHAPI nvstitchDestroyAudioRigInstance(nvstitchAudioRigHandle audio_rig_instance);

// FUNCTION NAME:   nvstitchGetAudioRigMicProperties
//
//! DESCRIPTION:    Copies the properties of the audio source with specified index into the location referenced by the output parameter.
//!                 Audio source indexes are based on the sources array in the nvstitchAudioRigProperties_t struct.
//!
//! \param [in]     audio_rig_instance  Audio rig handle
//! \param [in]     index               Audio source index in the audio rig
//! \param [out]    audio_source        Pointer to the destination audio source properties structure
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_BAD_PARAMETER
nvstitchResult NVSTITCHAPI nvstitchGetAudioRigMicProperties(const nvstitchAudioRigHandle audio_rig_instance,
                                                            const uint32_t index,
                                                            nvstitchAudioSourceProperties_t* audio_source);

// ==== In/Out specs ====

//! Supported image formats, used in nvstitchStitcherProperties_t
typedef enum
{
    NVSTITCH_MEDIA_FORMAT_RGBA8UI = 0,      //!< 32-bit rgb format with 8-bit channels
    NVSTITCH_MEDIA_FORMAT_YUV420,           //!< Planar YUV420, not yet supported
    NVSTITCH_MEDIA_FORMAT_H264_BITSTREAM,   //!< Stream of H264 frames
    NVSTITCH_MEDIA_FORMAT_MP4,              //!< MP4 file format (subset H.264+AAC)

    NVSTITCH_MEDIA_FORMAT_ENUM_SIZE = 0x7fffffff // Force int32_t
}
nvstitchMediaFormat;

//! Supported media forms, used in nvstitchStitcherProperties_t and nvstitchPayload_t
typedef enum
{
    NVSTITCH_MEDIA_FORM_NONE = 0,           //!< Empty payload (use with nvstitchGetStitcherOutputBuffer)
    NVSTITCH_MEDIA_FORM_HOST_BUFFER,        //!< System memory buffer
    NVSTITCH_MEDIA_FORM_DEVICE_BUFFER,      //!< GPU memory buffer
    NVSTITCH_MEDIA_FORM_FILE,               //!< File on a local HDD

    NVSTITCH_MEDIA_FORM_ENUM_SIZE = 0x7fffffff // Force int32_t
}
nvstitchMediaForm;

//! Media payload, describes properties of different input sources or output destinations
typedef struct nvstitchPayload_st
{
    nvstitchMediaForm payload_type;         //!< Media form described in the payload union
    struct { uint32_t x, y; } image_size;   //!< Image size, in pixels
    union
    {
        struct
        {
            void* ptr;                      //!< Pointer to the buffer containing the payload (system or GPU memory)
            uint32_t pitch;                 //!< Buffer pitch, in bytes
            int64_t timestamp;              //!< Timestamp of the payload, used to synchronize different input streams
        } buffer;
        struct
        {
            void* ptr;                      //!< Pointer to the buffer containing the payload (system or GPU memory)
            uint32_t size;                  //!< Size of the buffer, in bytes
            int64_t timestamp;              //!< Timestamp of the payload, used to synchronize different input streams
        } frame;
        struct
        {
            const char* name;               //!< File name
            uint32_t fps_num, fps_den;      //!< Video FPS, specified in numerator/denominator form
            float dt;                       //!< Seconds of audio/video to discard from the start of the file
        } file;
    } payload;
}
nvstitchPayload_t;


// ==== Calibration ====

//!< Specifies calibration parameters, used in nvstitchCreateCalibrationInstance
typedef struct nvstitchCalibrationProperties_st
{
    uint32_t version;                       //!< Structure version
    uint32_t frame_count;                   //!< Number of frames used for calibration
    nvstitchVideoRigHandle rig_estimate;    //!< video rig handle of the input rig, i.e. estimated rig parameters
    nvstitchMediaFormat input_format;       //!< Format of the input images that will be used for calibration
    nvstitchMediaForm input_form;           //!< Form   of the input images that will be used for calibration
}
nvstitchCalibrationProperties_t;

//! Calibration instance handle. Used to reference calibration instances after creation
typedef struct _nvstitchCalibrationInstance_t* nvstitchCalibrationInstanceHandle;

// FUNCTION NAME:   nvstitchCreateCalibrationInstance
//
//! DESCRIPTION:    Creates a calibration instance from the input properties and returns a handle to the newly created instance.
//!                 The handle is used as a parameter in subsequent API calls.
//!
//! \param [in]     properties              Pointer to the set of calibration parameters
//! \param [out]    calibration_instance    Pointer to the handle of the newly created calibration instance
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_BAD_PARAMETER
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_INVALID_VERSION
//! \retval ::NVSTITCH_CALIB_BAD_CAMERA_PROPERTY_VALUE
nvstitchResult NVSTITCHAPI nvstitchCreateCalibrationInstance(const nvstitchCalibrationProperties_t properties,
                                                             nvstitchCalibrationInstanceHandle* calibration_instance);

// FUNCTION NAME:   nvstitchFeedCalibrationInput
//
//! DESCRIPTION:    Feeds input from one of the cameras instance. In case when payload is a buffer, each call feeds a single frame.
//!                 In contrast, video files are fed in a single call.
//!
//! \param [in]     packet_index            Frame number of the input being fed
//! \param [in]     calibration_instance    Calibration instance handle
//! \param [in]     source_index            Index of the camera in the used video rig
//! \param [in]     payload                 A pointer to the payload structure describing the input
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_BAD_INDEX
//! \retval ::NVSTITCH_ERROR_BAD_PARAMETER
nvstitchResult NVSTITCHAPI nvstitchFeedCalibrationInput(uint32_t packet_index,
                                                        nvstitchCalibrationInstanceHandle calibration_instance,
                                                        uint32_t source_index,
                                                        const nvstitchPayload_t* payload);
// FUNCTION NAME:   nvstitchCalibrate
//
//! DESCRIPTION:    Run calibration and produce a calibrated rig.
//!                 All input images need to the fed in before this call executes
//!
//! \param [in]     calibration_instance    Calibration instance handle
//! \param [out]    calibrated_rig          Pointer to the output video rig handle, which is the result of the calibration process.
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_CALIB_MISSING_INPUT_IMAGE
//! \retval ::NVSTITCH_CALIB_BAD_INPUT_IMAGE
nvstitchResult NVSTITCHAPI nvstitchCalibrate(nvstitchCalibrationInstanceHandle calibration_instance,
                                             nvstitchVideoRigHandle* calibrated_rig);

// FUNCTION NAME:   nvstitchDestroyCalibrationInstance
//
//! DESCRIPTION:    Destroys a calibration instance. The input nvstitchCalibrationInstanceHandle parameter is not modified,
//!                 but it becomes invalid after this call executes
//!
//! \param [in]     calibration_instance    Calibration instance handle to be destroyed
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
nvstitchResult NVSTITCHAPI nvstitchDestroyCalibrationInstance(nvstitchCalibrationInstanceHandle calibration_instance);

//! Used in nvstitchCreateStitcher to specify video stitching options
typedef struct nvstitchVideoPipelineOptions_st
{
    nvstitchStitcherPipelineType pipeline_type; //!< type of stitching (mono, stereo)
    nvstitchStitcherQuality stitch_quality;     //!< stitching quality preset
    union
    {
        struct
        {
            float feather_width;                //!< width of the blend region used in multi-resolution blending
        } mono;
        struct
        {
            float ipd;                          //!< target interpupillary distance in the output pair of panoramas
                                                //!< if negative value is provided as input, the default value will be used
            int min_dist;                       //!< minimum required distance of objects from the camera rig, in cm
                                                //!< cannot be less than zero, defaults to 100 cm if 0 provided
        } stereo;
    } options;
}
nvstitchVideoPipelineOptions_t;

// ==== Encoder Settings ====

//! Used in nvstitchEncodeSettings_t
typedef enum
{
    NVSTITCH_ENCODER_AVG_BITRATE = 0,       //!< Setting that specifies average bitrate of the output video, in bits per second
    NVSTITCH_ENCODER_MAX_BITRATE,           //!< Setting that specifies maximum allowed bitrate of the output video, in bits per second

    NVSTITCH_ENCODER_ENUM_SIZE = 0x7fffffff // Force int32_t
}
nvstitchEncoderSettingType;

//! Used in nvstitchSetStitcherEncoderSetting to set output encoder settings
typedef struct nvstitchEncodeSettings_st
{
    nvstitchEncoderSettingType enc_setting_type;    //!< Type of the setting specified in the structure
    union
    {
        uint32_t bitrate;                              //!< target bitrate of the output video
    } settings;
}
nvstitchEncodeSettings_t;

//! Describes all properties used to create a stitcher instance
typedef struct nvstitchStitcherProperties_st
{
    uint32_t version;                                            //!< Structure version
    nvstitchVideoRigHandle video_rig;                            //!< Camera rig used for video input
    nvstitchAudioRigHandle audio_rig;                            //!< Set of microphones used for audio input

    nvstitchVideoPipelineOptions_t* video_pipeline_options;      //!< Video stitching pipeline options

    nvstitchMediaFormat input_format;                            //!< Format of the input images
    nvstitchMediaForm input_form;                                //!< Form of the input images (buffer, file)

    uint32_t num_audio_inputs;                                   //!< Number of audio inputs (must be same as number of inputs in rig)
    nvstitchAudioDataSource *audio_input_form;                   //!< Array of type of source (whether this is a file or a buffer based input)
    nvstitchAudioConfiguration_t **audio_input_configurations;   //!< Array of formats of sources (set the element corresponding to MP4 file to NULL, it will be read from demultiplexer)

    nvstitchPanoramaProjectionType output_projection;            //!< Projection type of the output 360 panorama
    nvstitchMediaFormat output_format;                           //!< Format of the output panorama
    uint32_t num_output_payloads;                                //!< Number of the output panorama descriptors
                                                                 //!< always one for mono stitching, can be either one or two for stereo
    nvstitchPayload_t* output_payloads;                          //!< Output panorama descriptor
                                                                 //!< if a single output is used for stereo, the output is in top-bottom format
                                                                 //!< if separate output buffers are used for left and right panoramas
                                                                 //!< left eye panorama will be save in the first buffer

    nvstitchAudioOutputType audio_output_blend;                  //!< Audio blend type
    nvstitchAudioDataSource audio_output_form;                   //!< Form of the audio output (buffer, file)
    nvstitchAudioConfiguration_t *audio_output_format;           //!< Format of the audio output
    float audio_output_gain;                                     //!< A linear gain applied to all output audio channels
    float audio_sound_field_parameters[NUM_SOUND_FIELD_PARAMETERS];//!< Sound field encoding parameters whose meaning depends on audio_output_blend
}
nvstitchStitcherProperties_t;

typedef struct _nvstitchStitcherHandle_t* nvstitchStitcherHandle;

// ==== Stitcher ====

// FUNCTION NAME:   nvstitchCreateStitcher
//
//! DESCRIPTION:    Creates a stitcher instance from the input properties and returns a handle to the newly created instance.
//!                 The handle is used as a parameter in subsequent API calls.
//!
//! \param [in]     properties  Pointer to the set of stitcher parameters, including the audio and video rig(s)
//! \param [out]    stitcher    Pointer to the handle of the newly created stitcher instance
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_BAD_PARAMETER
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_INVALID_VERSION
//! \retval ::NVSTITCH_ERROR_NOT_IMPLEMENTED
//! \retval ::NVSTITCH_ERROR_OUT_OF_SYSTEM_MEMORY
nvstitchResult NVSTITCHAPI nvstitchCreateStitcher(const nvstitchStitcherProperties_t* properties,
                                                  nvstitchStitcherHandle* stitcher);

// FUNCTION NAME:   nvstitchCreateStitcherUsingMaps
//
//! DESCRIPTION:    Creates a stitcher instance from the input properties and camera projection maps.
//!                 A handle to the newly created instance is returned via the stitcher parameter.
//!                 The handle is used as a parameter in subsequent API calls.
//!
//! \param [in]     properties  Pointer to the set of stitcher parameters, including the audio and video rig(s).
//!                             Camera intrinsics and extrinsics are not required when projection maps are used
//! \param [in]     cam_mapping Pointer to the structure containing projection maps
//! \param [out]    stitcher    Pointer to the handle of the newly created stitcher instance
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_INVALID_VERSION
//! \retval ::NVSTITCH_ERROR_OUT_OF_SYSTEM_MEMORY
nvstitchResult NVSTITCHAPI nvstitchCreateStitcherUsingMaps(const nvstitchStitcherProperties_t* properties,
                                                           const nvstitchCameraMapping_t* cam_mapping,
                                                           nvstitchStitcherHandle* stitcher);

// FUNCTION NAME:   nvstitchDestroyStitcher
//
//! DESCRIPTION:    Destroys the stitcher instance. The input nvstitchStitcherHandle parameter is not modified,
//!                 but it becomes invalid after this call executes
//!
//! \param [in]     stitcher    Stitcher instance handle to be destroyed
//!
//! \retval ::NVSTITCH_SUCCESS
nvstitchResult NVSTITCHAPI nvstitchDestroyStitcher(nvstitchStitcherHandle stitcher);

//! DESCRIPTION:    Stitcher callback type. Callbacks are fired when stitcher completes a frame
//!
//! \param [in]     packet_index        Packet index of the output payload, matching the index of the source input frames
//! \param [in]     out_payload         Pointer to the payload describing the stitched video/image output
//! \param [in]     out_audio_payload   Pointer to the payload describing the stitched audio output
//! \param [in]     app_data            Pointer to user defined data
typedef void (NVSTITCHCALLBACK *nvstitchStitcherOnOutput)(uint32_t packet_index,
                                                          const nvstitchPayload_t* out_payload,
                                                          const nvstitchAudioPayload_t* out_audio_payload,
                                                          void* app_data);

// FUNCTION NAME:   nvstitchStartStitcher
//
//! DESCRIPTION:    Sets the callback function and puts the stitcher into running state
//!                 Stitcher can accept feedInput calls only after this call executes.
//!                 Stitcher can only be started and stopped once. To stop and restart stitching, a new stitcher instance must be created.
//!
//! \param [in]     stitcher        Stitcher instance handle
//! \param [in]     output_callback Pointer to the callback function
//! \param [in]     app_data        Pointer to user defined data, which will be accessible in the callback
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_BAD_STATE
//! \retval ::NVSTITCH_ERROR_BAD_PARAMETER
//! \retval ::NVSTITCH_ERROR_GENERAL
nvstitchResult NVSTITCHAPI nvstitchStartStitcher(nvstitchStitcherHandle stitcher,
                                                 nvstitchStitcherOnOutput output_callback,
                                                 void* app_data);

// FUNCTION NAME:   nvstitchFeedStitcherVideo
//
//! DESCRIPTION:    Feeds input from a single camera into the stitcher. This input is a single frame
//!                 in case of buffer input, or a full video when ingesting mp4 files
//!
//! \param [in]     packet_index            Packet index of the input payload. This value will be propagated through
//!                                         the pipeline and returned with the matching output payload
//! \param [in]     stitcher                Stitcher instance handle
//! \param [in]     source_index            Index of the input's source camera
//! \param [in]     payload                 Pointer to the input payload
//! \param [in]     allow_dropping_frames   Boolean value that sets whether the pipeline blocks caller thread or
//!                                         drops input frames on overflow
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
nvstitchResult NVSTITCHAPI nvstitchFeedStitcherVideo(uint32_t packet_index,
                                                     nvstitchStitcherHandle stitcher,
                                                     uint32_t source_index,
                                                     const nvstitchPayload_t* payload,
                                                     bool allow_dropping_frames);

// FUNCTION NAME:   nvstitchFeedStitcherAudio
//
//! DESCRIPTION:    Feeds input from a single sound source into the stitcher. This input is a single audio frame
//!                 in case of buffer input, or a full audio file sequence
//!
//! \param [in]     packet_index    Not used, reserved for future use. Payload packet index
//! \param [in]     stitcher        Stitcher instance handle
//! \param [in]     source_index    Index of the input's source
//! \param [in]     payload         Pointer to the input payload
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
nvstitchResult NVSTITCHAPI nvstitchFeedStitcherAudio(uint32_t packet_index,
                                                     nvstitchStitcherHandle stitcher,
                                                     uint32_t source_index,
                                                     const nvstitchAudioPayload_t* payload);

// FUNCTION NAME:   nvstitchFeedStitcherAudioVideo
//
//! DESCRIPTION:    Feeds combined (muxed) input from a single camera and single sound source. The only input
//!                 currently supported in this function is an mp4 file containing audio
//!
//! \param [in]     packet_index            Packet index of the input payload. Unused for now
//! \param [in]     stitcher                Stitcher instance handle
//! \param [in]     source_index            Index of the input's camera
//! \param [in]     audio_source_index      Index of the input's audio source
//! \param [in]     stitcher                Stitcher instance handle
//! \param [in]     payload                 Pointer to the input payload
//! \param [in]     allow_dropping_frames   Boolean value that sets whether the pipeline blocks caller thread or
//!                                         drops input frames on overflow
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
nvstitchResult NVSTITCHAPI nvstitchFeedStitcherAudioVideo(uint32_t packet_index,
                                                          const nvstitchStitcherHandle stitcher,
                                                          uint32_t source_index,
                                                          uint32_t audio_source_index,
                                                          const nvstitchPayload_t* payload,
                                                          bool allow_dropping_frames);

// FUNCTION NAME:   nvstitchGetStitcherOutputBuffer
//
//! DESCRIPTION:    Copies info on stitcher output buffer into the location pointed to by the buffer parameter
//!                 In the case of stereoscopic stitching pipeline, left and right eye buffers are fetched separately
//!
//! \param [in]     stitcher    Stitcher instance handle
//! \param [in]     eye         Stereo eye index
//! \param [out]    buffer      Pointer to the output buffer structure
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_BAD_PARAMETER
//! \retval ::NVSTITCH_ERROR_BAD_STATE
nvstitchResult NVSTITCHAPI nvstitchGetStitcherOutputBuffer(const nvstitchStitcherHandle stitcher,
                                                           nvstitchEye eye,
                                                           nvstitchImageBuffer_t* buffer);

// FUNCTION NAME:   nvstitchGetStitcherOutputStream
//
//! DESCRIPTION:    Returns the CUDA stream which should be used when reading the output buffer to ensure correct synchronization.
//!                 Alternatively, the default stream can be used when reading the output buffer, but that can lead to performance loss.
//!
//! \param [in]     stitcher    Stitcher instance handle
//! \param [in]     eye         Index of the input's camera
//! \param [out]    stream      Pointer to the output CUDA stream
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_BAD_PARAMETER
//! \retval ::NVSTITCH_ERROR_BAD_STATE
nvstitchResult NVSTITCHAPI nvstitchGetStitcherOutputStream(const nvstitchStitcherHandle stitcher,
                                                           nvstitchEye eye,
                                                           struct CUstream_st** stream);

// FUNCTION NAME:   nvstitchGetStitcherOutputDevice
//
//! DESCRIPTION:    Returns the device index of device on which output buffer is stored, as associated in CUDA device enumeration
//!
//! \param [in]     stitcher    Stitcher instance handle
//! \param [in]     eye         Index of the input's camera
//! \param [out]    device      Pointer to the output device
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_BAD_PARAMETER
//! \retval ::NVSTITCH_ERROR_BAD_STATE
nvstitchResult NVSTITCHAPI nvstitchGetStitcherOutputDevice(const nvstitchStitcherHandle stitcher,
                                                           nvstitchEye eye,
                                                           int* device);

// FUNCTION NAME:   nvstitchStopStitcher
//
//! DESCRIPTION:    Puts the stitcher into a state where it no longer accepts input feed.
//!                 The call blocks until all remaining frames in the pipeline are stitched
//!                 Stitcher can only be started and stopped once. To restart stitching, a new stitcher instance must be created.
//!
//! \param [in]     stitcher    Stitcher instance handle
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
//! \retval ::NVSTITCH_ERROR_NOT_ALLOWED
nvstitchResult NVSTITCHAPI nvstitchStopStitcher(nvstitchStitcherHandle stitcher);

// FUNCTION NAME:   nvstitchSetStitcherEncoderSetting
//
//! DESCRIPTION:    Sets one of the supported encoder settings. The setting type is specified in the encode_settings parameter.
//!                 This function cannot be called after the stitcher has been put into running state.
//!
//! \param [in]     stitcher        Stitcher instance handle
//! \param [in]     encode_settings Structure specifying the encoder setting to set
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_BAD_PARAMETER
//! \retval ::NVSTITCH_ERROR_NOT_ALLOWED
nvstitchResult NVSTITCHAPI nvstitchSetStitcherEncoderSetting(nvstitchStitcherHandle stitcher,
                                                             nvstitchEncodeSettings_t encode_settings);

// FUNCTION NAME:   nvstitchGetLastErrorMessage
//
//! DESCRIPTION:    After a result other than NVSTITCH_SUCCESS is received, call this to get additional information about the error.
//!                 This string is ephemeral, and may change or become invalid after another nvstitch call,
//!                 so a copy should be made if persistency is desired.
//!
//! \param [out]    errorString A place to store a pointer to the error string.
//!
//! \retval ::NVSTITCH_SUCCESS
//! \retval ::NVSTITCH_ERROR_NULL_POINTER
nvstitchResult NVSTITCHAPI nvstitchGetLastErrorMessage(const char** errorString);



#ifdef __cplusplus
}
#endif

#endif
