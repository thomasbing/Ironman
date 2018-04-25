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

#include <fstream>
#include <vector>
#include <string>
#include <iostream>

#include "math_util/math_utility.h"
#include "xml_utility_common.h"
#include "xml_utility_audio.h"

namespace xmlutil
{
  bool readAudioSourcePropertiesType(rapidxml::xml_node<>* inputTypeNode,
    nvstitchAudioInputType& audioInputType)
  {
    if (!isAttrPresent(inputTypeNode, "type", "type attribute missing.")) { return false; }
    else
    {
      std::string attrValue = getAttrValueString(inputTypeNode, "type");
      if (attrValue == "omni")
        audioInputType = nvstitchAudioInputType::NVSTITCH_AUDIO_INPUT_TYPE_OMNI;
      else if (attrValue == "shotgun")
        audioInputType = nvstitchAudioInputType::NVSTITCH_AUDIO_INPUT_TYPE_SHOTGUN_MIC;
      else if (attrValue == "cardioid")
        audioInputType = nvstitchAudioInputType::NVSTITCH_AUDIO_INPUT_TYPE_CARDIOID_MIC;
      else if (attrValue == "supercardioid")
        audioInputType = nvstitchAudioInputType::NVSTITCH_AUDIO_INPUT_TYPE_SUPERCARDIOID_MIC;
      else
      {
        displayError("invalid audio input type specified.");
        return false;
      }
    }
    return true;
  }
  
  bool setAudioSourcePropertiesPose(int audioSrcNum, rapidxml::xml_node<>* poseNode, 
    nvstitchAudioSourceProperties_t* audioSrcProperties)
  {
    audioSrcProperties->version = NVSTITCH_VERSION;
    // Rotation
    rapidxml::xml_node<>* rotationNode = poseNode->first_node("rotation");
    if (!isNodePresent(rotationNode, "rotation node not found.")) { return false; }
    else
    {
      // Rotation
      if (
        isAttrPresent(rotationNode, "yaw_deg")   &&
        isAttrPresent(rotationNode, "pitch_deg") &&
        isAttrPresent(rotationNode, "roll_deg"))
      {
        (void)math_util::cameraToWorldTransform(
          math_util::degToRad(getAttrValueFloat(rotationNode, "yaw_deg")),
          math_util::degToRad(getAttrValueFloat(rotationNode, "pitch_deg")),
          math_util::degToRad(getAttrValueFloat(rotationNode, "roll_deg")),
          audioSrcProperties->pose.rotation);
      }
      else if (
        isAttrPresent(rotationNode, "yaw_rad")   &&
        isAttrPresent(rotationNode, "pitch_rad") &&
        isAttrPresent(rotationNode, "roll_rad"))
      {
        (void)math_util::cameraToWorldTransform(
          (getAttrValueFloat(rotationNode, "yaw_rad")),
          (getAttrValueFloat(rotationNode, "pitch_rad")),
          (getAttrValueFloat(rotationNode, "roll_rad")),
          audioSrcProperties->pose.rotation);
      }
      else if (
        isAttrPresent(rotationNode, "m0") && isAttrPresent(rotationNode, "m1") && isAttrPresent(rotationNode, "m2") &&
        isAttrPresent(rotationNode, "m3") && isAttrPresent(rotationNode, "m4") && isAttrPresent(rotationNode, "m5") &&
        isAttrPresent(rotationNode, "m6") && isAttrPresent(rotationNode, "m7") && isAttrPresent(rotationNode, "m8") )
      {
        for (int i = 0; i < 9; i++)
          audioSrcProperties->pose.rotation[i] = getAttrValueFloat(rotationNode, ("m" + std::to_string(i)));
      }
      else
      {
        displayAudioSrcError(audioSrcNum, "missing rotation yaw/pitch/roll or matrix attributes.");
      }
    }

    // Translation
    rapidxml::xml_node<>* translationNode = poseNode->first_node("translation");
    if (!isNodePresent(translationNode, "translation node not found.")) { return false; }
    else
    {
      if (
        isAttrPresent(translationNode, "x_cm") &&
        isAttrPresent(translationNode, "y_cm") && 
        isAttrPresent(translationNode, "z_cm"))
      {
        audioSrcProperties->pose.translation[0] = getAttrValueFloat(translationNode, "x_cm");
        audioSrcProperties->pose.translation[1] = getAttrValueFloat(translationNode, "y_cm");
        audioSrcProperties->pose.translation[2] = getAttrValueFloat(translationNode, "z_cm");
      }
      else
      {
        displayAudioSrcError(audioSrcNum, "missing translation attributes x_cm/y_cm/z_cm.");
      }
    }
    return true;
  }


  bool setAudioSourceProperties(int audioSrcNum, rapidxml::xml_node<>* audioSrcNode, 
    nvstitchAudioSourceProperties_t* audioSrcProperties)
  {
    // Pose
    rapidxml::xml_node<>* poseNode = audioSrcNode->first_node("pose");
    if (!isNodePresent(poseNode, "Pose node not found.")) { return false; }
    setAudioSourcePropertiesPose(audioSrcNum, poseNode, audioSrcProperties);

    // Properties
    rapidxml::xml_node<>* inputTypeNode = audioSrcNode->first_node("input_type");
    if (!isNodePresent(inputTypeNode, "input_type node not found.")) { return false; }
    else
    {
      if (!readAudioSourcePropertiesType(inputTypeNode, audioSrcProperties->input_type)) { return false; }
    }

    return true;
  }

  static rapidxml::xml_node<>* findAudioRigNode(rapidxml::xml_document<> *root)
  {
    rapidxml::xml_node<>  *rigNode, *nvstitchNode;
    if (!(nullptr != (nvstitchNode = root->first_node("nvstitch")) && nullptr != (rigNode = nvstitchNode->first_node("rig"))) && // nested in nvstitch node
          nullptr == (rigNode = root->first_node("rig"))      // rig is a root node
    )
      rigNode = root->first_node("audio_rig");             // backwards compatibility
    return rigNode;
  }

  bool readAudioRigXml(const std::string& xmlPath, nvstitchAudioRigProperties_t* audioRigProperties)
  {
    std::cout << std::endl << "XmlUtil: Reading Audio Rig XML: " << xmlPath;
    
    std::string xmlFilePath(xmlPath);
    rapidxml::xml_document<> doc;
    // Get Document
    std::string xmFileString;
    if (!getXmlFileString(xmlFilePath, xmFileString))
    {
      displayError("Unable to open xml file: " + xmlFilePath);
      return false;
    }
    try
    {
      doc.parse<0>((char*)xmFileString.c_str());
    }
    catch (const rapidxml::parse_error& e)
    {
      std::cerr << "Parsing error with xml:" << xmlFilePath << "\n";
      std::cerr << "Parse error was: " << e.what() << "\n";
      return false;
    }

    if (audioRigProperties == nullptr)
    {
      std::cerr << "Input arg audioRigProperties is null\n";
    }

    // Version
    audioRigProperties->version = NVSTITCH_VERSION;

    rapidxml::xml_node<>* audioRigNode = findAudioRigNode(&doc);
    if (!isNodePresent(audioRigNode, "no rig node found with audio."))
    {
      return false;
    }

    int audioSrcCount = 0;
    std::vector<nvstitchAudioSourceProperties_t>* audioSrcProperties = new std::vector<nvstitchAudioSourceProperties_t>;
    for (rapidxml::xml_node<>* currAudioNode = audioRigNode->first_node("audio_source"); currAudioNode;
      currAudioNode = currAudioNode->next_sibling("audio_source"))
    {
      audioSrcCount++;
      audioSrcProperties->push_back(nvstitchAudioSourceProperties_t());
      
      if (!setAudioSourceProperties(audioSrcCount, currAudioNode, &audioSrcProperties->back()))
        return false;
    }

    if (audioSrcCount == 0)
    {
      displayError("No audio nodes found.");
      return false;
    }

    audioRigProperties->num_sources = audioSrcCount;
    audioRigProperties->sources = &(audioSrcProperties->at(0));

    return true;
  }


  void appendAudioPropertiesToAudioSourceNode(const nvstitchAudioSourceProperties_t* audioProps, rapidxml::xml_document<>* doc, rapidxml::xml_node<>* audioSourceNode)
  {
    //<pose>
    rapidxml::xml_node<>* poseNode = appendNewNode(doc, audioSourceNode, "pose");
    {
      // <rotation>
      {
        rapidxml::xml_node<>* rotationNode = appendNewNode(doc, poseNode, "rotation");

        // Matrix
        for (int i = 0; i < 9; i++)
        {
          float rotMatValue = audioProps->pose.rotation[i];
          if (fabsf(rotMatValue) < 1.e-7f)
            rotMatValue = 0.f;
          appendNewAttribute(doc, rotationNode, ("m" + std::to_string(i)), (rotMatValue));
        }
      }

      //<translation>
      {
        rapidxml::xml_node<>* translationNode = appendNewNode(doc, poseNode, "translation");
        appendNewAttribute(doc, translationNode, "x_cm", (audioProps->pose.translation[0]));
        appendNewAttribute(doc, translationNode, "y_cm", (audioProps->pose.translation[1]));
        appendNewAttribute(doc, translationNode, "z_cm", (audioProps->pose.translation[2]));
      }
    }

    // Properties
    rapidxml::xml_node<>* propertiesNode = appendNewNode(doc, audioSourceNode, "input_type");
    switch (audioProps->input_type)
    {
      case nvstitchAudioInputType::NVSTITCH_AUDIO_INPUT_TYPE_OMNI:
        appendNewAttribute(doc, propertiesNode, "type", "omni");
        break;
      case nvstitchAudioInputType::NVSTITCH_AUDIO_INPUT_TYPE_SHOTGUN_MIC:
        appendNewAttribute(doc, propertiesNode, "type", "shotgun");
        break;
      case nvstitchAudioInputType::NVSTITCH_AUDIO_INPUT_TYPE_CARDIOID_MIC:
        appendNewAttribute(doc, propertiesNode, "type", "cardioid");
        break;
      case nvstitchAudioInputType::NVSTITCH_AUDIO_INPUT_TYPE_SUPERCARDIOID_MIC:
        appendNewAttribute(doc, propertiesNode, "type", "supercardioid");
        break;
    }
  }


  void appendAudioRigToRootNode(const nvstitchAudioRigProperties_t* audioRigProperties, rapidxml::xml_document<>* doc, rapidxml::xml_node<>* root)
  {
    if (!isAttrPresent(root, "coord_axes"))
      appendNewAttribute(doc, root, "coord_axes", "y-up");

    for (size_t audioSrcNum = 0; audioSrcNum < (size_t)audioRigProperties->num_sources; audioSrcNum++)
    {
      rapidxml::xml_node<>* currAudioSrcNode = appendNewNode(doc, root, "audio_source");
      appendAudioPropertiesToAudioSourceNode(&audioRigProperties->sources[audioSrcNum], doc, currAudioSrcNode);
    }
  }


  bool writeAudioRigXml(const std::string& xmlPath, const nvstitchAudioRigProperties_t* audioRigProperties)
  {
    // Open XML file for writing
    std::ofstream fout;

    fout.open(xmlPath, std::ios::trunc);
    
    // Generate XML document
    rapidxml::xml_document<>* doc = new rapidxml::xml_document<>();
    
    rapidxml::xml_node<>* decl = doc->allocate_node(rapidxml::node_declaration);
    decl->append_attribute(doc->allocate_attribute("version", "1.0"));
    decl->append_attribute(doc->allocate_attribute("encoding", "utf-8"));
    doc->append_node(decl);

    //<audio_rig>
    rapidxml::xml_node<>* root = appendNewNode(doc, doc, "rig");
    appendAudioRigToRootNode(audioRigProperties, doc, root);

    fout << *doc;
    fout.close();
    delete doc;
    return true;
  }
}