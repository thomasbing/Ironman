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
#include <sstream>

#include "math_util/math_utility.h"
#include "xml_utility_common.h"
#include "xml_utility_video.h"

#ifndef   M_PI
#define M_PI      3.1415926535897932385
#endif /* M_PI */
#define F_DEGREES_PER_RADIAN (float)(180. / M_PI)

namespace xmlutil
{
  bool setCameraPropertiesPose(int cameraNum, rapidxml::xml_node<>* poseNode, nvstitchCameraProperties_t* cameraProperties)
  {
    // Rotation
    rapidxml::xml_node<>* rotationNode = poseNode->first_node("rotation");
    if (!isNodePresent(rotationNode, cameraNum, "rotation node not specified, using defaults.", true)) { /*Optional*/ }
    else
    {
      // Rotation
      if (
        isAttrPresent(rotationNode, "yaw_deg")   &&
        isAttrPresent(rotationNode, "pitch_deg") &&
        isAttrPresent(rotationNode, "roll_deg"))
      {
        float yaw   = getAttrValueFloat(rotationNode, "yaw_deg");
        float pitch = getAttrValueFloat(rotationNode, "pitch_deg");
        float roll  = getAttrValueFloat(rotationNode, "roll_deg");

        (void)math_util::cameraToWorldTransform(
          math_util::degToRad(yaw),
          math_util::degToRad(pitch),
          math_util::degToRad(roll),
          cameraProperties->extrinsics.rotation); // Y-up

        cameraProperties->set_flags = (nvstitchCameraSetPropertyFlag)
          (cameraProperties->set_flags | nvstitchCameraSetPropertyFlag::CAMERA_ROTATION_SET);
      }
      else if (
        isAttrPresent(rotationNode, "yaw_rad") &&
        isAttrPresent(rotationNode, "pitch_rad") &&
        isAttrPresent(rotationNode, "roll_rad"))
      {

        // Pose matrix
        math_util::cameraToWorldTransform(
          (getAttrValueFloat(rotationNode, "yaw_rad")),
          (getAttrValueFloat(rotationNode, "pitch_rad")),
          (getAttrValueFloat(rotationNode, "roll_rad")),
          cameraProperties->extrinsics.rotation);


        cameraProperties->set_flags = (nvstitchCameraSetPropertyFlag)
          (cameraProperties->set_flags | nvstitchCameraSetPropertyFlag::CAMERA_ROTATION_SET);
      }
      else if (
        isAttrPresent(rotationNode, "m0") && isAttrPresent(rotationNode, "m1") && isAttrPresent(rotationNode, "m2") &&
        isAttrPresent(rotationNode, "m3") && isAttrPresent(rotationNode, "m4") && isAttrPresent(rotationNode, "m5") &&
        isAttrPresent(rotationNode, "m6") && isAttrPresent(rotationNode, "m7") && isAttrPresent(rotationNode, "m8"))
      {
        for (int i = 0; i < 9; i++)
          cameraProperties->extrinsics.rotation[i] = getAttrValueFloat(rotationNode, ("m" + std::to_string(i)));

        cameraProperties->set_flags = (nvstitchCameraSetPropertyFlag)
          (cameraProperties->set_flags | nvstitchCameraSetPropertyFlag::CAMERA_ROTATION_SET);
      }
      else
      {
        displayCameraError(cameraNum, "missing rotation yaw/pitch/roll or matrix attributes.");
      }
    }

    // Translation
    rapidxml::xml_node<>* translationNode = poseNode->first_node("translation");
    if (!isNodePresent(translationNode, cameraNum, "translation node not specified, setting defaults.", true)) { /*Optional*/ }
    else
    {
      if (
        isAttrPresent(translationNode, "x_cm") &&
        isAttrPresent(translationNode, "y_cm") && 
        isAttrPresent(translationNode, "z_cm"))
      {
        cameraProperties->extrinsics.translation[0] = getAttrValueFloat(translationNode, "x_cm");
        cameraProperties->extrinsics.translation[1] = getAttrValueFloat(translationNode, "y_cm");
        cameraProperties->extrinsics.translation[2] = getAttrValueFloat(translationNode, "z_cm");

        cameraProperties->set_flags = (nvstitchCameraSetPropertyFlag)
          (cameraProperties->set_flags | nvstitchCameraSetPropertyFlag::CAMERA_TRANSLATION_SET);
      }
      else
      {
        displayCameraError(cameraNum, "missing translation attributes x_cm/y_cm/z_cm.");
      }
    }
    return true;
  }

  bool setCameraPropertiesOptics(int cameraNum, rapidxml::xml_node<>* opticsNode, nvstitchCameraProperties_t* cameraProperties)
  {
    // Focal Length
    rapidxml::xml_node<>* focalLengthNode = opticsNode->first_node("focal_length");
    if (!isNodePresent(focalLengthNode, cameraNum, "focal_length node not specified, using defaults.", true)) { /*Optional*/ }
    else
    {
      if (isAttrPresent(focalLengthNode, "focal_pixels"))
      {
        cameraProperties->focal_length = getAttrValueFloat(focalLengthNode, "focal_pixels");
      }
      else if (isAttrPresent(focalLengthNode, "hfov_deg") && isAttrPresent(focalLengthNode, "vfov_deg"))
      {
        // tpatil - TODO : Add function to convert FOV to focal length
        //math_util::convertFOVtoFocalLength();
      }
      else
      {
          std::cerr << "Focal length attributes not present, or invalid\n";
      }
    }

    // Principal point
    rapidxml::xml_node<>* principalPointNode = opticsNode->first_node("principal_point");
    if (!isNodePresent(principalPointNode, cameraNum, "principal_point node not specified, using defaults.", true)) { /*Optional*/ }
    else
    {
      if (isAttrPresent(principalPointNode, "center_offset_x") && isAttrPresent(principalPointNode, "center_offset_y"))
      {
        math_util::convertPrincipalPointCenterToTopLeftOffset(
          cameraProperties->image_size.x,
          cameraProperties->image_size.y,
          getAttrValueFloat(principalPointNode, "center_offset_x"),
          getAttrValueFloat(principalPointNode, "center_offset_y"),
          &cameraProperties->principal_point.x,
          &cameraProperties->principal_point.y);

        cameraProperties->set_flags = (nvstitchCameraSetPropertyFlag)
          (cameraProperties->set_flags | nvstitchCameraSetPropertyFlag::CAMERA_PRINCIPAL_POINT_SET);
      }
      else if (isAttrPresent(principalPointNode, "top_left_offset_x") && isAttrPresent(principalPointNode, "top_left_offset_y"))
      {
        cameraProperties->principal_point.x = getAttrValueFloat(principalPointNode, "top_left_offset_x");
        cameraProperties->principal_point.y = getAttrValueFloat(principalPointNode, "top_left_offset_y");

        cameraProperties->set_flags = (nvstitchCameraSetPropertyFlag)
          (cameraProperties->set_flags | nvstitchCameraSetPropertyFlag::CAMERA_PRINCIPAL_POINT_SET);
      }
      else
      {
        displayCameraError(cameraNum, "Invalid/Missing attributes for principal point.");
        return false;
      }
    }

    // Lens
    rapidxml::xml_node<>* lensNode = opticsNode->first_node("lens");
    if (!isNodePresent(lensNode, cameraNum, "lens node not specified, using defaults.", true)) { /*return false;*/ }
    else
    {
      if (isAttrPresent(lensNode, "type"))
      {
        std::string lensType = getAttrValueString(lensNode, "type");

        if (lensType == "brown")
        {
          cameraProperties->set_flags = (nvstitchCameraSetPropertyFlag)
            (cameraProperties->set_flags | nvstitchCameraSetPropertyFlag::CAMERA_DISTORTION_TYPE_SET);

          if (isAttrPresent(lensNode, "k1") && isAttrPresent(lensNode, "k2") &&
            isAttrPresent(lensNode, "k3") && isAttrPresent(lensNode, "k4") && isAttrPresent(lensNode, "k5"))
          {
            cameraProperties->distortion_type = nvstitchDistortionType::NVSTITCH_DISTORTION_TYPE_BROWN;
            cameraProperties->distortion_coefficients[0] = getAttrValueFloat(lensNode, "k1");
            cameraProperties->distortion_coefficients[1] = getAttrValueFloat(lensNode, "k2");
            cameraProperties->distortion_coefficients[2] = getAttrValueFloat(lensNode, "k3");
            cameraProperties->distortion_coefficients[3] = getAttrValueFloat(lensNode, "k4");
            cameraProperties->distortion_coefficients[4] = getAttrValueFloat(lensNode, "k5");

            cameraProperties->set_flags = (nvstitchCameraSetPropertyFlag)
              (cameraProperties->set_flags | nvstitchCameraSetPropertyFlag::CAMERA_DISTORTION_COEFFICIENTS_SET);
          }
          else if (isAttrPresent(lensNode, "r1") && isAttrPresent(lensNode, "r2") &&
            isAttrPresent(lensNode, "r3") && isAttrPresent(lensNode, "t1") && isAttrPresent(lensNode, "t2"))
          {
            cameraProperties->distortion_type = nvstitchDistortionType::NVSTITCH_DISTORTION_TYPE_BROWN;
            cameraProperties->distortion_coefficients[0] = getAttrValueFloat(lensNode, "r1");
            cameraProperties->distortion_coefficients[1] = getAttrValueFloat(lensNode, "r2");
            cameraProperties->distortion_coefficients[2] = getAttrValueFloat(lensNode, "t1");
            cameraProperties->distortion_coefficients[3] = getAttrValueFloat(lensNode, "t2");
            cameraProperties->distortion_coefficients[4] = getAttrValueFloat(lensNode, "r3");

            cameraProperties->set_flags = (nvstitchCameraSetPropertyFlag)
              (cameraProperties->set_flags | nvstitchCameraSetPropertyFlag::CAMERA_DISTORTION_COEFFICIENTS_SET);
          }
          else
          {
            displayCameraError(cameraNum, "Missing coefficent attributes for lens type brown.");
          }
        }
        else if (lensType == "fisheye")
        {
          cameraProperties->set_flags = (nvstitchCameraSetPropertyFlag)
            (cameraProperties->set_flags | nvstitchCameraSetPropertyFlag::CAMERA_DISTORTION_TYPE_SET);

          if (isAttrPresent(lensNode, "k1") && isAttrPresent(lensNode, "k2") &&
            isAttrPresent(lensNode, "k3") && isAttrPresent(lensNode, "k4"))
          {
            cameraProperties->distortion_type = nvstitchDistortionType::NVSTITCH_DISTORTION_TYPE_FISHEYE;
            cameraProperties->distortion_coefficients[0] = getAttrValueFloat(lensNode, "k1");
            cameraProperties->distortion_coefficients[1] = getAttrValueFloat(lensNode, "k2");
            cameraProperties->distortion_coefficients[2] = getAttrValueFloat(lensNode, "k3");
            cameraProperties->distortion_coefficients[3] = getAttrValueFloat(lensNode, "k4");

            cameraProperties->set_flags = (nvstitchCameraSetPropertyFlag)
              (cameraProperties->set_flags | nvstitchCameraSetPropertyFlag::CAMERA_DISTORTION_COEFFICIENTS_SET);
          }
        }
        else if (isAttrPresent(lensNode, "r1") && isAttrPresent(lensNode, "r2") &&
          isAttrPresent(lensNode, "r3") && isAttrPresent(lensNode, "r4"))
        {
          cameraProperties->distortion_type = nvstitchDistortionType::NVSTITCH_DISTORTION_TYPE_FISHEYE;
          cameraProperties->distortion_coefficients[0] = getAttrValueFloat(lensNode, "r1");
          cameraProperties->distortion_coefficients[1] = getAttrValueFloat(lensNode, "r2");
          cameraProperties->distortion_coefficients[2] = getAttrValueFloat(lensNode, "r3");
          cameraProperties->distortion_coefficients[3] = getAttrValueFloat(lensNode, "r4");

          cameraProperties->set_flags = (nvstitchCameraSetPropertyFlag)
            (cameraProperties->set_flags | nvstitchCameraSetPropertyFlag::CAMERA_DISTORTION_COEFFICIENTS_SET);
        }
        else
        {
          displayCameraError(cameraNum, "Missing coefficent attributes for lens type fisheye.");
        }
      }
    }

    // Fisheye Radius
    if (cameraProperties->distortion_type == nvstitchDistortionType::NVSTITCH_DISTORTION_TYPE_FISHEYE)
    {
      rapidxml::xml_node<>* fisheyeRadiusNode = opticsNode->first_node("fisheye_radius");
      
      if (!isNodePresent(fisheyeRadiusNode, cameraNum, "fisheye_radius not specified, using defaults.", true)) { /*Optional*/ }
      else
      {
        if (!isAttrPresent(fisheyeRadiusNode, "radius_pixels")) { return false; }
        else
        {
          cameraProperties->fisheye_radius = getAttrValueFloat(fisheyeRadiusNode, "radius_pixels");

          cameraProperties->set_flags = (nvstitchCameraSetPropertyFlag)
            (cameraProperties->set_flags | nvstitchCameraSetPropertyFlag::CAMERA_FISHEYE_RADIUS_SET);
        }
      }
    }
    return true;
  }

  bool setCameraProperties(int cameraNum, rapidxml::xml_node<>* cameraNode, nvstitchCameraProperties_t* cameraProperties)
  {
    // Clear all flags
    cameraProperties->version = NVSTITCH_VERSION;
    cameraProperties->set_flags = (nvstitchCameraSetPropertyFlag)0;

    // Width, Height
    if (!( (isAttrPresent(cameraNode, "width") && isAttrPresent(cameraNode, "height")) ) )
    {
      displayCameraError(cameraNum, "missing width/height attribute.");
      return false;
    }
    else
    {
      cameraProperties->image_size.x = getAttrValueInt(cameraNode, "width");
      cameraProperties->image_size.y = getAttrValueInt(cameraNode, "height");
    }

    // Camera layout
    if (!(isAttrPresent(cameraNode, "layout")))
    {
      displayCameraWarning(cameraNum, "Missing layout attribute, setting to default=equatorial");
    }
    else
    {
      std::string layoutValueStr = getAttrValueString(cameraNode, "layout");
      if (layoutValueStr == "equatorial")
        cameraProperties->camera_layout = nvstitchCameraLayout::NVSTITCH_CAMERA_LAYOUT_EQUATORIAL;
      else if(layoutValueStr == "general")
        cameraProperties->camera_layout = nvstitchCameraLayout::NVSTITCH_CAMERA_LAYOUT_GENERAL;
      else
        cameraProperties->camera_layout = nvstitchCameraLayout::NVSTITCH_CAMERA_LAYOUT_EQUATORIAL;
    }

    // Pose
    rapidxml::xml_node<>* poseNode = cameraNode->first_node("pose");
    if (!isNodePresent(poseNode, cameraNum, "Pose node not specifed, setting defaults", true)) { /*Optional*/ }
    setCameraPropertiesPose(cameraNum, poseNode, cameraProperties);

    // Optics
    rapidxml::xml_node<>* opticsNode = cameraNode->first_node("optics");
    if (!isNodePresent(opticsNode, cameraNum, "Optics node not specified, setting defaults.", true)) { /*Optional*/ }
    setCameraPropertiesOptics(cameraNum, opticsNode, cameraProperties);

    return true;
  }

  static rapidxml::xml_node<>* findCameraRigNode(rapidxml::xml_document<> *root)
  {
    rapidxml::xml_node<>  *rigNode, *nvstitchNode;
    if (!(nullptr != (nvstitchNode = root->first_node("nvstitch")) && nullptr != (rigNode = nvstitchNode->first_node("rig"))) && // nested in nvstitch node
          nullptr == (rigNode = root->first_node("rig"))   // rig is a root node
    )
      rigNode = root->first_node("camera_rig");            // backwards compatibility
    return rigNode;
  }

  bool readCameraRigXml(const std::string& xmlPath, std::vector<nvstitchCameraProperties_t>& cameraProperties, nvstitchVideoRigProperties_t* videoRigProperties)
  {
    rapidxml::xml_document<> doc;
    
    // Get Document
    std::string xmFileString;
    if (!getXmlFileString(xmlPath, xmFileString))
    {
      displayError("Unable to open xml file: " + xmlPath);
      return false;
    }
    try
    {
      doc.parse<0>((char*)xmFileString.c_str());
    }
    catch (const rapidxml::parse_error& e)
    {
        std::cerr << "Parsing error with xml:" << xmlPath << "\n";
        std::cerr << "Parse error was: " << e.what() << "\n";
        return false;
    }

    if (videoRigProperties == nullptr)
    {
        std::cerr << "Input arg videoRigProperties is null\n";
    }

    // <camera_rig>
    rapidxml::xml_node<> *camRigNode = findCameraRigNode(&doc);
    if (!isNodePresent(camRigNode, "no rig node found with cameras."))
    {
      return false;
    }

    // Version
    videoRigProperties->version = NVSTITCH_VERSION;

    // rig diameter
    if (isAttrPresent(camRigNode, "rig_diameter_cm"))
    {
      videoRigProperties->rig_diameter = getAttrValueFloat(camRigNode, "rig_diameter_cm");
    }
    else
    {
      videoRigProperties->rig_diameter = 0.0f;
    }

    // rig rotation
    math_util::setMatrixToIdentity(videoRigProperties->rotation);

    // Cameras
    int camCount = 0;
    for (rapidxml::xml_node<>* currCamNode = camRigNode->first_node("camera"); currCamNode;
      currCamNode = currCamNode->next_sibling("camera"))
    {
      camCount++;
      cameraProperties.push_back(nvstitchCameraProperties_t());
      
      if (!setCameraProperties(camCount, currCamNode, &cameraProperties.back()))
        return false;
    }

    if (camCount == 0)
    {
      displayError("No camera nodes found.");
      return false;
    }

    videoRigProperties->num_cameras = camCount;
    videoRigProperties->cameras = &(cameraProperties.at(0));
    
    return true;
  }

  std::string getCameraLayoutString(nvstitchCameraLayout camLayout)
  {
    if (camLayout == nvstitchCameraLayout::NVSTITCH_CAMERA_LAYOUT_EQUATORIAL)
      return "equatorial";
    else if (camLayout == nvstitchCameraLayout::NVSTITCH_CAMERA_LAYOUT_GENERAL)
      return "general";
    else
      return "invalid";
  }


  void appendCameraPropertiesToCameraNode(const nvstitchCameraProperties_t* camProp,  rapidxml::xml_document<>* doc, rapidxml::xml_node<>* camNode)
  {
    // <camera>
    appendNewAttribute(doc, camNode, "width", std::to_string(camProp->image_size.x));
    appendNewAttribute(doc, camNode, "height", std::to_string(camProp->image_size.y));
    appendNewAttribute(doc, camNode, "layout", getCameraLayoutString(camProp->camera_layout));

    {
      rapidxml::xml_node<>* poseNode = appendNewNode(doc, camNode, "pose");                                     // <pose>
      {
        rapidxml::xml_node<>* rotationNode = appendNewNode(doc, poseNode, "rotation");                          // <rotation>
        for (int i = 0; i < 9; i++)
        {
          float rotMatValue = camProp->extrinsics.rotation[i];
          if (fabsf(rotMatValue) < 1.e-7f)
            rotMatValue = 0.f;
          appendNewAttribute(doc, rotationNode, ("m" + std::to_string(i)), rotMatValue);
        }
        float ypr[3];
        math_util::cameraToWorldAngles(camProp->extrinsics.rotation, ypr);
        appendNewAttribute(doc, rotationNode, "yaw_deg",   (ypr[0] * F_DEGREES_PER_RADIAN));
        appendNewAttribute(doc, rotationNode, "pitch_deg", (ypr[1] * F_DEGREES_PER_RADIAN));
        appendNewAttribute(doc, rotationNode, "roll_deg",  (ypr[2] * F_DEGREES_PER_RADIAN));
      }

      {
        rapidxml::xml_node<>* translationNode = appendNewNode(doc, poseNode, "translation");                    // <translation>
        appendNewAttribute(doc, translationNode, "x_cm", (camProp->extrinsics.translation[0]));
        appendNewAttribute(doc, translationNode, "y_cm", (camProp->extrinsics.translation[1]));
        appendNewAttribute(doc, translationNode, "z_cm", (camProp->extrinsics.translation[2]));
      }
    }


    {
      rapidxml::xml_node<>* opticsNode = appendNewNode(doc, camNode, "optics");                                 // <optics>

      {
        rapidxml::xml_node<>* focalLengthNode = appendNewNode(doc, opticsNode, "focal_length");                 // <focal_length>
        appendNewAttribute(doc, focalLengthNode, "focal_pixels", (camProp->focal_length));
      }

      {
        rapidxml::xml_node<>* principalPointNode = appendNewNode(doc, opticsNode, "principal_point");           // <principal_point>
        float ppCenterOffsetX, ppCenterOffsetY;
        math_util::convertPrincipalPointTopLeftToCenterOffset(camProp->image_size.x, camProp->image_size.y, 
          camProp->principal_point.x, camProp->principal_point.y, &ppCenterOffsetX, &ppCenterOffsetY);
        appendNewAttribute(doc, principalPointNode, "center_offset_x", (ppCenterOffsetX));
        appendNewAttribute(doc, principalPointNode, "center_offset_y", (ppCenterOffsetY));
      }

      {
        rapidxml::xml_node<>* lensNode = appendNewNode(doc, opticsNode, "lens");                                // <lens>
        if (camProp->distortion_type == nvstitchDistortionType::NVSTITCH_DISTORTION_TYPE_BROWN)
        {
          appendNewAttribute(doc, lensNode, "type", "brown");
          for (int disCoeffNum = 0; disCoeffNum < 5; disCoeffNum++)
          {
            appendNewAttribute(doc, lensNode, "k" + std::to_string(disCoeffNum+1), (camProp->distortion_coefficients[disCoeffNum]));
          }
        }
        else if (camProp->distortion_type == nvstitchDistortionType::NVSTITCH_DISTORTION_TYPE_FISHEYE)
        {
          appendNewAttribute(doc, lensNode, "type", "fisheye");
          for (int disCoeffNum = 0; disCoeffNum < 4; disCoeffNum++)
          {
            appendNewAttribute(doc, lensNode, "k" + std::to_string(disCoeffNum+1), (camProp->distortion_coefficients[disCoeffNum]));
          }

          // Fisheye Radius
          if (camProp->distortion_type == nvstitchDistortionType::NVSTITCH_DISTORTION_TYPE_FISHEYE)
          {
            rapidxml::xml_node<>* fisheyeRadius = appendNewNode(doc, opticsNode, "fisheye_radius");           // <fisheye_radius>
            appendNewAttribute(doc, fisheyeRadius, "radius_pixels", (camProp->fisheye_radius));
          }
        }
      }
    }
  }


  bool appendCameraRigToRigNode(const nvstitchVideoRigProperties_t* videoRigProperties, rapidxml::xml_document<>* doc, rapidxml::xml_node<>* rigNode)
  {
    //<camera_rig>
    if (!isAttrPresent(rigNode, "coord_axes"))
      appendNewAttribute(doc, rigNode, "coord_axes", "y-up");
    appendNewAttribute(doc, rigNode, "rig_diameter_cm", (videoRigProperties->rig_diameter) );

    for (size_t camNum = 0; camNum < (size_t)videoRigProperties->num_cameras; camNum++)
    {
      rapidxml::xml_node<>* currCameraNode = appendNewNode(doc, rigNode, "camera");
      appendCameraPropertiesToCameraNode(&(videoRigProperties->cameras[camNum]), doc, currCameraNode);
    }

    return true;
  }


  bool writeCameraRigXml(const std::string& xmlPath, const nvstitchVideoRigProperties_t* videoRigProperties)
  {
    // Open XML file for writing
    std::ofstream fout;
    fout.open(xmlPath, std::ios::trunc);

    if (!fout.is_open())
    {
      std::cerr << "Error: Cannot open output XML file:" << xmlPath;
      return 1;
    }
    
    // Generate XML document
    rapidxml::xml_document<>* doc = new rapidxml::xml_document<>();
    rapidxml::xml_node<>* decl = doc->allocate_node(rapidxml::node_declaration);
    
    decl->append_attribute(doc->allocate_attribute("version", "1.0"));
    decl->append_attribute(doc->allocate_attribute("encoding", "utf-8"));
    doc->append_node(decl);

    // <rig>
    rapidxml::xml_node<>* rigNode = appendNewNode(doc, doc, "rig");
    if (!appendCameraRigToRigNode(videoRigProperties, doc, rigNode))
      return false;

    fout << (*doc);
    fout.close();
    delete doc;

    return true;
  }

  bool splitString(const std::string& in, std::vector<std::string>& splitStr, char delim)
  {
    splitStr.clear();
    if (in.length() > 0)
    {
      std::istringstream f(in);
      std::string s;
      while (std::getline(f, s, delim))
      {
        splitStr.push_back(s);
      }
      return true;
    }
    return false;
  }

  bool readInputCalibFilenamesXml(const std::string& xmlPath, std::vector<std::vector<std::string>>& filenames)
  {
    std::cout << std::endl << "XmlUtil: Reading Input Calib filenames Xml: " << xmlPath;
    
    rapidxml::xml_document<> doc;

    // Get Document
    std::string xmFileString;
    if (!getXmlFileString(xmlPath, xmFileString))
    {
      displayError("Unable to open xml file: " + xmlPath);
      return false;
    }
    try
    {
      doc.parse<0>((char*)xmFileString.c_str());
    }
    catch (const rapidxml::parse_error& e)
    {
        std::cerr << "Parsing error with xml:" << xmlPath << "\n";
        std::cerr << "Parse error was: " << e.what() << "\n";
        return false;
    }

    rapidxml::xml_node<> *camRigNode = findCameraRigNode(&doc);
    if (!isNodePresent(camRigNode, "no rig node found with cameras."))
    {
      return false;
    }

    filenames.clear();
    filenames.push_back(std::vector<std::string>());

    int camCount = 0;
    int frameCount = 0;
    for (rapidxml::xml_node<>* currCamNode = camRigNode->first_node("camera"); currCamNode;
      currCamNode = currCamNode->next_sibling("camera"))
    {
      camCount++;

      rapidxml::xml_node<>* inputCalibFileNode = currCamNode->first_node("input_calib_file");
      if (!isNodePresent(inputCalibFileNode, "input_calib_file node not found.")) { return false; }

      
      if (isAttrPresent(inputCalibFileNode, "name"))
      {
        // Separate 
        std::string allFilesStr = getAttrValueString(inputCalibFileNode, "name");

        std::vector<std::string> frameNames;
        splitString(allFilesStr, frameNames, ',');
        if (frameNames.size() == 0)
        {
          displayCameraError(camCount, "no file specified in attribute input_calib_file name.");
          return false;
        }

        if (camCount == 1)
        {
          frameCount = (int)frameNames.size();
          filenames.resize(frameCount);
        }
        else if (frameNames.size() != frameCount)
        {
          displayError("Mismatch in filename frames count.");
        }

        for (int frameIndex =0; frameIndex < frameCount; frameIndex++)
        {
          filenames[frameIndex].push_back(frameNames.at(frameIndex));
        }
        
      }
      else
      {
        displayCameraError(camCount, "missing attribute name.");
        return false;
      }
    }

    if (camCount == 0)
    {
      displayError("No camera nodes found.");
      return false;
    }

    return true;
  }

  bool readInputMediaFeedFilenamesXml(const std::string& xmlPath, std::vector<std::string>& filenames)
  {
    std::cout << std::endl << "XmlUtil: Reading Input Media Feed XML: " << xmlPath;
    
    rapidxml::xml_document<> doc;
    // Get Document
    std::string xmFileString;
    if (!getXmlFileString(xmlPath, xmFileString))
    {
      displayError("Unable to open xml file: " + xmlPath);
      return false;
    }
    try
    {
      doc.parse<0>((char*)xmFileString.c_str());
    }
    catch (const rapidxml::parse_error& e)
    {
        std::cerr << "Parsing error with xml:" << xmlPath << "\n";
        std::cerr << "Parse error was: " << e.what() << "\n";
        return false;
    }

    filenames.clear();

    rapidxml::xml_node<> *camRigNode = findCameraRigNode(&doc);
    if (!isNodePresent(camRigNode, "no rig node found with cameras."))
    {
      return false; 
    }


    int camCount = 0;
    for (rapidxml::xml_node<>* currCamNode = camRigNode->first_node("camera"); currCamNode;
      currCamNode = currCamNode->next_sibling("camera"))
    {

      camCount++;

      rapidxml::xml_node<>* inputMediaFeedNode = currCamNode->first_node("input_media_feed");
      if (!isNodePresent(inputMediaFeedNode, "input_media_feed node not found.")) { return false; }
      else
      {
        //<input_media_feed>

          //<input_media_form>
          rapidxml::xml_node<>* inputMediaFormNode = inputMediaFeedNode->first_node("input_media_form");
          if (!isNodePresent(inputMediaFormNode, "input_media_form node not found.")) { return false; }
          else
          {
            if (!isAttrPresent(inputMediaFormNode, "form", "form attribute missing.")) { return false; }
            else
            {
              std::string formStr = getAttrValueString(inputMediaFormNode, "form");
            
              // Assign blank string if file format not a file 
              if (formStr != "file")
              {
                filenames.push_back("");
                continue;
              }
            }
          }
          //</input_media_form>

          //<input_media_file>
          rapidxml::xml_node<>* inputMediaFileNode = inputMediaFeedNode->first_node("input_media_file");
          if (!isNodePresent(inputMediaFileNode)) { /* Optional */ }
          else
          {
            filenames.push_back(getAttrValueString(inputMediaFileNode, "name"));
          }
          //</input_media_file>

        //</input_media_feed>
      }
    }


    return true;

  }
}