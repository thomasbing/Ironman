#include <iostream>
#include <vector>
#include <string>
#include <array>

#include "calibration_api.h"
#include "nvstitch_common.h"
#include "nvstitch_common_video.h"

#include "CmdArgsMap.hpp"
#include "image_io_util.hpp"
#include "math_util/math_utility.h"
#include "xml_util/xml_utility_video.h"
#include "filesys_util.h"

struct CmdLineParams
{
  std::string workingDirectoryPath;
  std::string inputXmlPath;
  std::string outputXmlPath;
  uint32_t useTranslation;

  // If input xml not provided
  array<float, 3> yprDeg;
  float focalLength;
  std::string lens;
  std::string imgExtension;
  float radius;
  float hfov;
  float vfov;
};

bool getCmdParams(int argc, char** argv, CmdLineParams& cmdLineParams)
{
  bool show_help = false, useTrans = false;
  std::string workingDir, inputXmlFilename, outputXmlFilename;


  CmdArgsMap cmdArgs = CmdArgsMap(argc, argv, "--")
    ("help", "Produce help message", &show_help)
    ("wd", "Working directory", &workingDir, std::string(""))
    ("in_xml", "Input XML filename", &inputXmlFilename, std::string(""))
    ("out_xml", "Output XML filename", &outputXmlFilename, std::string(""))
    ("trans", "Use translation for triangulation based calibration. 0=OFF(Default), 1=ON", &useTrans)
    ("ext", "Image extension (png or jpg or bmp), Default=(png->jpg->bmp)", &cmdLineParams.imgExtension, std::string(""))
    ("yaw", "Yaw of first camera in degrees, Default=0", &cmdLineParams.yprDeg[0], 0.0f)
    ("pitch", "Pitch of first camera in degrees, Default=0", &cmdLineParams.yprDeg[1], 0.0f)
    ("roll", "Roll of first camera in degrees, Default=0", &cmdLineParams.yprDeg[2], 0.0f)
    ("lens", "Lens type of cameras: 'fisheye'(Default) or 'brown'(For normal angle lens)", &cmdLineParams.lens, std::string("fisheye"))
    ("fl", "Focal length in pixels, Default=0(Auto)", &cmdLineParams.focalLength, 0.0f)
    ("radius", "Fisheye radius in pixels, Default=0(Auto)", &cmdLineParams.radius, 0.0f)
    ("hfov", "Horizontal field of view in degrees, used for obtaining focal length (Ignored if --fl provided)", &cmdLineParams.hfov, 0.0f)
    ("vfov", "Vertical field of view in degrees, used for obtaining focal length (Ignored --fl provided)", &cmdLineParams.vfov, 0.0f)
    ;

  if (show_help)
  {
    std::cout << "Usage:" << std::endl;
    std::cout << cmdArgs.help();
    exit(0);
  }
  if (outputXmlFilename.empty())
  {
    if (!inputXmlFilename.empty())
      outputXmlFilename = "calib_" + inputXmlFilename;
    else
      outputXmlFilename = "out_calib.xml";
  }

  if (!workingDir.empty())
  {
    cmdLineParams.workingDirectoryPath = workingDir;
  }
  else
  {
    filesys_util::getCurrentWorkingDirectoryPath(cmdLineParams.workingDirectoryPath);
  }

  if (!inputXmlFilename.empty())
  {
    cmdLineParams.inputXmlPath = cmdLineParams.workingDirectoryPath + "/" + inputXmlFilename;
  }

  cmdLineParams.outputXmlPath = cmdLineParams.workingDirectoryPath + "/" + outputXmlFilename;

  if (useTrans)
  {
    cmdLineParams.useTranslation = 1;
  }
  else
  {
    cmdLineParams.useTranslation = 0;
  }

  return true;
}

inline bool returnedError(nvcalibResult calibResult)
{
  if (calibResult != nvcalibResult::NVCALIB_SUCCESS)
  {
    std::cerr << "Error : CALIB_RESULT " << calibResult;
    return true;
  }
  
  return false;
}

static std::string getErrorString(nvcalibResult result, nvcalibInstance hInstance)
{
  std::stringstream strStream;
  if (nvcalibResult::NVCALIB_SUCCESS != result)
  {
    strStream << " (" << result << ")";
    const char* lastErrorString = nullptr;
    if (nvcalibGetLastErrorString(hInstance, &lastErrorString) == nvcalibResult::NVCALIB_SUCCESS)
    {
      strStream << lastErrorString;
    }
  }
  return strStream.str();
}


bool setCalibrationOptions(nvcalibInstance hCalibration, const CmdLineParams& cmdLineParams)
{
  if (cmdLineParams.useTranslation)
  {
    if (nvcalibSetOption(hCalibration, NVCALIB_OPTION_USE_TRANS, nvcalibDataType::NVCALIB_DATATYPE_UINT32, 1,
      (void*)(&cmdLineParams.useTranslation)) != nvcalibResult::NVCALIB_SUCCESS)
      return false;
  }
  return true;
}


bool setCalibrationImages(nvcalibInstance hCalibration, const CmdLineParams& cmdLineParams, 
  const nvstitchVideoRigProperties_t& videoRig, const std::vector<std::vector<std::string>>& filenames)
{
  nvcalibResult res = nvcalibResult::NVCALIB_SUCCESS;

  for (int frameIndex = 0; frameIndex < filenames.size(); frameIndex++)
  {
    std::vector<unsigned char*> imagePtrs(videoRig.num_cameras);

    for (int camIndex = 0; camIndex < (int)videoRig.num_cameras; camIndex++)
    {
      int width, height;
      width = videoRig.cameras[camIndex].image_size.x;
      height = videoRig.cameras[camIndex].image_size.y;


      std::string imgPath = cmdLineParams.workingDirectoryPath + "/" + filenames[frameIndex][camIndex];

      int outImgWidth, outImgHeight;
      unsigned char* currImgPtr;
      if (getRgbImage(imgPath, &currImgPtr, outImgWidth, outImgHeight) == false)
      {
        std::cout << "Error at Cam:" << camIndex << "Frame:" << frameIndex << " reading Image " << filenames[frameIndex][camIndex];
        return false;
      }

      if (outImgWidth != width || outImgHeight != height)
      {
        std::cout << "Input image resolution [" << outImgWidth << "," << outImgHeight << "] mismatches with camera res ["
          << width << "," << height << "].";
        return false;
      }

      nvcalibInputImageFormat inputFormat = nvcalibInputImageFormat::NVCALIB_IN_FORMAT_BGR8;
      if ((res = nvcalibSetCameraProperty(hCalibration, camIndex, nvcalibCameraProperties::NVCALIB_CAM_PROP_INPUT_IMAGE_FORMAT,
        nvcalibDataType::NVCALIB_DATATYPE_UINT32, 1, (void*)(&inputFormat))) != nvcalibResult::NVCALIB_SUCCESS)
      {
        std::cout << "Error adding input format, frame:" << frameIndex << " camera:"
          << camIndex <<" call to nvcalibSetCameraProperty() Failed." << " - " << getErrorString(res, hCalibration);

        return false;
      }
      
      int32_t pitch = outImgWidth * 3;
      if ((res = nvcalibSetCameraProperty(hCalibration, camIndex, nvcalibCameraProperties::NVCALIB_CAM_PROP_INPUT_IMAGE_PITCH,
        nvcalibDataType::NVCALIB_DATATYPE_UINT32, 1, (void*)(&pitch))) != nvcalibResult::NVCALIB_SUCCESS)
      {
        std::cout << "Error adding input pitch, frame:" << frameIndex << " camera:"
          << camIndex << " call to nvcalibSetCameraProperty() Failed." << " - " << getErrorString(res, hCalibration);
        return false;
      }
      imagePtrs[camIndex] = currImgPtr;
    }

    // Add all images 
    if ((res = nvcalibSetImages(hCalibration, (const void**)imagePtrs.data())) != nvcalibResult::NVCALIB_SUCCESS)
    {
      std::cout << "Error adding frame:" << frameIndex << " call to nvcalibSetImages() Failed."
        << " - " << getErrorString(res, hCalibration);
      return false;
    }

    // Free the rgb pointers
    for (int camIndex = 0; camIndex < (int)videoRig.num_cameras; camIndex++)
      free(imagePtrs[camIndex]);
  }
  return true;
}

bool setCameraPropertiesDefaults(nvstitchCameraProperties_t& cameraProperties, int width, int height, const CmdLineParams& cmdLineParams)
{
  // Version
  cameraProperties.version = NVSTITCH_VERSION;

  // Width, Height from image
  cameraProperties.image_size.x = width;
  cameraProperties.image_size.y = height;

  // Camera Layout equatorial
  cameraProperties.camera_layout = nvstitchCameraLayout::NVSTITCH_CAMERA_LAYOUT_EQUATORIAL;

  // Distortion type
  if (cmdLineParams.lens == "fisheye")
    cameraProperties.distortion_type = nvstitchDistortionType::NVSTITCH_DISTORTION_TYPE_FISHEYE;
  else if (cmdLineParams.lens == "brown")
    cameraProperties.distortion_type = nvstitchDistortionType::NVSTITCH_DISTORTION_TYPE_BROWN;
  else
  {
    std::cout << "Lens type not supported";
    return false;
  }

  // Focal length
  float focalLength = 0.0f;
  if (cmdLineParams.focalLength != 0)
  {
    focalLength = cmdLineParams.focalLength;
  }
  else if (cmdLineParams.hfov != 0 || cmdLineParams.vfov != 0)
  {
    math_util::getFocalLengthEstimateFromFov(width, height, cmdLineParams.hfov, cmdLineParams.vfov,
      cameraProperties.distortion_type, cmdLineParams.radius, focalLength);
  }

  cameraProperties.focal_length = focalLength;

  // Principal point center offset 0, 0
  math_util::convertPrincipalPointCenterToTopLeftOffset(
    cameraProperties.image_size.x, cameraProperties.image_size.y,
    0.0f, 0.0f,
    &cameraProperties.principal_point.x,
    &cameraProperties.principal_point.y);

  // Distortion Coeff.
  for (int i = 0; i < 5; i++)
    cameraProperties.distortion_coefficients[i] = 0.0f;

  // Fisheye radius
  cameraProperties.fisheye_radius = cmdLineParams.radius;

  // Rotation
  math_util::cameraToWorldTransform(
    math_util::degToRad(cmdLineParams.yprDeg[0]),
    math_util::degToRad(cmdLineParams.yprDeg[1]),
    math_util::degToRad(cmdLineParams.yprDeg[2]),
    cameraProperties.extrinsics.rotation);

  // Translation
  for (int i = 0; i < 3; i++)
    cameraProperties.extrinsics.translation[i] = 0;

  // Set flags
  cameraProperties.set_flags =
    nvstitchCameraSetPropertyFlag::CAMERA_PRINCIPAL_POINT_SET |
    nvstitchCameraSetPropertyFlag::CAMERA_DISTORTION_TYPE_SET |
    nvstitchCameraSetPropertyFlag::CAMERA_DISTORTION_COEFFICIENTS_SET |
    nvstitchCameraSetPropertyFlag::CAMERA_FISHEYE_RADIUS_SET |
    nvstitchCameraSetPropertyFlag::CAMERA_ROTATION_SET |
    nvstitchCameraSetPropertyFlag::CAMERA_TRANSLATION_SET;

  return true;
}
void setVideoRigPropertiesDefaults(nvstitchVideoRigProperties_t& videoRig, int numCameras, nvstitchCameraProperties_t* cameraPropertiesPtr)
{
  videoRig.version = NVSTITCH_VERSION;
  videoRig.rig_diameter = 10.0f;
  math_util::cameraToWorldTransform(0, 0, 0, videoRig.rotation);
  videoRig.num_cameras = numCameras;
  videoRig.cameras = cameraPropertiesPtr;
}


// Get the input filenames to calibrate on from input XML, or file from working directory 
bool getCalibInputFilenames(const CmdLineParams& cmdLineParams, std::vector<std::vector<std::string>>& calibInputFilenames)
{
  // Get videoRig and filenames from XML if path is specified 
  if (!cmdLineParams.inputXmlPath.empty())
  {
    if (!xmlutil::readInputCalibFilenamesXml(cmdLineParams.inputXmlPath.c_str(), calibInputFilenames))
    {
      std::cout << std::endl << "Failed to get input filenames.";
      return false;
    }
  }
  else
  {
    string currWorkingDir = cmdLineParams.workingDirectoryPath;

    // Get filenames
    std::vector<std::string> fileList;

    if (!cmdLineParams.imgExtension.empty())
    {
      filesys_util::getFilelistFromDir(currWorkingDir, "." + cmdLineParams.imgExtension, fileList);
    }
    else
    {
      // Search images
      std::vector<std::string> extSearch{ ".png", ".jpg", ".bmp" };

      for (const auto& ext : extSearch)
      {
        fileList.clear();
        filesys_util::getFilelistFromDir(currWorkingDir, ext, fileList);
        if (fileList.size() != 0)
          break;
      }
    }
    calibInputFilenames.push_back(fileList);

    std::cout << "\nUsing input calibration files :";
    for (const auto& file : fileList)
      std::cout << "\n" << file;
  }
  return true;
}

// Get input estimates camera rig from XML or Cmd args specified
bool getCameraRigEstimates(const CmdLineParams& cmdLineParams, const std::vector<std::vector<std::string>>& calibInputFilenames,
  std::vector<nvstitchCameraProperties_t>& cameraProperties,
  nvstitchVideoRigProperties_t& videoRig)
{
  if (cmdLineParams.inputXmlPath.size() != 0)
  {
    if (!xmlutil::readCameraRigXml(cmdLineParams.inputXmlPath.c_str(), cameraProperties, &videoRig))
    {
      std::cout << std::endl << "Failed to set camera params from XML.";
      return false;
    }
  }
  else
  {
    std::cout << "\nNo input estimates rigspec XML provided, using auto estimates.";

    int numCameras = calibInputFilenames[0].size();
    cameraProperties.resize(numCameras);

    // Read images with OpenCV to get height, width
    int camIndex = 0;
    for (const auto& imgFile : calibInputFilenames[0])
    {
      int imgWidth, imgHeight;
      if (!getImageWidthHeightFromFilePath(cmdLineParams.workingDirectoryPath + "/" + imgFile, imgWidth, imgHeight))
        return false;

      nvstitchCameraProperties_t &currCameraProperty = cameraProperties.at(camIndex);
      currCameraProperty = nvstitchCameraProperties_t{};

      setCameraPropertiesDefaults(currCameraProperty, imgWidth, imgHeight,
        cmdLineParams);

      camIndex++;
    }

    setVideoRigPropertiesDefaults(videoRig, numCameras, cameraProperties.data());
  }
  return true;
}

int main(int argc, char** argv)
{
  CmdLineParams cmdLineParams;
  getCmdParams(argc, argv, cmdLineParams);
  nvcalibResult result;

  std::cout << "NVCALIB SAMPLE APP" << std::endl;

  nvstitchVideoRigProperties_t videoRig;
  std::vector<nvstitchCameraProperties_t> cameraProperties;
  std::vector<std::vector<std::string>> filenames;

  if (!getCalibInputFilenames(cmdLineParams, filenames))
  {
    std::cout << std::endl << "Failed to getCalibInputFilenames(). ";
    return 1;
  }

  if (!getCameraRigEstimates(cmdLineParams, filenames, cameraProperties, videoRig))
  {
    std::cout << std::endl << "Failed to getCameraRigEstimates(). ";
    return 1;
  }

  // Create instance
  nvcalibInstance hInstance = 0;
  int camCount = videoRig.num_cameras;
  int framesCount = (int)filenames.size();
  if ((result = nvcalibCreateInstance(framesCount, camCount, &hInstance)) != nvcalibResult::NVCALIB_SUCCESS)
  {
    std::cout << std::endl << "Failed to create calibration instance. " << getErrorString(result, hInstance);
    return 1;
  }

  // Set rig properties
  if ((result = nvcalibSetRigProperties(hInstance, &videoRig)) != nvcalibResult::NVCALIB_SUCCESS)
  {
    std::cout << std::endl << "Failed to set calibration rig properties. " << getErrorString(result, hInstance);
    return 1;
  }

  // Set calibration options
  if (!setCalibrationOptions(hInstance, cmdLineParams))
  {
    std::cout << std::endl << "Failed to set calibration options. ";
    return 1;
  }

  // Set images
  if (!setCalibrationImages(hInstance, cmdLineParams, videoRig, filenames))
  {
    std::cout << std::endl << "Failed to set images. ";
    return 1;
  }

  // Calibrate 
  if ((result = nvcalibCalibrate(hInstance)) != nvcalibResult::NVCALIB_SUCCESS)
  {
    std::cout << std::endl << "Failed to calibrate. " << getErrorString(result, hInstance);
    return 1;
  }

  // Get rig properties
  if ((result = nvcalibGetRigProperties(hInstance, &videoRig)) != nvcalibResult::NVCALIB_SUCCESS)
  {
    std::cout << std::endl << "Failed to get calibration rig properties. " << getErrorString(result, hInstance);
    return 1;
  }

  // Evaluate quality
  float quality = 0.0f;
  if ((result = nvcalibGetResultParameter(hInstance, NVCALIB_RESULTS_ACCURACY, NVCALIB_DATATYPE_FLOAT32, 1, &quality))
    != nvcalibResult::NVCALIB_SUCCESS)
  {
    cout << std::endl << "Failed to evaluate quality. " << getErrorString(result, hInstance);
  }
  else
  {
    cout << std::endl << "Resultant quality:" << std::to_string(quality);
  }

  // Write videoRig to XML 
  if (!xmlutil::writeCameraRigXml(cmdLineParams.outputXmlPath.c_str(), &videoRig))
  {
    std::cout << std::endl << "Failed to write camera xml.";
    return 1;
  }

  // Destroy instance
  if ((result = nvcalibDestroyInstance(hInstance)) != nvcalibResult::NVCALIB_SUCCESS)
  {
    std::cout << std::endl << "Failed to destroy calibration instance. " << getErrorString(result, hInstance);
    return 1;
  }

  // Free video rig cameras
  free(videoRig.cameras);

  return 0;
}