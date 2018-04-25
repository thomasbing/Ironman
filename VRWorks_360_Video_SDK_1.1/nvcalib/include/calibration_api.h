#ifndef __CALIBRATION_API_H_
#define __CALIBRATION_API_H_


#if defined(_WIN32) || defined(_WIN64)
#define NVCALIBAPI __stdcall
#if (_MSC_VER < 1800) //VS2013
#else
#include <stdint.h>
#endif
#else
#define NVCALIBAPI 
#endif


#include "nvstitch_common.h"
#include "nvstitch_common_video.h"

#ifdef __cplusplus
extern "C" {
#endif



  /*! @file calibration_api.h
  *  @brief The header of the NvCalib API.
  *
  */

#define NVCALIB_H_MAJOR_VERSION 1
#define NVCALIB_H_MINOR_VERSION 0


  // Result enums, returned by API functions
  typedef enum
  {
    NVCALIB_SUCCESS = 0,
    NVCALIB_ERROR_UNSUPPORTED,
    NVCALIB_ERROR_ARGUMENT_INVALID,
    NVCALIB_ERROR_ARGUMENT_MISSING,
    NVCALIB_ERROR_ARGUMENT_UNSUPPORTED,
    NVCALIB_ERROR_ARGUMENT_COUNT_INVALID,
    NVCALIB_ERROR_ARGUMENT_DATATYPE_INVALID,
    NVCALIB_ERROR_ARGUMENT_DATA_INVALID,
    NVCALIB_ERROR_HANDLE_INVALID,
    NVCALIB_ERROR_ADD_IMAGES_COUNT_MISMATCH,
    NVCALIB_ERROR_CAMERA_COUNT_INVALID,

    NVCALIB_ERROR_INSUFFICIENT_FEATURES,
    NVCALIB_ERROR_FAILED_CONVERGENCE,

    NVCALIB_ERROR_ENUM_SIZE = 0x7fffffff          // Force uint32_t
  }nvcalibResult;


  // Datatype enums, used to get/set camera properties
  typedef enum {
    NVCALIB_DATATYPE_UINT32 = 0,                  // 32bit unsigned integer
    NVCALIB_DATATYPE_FLOAT32,                     // single precision float IEEE 32bit format
    NVCALIB_DATATYPE_CSTRING,                     // C-string (null terminated).

    NVCALIB_DATATYPE_ENUM_SIZE = 0x7fffffff       // Force uint32_t
  } nvcalibDataType;


  typedef uint32_t nvcalibInstance;

  // -------------------------------------------------------------------
  // -------------------  CREATE NVCALIB INSTANCE ----------------------
  // -------------------------------------------------------------------

  // Create the calibration instance
  // framesCount - Input - Number of frames per camera to be used for calibration
  // camerasCount -Input - Number of cameras in rig to calibrate
  // hInstance - Output - calibration instance handle
  nvcalibResult NVCALIBAPI nvcalibCreateInstance(
    const uint32_t framesCount,
    const uint32_t camerasCount,
    nvcalibInstance* hInstance
  );



  // -------------------------------------------------------------------
  // -----------------  SET CALIBRATION OPTIONS ------------------------
  // -------------------------------------------------------------------
  
  // Optimize Params enums, types for nvcalibOption::NVCALIB_OPTION_OPTIMIZE_PARAMS
  // NVCALIB_OPTION_OPTIMIZE_PARAMS_ALL -         Optimize all parameters (Intrinsics, Extrinsics and Distortion)
  // NVCALIB_OPTION_OPTIMIZE_PARAMS_EXTRINSICS -  Optimize only extrinsics parameters.
  typedef enum
  {
    NVCALIB_OPTION_OPTIMIZE_PARAMS_ALL = 0,
    NVCALIB_OPTION_OPTIMIZE_PARAMS_EXTRINSICS,

    NVCALIB_OPTION_OPTIMIZE_PARAMS_ENUM_SIZE = 0x7fffffff // Force uint32_t
  }nvcalibOptionOptimizeParams;



  // Calibration options
  // NVCALIB_OPTION_USE_TRANS -                   Use rig translation parameters for triangulation based calibration
  //                                              Type NVCALIB_DATATYPE_UINT32, 0=Don't Use (Default), 1=Use.
  // NVCALIB_OPTION_OPTIMIZE_PARAMS -             Select Camera parameters to be optimized.
  //                                              Type nvcalibOptionOptimizeParams,
  //                                              Default = NVCALIB_OPTION_OPTIMIZE_PARAMS_ALL
  // NVCALIB_OPTION_MODE -                        Select the quality/speed modes of calibration
  //                                              Type NVCALIB_DATATYPE_UINT32, 0= High Quality Mode (Default), 1=Fast Mode.
  // NVCALIB_OPTION_CORRESPONDENCE_DIRECTORY -    Write images with feature points and correspondence overlaid to the specified directory
  //                                              Type NVCALIB_DATATYPE_CSTRING, set to "" or NULL to clear.
  typedef enum
  {
    NVCALIB_OPTION_USE_TRANS = 0,
    NVCALIB_OPTION_OPTIMIZE_PARAMS,
    NVCALIB_OPTION_MODE,
    NVCALIB_OPTION_CORRESPONDENCE_DIRECTORY,
    NVCALIB_OPTION_PANORAMA_ORIENTATION,

    NVCALIB_OPTION_ENUM_SIZE = 0x7fffffff // Force uint32_t
  }nvcalibOption;

  // How to orient the resultant stitched panorama
  // NVCALIB_ORIENTATION_AUTO_BALANCE           - Orient the panorama to the dominant axis (default).
  // NVCALIB_ORIENT_PANO_FREEZE_FIRST_CAMERA    - Keep the first image stationary.
  // NVCALIB_ORIENT_PANO_APPLY_TRANSFORM        - Apply the inverse of the supplied transform, assuming camera[0] pose is frozen in calibration.
  // NVCALIB_ORIENT_PANO_APPLY_YPR              - Apply the inverse of the supplied yaw-pitch-roll, assuming camera[0] pose is frozen in calibration.
  typedef enum
  {
    NVCALIB_ORIENT_PANO_AUTO_BALANCE          = 0,
    NVCALIB_ORIENT_PANO_FREEZE_FIRST_CAMERA   = 1,
    NVCALIB_ORIENT_PANO_APPLY_TRANSFORM       = 3,
    NVCALIB_ORIENT_PANO_APPLY_YPR             = 4,

    NVCALIB_ORIENT_ENUM_SIZE                  = 0x7fffffff // Force uint32_t
  } nvOrientationOption;

  // Set the calibration options
  // Call before nvcalibCalibrate()
  // hInstance        - Input - Handle to calibration Instance
  // optionName       - Input - The options to set, Type nvcalibOption
  // nvcalibDataType  - Input - Datatype of option, Type nvcalibDataType
  // dataCount        - Input - The count of the data
  // optionData       - Input - void* to the option data
  nvcalibResult NVCALIBAPI nvcalibSetOption(
    nvcalibInstance hInstance,
    nvcalibOption optionName,
    nvcalibDataType optionDataType,
    uint32_t dataCount,
    void* optionData);

  // ------------------------------------------------------------------
  // ------------  SET AND GET CAMERA PROPERTIES ----------------------
  // ------------------------------------------------------------------


  // Extrinsics Coordinate system axes
  // Y-up     : (Right hand system, X = Right, Y = Up, Z = Opp. Look-at (Out) )
  // Y Down   : (Right hand system, X = Right, Y = Down, Z = Look-at)
  // Z-up     : (Right hand system, X = Right, Y = Look-at, Z = Up)
  typedef enum
  {
    NVCALIB_AXES_Y_UP = 0,
    NVCALIB_AXES_Y_DOWN,
    NVCALIB_AXES_Z_UP,

    NVCALIB_AXES_ENUM_SIZE = 0x7fffffff // Force uint32_t
  }nvcalibExtrinsicsAxes;

  // Camera Lens model type
  // NVCALIB_LENS_MODEL_BROWN -                Brown model, suitable for normal angle lenses
  // NVCALIB_LENS_MODEL_FISHEYE -              Fisheye f-theta model with coefficients, suitable for wide angle camera lens
  // NVCALIB_LENS_MODEL_FISHEYE_EQUIDISTANT -  Fisheye with f.theta fixed projection, (No optimization on distortion)
  // NVCALIB_LENS_MODEL_FISHEYE_STEREOGRAPHIC- Fisheye with 2f.tan(theta/2) fixed projection (No optimization on distortion)
  // NVCALIB_LENS_MODEL_FISHEYE_EQUISOLID -    Fisheye with 2f.sin(theta/2) fixed projection (No optimization on distortion)
  // NVCALIB_LENS_MODEL_FISHEYE_ORTHOGRAPHIC - Fisheye with f.sin(theta) fixed projection (No optimization on distortion)
  typedef enum
  {
    NVCALIB_LENS_MODEL_BROWN = 0,              // 5 coeffs
    NVCALIB_LENS_MODEL_FISHEYE,                // 4 coeffs
    NVCALIB_LENS_MODEL_FISHEYE_EQUIDISTANT,    // 0 coeffs
    NVCALIB_LENS_MODEL_FISHEYE_STEREOGRAPHIC,  // 0 coeffs 
    NVCALIB_LENS_MODEL_FISHEYE_EQUISOLID,      // 0 coeffs
    NVCALIB_LENS_MODEL_FISHEYE_ORTHOGRAPHIC,   // 0 coeffs

    NVCALIB_LENS_MODEL_ENUM_SIZE = 0x7fffffff // Force uint32_t

  }nvcalibCameraLensModelType;

  // Properties of camera
  /*
  // NVCALIB_CAM_PROP_WIDTH -                       Camera Width, (Required) 
  // NVCALIB_CAM_PROP_HEIGHT-                       Camera Height, (Required)
  // NVCALIB_CAM_PROP_INPUT_IMAGE_FORMAT -          Format of Input image data (Required)
  // NVCALIB_CAM_PROP_INPUT_IMAGE_PITCH -           Pitch of Input image data (Optional), Default = NVCALIB_CAM_PROP_WIDTH * 4
  //
  // NVCALIB_CAM_PROP_INTRINSICS_FOCAL_LENGTH -     Focal length of camera in pixels (Required)
  // NVCALIB_CAM_PROP_INTRINSICS_PRINCIPAL_POINT -  Principal point (X,Y) in pixels (Optional), Default = (Width-1)/2, (Height-1)/2
  //
  // Extrinsics - For Rotation (Required) specify YPR or Axes+Rotation-Matrix
  //              For Translation (Optional) specify Axes+Translation
  //
  // NVCALIB_CAM_PROP_EXTR_ROT_YPR -                Extrinsics rotation specified in Yaw, Pitch Roll in radians 
  //                                                (Order in which they are applied: Roll->Pitch->Yaw)
  // NVCALIB_CAM_PROP_EXTR_AXES -                   Coordinate system axes in which rotation matrix and translation is specified
  // NVCALIB_CAM_PROP_EXTR_ROT_MATRIX -             Extrinsics rotation matrix in row major format, specify NVCALIB_CAM_PROP_EXTR_AXES
  // NVCALIB_CAM_PROP_EXTR_TRANSLATION -            Translation X,Y,Z from rig center in axes, (Optional) Default = 0,0,0
  //
  // NVCALIB_CAM_PROP_DISTORTION_TYPE -             Lens type of camera (Required)
  // NVCALIB_CAM_PROP_DISTORTION_COEFFS -           Coefficients of camera lens distortion model (Optional), Default = (0,0,0,0,0)
  //                                                Pass only the number of coeff. required by CameraLensModelType
  // NVCALIB_CAM_PROP_FISHEYE_RADIUS -              Radius of the valid region of pixels for fisheye lens cameras (Optional)
  // NVCALIB_CAM_PROP_LAYOUT -                      Layout of the camera in the rig (Optional), Default = Equatorial
  */
  typedef enum
  {
    NVCALIB_CAM_PROP_WIDTH = 0,                  // NVCALIB_DATATYPE_UINT32
    NVCALIB_CAM_PROP_HEIGHT,                     // NVCALIB_DATATYPE_UINT32
    NVCALIB_CAM_PROP_INPUT_IMAGE_FORMAT,         // nvcalibInputImageFormat, NVCALIB_DATATYPE_UINT32
    NVCALIB_CAM_PROP_INPUT_IMAGE_PITCH,          // NVCALIB_DATATYPE_UINT32

    // Intrinsics
    NVCALIB_CAM_PROP_INTR_PRINCIPAL_POINT,      // NVCALIB_DATATYPE_FLOAT32 array[2]
    NVCALIB_CAM_PROP_INTR_FOCAL_LENGTH,         // NVCALIB_DATATYPE_FLOAT32

    // Extrinsics
    NVCALIB_CAM_PROP_EXTR_ROT_YPR,              // NVCALIB_DATATYPE_FLOAT32 array[3]

    NVCALIB_CAM_PROP_EXTR_AXES,                 // CalibExtrinsicsMatrixAxes, NVCALIB_DATATYPE_UINT32
    NVCALIB_CAM_PROP_EXTR_ROT_MATRIX,           // NVCALIB_DATATYPE_FLOAT32 array[9]


    NVCALIB_CAM_PROP_EXTR_TRANSLATION,          // NVCALIB_DATATYPE_FLOAT32 array[3]

    // Distortion
    NVCALIB_CAM_PROP_DISTORTION_TYPE,           // CameraLensModelType, NVCALIB_DATATYPE_UINT32
    NVCALIB_CAM_PROP_DISTORTION_COEFFS,         // NVCALIB_DATATYPE_FLOAT32 array[1-5]

    NVCALIB_CAM_PROP_FISHEYE_RADIUS,            // NVCALIB_DATATYPE_FLOAT32
    NVCALIB_CAM_PROP_LAYOUT,                    // nvstitchCameraLayout, NVCALIB_DATATYPE_UINT32

    NVCALIB_CAM_PROP_ENUM_SIZE = 0x7fffffff     // Force uint32_t
  }nvcalibCameraProperties;




  // Sets the specified property of a particular camera
  // Call before nvcalibCalibrate()
  // hInstance            Input - Handle to calibration Instance
  // cameraIndex          Input - Index of the camera to set properties
  // cameraProperty       Input - Property of camera to set, Type nvcalibCameraProperties
  // propertyDataType     Input - Datatype of property, to set Type nvcalibDataType
  //                              Calling with other datatypes will result in typecasting/error
  // dataCount            Input - Number of data elements. Type nvcalibDataType
  // propertyData         Input - Pointer to source data buffer
  nvcalibResult NVCALIBAPI nvcalibSetCameraProperty(nvcalibInstance hInstance, uint32_t cameraIndex,
    nvcalibCameraProperties cameraProperty, nvcalibDataType propertyDataType, uint32_t dataCount, const void* propertyData);

  // Gets the specified property of a particular camera
  // Call after nvcalibCalibrate() to get calibrated properties
  // hInstance            Input - Handle to calibration Instance
  // cameraIndex          Input - Index of the camera to set properties
  // cameraProperty       Input - Property of camera to set, Type nvcalibCameraProperties
  // propertyDataType     Input - Type of data, set with the appropriate datatype specified in the enums, 
  //                              calling with other datatypes will result in typecasting/error
  // dataCount            Input - Number of data elements. Type nvcalibDataType
  // propertyData         Output - Pointer to destination data buffer
  nvcalibResult NVCALIBAPI nvcalibGetCameraProperty(nvcalibInstance hInstance, uint32_t cameraIndex,
    nvcalibCameraProperties cameraProperty, nvcalibDataType propertyDataType, uint32_t dataCount, void* propertyData);


  // Sets all the camera properties from the nvstitchVideoRigProperties_t struct in nvstitch_common_video.h 
  // Call after nvcalibCalibrate() to get calibrated properties
  // hInstance            Input - Handle to calibration Instance
  // cameraRigProperties  Input - nvstitchVideoRigProperties_t struct
  nvcalibResult NVCALIBAPI nvcalibSetRigProperties(nvcalibInstance hInstance,
    const nvstitchVideoRigProperties_t* cameraRigProperties);


  // Gets all the camera properties in the nvstitchVideoRigProperties_t 
  // struct nvstitch_common_video.h 
  // Call after nvcalibCalibrate() to get calibrated properties
  // hInstance            Input - Handle to calibration Instance
  // cameraRigProperties  Output - nvstitchVideoRigProperties_t struct
  nvcalibResult NVCALIBAPI nvcalibGetRigProperties(nvcalibInstance hInstance,
    nvstitchVideoRigProperties_t* cameraProperties);

  // -------------------------------------------------------------------
  // --------------------  SET CALIBRATION IMAGE -----------------------
  // -------------------------------------------------------------------

  // Enum for input image types supported
  // Only RGBA8, RGA8, BGR8, BGRA8 currently supported 
  typedef enum
  {
    NVCALIB_IN_FORMAT_RGBA8 = 0,
    NVCALIB_IN_FORMAT_RGB8,
    NVCALIB_IN_FORMAT_BGRA8,
    NVCALIB_IN_FORMAT_BGR8,
    NVCALIB_IN_FORMAT_GRAY8,
    NVCALIB_IN_FORMAT_PNG,
    NVCALIB_IN_FORMAT_JPG,

    NVCALIB_IN_FORMAT_ENUM_SIZE = 0x7fffffff // Force uint32_t

  }nvcalibInputImageFormat;

  // Adds images to a calibration instance
  // Call before nvcalibCalibrate()
  // hInstance - Input - Handle to calibration Instance
  // imageData - Input - Array of pointers to image data, size of array should be number of cameras passed
  //                     Each pointer points to buffer of size NVCALIB_CAM_PROP_WIDTH x NVCALIB_CAM_PROP_HEIGHT
  //                     X bytes corresponding to nvcalibInputImageFormat
  nvcalibResult NVCALIBAPI nvcalibSetImages(
    nvcalibInstance hInstance,       //Input
    const void* imageData[]                     //Input
  );


  // ---------------------------------------------------------------------
  // -------------------------------  CALIBRATE --------------------------
  // ---------------------------------------------------------------------


  // Run the calibration
  // Call nvcalibSetOption(), nvcalibSetImages() and nvcalibSetCameraProperty() before
  // hInstance - Input - Handle to calibration Instance
  nvcalibResult NVCALIBAPI nvcalibCalibrate(nvcalibInstance hInstance);


  // ---------------------------------------------------------------------
  // ---------------------------- GET PROPERTIES -------------------------
  // ---------------------------------------------------------------------

  
  // Enums for nvcalibGetResultParameter
  // NVCALIB_RESULTS_ACCURACY - Accuracy indication of results from [0-1] 
  //                            Type - NVCALIB_DATATYPE_FLOAT32        
  typedef enum
  {
    NVCALIB_RESULTS_ACCURACY,               
    NVCALIB_RESULTS_ENUM_SIZE = 0x7fffffff  // Force uint32_t

  }nvcalibResultParameter;


  // Gets calibration results after successful calibration
  // Call after nvcalibCalibrate()
  // hInstance              - Input - Handle to calibration Instance
  // resultParameter        - Input - Result data to get, Type nvcalibResultParameter
  // resParamDataType       - Input - Datatype of result data, Type nvcalibDataType
  // dataCount              - Input - Data Count of result data, Type uint32_t
  // resultParameterData    - Output - Result data
  nvcalibResult NVCALIBAPI nvcalibGetResultParameter(
    nvcalibInstance hInstance,
    nvcalibResultParameter resultParameter,
    nvcalibDataType resParamDataType,
    uint32_t dataCount,
    void* resultParameterData);


  // After a result other than NVCALIB_SUCCESS is received, call this to get additional information about the error.
  // hInstance        Input - Handle to calibration Instance
  // errorStringPtr   Input - Pointer to a null terminated char[], describing the last error
  nvcalibResult NVCALIBAPI nvcalibGetLastErrorString(nvcalibInstance hInstance, const char** errorStringPtr);


  // ---------------------------------------------------------------------
  // -------------------  DESTROY NVCALIB INSTANCE -----------------------
  // ---------------------------------------------------------------------

  // Destroy the calibration instance
  // hInstance  - Input - Handle to calibration Instance
  nvcalibResult NVCALIBAPI nvcalibDestroyInstance(nvcalibInstance hInstance);


#ifdef __cplusplus
}
#endif

#endif
