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

#ifndef XML_UTILITY_H
#define XML_UTILITY_H


#include <rapidxml.hpp>
#include <rapidxml_print.hpp>
#include <rapidxml_utils.hpp>

#include "nvstitch_common.h"
#include "nvstitch_common_audio.h"
#include "nvstitch_common_video.h"
#include "nvstitch.h"

#include "math_util/math_utility.h"
#include "xml_utility_common.h"
#include "xml_utility_video.h"
#include "xml_utility_audio.h"

namespace xmlutil
{
  // Read from Xml
  bool readInputMediaFeedXml(const std::string& inputFeedXmlPath, std::vector<nvstitchPayload_t>& mediaPayloadArray, 
    std::vector<std::string>& payloadFilenames);

  bool readInputAudioFeedXml(const std::string& inputFeedXmlPath, std::vector<nvstitchAudioPayload_t>& audioPayloadArray,
    std::vector<std::string>& payloadFilenames);

  bool readStitcherPropertiesXml(const std::string& stitcherPropertiesXmlPath, 
    nvstitchStitcherProperties_t* stitcherProperties, std::vector<std::string>& payloadFilenames, 
    const std::string& inputAudioFeedXmlPath = std::string());


  // Write to XML
  bool writeRigXml(
    const std::string& xmlPath,
    const nvstitchVideoRigProperties_t* videoRigProperties,
    const nvstitchPayload_t mediaPayloadArray[],
    const nvstitchAudioRigProperties_t* audioRigProperties,
    const nvstitchAudioPayload_t audioPayloadArray[]
  );

  bool writeInputFeedXml(
    const std::string& xmlPath,
    const nvstitchPayload_t mediaPayloadArray[], int mediaPayloadCount,
    const nvstitchAudioPayload_t audioPayload[], int audioPayloadCount,
    const nvstitchStitcherProperties_t* stitcherProperties);


  bool writeStitcherPropertiesXml(const std::string&, const nvstitchStitcherProperties_t* stitcherProperties);


  bool writeAllInOneXml(
    const std::string&                            xmlPath,              // output file name
    const nvstitchVideoRigProperties_t*           videoRigProperties,   // video
    const nvstitchPayload_t                       mediaPayloadArray[],
    const std::vector<std::vector<std::string> >* calibFiles,
    const nvstitchAudioRigProperties_t*           audioRigProperties,   // audio
    const nvstitchAudioPayload_t                  audioPayloadArray[],
    const nvstitchStitcherProperties_t*           stitcherProperties);   //stitcher

}

#endif
