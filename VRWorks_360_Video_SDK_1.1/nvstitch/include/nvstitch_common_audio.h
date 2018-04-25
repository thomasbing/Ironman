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

#ifndef __NVSTITCH_COMMON_AUDIO_H__
#define __NVSTITCH_COMMON_AUDIO_H__

#include "nvstitch_common.h"

//! Indicates the encoding type or data format of an audio stream
typedef enum
{
    NVSTITCH_AUDIO_FORMAT_PCM16LE = 1, //!< Little Endian 16 bit signed PCM
    NVSTITCH_AUDIO_FORMAT_PCM24LE,     //!< Little Endian 24 bit signed PCM
    NVSTITCH_AUDIO_FORMAT_PCM32LE,     //!< Little Endian 32 bit signed PCM
    NVSTITCH_AUDIO_FORMAT_PCM32FLOAT,  //!< Single precision (32 bit) floating point PCM
    NVSTITCH_AUDIO_FORMAT_AAC_ADTS,    //!< AAC Stream with ADTS headers
    NVSTITCH_AUDIO_FORMAT_AAC_LC_RAW,  //!< RAW AAC Stream

    NVSTITCH_AUDIO_FORMAT_LAST,
    NVSTITCH_AUDIO_FORMAT_ENUM_SIZE = 0x7fffffff // Force int32_t
}
nvstitchAudioFormat;

// ! Indicates whether the audio data is read from a file or presented to the API in memory buffers
typedef enum
{
    NVSTITCH_AUDIO_DATA_SOURCE_FILE = 0,  //!< Payload will have a filename which API will read
    NVSTITCH_AUDIO_DATA_SOURCE_BUFFER,    //!< Payloads will contain buffers of audio data

    NVSTITCH_AUDIO_DATA_SOURCE_LAST,
    NVSTITCH_AUDIO_DATA_SOURCE_ENUM_SIZE = 0x7fffffff // Force int32_t
}
nvstitchAudioDataSource;

//!< Indicates the type of an audio soucre
typedef enum
{
    NVSTITCH_AUDIO_INPUT_TYPE_OMNI = 0,    //!< Each channel of this input is treated as independent and having no directional information, 
	NVSTITCH_AUDIO_INPUT_TYPE_SHOTGUN_MIC, //!< Each channel of this input is treated as independent and having identical directionality with 
	                                       //!  a shotgun microphone-style pickup pattern
	NVSTITCH_AUDIO_INPUT_TYPE_CARDIOID_MIC, //!< Each channel of this input is treated as independent and having identical directionality with 
	                                       //!  a cardiod microphone-style pickup pattern
	NVSTITCH_AUDIO_INPUT_TYPE_SUPERCARDIOID_MIC, //!< Each channel of this input is treated as independent and having identical directionality with 
	                                       //!  a shotgun supercardiod-style pickup pattern

    NVSTITCH_AUDIO_INPUT_TYPE_LAST,
    NVSTITCH_AUDIO_INPUT_TYPE_ENUM_SIZE = 0x7fffffff // Force int32_t
}
nvstitchAudioInputType;

//! Contains information describing the audio data format for a single input or output
typedef struct nvstitchAudioConfiguration_st
{
    uint32_t version;               //!< Set to NVSTITCH_VERSION
    nvstitchAudioFormat format;     //!< Data format of audio
    uint32_t channels;              //!< Number of audio channels 
    uint32_t sampleRate;            //!< Audio sampling Rate
    uint32_t bitRate;               //!< bit Rate
    uint32_t samplesPerFrame;       //!< Samples per audio-frame
} nvstitchAudioConfiguration_t;

//! Contains information about an audio source used in creating the sound field
typedef struct nvstitchAudioSourceProperties_st
{
    uint32_t version;                       //!< Set to NVSTITCH_VERSION
    nvstitchAudioInputType input_type;      //!< Type of input
    nvstitchPose_t pose;                    //!< Pose, location and translation, of audio source or capture device
}
nvstitchAudioSourceProperties_t;

//! A rig is a set of audio sources. 
typedef struct nvstitchAudioRigProperties_st
{
    uint32_t version;                          //!< Set to NVSTITCH_VERSION
    uint32_t num_sources;                      //!< Number of audio sources
    nvstitchAudioSourceProperties_t* sources;  //!< Array of length num_sources of nvstitchAudioSourceProperties_t, one for each audio source
}
nvstitchAudioRigProperties_t;

//! An audio payload contains audio data in buffers or the name of a file from which data can be read
typedef struct nvstitchAudioPayload_st
{
    union
    {
        struct {void* ptr; uint64_t timestamp; uint32_t size; } buffer;   //!< Fill when nvstitchAudioDataSource is NVSTITCH_AUDIO_DATA_SOURCE_BUFFER
        struct {char* name;} file;                                        //!< Fill when nvstitchAudioDataSource is NVSTITCH_AUDIO_DATA_SOURCE_FILE
    } payload;
}
nvstitchAudioPayload_t;

//! Designates the type of output sound field to be created 
typedef enum
{
    NVSTITCH_AUDIO_OUTPUT_NONE = 0,
    NVSTITCH_AUDIO_OUTPUT_STEREO_MIXDOWN,   //!< Mix sources with stereo spreading effect

    NVSTITCH_AUDIO_OUTPUT_LAST,
    NVSTITCH_AUDIO_OUTPUT_ENUM_SIZE = 0x7fffffff 
}
nvstitchAudioOutputType;

#define NUM_SOUND_FIELD_PARAMETERS 4

#endif
