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
#include <memory>

#include "math_util/math_utility.h"
#include "xml_utility_common.h"
#include "xml_utility_hl.h"


namespace xmlutil
{
  bool readMediaForm(rapidxml::xml_node<>* mediaFormNode, nvstitchMediaForm& mediaForm)
  {
    if (!isAttrPresent(mediaFormNode, "form", "form attribute missing.")) { return false; }
    else
    {
      std::string formStr = getAttrValueString(mediaFormNode, "form");
      if (formStr == "file")
        mediaForm = nvstitchMediaForm::NVSTITCH_MEDIA_FORM_FILE;
      else if (formStr == "host_buffer")
        mediaForm = nvstitchMediaForm::NVSTITCH_MEDIA_FORM_HOST_BUFFER;
      else if (formStr == "device_buffer")
        mediaForm = nvstitchMediaForm::NVSTITCH_MEDIA_FORM_DEVICE_BUFFER;
      else if (formStr == "none")
        mediaForm = nvstitchMediaForm::NVSTITCH_MEDIA_FORM_NONE;
      else
      {
        displayError("invalid media form type specified.");
        return false;
      }
    }
    return true;
  }

  bool readMediaFormat(rapidxml::xml_node<>* mediaFormatNode, nvstitchMediaFormat& mediaFormat)
  {
    if (!isAttrPresent(mediaFormatNode, "format", "form attribute missing.")) { return false; }
    else
    {
      std::string formatStr = getAttrValueString(mediaFormatNode, "format");
      if (formatStr == "rgb8ui")
        mediaFormat = nvstitchMediaFormat::NVSTITCH_MEDIA_FORMAT_RGBA8UI;
      else if (formatStr == "yuv420")
        mediaFormat = nvstitchMediaFormat::NVSTITCH_MEDIA_FORMAT_YUV420;
      else if (formatStr == "h264")
        mediaFormat = nvstitchMediaFormat::NVSTITCH_MEDIA_FORMAT_H264_BITSTREAM;
      else if (formatStr == "mp4")
        mediaFormat = nvstitchMediaFormat::NVSTITCH_MEDIA_FORMAT_MP4;
      else
      {
        displayError("invalid media format type specified.");
        return false;
      }
    }
    return true;
  }

  bool readAudioForm(rapidxml::xml_node<>* audioFormNode, nvstitchAudioDataSource& audioDataSource)
  {
    if (!isAttrPresent(audioFormNode, "form", "form attribute missing.")) { return false; }
    else
    {
      std::string formStr = getAttrValueString(audioFormNode, "form");
      if (formStr == "file")
        audioDataSource = nvstitchAudioDataSource::NVSTITCH_AUDIO_DATA_SOURCE_FILE;
      else if (formStr == "buffer")
        audioDataSource = nvstitchAudioDataSource::NVSTITCH_AUDIO_DATA_SOURCE_BUFFER;
      else
      {
        displayError("invalid audio form type specified.");
        return false;
      }
    }
    return true;
  }

  bool readAudioFormat(rapidxml::xml_node<>* audioFormatNode, nvstitchAudioFormat& audioFormat)
  {
    if (!isAttrPresent(audioFormatNode, "format", "form attribute missing.")) { return false; }
    else
    {
      std::string formatStr = getAttrValueString(audioFormatNode, "format");
      if (formatStr == "aac")
        audioFormat = nvstitchAudioFormat::NVSTITCH_AUDIO_FORMAT_AAC_ADTS;
      else if (formatStr == "pcm16lsb" || formatStr == "pcm16le")
        audioFormat = nvstitchAudioFormat::NVSTITCH_AUDIO_FORMAT_PCM16LE;
      else if (formatStr == "pc32float")
        audioFormat = nvstitchAudioFormat::NVSTITCH_AUDIO_FORMAT_PCM32FLOAT;
      else
      {
        displayError("invalid audio format type specified.");
        return false;
      }
    }
    return true;
  }


  bool readAudioConfiguration(rapidxml::xml_node<>* audioConfigNode, nvstitchAudioConfiguration_t& audioConfig)
  {
    // Version
    audioConfig.version = NVSTITCH_VERSION;

    // format
    if (!readAudioFormat(audioConfigNode, audioConfig.format)) { return false; }


    // channels
    if (!isAttrPresent(audioConfigNode, "channels", "channels attribute missing.")) { return false; }
    else
    {
      audioConfig.channels = getAttrValueInt(audioConfigNode, "channels");
    }

    // sample_rate
    if (!isAttrPresent(audioConfigNode, "sample_rate", "sample_rate attribute missing.")) { return false; }
    else
    {
      audioConfig.sampleRate = getAttrValueInt(audioConfigNode, "sample_rate");
    }

    // bit_rate
    if (!isAttrPresent(audioConfigNode, "bit_rate", "bit_rate attribute missing.")) { return false; }
    else
    {
      audioConfig.bitRate = getAttrValueInt(audioConfigNode, "bit_rate");
    }

    // samples_per_frame
    if (!isAttrPresent(audioConfigNode, "samples_per_frame", "samples_per_frame attribute missing.")) { return false; }
    else
    {
      audioConfig.samplesPerFrame = getAttrValueInt(audioConfigNode, "samples_per_frame");
    }

    return true;
  }

  bool readVideoProjection(rapidxml::xml_node<>* videoProjectionNode, nvstitchPanoramaProjectionType& panoProjType)
  {
    if (!isAttrPresent(videoProjectionNode, "type", "type attribute missing.")) { return false; }
    else
    {
      std::string typeStr = getAttrValueString(videoProjectionNode, "type");
      if (typeStr == "equirectangular")
        panoProjType = nvstitchPanoramaProjectionType::NVSTITCH_PANORAMA_PROJECTION_EQUIRECTANGULAR;
      else
      {
        displayError("invalid video projection type specified.");
        return false;
      }
    }
    return true;
  }

  bool readAudioBlend(rapidxml::xml_node<>* audioBlendNode, nvstitchAudioOutputType& audioOutputType)
  {
    if (!isAttrPresent(audioBlendNode, "type", "type attribute missing.")) { return false; }
    else
    {
      std::string typeStr = getAttrValueString(audioBlendNode, "type");
      if (typeStr == "none")
        audioOutputType = nvstitchAudioOutputType::NVSTITCH_AUDIO_OUTPUT_NONE;
      else if (typeStr == "stereo_mixdown")
        audioOutputType = nvstitchAudioOutputType::NVSTITCH_AUDIO_OUTPUT_STEREO_MIXDOWN;
      else
      {
        displayError(audioBlendNode, "invalid audio blend type specified.");
        return false;
      }
    }
    return true;
  }

  bool readMediaFile(int camCount, rapidxml::xml_node<>* mediaFileNode, nvstitchPayload_t& mediaPayload, std::string& payloadFilename)
  {
		if (mediaPayload.payload_type != nvstitchMediaForm::NVSTITCH_MEDIA_FORM_FILE)
		{
			return false;
		}

    // name
    if (!isAttrPresent(mediaFileNode, "name", "name attribute missing.")) { return false; }
    else
    {
      payloadFilename = getAttrValueString(mediaFileNode, "name");
      mediaPayload.payload.file.name = payloadFilename.c_str();
    }

    // fps
    if (isAttrPresent(mediaFileNode, "fps_num") && isAttrPresent(mediaFileNode, "fps_den"))
    {
      mediaPayload.payload.file.fps_num = getAttrValueInt(mediaFileNode, "fps_num");
      mediaPayload.payload.file.fps_den = getAttrValueInt(mediaFileNode, "fps_den");
    }
    else if (isAttrPresent(mediaFileNode, "fps"))
    {
      std::string fpsStr = getAttrValueString(mediaFileNode, "fps");
      std::size_t delimiterPos = fpsStr.find("/");
      if (delimiterPos == std::string::npos)
        displayCameraError(camCount, "fps delimiter '/' missing.");
      else
      {
        std::string numFpsStr = fpsStr.substr(0, delimiterPos);
        std::string denFpsStr = fpsStr.substr(delimiterPos + 1, fpsStr.length() - delimiterPos);
        mediaPayload.payload.file.fps_num = std::stoi(numFpsStr);
        mediaPayload.payload.file.fps_den = std::stoi(denFpsStr);
      }
    }

    // dt
    if (isAttrPresent(mediaFileNode, "dt"))
    {
        mediaPayload.payload.file.dt = getAttrValueFloat(mediaFileNode, "dt");
        if (mediaPayload.payload.file.dt < 0.f)
            displayCameraError(camCount, "file dt parameter must be positive");
    }
    else
    {
        mediaPayload.payload.file.dt = 0.f;
    }

    // width, height
    if (!(isAttrPresent(mediaFileNode, "width") &&
      isAttrPresent(mediaFileNode, "height")))
      displayCameraError(camCount, std::string(mediaFileNode->name()) + " : Missing width or height attribute.");
    else
    {
      mediaPayload.image_size.x = getAttrValueInt(mediaFileNode, "width");
      mediaPayload.image_size.y = getAttrValueInt(mediaFileNode, "height");
    }
    return true;
  }

	bool readMediaBuffer(int camCount, rapidxml::xml_node<>* mediaBufferNode, nvstitchPayload_t& mediaPayload)
	{
		if (mediaPayload.payload_type != nvstitchMediaForm::NVSTITCH_MEDIA_FORM_DEVICE_BUFFER &&
			mediaPayload.payload_type != nvstitchMediaForm::NVSTITCH_MEDIA_FORM_HOST_BUFFER)
		{
			return false;
		}
			
		// width, height
		if (!(isAttrPresent(mediaBufferNode, "width") && isAttrPresent(mediaBufferNode, "height")))
			displayCameraError(camCount, std::string(mediaBufferNode->name()) + ": Missing width or height attribute.");
		else
		{
			mediaPayload.image_size.x = getAttrValueInt(mediaBufferNode, "width");
			mediaPayload.image_size.y = getAttrValueInt(mediaBufferNode, "height");
		}
		return true;
	}

  bool readAudioFile(int audioSrcNum, rapidxml::xml_node<>* audioFileNode, nvstitchAudioPayload_t& audioPayload, std::string& payloadFilename)
  {
    audioSrcNum = 0;

    // name
    if (!isAttrPresent(audioFileNode, "name", "name attribute missing.")) { return false; }
    else
    {
      payloadFilename = getAttrValueString(audioFileNode, "name");
      audioPayload.payload.file.name = (char*)payloadFilename.c_str();
    }

    return true;
  }
  
  static rapidxml::xml_node<>* findAudioRigNode(rapidxml::xml_document<> *root)
  {
    rapidxml::xml_node<>  *rigNode, *nvstitchNode;
    if (!(nullptr != (nvstitchNode = root->first_node("nvstitch")) && nullptr != (rigNode = nvstitchNode->first_node("rig"))) && // nested in nvstitch node
          nullptr == (rigNode = root->first_node("rig"))  // rig is a root node
    )
      rigNode = root->first_node("audio_rig");            // backwards compatibility
    return rigNode;
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
  
  bool readInputMediaFeedXml(const std::string& xmlPath, std::vector<nvstitchPayload_t>& mediaPayloadArray, 
    std::vector<std::string>& payloadFilenames)
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

    rapidxml::xml_node<>* camRigNode = findCameraRigNode(&doc);
    if (!isNodePresent(camRigNode, "no rig node found with cameras."))
    {
      return false;
    }

    int camCount = 0;
    for (rapidxml::xml_node<>* currCamNode = camRigNode->first_node("camera"); currCamNode;
      currCamNode = currCamNode->next_sibling("camera"))
    {
      
      camCount++;

      nvstitchPayload_t currMediaPayload;

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
              std::string formStr =  getAttrValueString(inputMediaFormNode, "form");
              if(formStr == "file")
                currMediaPayload.payload_type = nvstitchMediaForm::NVSTITCH_MEDIA_FORM_FILE;
              else if (formStr == "host_buffer")
                currMediaPayload.payload_type = nvstitchMediaForm::NVSTITCH_MEDIA_FORM_HOST_BUFFER;
              else if (formStr == "device_buffer")
                currMediaPayload.payload_type = nvstitchMediaForm::NVSTITCH_MEDIA_FORM_DEVICE_BUFFER;
              else
              {
                std::cerr << "Invalid input_media_form: " << formStr << "\n";
                return false;
              }
            }
          }
          //</input_media_form>
          
          //<input_media_file>
          rapidxml::xml_node<>* inputMediaFileNode = inputMediaFeedNode->first_node("input_media_file");
          if (!isNodePresent(inputMediaFileNode)) { /* Optional */ }
          else
          {
            payloadFilenames.push_back(std::string());
            if (!readMediaFile(camCount, inputMediaFileNode, currMediaPayload, payloadFilenames.back())) { return false; }
          }
        mediaPayloadArray.push_back(currMediaPayload);
          //</input_media_file>

        //</input_media_feed>
      }
    }

    return true;
  }

  bool readInputAudioFeedXml(const std::string& xmlPath, std::vector<nvstitchAudioPayload_t>& audioPayloadArray, 
    std::vector<std::string>& payloadFilenames)
  {
    std::cout << std::endl << "XmlUtil: Reading Input Audio Feed XML: " << xmlPath;
    
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

    rapidxml::xml_node<>* audioRigNode = findAudioRigNode(&doc);
    if (!isNodePresent(audioRigNode, "no rig node found with audio."))
    {
      return false;
    }

    int audioSrcCount = 0;
    for (rapidxml::xml_node<>* currAudioSrcNode = audioRigNode->first_node("audio_source"); currAudioSrcNode;
      currAudioSrcNode = currAudioSrcNode->next_sibling("audio_source"))
    {
      audioSrcCount++;
      
      rapidxml::xml_node<>* inputAudioFeedNode = currAudioSrcNode->first_node("input_audio_feed");
      if (!isNodePresent(inputAudioFeedNode, "input_audio_feed node not found.")) { return false; }
      else
      {
        nvstitchAudioPayload_t currAudioPayload = nvstitchAudioPayload_t();
        //<input_audio_feed>

          //<input_audio_file>
          rapidxml::xml_node<>* inputAudioFileNode = inputAudioFeedNode->first_node("input_audio_file");
          if (!isNodePresent(inputAudioFileNode)) { /* Optional */ }
          else
          {
            payloadFilenames.push_back(std::string());
            if (!readAudioFile(audioSrcCount, inputAudioFileNode, currAudioPayload, payloadFilenames.back())) { return false; }
          }
          audioPayloadArray.push_back(currAudioPayload);
          //</input_audio_file>

        //</input_audio_feed>
      }
    }

    if (audioSrcCount == 0)
    {
      displayError("No audio input feed  nodes found.");
      return false;
    }

    return true;
  }

  static rapidxml::xml_node<>* findStitcherPropertiesNode(rapidxml::xml_document<> *root)
  {
    rapidxml::xml_node<>  *propNode, *nvstitchNode;
    if (!(nullptr != (nvstitchNode = root->first_node("nvstitch")) && nullptr != (propNode = nvstitchNode->first_node("stitcher_properties")))) // nested in nvstitch node
      propNode = root->first_node("stitcher_properties");
    return propNode;
  }

#if 0
  static rapidxml::xml_node<>* findOutputAudioPropertiesNode(rapidxml::xml_document<> *root)
  {
    rapidxml::xml_node<>  *propNode, *nvstitchNode;
    if (!(nullptr != (nvstitchNode = root->first_node("nvstitch")) && nullptr != (propNode = nvstitchNode->first_node("output_audio_properties")))) // nested in nvstitch node
      propNode = root->first_node("output_audio_properties");
    return propNode;
  }
#endif

  bool readStitcherPropertiesFromStitcherPropertiesXml(const std::string& stitcherPropertiesXmlPath,
    nvstitchStitcherProperties_t* stitcherProperties, std::vector<std::string>& payloadFilenames, bool useAudio = true)
  {
    rapidxml::xml_document<> doc;
    // Get Document
    std::string xmFileString;
    if (!getXmlFileString(stitcherPropertiesXmlPath, xmFileString))
    {
      displayError("Unable to open stitcher properties xml file: " + stitcherPropertiesXmlPath);
      return false;
    }
    try
    {
      doc.parse<0>((char*)xmFileString.c_str());
    }
    catch (const rapidxml::parse_error& e)
    {
      std::cerr << "Parsing error with xml:" << stitcherPropertiesXmlPath << "\n";
      std::cerr << "Parse error was: " << e.what() << "\n";
      return false;
    }

    // Check stitcherProperties is null
    if (stitcherProperties == nullptr)
    {
        std::cerr << "Input arg. stitcherProperties is null\n";
    }

    stitcherProperties->version = NVSTITCH_VERSION;

    //<stitcher_properties>
    rapidxml::xml_node<>* stitcherPropertiesNode = findStitcherPropertiesNode(&doc);
    if (!isNodePresent(stitcherPropertiesNode, "stitcher_properties node not found.")) { return false; }


    //<input_media_form>
    rapidxml::xml_node<>* inputMediaFormNode = stitcherPropertiesNode->first_node("input_media_form");
    if (!isNodePresent(inputMediaFormNode, "input_media_form node not found.")) { return false; }
    else
    {
      if (!readMediaForm(inputMediaFormNode, stitcherProperties->input_form)) { return false; }
    }

    //<input_media_format>
    rapidxml::xml_node<>* inputMediaFormatNode = stitcherPropertiesNode->first_node("input_media_format");
    if (!isNodePresent(inputMediaFormatNode, "input_media_format node not found.")) { return false; }
    else
    {
      if (!readMediaFormat(inputMediaFormatNode, stitcherProperties->input_format)) { return false; }
    }

    auto tmp_video_pipeline_options = std::make_unique<nvstitchVideoPipelineOptions_t>();
    auto tmp_output_payloads = std::make_unique<nvstitchPayload_t>();

    //<output_video_properties>
    rapidxml::xml_node<>* outputVideoPropertiesNode = stitcherPropertiesNode->first_node("output_video_properties");
    if (!isNodePresent(outputVideoPropertiesNode, "output_video_properties node not found.")) { return false; }
    else
    {
      //<output_video_projection>
      rapidxml::xml_node<>* outputVideoProjectionNode = outputVideoPropertiesNode->first_node("output_video_projection");
      if (!isNodePresent(outputVideoProjectionNode, "output_video_properties node not found.")) { return false; }
      else
      {
        if (!readVideoProjection(outputVideoProjectionNode, stitcherProperties->output_projection)) { return false; }
      }

      stitcherProperties->video_pipeline_options = tmp_video_pipeline_options.get();

      //<output_video_options>
      rapidxml::xml_node<>* outputVideoOptionsNode = outputVideoPropertiesNode->first_node("output_video_options");
      if (!isNodePresent(outputVideoOptionsNode, "output_video_options node not found.")) { return false; }
      if (!isAttrPresent(outputVideoOptionsNode, "stereo_ipd", "stereo_ipd attribute missing. assigning negative value- stitcher will use default value.", true))
      { 
          stitcherProperties->video_pipeline_options->options.stereo.ipd = -1.0f;
      }
      else
      {
        stitcherProperties->video_pipeline_options->options.stereo.ipd =
          getAttrValueFloat(outputVideoOptionsNode, "stereo_ipd");
      }
      if (!isAttrPresent(outputVideoOptionsNode, "min_dist", "min_dist attribute missing. assigning default value.", true))
      {
          stitcherProperties->video_pipeline_options->options.stereo.min_dist = 0;
      }
      else
      {
          stitcherProperties->video_pipeline_options->options.stereo.min_dist =
              getAttrValueFloat(outputVideoOptionsNode, "min_dist");
      }
      if (!isAttrPresent(outputVideoOptionsNode, "quality", "quality attribute missing.")) { return false; }
      else
      {
        std::string qualityStr = getAttrValueString(outputVideoOptionsNode, "quality");
        if (qualityStr == "low")
          stitcherProperties->video_pipeline_options->stitch_quality = NVSTITCH_STITCHER_QUALITY_LOW;
        else if (qualityStr == "medium")
          stitcherProperties->video_pipeline_options->stitch_quality = NVSTITCH_STITCHER_QUALITY_MEDIUM;
        else if (qualityStr == "high")
          stitcherProperties->video_pipeline_options->stitch_quality = NVSTITCH_STITCHER_QUALITY_HIGH;
      }
      if (!isAttrPresent(outputVideoOptionsNode, "pipeline", "video pipeline type attribute missing.")) { return false; }
      else
      {
        std::string pipelineStr = getAttrValueString(outputVideoOptionsNode, "pipeline");
        if (pipelineStr == "mono")
          stitcherProperties->video_pipeline_options->pipeline_type = NVSTITCH_STITCHER_PIPELINE_MONO;
        else if (pipelineStr == "stereo")
          stitcherProperties->video_pipeline_options->pipeline_type = NVSTITCH_STITCHER_PIPELINE_STEREO;
      }

      //<output_video_format>
      rapidxml::xml_node<>* outputMediaFormatNode = outputVideoPropertiesNode->first_node("output_video_format");
      if (!isNodePresent(outputMediaFormatNode, "output_video_format node not found.")) { return false; }
      else
      {
        if (!readMediaFormat(outputMediaFormatNode, stitcherProperties->output_format)) { return false; }
      }

      //<output_video_payloads>
      rapidxml::xml_node<>* outputVideoPayloadsNode = outputVideoPropertiesNode->first_node("output_video_payloads");
      if (isNodePresent(outputVideoPayloadsNode, "output_video_payloads node not found.", true))
      {
        if (!isAttrPresent(outputVideoPayloadsNode, "count", "count attribute missing.")) { return false; }
        else
        {
          stitcherProperties->num_output_payloads = getAttrValueInt(outputVideoPayloadsNode, "count");
        }
      }
      else
      {
          stitcherProperties->num_output_payloads = 1;
      }

      //<output_video_form>
      stitcherProperties->output_payloads = tmp_output_payloads.get();
      rapidxml::xml_node<>* outputVideoFormNode = outputVideoPropertiesNode->first_node("output_video_form");
      if (!isNodePresent(outputVideoFormNode, "output_video_form node not found.")) { return false; }
      else
      {
        if (!readMediaForm(outputVideoFormNode, stitcherProperties->output_payloads->payload_type)) { return false; }
      }

      //<output_video_file>
			if (stitcherProperties->output_payloads->payload_type == nvstitchMediaForm::NVSTITCH_MEDIA_FORM_FILE)
			{
				rapidxml::xml_node<>* outputVideoFileNode = outputVideoPropertiesNode->first_node("output_video_file");
				if (!isNodePresent(outputVideoFileNode, "output_video_file node not found.")) { return false; }
				
        payloadFilenames.push_back(std::string());
        if (!readMediaFile(-1, outputVideoFileNode, *stitcherProperties->output_payloads, payloadFilenames.back())) { return false; }
			}

			//<output_video_payload_buffer>
			// Currently only single payload buffer input supported.
			else if(stitcherProperties->output_payloads->payload_type == nvstitchMediaForm::NVSTITCH_MEDIA_FORM_DEVICE_BUFFER 
				|| stitcherProperties->output_payloads->payload_type == nvstitchMediaForm::NVSTITCH_MEDIA_FORM_HOST_BUFFER)
			{
				rapidxml::xml_node<>* outputVideoFileNode = outputVideoPropertiesNode->first_node("output_video_payload_buffer");
				
				// For backward compatibility, search for "output_video_file" node,
				// if "output_video_payload_buffer" not found
				if(outputVideoFileNode == nullptr)
					outputVideoFileNode = outputVideoPropertiesNode->first_node("output_video_file");

				if (!readMediaBuffer(-1, outputVideoFileNode, *stitcherProperties->output_payloads)) { return false; }
			}
    }

    tmp_video_pipeline_options.release();
    tmp_output_payloads.release();

    //<output_audio_properties>
    if(useAudio)
    {
      rapidxml::xml_node<>* outputAudioPropertiesNode = stitcherPropertiesNode->first_node("output_audio_properties");
      if (!isNodePresent(outputAudioPropertiesNode, "output_audio_properties node not found.")) { return false; }
      else
      {
      // <output_audio_gain>
      // Set volume of output audio (defaults to 1.0 if not set)
        rapidxml::xml_node<>* outputAudioGain = outputAudioPropertiesNode->first_node("output_audio_gain");
        if (!isNodePresent(outputAudioGain) || !isAttrPresent(outputAudioGain, "value"))
        {
            stitcherProperties->audio_output_gain = 1.0f;
        }
        else
        {
            stitcherProperties->audio_output_gain =  getAttrValueFloat(outputAudioGain, "value");
        }
        //<output_audio_blend>
        rapidxml::xml_node<>* outputAudioBlendNode = outputAudioPropertiesNode->first_node("output_audio_blend");
        if (!isNodePresent(outputAudioBlendNode, "output_audio_blend node not found.")) { return false; }
        else
        {
          if (!readAudioBlend(outputAudioBlendNode, stitcherProperties->audio_output_blend)) { return false; }
        }

        if (stitcherProperties->audio_output_blend != nvstitchAudioOutputType::NVSTITCH_AUDIO_OUTPUT_NONE)
        {
          //<output_audio_form>
          rapidxml::xml_node<>* outputAudioFormNode = outputAudioPropertiesNode->first_node("output_audio_form");
          if (!isNodePresent(outputAudioFormNode, "output_audio_form node not found.")) { return false; }
          else
          {
            if (!readAudioForm(outputAudioFormNode, stitcherProperties->audio_output_form)) { return false; }
          }

          //<output_audio_feed_config>
          rapidxml::xml_node<>* outputAudioFeedConfigNode = outputAudioPropertiesNode->first_node("output_audio_feed_config");
          if (!isNodePresent(outputAudioFeedConfigNode, "output_audio_feed_config node not found.")) { return false; }
          else
          {
            stitcherProperties->audio_output_format = new nvstitchAudioConfiguration_t();
            if (!readAudioConfiguration(outputAudioFeedConfigNode, *stitcherProperties->audio_output_format))
            {
              delete stitcherProperties->audio_output_format;
              return false;
            }
          }
        }
      }
    }

    return true;
  }

  bool readStitcherPropertiesFromAudioInputFeedXml(const std::string& inputAudioFeedXmlPath,
    nvstitchStitcherProperties_t* stitcherProperties)
  {
    // Check stitcherProperties is null
    if (stitcherProperties == nullptr)
    {
      std::cerr << "Input arg. stitcherProperties is null\n";
    }

    rapidxml::xml_document<> doc;
    // Get Document
    std::string xmFileString;
    if (!getXmlFileString(inputAudioFeedXmlPath, xmFileString))
    {
      displayError("Unable to open xml file: " + inputAudioFeedXmlPath);
      return false;
    }
    try
    {
      doc.parse<0>((char*)xmFileString.c_str());
    }
    catch (const rapidxml::parse_error& e)
    {
        std::cerr << "Parsing error with xml:" << inputAudioFeedXmlPath << "\n";
        std::cerr << "Parse error was: " << e.what() << "\n";
        return false;
    }

    stitcherProperties->version = NVSTITCH_VERSION;

    rapidxml::xml_node<>* audioRigNode = findAudioRigNode(&doc);
    if (!isNodePresent(audioRigNode, "no rig node found with audio."))
    {
      return false;
    }

    uint32_t audioSrcCount = 0;
    auto audioInputFormList = std::make_unique<std::vector<nvstitchAudioDataSource>>();
    //auto audioInputConfigList = std::make_unique<std::vector<nvstitchAudioConfiguration_t>>();
    auto audioInputConfigPtrList = std::make_unique<std::vector<nvstitchAudioConfiguration_t*>>();

    for (rapidxml::xml_node<>* currAudioSrcNode = audioRigNode->first_node("audio_source"); currAudioSrcNode;
      currAudioSrcNode = currAudioSrcNode->next_sibling("audio_source"))
    {
      audioSrcCount++;

      rapidxml::xml_node<>* inputAudioFeedNode = currAudioSrcNode->first_node("input_audio_feed");
      if (!isNodePresent(inputAudioFeedNode, "input_audio_feed node not found.")) { return false; }
      else
      {
        //<input_audio_form>
        nvstitchAudioDataSource currAudioInputForm;
        rapidxml::xml_node<>* inputAudioFormNode = inputAudioFeedNode->first_node("input_audio_form");
        if (!isNodePresent(inputAudioFormNode, "input_audio_form node not found.")) { return false; }
        else
        {
          if (!readAudioForm(inputAudioFormNode, currAudioInputForm)) { return false; }
        }
        audioInputFormList->push_back(currAudioInputForm);

        //<input_audio_config> 
        nvstitchAudioConfiguration_t currAudioInputConfig;
        rapidxml::xml_node<>* inputAudioConfigNode = inputAudioFeedNode->first_node("input_audio_config");
        if (!isNodePresent(inputAudioConfigNode))
        {
          // Config info is not present
          // Get it from the demuxer
          audioInputConfigPtrList->push_back(nullptr);
        }
        else
        {
          if (!readAudioConfiguration(inputAudioConfigNode, currAudioInputConfig)) { return false; }
          audioInputConfigPtrList->push_back(new nvstitchAudioConfiguration_t(currAudioInputConfig));
        }

        //audioInputConfigList->push_back(currAudioInputConfig);
      }
    }

    if (audioSrcCount == 0)
    {
      displayError("No audio input feed nodes found.");
      return false;
    }

    stitcherProperties->num_audio_inputs = audioSrcCount;
    stitcherProperties->audio_input_form = audioInputFormList->data();
    stitcherProperties->audio_input_configurations = audioInputConfigPtrList->data();

    audioInputFormList.release();
    audioInputConfigPtrList.release();

    return true;
  }


  bool readStitcherPropertiesXml(const std::string& stitcherPropertiesXmlPath,
    nvstitchStitcherProperties_t* stitcherProperties, std::vector<std::string>& payloadFilenames,
    const std::string& inputAudioFeedXmlPath)
  {
    std::cout << std::endl << "XmlUtil: Reading Stitcher Properties XML: " << stitcherPropertiesXmlPath;
    
    bool useAudio = !std::empty(inputAudioFeedXmlPath);
    

    if (!readStitcherPropertiesFromStitcherPropertiesXml(stitcherPropertiesXmlPath, stitcherProperties, payloadFilenames, useAudio))
      return false;
    
    if(useAudio)
      if (!readStitcherPropertiesFromAudioInputFeedXml(inputAudioFeedXmlPath, stitcherProperties))
        return false;

    return true;
  }

  bool writeRigXml(
    const std::string& xmlPath,
    const nvstitchVideoRigProperties_t* videoRigProperties, const nvstitchPayload_t mediaPayloadArray[],
    const nvstitchAudioRigProperties_t* audioRigProperties, const nvstitchAudioPayload_t audioPayloadArray[]
  )
  {
    // Open XML file for writing
    std::ofstream fout;
    fout.open(xmlPath, std::ios::trunc);

    if (!fout.is_open())
    {
      std::cerr << "Error: Cannot open output XML file\n";
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
    appendAudioRigToRootNode(audioRigProperties, doc, rigNode);

    // Writing payload in the rig_desc is currently unsupported 
    (void)mediaPayloadArray;
    (void)audioPayloadArray;

    fout << (*doc);
    fout.close();
    delete doc;

    return true;
  }

  const std::string getMediaFormatString(const nvstitchMediaFormat& mediaFormat)
  {
    if (mediaFormat == nvstitchMediaFormat::NVSTITCH_MEDIA_FORMAT_MP4)
      return "mp4";
    else if (mediaFormat == nvstitchMediaFormat::NVSTITCH_MEDIA_FORMAT_H264_BITSTREAM)
      return "h264";
    else if (mediaFormat == nvstitchMediaFormat::NVSTITCH_MEDIA_FORMAT_RGBA8UI)
      return "rgb8ui";
    else if (mediaFormat == nvstitchMediaFormat::NVSTITCH_MEDIA_FORMAT_YUV420)
      return "yuv420";
    else
      return "invalid";
  }
  const std::string getMediaFormString(const nvstitchMediaForm& mediaForm)
  {
    if (mediaForm == nvstitchMediaForm::NVSTITCH_MEDIA_FORM_DEVICE_BUFFER)
      return "device_buffer";
    else if (mediaForm == nvstitchMediaForm::NVSTITCH_MEDIA_FORM_HOST_BUFFER)
      return "host_buffer";
    else if (mediaForm == nvstitchMediaForm::NVSTITCH_MEDIA_FORM_FILE)
      return "file";
    else if (mediaForm == nvstitchMediaForm::NVSTITCH_MEDIA_FORM_NONE)
      return "none";
    else
      return "invalid";
  }

  const std::string getAudioFormatString(const nvstitchAudioFormat& audioFormat)
  {
    if (audioFormat == nvstitchAudioFormat::NVSTITCH_AUDIO_FORMAT_AAC_ADTS)
      return "aac";
    else if (audioFormat == nvstitchAudioFormat::NVSTITCH_AUDIO_FORMAT_PCM16LE)
      return "pcm16le";
    else if (audioFormat == nvstitchAudioFormat::NVSTITCH_AUDIO_FORMAT_PCM32FLOAT)
      return "pcm32float";
    else
      return "invalid";
  }
  const std::string getAudioFormString(const nvstitchAudioDataSource& audioForm)
  {
    if (audioForm == nvstitchAudioDataSource::NVSTITCH_AUDIO_DATA_SOURCE_BUFFER)
      return "buffer";
    else if (audioForm == nvstitchAudioDataSource::NVSTITCH_AUDIO_DATA_SOURCE_FILE)
      return "file";
    else
      return "invalid";
  }

  const std::string getAudioInputTypeString(const nvstitchAudioInputType& audioInputType)
  {
    if (audioInputType == nvstitchAudioInputType::NVSTITCH_AUDIO_INPUT_TYPE_OMNI)
      return "omni";
    else if (audioInputType == nvstitchAudioInputType::NVSTITCH_AUDIO_INPUT_TYPE_SHOTGUN_MIC)
      return "shotgun";
    else if (audioInputType == nvstitchAudioInputType::NVSTITCH_AUDIO_INPUT_TYPE_CARDIOID_MIC)
      return "cardioid";
    else if (audioInputType == nvstitchAudioInputType::NVSTITCH_AUDIO_INPUT_TYPE_SUPERCARDIOID_MIC)
      return "supercardioid";
    else
      return "invalid";
  }
  const std::string getAudioOutputTypeString(const nvstitchAudioOutputType& audioBlend)
  {
      if (audioBlend == nvstitchAudioOutputType::NVSTITCH_AUDIO_OUTPUT_NONE)
          return "none";
      else if (audioBlend == nvstitchAudioOutputType::NVSTITCH_AUDIO_OUTPUT_STEREO_MIXDOWN)
          return "stereo_mixdown";
      else
          return "invalid";
  }

  bool appendStitcherPropertiesToRootNode(const nvstitchStitcherProperties_t* stitcherProperties, rapidxml::xml_document<>* doc, rapidxml::xml_node<>* root)
  {
    if (!root)
      root = doc;

    // <stitcher_properties>
    rapidxml::xml_node<>* stitcherPropNode = appendNewNode(doc, root, "stitcher_properties");

    //<input_media_form>
    rapidxml::xml_node<>* inputMediaFormNode = appendNewNode(doc, stitcherPropNode, "input_media_form");
    appendNewAttribute(doc, inputMediaFormNode, "form", getMediaFormString(stitcherProperties->input_form));

    //<input_media_format>
    rapidxml::xml_node<>* inputMediaFormatNode = appendNewNode(doc, stitcherPropNode, "input_media_format");
    appendNewAttribute(doc, inputMediaFormatNode, "format", getMediaFormatString(stitcherProperties->input_format));

    //<output_video_properties>
    {
      rapidxml::xml_node<>* outputVideoPropNode = appendNewNode(doc, stitcherPropNode, "output_video_properties");

      //<output_video_properties>
      rapidxml::xml_node<>* outputVideoProjNode = appendNewNode(doc, outputVideoPropNode, "output_video_projection");
      if (stitcherProperties->output_projection == nvstitchPanoramaProjectionType::NVSTITCH_PANORAMA_PROJECTION_EQUIRECTANGULAR)
        appendNewAttribute(doc, outputVideoProjNode, "type", "equirectangular");

      //<output_video_options>
      rapidxml::xml_node<>* outputVideoOptionsNode = appendNewNode(doc, outputVideoPropNode, "output_video_options");
      float ipd = stitcherProperties->video_pipeline_options->options.stereo.ipd;
      if (ipd != 0)
        appendNewAttribute(doc, outputVideoOptionsNode, "stereo_ipd", (ipd));
      nvstitchStitcherQuality quality = stitcherProperties->video_pipeline_options->stitch_quality;
      appendNewAttribute(doc, outputVideoOptionsNode, "quality",
        (quality == NVSTITCH_STITCHER_QUALITY_LOW)  ? "low" :
        (quality == NVSTITCH_STITCHER_QUALITY_HIGH) ? "high" : "medium"
      );
      appendNewAttribute(doc, outputVideoOptionsNode, "pipeline",
        (stitcherProperties->video_pipeline_options->pipeline_type == NVSTITCH_STITCHER_PIPELINE_STEREO) ? "stereo" : "mono");

      // <output_video_form>
      rapidxml::xml_node<>* outputMediaFormNode = appendNewNode(doc, outputVideoPropNode, "output_video_form");
      appendNewAttribute(doc, outputMediaFormNode, "form", getMediaFormString(stitcherProperties->output_payloads->payload_type));

      // <output_video_format>
      rapidxml::xml_node<>* outputMediaFormatNode = appendNewNode(doc, outputVideoPropNode, "output_video_format");
      appendNewAttribute(doc, outputMediaFormatNode, "format", getMediaFormatString(stitcherProperties->output_format));

      // <output_video_payloads>
      rapidxml::xml_node<>* outputVideoPayloadsNode = appendNewNode(doc, outputVideoPropNode, "output_video_payloads");
      appendNewAttribute(doc, outputVideoPayloadsNode, "count", std::to_string(stitcherProperties->num_output_payloads));

      // <output_video_file>
      if (stitcherProperties->output_payloads->payload_type == nvstitchMediaForm::NVSTITCH_MEDIA_FORM_FILE)
      {
        rapidxml::xml_node<>* outputVideoFileNode = appendNewNode(doc, outputVideoPropNode, "output_video_file");
        appendNewAttribute(doc, outputVideoFileNode, "name", stitcherProperties->output_payloads->payload.file.name);
        appendNewAttribute(doc, outputVideoFileNode, "fps_num", 
          std::to_string(stitcherProperties->output_payloads->payload.file.fps_num));
        appendNewAttribute(doc, outputVideoFileNode, "fps_den",
          std::to_string(stitcherProperties->output_payloads->payload.file.fps_den));
        appendNewAttribute(doc, outputVideoFileNode, "width",
          std::to_string(stitcherProperties->output_payloads->image_size.x));
        appendNewAttribute(doc, outputVideoFileNode, "height",
          std::to_string(stitcherProperties->output_payloads->image_size.y));
      }
    }


    // <output_audio_properties>
    {
      rapidxml::xml_node<>* outputAudioPropNode = appendNewNode(doc, stitcherPropNode, "output_audio_properties");

      // <output_audio_gain>
      rapidxml::xml_node<>* outputAudioGainNode = appendNewNode(doc, outputAudioPropNode, "output_audio_gain");
      appendNewAttribute(doc, outputAudioGainNode, "value", stitcherProperties->audio_output_gain);

      // <output_audio_blend>
      rapidxml::xml_node<>* outputAudioBlendNode = appendNewNode(doc, outputAudioPropNode, "output_audio_blend");
      appendNewAttribute(doc, outputAudioBlendNode, "type",
        (stitcherProperties->audio_output_blend == nvstitchAudioOutputType::NVSTITCH_AUDIO_OUTPUT_STEREO_MIXDOWN) ? "stereo_mixdown" : "none"
      );

      // <output_audio_form>
      rapidxml::xml_node<>* outputAudioFormNode = appendNewNode(doc, outputAudioPropNode, "output_audio_form");
      appendNewAttribute(doc, outputAudioFormNode, "form",
        (stitcherProperties->audio_output_form == nvstitchAudioDataSource::NVSTITCH_AUDIO_DATA_SOURCE_FILE)   ? "file" : 
        (stitcherProperties->audio_output_form == nvstitchAudioDataSource::NVSTITCH_AUDIO_DATA_SOURCE_BUFFER) ? "buffer" : "unknown"
      );

      // <output_audio_feed_config>
      rapidxml::xml_node<>* outputAudioFeedConfigNode = appendNewNode(doc, outputAudioPropNode, "output_audio_feed_config");
      appendNewAttribute(doc, outputAudioFeedConfigNode, "format",
        (stitcherProperties->audio_output_format->format == nvstitchAudioFormat::NVSTITCH_AUDIO_FORMAT_AAC_ADTS)   ? "aac" :
        (stitcherProperties->audio_output_format->format == nvstitchAudioFormat::NVSTITCH_AUDIO_FORMAT_PCM16LE)    ? "pcm16le" :
        (stitcherProperties->audio_output_format->format == nvstitchAudioFormat::NVSTITCH_AUDIO_FORMAT_PCM32FLOAT) ? "pc32float" : "unknown"
      );
      appendNewAttribute(doc, outputAudioFeedConfigNode, "channels",          std::to_string(stitcherProperties->audio_output_format->channels));
      appendNewAttribute(doc, outputAudioFeedConfigNode, "sample_rate",       std::to_string(stitcherProperties->audio_output_format->sampleRate));
      appendNewAttribute(doc, outputAudioFeedConfigNode, "bit_rate",          std::to_string(stitcherProperties->audio_output_format->bitRate));
      appendNewAttribute(doc, outputAudioFeedConfigNode, "samples_per_frame", std::to_string(stitcherProperties->audio_output_format->samplesPerFrame));
    }
      
    return true;
  }

  bool writeStitcherPropertiesXml(const std::string& xmlPath, const nvstitchStitcherProperties_t* stitcherProperties)
  {
    // Open XML file for writing
    std::ofstream fout;
    fout.open(xmlPath, std::ios::trunc);

    if (!fout.is_open())
    {
      std::cerr << "Error: Cannot open output XML file\n";
      return 1;
    }

    // Generate XML document
    rapidxml::xml_document<>* doc = new rapidxml::xml_document<>();
    rapidxml::xml_node<>* decl = doc->allocate_node(rapidxml::node_declaration);
    
    // Write decl tag
    decl->append_attribute(doc->allocate_attribute("version", "1.0"));
    decl->append_attribute(doc->allocate_attribute("encoding", "utf-8"));
    doc->append_node(decl);
    
    if (!appendStitcherPropertiesToRootNode(stitcherProperties, doc, doc))
    {
      return false;
    }

    fout << *doc;
    fout.close();
    delete doc;
    return true;
  }


  void appendMediaPayloadToCameraNode(const nvstitchPayload_t& mediaPayload, rapidxml::xml_document<>* doc, rapidxml::xml_node<>* cameraNode)
  {
    rapidxml::xml_node<>* inputMediaFeedNode = appendNewNode(doc, cameraNode, "input_media_feed");
    rapidxml::xml_node<>* inputMediaForm = appendNewNode(doc, inputMediaFeedNode, "input_media_form");
    appendNewAttribute(doc, inputMediaForm, "form", getMediaFormString(mediaPayload.payload_type));

    // <input_media_file>
    if (mediaPayload.payload_type == nvstitchMediaForm::NVSTITCH_MEDIA_FORM_FILE)
    {
      rapidxml::xml_node<>* outputVideoFileNode = appendNewNode(doc, inputMediaFeedNode, "input_media_file");
      appendNewAttribute(doc, outputVideoFileNode, "name", mediaPayload.payload.file.name);
      appendNewAttribute(doc, outputVideoFileNode, "fps_num",
        std::to_string(mediaPayload.payload.file.fps_num));
      appendNewAttribute(doc, outputVideoFileNode, "fps_den",
        std::to_string(mediaPayload.payload.file.fps_den));
      appendNewAttribute(doc, outputVideoFileNode, "dt",
        std::to_string(mediaPayload.payload.file.dt));
      appendNewAttribute(doc, outputVideoFileNode, "width",
        std::to_string(mediaPayload.image_size.x));
      appendNewAttribute(doc, outputVideoFileNode, "height",
        std::to_string(mediaPayload.image_size.y));
    }
    else
    {
      // TODO: deal with other types
    }
  }


  void appendMediaPayloadToRigNode(const nvstitchPayload_t mediaPayloadArray[], int mediaPayloadCount, rapidxml::xml_document<>* doc, rapidxml::xml_node<>* rigNode)
  {
    for (int camNum = 0; camNum < mediaPayloadCount; ++camNum)
    {
      // <camera>
      rapidxml::xml_node<>* cameraNode = appendNewNode(doc, rigNode, "camera");
      appendMediaPayloadToCameraNode(mediaPayloadArray[camNum], doc, cameraNode);
    }
  }


  void appendAudioPayloadToAudioSourceNode(const nvstitchAudioPayload_t& audioPayload, nvstitchAudioDataSource form, nvstitchAudioConfiguration_t* config,
    rapidxml::xml_document<>* doc, rapidxml::xml_node<>* audioSourceNode)
  {
    rapidxml::xml_node<>* inputAudioFeedNode = appendNewNode(doc, audioSourceNode, "input_audio_feed");

    {
      rapidxml::xml_node<>* inputAudioFormNode = appendNewNode(doc, inputAudioFeedNode, "input_audio_form");
      appendNewAttribute(doc, inputAudioFormNode, "form",
        (form == nvstitchAudioDataSource::NVSTITCH_AUDIO_DATA_SOURCE_FILE)   ? "file" :
        (form == nvstitchAudioDataSource::NVSTITCH_AUDIO_DATA_SOURCE_BUFFER) ? "buffer" : "unknown"
      );
    }

    if (form == nvstitchAudioDataSource::NVSTITCH_AUDIO_DATA_SOURCE_FILE)
    {
      rapidxml::xml_node<>* inputAudioFileNode = appendNewNode(doc, inputAudioFeedNode, "input_audio_file");
      appendNewAttribute(doc, inputAudioFileNode, "name", audioPayload.payload.file.name);
    }

    if (config)
    {
      rapidxml::xml_node<>* inputAudioConfigNode = appendNewNode(doc, inputAudioFeedNode, "input_audio_config");
      appendNewAttribute(doc, inputAudioConfigNode, "format",
        (config->format == NVSTITCH_AUDIO_FORMAT_AAC_ADTS)   ? "aac" :
        (config->format == NVSTITCH_AUDIO_FORMAT_PCM16LE)    ? "pcm16le" :
        (config->format == NVSTITCH_AUDIO_FORMAT_PCM32FLOAT) ? "pc32float" : "unknown"
      );
      appendNewAttribute(doc, inputAudioConfigNode, "channels",          std::to_string(config->channels));
      appendNewAttribute(doc, inputAudioConfigNode, "sample_rate",       std::to_string(config->sampleRate));
      appendNewAttribute(doc, inputAudioConfigNode, "bit_rate",          std::to_string(config->bitRate));
      appendNewAttribute(doc, inputAudioConfigNode, "samples_per_frame", std::to_string(config->samplesPerFrame));
    }
  }


  void appendAudioPayloadToRigNode(const nvstitchAudioPayload_t audioPayloadArray[], int audioPayloadCount,
    const nvstitchStitcherProperties_t* stitcherProperties, rapidxml::xml_document<>* doc, rapidxml::xml_node<>* rigNode)
  {
    for (int mikeNum = 0; mikeNum < audioPayloadCount; ++mikeNum)
    { // <audio_source>
      rapidxml::xml_node<>* audioSourceNode = appendNewNode(doc, rigNode, "audio_source");
      appendAudioPayloadToAudioSourceNode(audioPayloadArray[mikeNum], stitcherProperties->audio_input_form[mikeNum],
        stitcherProperties->audio_input_configurations[mikeNum], doc, audioSourceNode);
    }
  }


  bool writeInputFeedXml(
    const std::string& xmlPath,
    const nvstitchPayload_t mediaPayloadArray[], int mediaPayloadCount,
    const nvstitchAudioPayload_t audioPayloadArray[], int audioPayloadCount,
    const nvstitchStitcherProperties_t* stitcherProperties)
  {
    // Open XML file for writing
    std::ofstream fout;
    fout.open(xmlPath, std::ios::trunc);

    if (!fout.is_open())
    {
        std::cerr << "Error: Cannot open output XML file\n";
        return false;
    }

    // Generate XML document
    rapidxml::xml_document<>* doc = new rapidxml::xml_document<>();
    rapidxml::xml_node<>* decl = doc->allocate_node(rapidxml::node_declaration);

    // Write decl tag
    decl->append_attribute(doc->allocate_attribute("version", "1.0"));
    decl->append_attribute(doc->allocate_attribute("encoding", "utf-8"));
    doc->append_node(decl);


    // <rig>
    rapidxml::xml_node<>* rigNode = appendNewNode(doc, doc, "rig");

    if (mediaPayloadArray && mediaPayloadCount)
    {
      appendMediaPayloadToRigNode(mediaPayloadArray, mediaPayloadCount, doc, rigNode);
    }
    
    if (audioPayloadArray && audioPayloadCount)
    {
      appendAudioPayloadToRigNode(audioPayloadArray, audioPayloadCount, stitcherProperties, doc, rigNode);
    }

    // TODO: nvstitchStitcherProperties_t
    fout << *doc;
    fout.close();
    delete doc;
    return true;
  }

  static std::string JoinTransposedStringsWithSeparator(const std::vector<std::vector<std::string> >& strings, unsigned index, char sep)
  {
    std::string joined;
    for (unsigned i = 0; i < strings.size(); ++i)
    {
      if (i)
        joined += sep;
      joined += strings[i][index];
    }
    return joined;
  }

  bool writeAllInOneXml(
    const std::string&                            xmlPath,              // output file name
    const nvstitchVideoRigProperties_t*           videoRigProperties,   // video
    const nvstitchPayload_t                       mediaPayloadArray[],
    const std::vector<std::vector<std::string> >* calibFiles,
    const nvstitchAudioRigProperties_t*           audioRigProperties,   // audio
    const nvstitchAudioPayload_t                  audioPayloadArray[],
    const nvstitchStitcherProperties_t*           stitcherProperties)   //stitcher
  {
    // Open XML file for writing
    std::ofstream fout;
    fout.open(xmlPath, std::ios::trunc);

    if (!fout.is_open())
    {
        std::cerr << "Error: Cannot open output XML file\n";
        return false;
    }

    // Generate XML document
    rapidxml::xml_document<>* doc = new rapidxml::xml_document<>();
    rapidxml::xml_node<>* decl = doc->allocate_node(rapidxml::node_declaration);

    // Write decl tag
    decl->append_attribute(doc->allocate_attribute("version", "1.0"));
    decl->append_attribute(doc->allocate_attribute("encoding", "utf-8"));
    doc->append_node(decl);

    rapidxml::xml_node<>* nvstitchNode = appendNewNode(doc, doc, "nvstitch");                              // <nvstitch>


    rapidxml::xml_node<>* rigNode = appendNewNode(doc, nvstitchNode, "rig");                               // <rig>
    appendNewAttribute(doc, rigNode, "coord_axes", "y-up");
    appendNewAttribute(doc, rigNode, "rig_diameter_cm", (videoRigProperties->rig_diameter) );

    for (unsigned i = 0; i < videoRigProperties->num_cameras; ++i)
    {
      rapidxml::xml_node<>* camNode = appendNewNode(doc, rigNode, "camera");                                // <camera>
      appendCameraPropertiesToCameraNode(&videoRigProperties->cameras[i], doc, camNode);
      if (mediaPayloadArray)
      {
        appendMediaPayloadToCameraNode(mediaPayloadArray[i], doc, camNode);
      }
      if (calibFiles)
      {
        rapidxml::xml_node<>* caibNode = appendNewNode(doc, camNode, "input_calib_file");
        appendNewAttribute(doc, caibNode, "name", JoinTransposedStringsWithSeparator(*calibFiles, i, ','));
      }
    }

    if (audioRigProperties)
    {
      for (unsigned i = 0; i < audioRigProperties->num_sources; ++i)
      {
        rapidxml::xml_node<>* audioSourceNode = appendNewNode(doc, rigNode, "audio_source");                // <audio_source>
        appendAudioPropertiesToAudioSourceNode(&audioRigProperties->sources[i], doc, audioSourceNode);
        if (audioPayloadArray)
        {
          appendAudioPayloadToAudioSourceNode(audioPayloadArray[i], stitcherProperties->audio_input_form[i],
            stitcherProperties->audio_input_configurations[i], doc, audioSourceNode);
        }
      }
    }

    if (stitcherProperties)
    {
      appendStitcherPropertiesToRootNode(stitcherProperties, doc, nvstitchNode);
    }

    fout << *doc;
    fout.close();
    delete doc;
    return true;
  }


}