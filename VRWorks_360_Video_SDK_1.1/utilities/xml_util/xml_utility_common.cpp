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

#include "xml_utility_common.h"

namespace xmlutil
{
  bool getXmlFileString(const std::string& xmlFilePath, std::string& xmlFileString)
  {
    std::ifstream xmlFile(xmlFilePath);
    if (!xmlFile.is_open())
      return false;

    xmlFileString = std::string((std::istreambuf_iterator<char>(xmlFile)), std::istreambuf_iterator<char>());

    if (xmlFileString.size() == 0)
      return false;

    return true;
  }

  bool getXmlDoc(const std::string& xmlFilePath, rapidxml::xml_document<>* doc)
  {
    std::string xmFileString;

    if (!getXmlFileString(xmlFilePath, xmFileString))
    {
      displayError("Unable to open xml file: " + xmlFilePath);
      return false;
    }

    try
    {
      doc->parse<0>((char*)xmFileString.c_str());
    }
    catch (const rapidxml::parse_error& e)
    {
        std::cerr << std::endl << "Parsing error with xml:" << xmlFilePath;
        std::cerr << std::endl << "Parse error was: " << e.what();
    }

    return true;
  }
  bool getXmlDoc(const std::string& xmlFilePath, rapidxml::xml_document<>& doc)
  {
    std::string xmFileString;

    if(!getXmlFileString(xmlFilePath, xmFileString))
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
        std::cerr << std::endl << "Parsing error with xml:" << xmlFilePath;
        std::cerr << std::endl << "Parse error was: " << e.what();
    }

    return true;
  }
  

  // Display Error
  void displayError(const std::string& errorMsg)
  {
    std::cerr << std::endl << "XML read error: " << errorMsg;
  }
  void displayError(rapidxml::xml_node<>* xmlNode, const char* errorMsg)
  {
    std::cerr << std::endl << "XML read error: " << xmlNode->name() << errorMsg;
  }
  void displayError(rapidxml::xml_node<>* xmlNode, const std::string& errorMsg)
  {
    std::cerr << std::endl << "XML read error: " << xmlNode->name() << errorMsg;
  }
  void displayCameraError(int cameraNum, const std::string& errorMsg)
  {
    std::cerr << std::endl << "XML read error: Camera " << cameraNum << " : " << errorMsg;
  }
  void displayAudioSrcError(int audioSrcNum, const std::string& errorMsg)
  {
    std::cerr << std::endl << "XML read error: Audio Source " << audioSrcNum << " : " << errorMsg;
  }

  // Display Warning
  void displayWarning(const std::string& errorMsg)
  {
    std::cout << std::endl << "XML read warning: " << errorMsg;
  }
  void displayWarning(rapidxml::xml_node<>* xmlNode, const char* errorMsg)
  {
    std::cout << std::endl << "XML read warning: " << errorMsg;
  }
  void displayWarning(rapidxml::xml_node<>* xmlNode, const std::string& errorMsg)
  {
    std::cout << std::endl << "XML read warning: " << xmlNode->name() << errorMsg;
  }
  void displayCameraWarning(int cameraNum, const std::string& errorMsg)
  {
    std::cout << std::endl << "XML read warning: Camera " << cameraNum << " : " << errorMsg;
  }
  void displayAudioSrcWarning(int audioSrcNum, const std::string& errorMsg)
  {
    std::cout << std::endl << "XML read warning: Audio Source " << audioSrcNum << " : " << errorMsg;
  }


  // Is node present
  bool isNodePresent(rapidxml::xml_node<>* xmlNode)
  {
    if (xmlNode == nullptr)
      return false;
    else
      return true;
  }

  bool isNodePresent(rapidxml::xml_node<>* xmlNode, const std::string& errMsg, bool isWarning)
  {
    if (xmlNode == nullptr)
    {
      if (isWarning)
      {
        displayWarning(errMsg);
      }
      else
      {
        displayWarning(errMsg);
      }
      return false;
    }
    return true;
  }
  bool isNodePresent(rapidxml::xml_node<>* xmlNode, int camNum, const std::string& errMsg, bool isWarning)
  {
    if (xmlNode == nullptr)
    {
      if (isWarning)
      {
        displayCameraWarning(camNum, errMsg);
      }
      else
      {
        displayCameraError(camNum, errMsg);
      }
      return false;
    }
    return true;
  }

  // Is Atrributes present
  bool isAttrPresent(rapidxml::xml_node<>* parentNode, const std::string& attrName)
  {
    if (parentNode->first_attribute(attrName.c_str()))
      return true;
    else
      return false;
  }
  bool isAttrPresent(rapidxml::xml_node<>* parentNode, const std::string& attrName, const std::string& errMsg, bool isWarning)
  {
    if (parentNode->first_attribute(attrName.c_str()))
    {
      return true;
    }
    else
    {
      if (isWarning)
      {
        std::cout << std::endl << "XML read warning: " << parentNode->name() << " - Attribute Missing - " << attrName << " - " << errMsg;
      }
      else
      {
        std::cerr << std::endl << "XML read error: " << parentNode->name() << " - Attribute Missing - " << attrName << " - " << errMsg;
      }
      return false;
    }
  }

  // Get Atrributes
  float getAttrValueFloat(rapidxml::xml_node<>* parentNode, const std::string& attrName)
  {
    if(parentNode != nullptr)
    { 
      return std::stof(parentNode->first_attribute(attrName.c_str())->value());
    }
    else
    {
      return 0.0f;
    }
  }
  int getAttrValueInt(rapidxml::xml_node<>* parentNode, const std::string& attrName)
  {
    if (parentNode != nullptr)
    {
      return std::stoi(parentNode->first_attribute(attrName.c_str())->value());
    }
    else
    {
      return 0;
    }
  }
  std::string getAttrValueString(rapidxml::xml_node<>* parentNode, const std::string& attrName)
  {
    if (parentNode != nullptr)
    {
      return (parentNode->first_attribute(attrName.c_str())->value());
    }
    else
    {
      return "null";
    }
  }

  // Append nodes/atributes
  rapidxml::xml_node<>* appendNewNode(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* parentNode,
    const std::string& nodeName)
  {
    rapidxml::xml_node<>* newNode;
    char *node_name = doc->allocate_string(nodeName.c_str());
    newNode = doc->allocate_node(rapidxml::node_element, node_name);
    parentNode->append_node(newNode);
    return newNode;
  }

  rapidxml::xml_attribute<>* appendNewAttribute(rapidxml::xml_document<>* doc,
    rapidxml::xml_node<>* node, const std::string& attrName, const std::string& attrValue)
  {
    char* attrNameAlloc = doc->allocate_string(attrName.c_str());
    char* attrValueAlloc = doc->allocate_string(attrValue.c_str());
    auto newAttr = doc->allocate_attribute(attrNameAlloc, attrValueAlloc);
    node->append_attribute(newAttr);
    return newAttr;
  }

  rapidxml::xml_attribute<>* appendNewAttribute(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* node, const std::string& attrName, float attrValue, const char *fmt)
  {
    char buf[26];
    snprintf(buf, sizeof(buf), (fmt ? fmt : "%.7g"), attrValue);  // 7 digits for readability, 8 for lossless
    char* attrNameAlloc = doc->allocate_string(attrName.c_str());
    char* attrValueAlloc = doc->allocate_string(buf);
    auto newAttr = doc->allocate_attribute(attrNameAlloc, attrValueAlloc);
    node->append_attribute(newAttr);
    return newAttr;
  }

}