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

#include <stdint.h>
#include <iostream>

#include "CmdArgsMap.hpp"

#include "xml_util/xml_utility_video.h"
#include "xml_util/xml_utility_audio.h"
#include "xml_util/xml_utility_hl.h"

#include "app.h"
#include <opencv2/opencv.hpp>

uint32_t
main(int argc, char *argv[])
{
	app myApp;
	appParams myAppParams = {};

	bool show_help = false;
	std::string rig_spec_name;
	std::string audio_rig_spec_name;
	std::string calib_rig_spec_name("calibrated_rig_spec.xml");
	std::string stitcher_spec_name;
	std::string video_input_name;
	std::string audio_input_name;
	bool out_calib_name_present = false;

	// Process command line arguments
	CmdArgsMap cmdArgs = CmdArgsMap(argc, argv, "--")
		("help", "Produce help message", &show_help)
		("rig_spec", "XML file containing rig specification", &rig_spec_name, rig_spec_name)
		("audio_rig_spec", "XML file containing specification of audio sources", &audio_rig_spec_name, audio_rig_spec_name)
		("calib_rig_spec", "Output XML file containing updated rig specification from calibration", &calib_rig_spec_name, calib_rig_spec_name, &out_calib_name_present)
		("stitcher_spec", "XML file containing stitcher specification", &stitcher_spec_name, stitcher_spec_name)
		("video_input", "XML file containing video input specification", &video_input_name, video_input_name)
		("audio_input", "XML file containing audio input specification", &audio_input_name, audio_input_name)
		("input_dir_base", "Base directory for input MP4 files", &myAppParams.input_dir_base, myAppParams.input_dir_base)
		("calib", "Flag to indicate that calibration should be performed", &myAppParams.calib_flag)
		("audio", "Flag to indicate that audio stitching should be performed", &myAppParams.audio_flag);

	if (show_help)
	{
		std::cout << "High-Level Video Stitch Sample Application" << std::endl;
		std::cout << cmdArgs.help();
		return 1;
	}

	if (rig_spec_name.empty())
	{
		std::cout << "Rig specification XML name needs to be set using --rig_spec." << std::endl;
		std::cout << cmdArgs.help();
		return 1;
	}

	if (audio_rig_spec_name.empty() && myAppParams.audio_flag)
	{
		std::cout << "If audio is enabled, audio sources specification XML name needs to be set using --audio_rig_spec." << std::endl;
		std::cout << cmdArgs.help();
		return 1;
	}

	if (stitcher_spec_name.empty())
	{
		std::cout << "Stitcher specification XML name needs to be set using --stitcher_spec." << std::endl;
		std::cout << cmdArgs.help();
		return 1;
	}

	if (video_input_name.empty())
	{
		std::cout << "Video input specification XML name needs to be set using --video_input." << std::endl;
		std::cout << cmdArgs.help();
		return 1;
	}

	if (audio_input_name.empty() && myAppParams.audio_flag)
	{
		std::cout << "If audio is enabled, audio input specification XML name needs to be set using --audio_input." << std::endl;
		std::cout << cmdArgs.help();
		return 1;
	}

	if (!out_calib_name_present && myAppParams.calib_flag)
	{
		std::cout << std::endl << "Calibration output XML file not specified, calibrated rig will be saved to " << calib_rig_spec_name << std::endl;
	}

  // Add a directory separator if there is not already one
  if (!myAppParams.input_dir_base.empty())
  {
    switch (myAppParams.input_dir_base[myAppParams.input_dir_base.size() - 1])
    {
    #ifdef _MSC_VER
      case '\\':
    #endif // _MSC_VER
    case '/':
        break;
    default:
      myAppParams.input_dir_base += '/';
      break;
    }
  }

	// Fetch rig parameters from XML file.
	if (!xmlutil::readCameraRigXml(myAppParams.input_dir_base + rig_spec_name, myAppParams.cam_properties, &myAppParams.rig_properties))
	{
		std::cout << std::endl << "Failed to retrieve rig parameters from XML file." << std::endl;
		return 1;
	}

	// Fetch image filenames for calibration
	if (myAppParams.calib_flag)
	{
		if (!xmlutil::readInputCalibFilenamesXml(myAppParams.input_dir_base + rig_spec_name, myAppParams.calib_filenames))
		{
			std::cout << "XML Error reading calibration filenames." << std::endl;
			return 1;
		}

		// Calibrate
		if (myApp.calibrate(&myAppParams) != NVSTITCH_SUCCESS)
		{
			std::cout << "Calibration failure." << std::endl;
			return 1;
		}

		// Save updated rig properties resulting from calibration
		if (!xmlutil::writeCameraRigXml(myAppParams.input_dir_base + calib_rig_spec_name, &myAppParams.calibrated_rig_properties))
		{
			std::cout << "XML Error writing updated rig parameters resulting from calibration." << std::endl;
			return 1;
		}
	}

	// Fetch input video feeds from XML file.
	std::vector<std::string> payloadFilenames;
	payloadFilenames.reserve(myAppParams.cam_properties.size());
	if (!xmlutil::readInputMediaFeedXml(myAppParams.input_dir_base + video_input_name, myAppParams.payloads, payloadFilenames))
	{
		std::cout << std::endl << "Failed to retrieve input video feeds from XML file." << std::endl;
		return 1;
	}

	std::string audio_input;
	if (myAppParams.audio_flag)
	{
		audio_input = myAppParams.input_dir_base + audio_input_name;
	}

	// Fetch stitcher parameters from XML file.
	std::vector<std::string> outputPayloadFilenames;
	outputPayloadFilenames.reserve(myAppParams.cam_properties.size());
	if (!xmlutil::readStitcherPropertiesXml(myAppParams.input_dir_base + stitcher_spec_name,
											&myAppParams.stitcher_properties, outputPayloadFilenames, audio_input))
	{
		std::cout << std::endl << "Failed to retrieve stitcher parameters from XML file." << std::endl;
		return 1;
	}

	if (myAppParams.audio_flag)
	{
		// Fetch audio rig properties from XML file.
		if (!xmlutil::readAudioRigXml(myAppParams.input_dir_base + audio_rig_spec_name, &myAppParams.audio_rig_properties))
		{
			std::cout << std::endl << "Failed to retrieve audio rig parameters from XML file." << std::endl;
			return 1;
		}

		// Fetch input audio feeds from XML file.
		std::vector<std::string> audioPayloadFilenames;
		audioPayloadFilenames.reserve(myAppParams.cam_properties.size());
		if (!xmlutil::readInputAudioFeedXml(myAppParams.input_dir_base + audio_input_name, myAppParams.audio_payloads, audioPayloadFilenames))
		{
			std::cout << std::endl << "Failed to retrieve input audio feeds from XML file." << std::endl;
			return 1;
		}
	}

	cv::VideoCapture cap("udpsrc port=5000 ! application/x-rtp,media=video,payload=26,clock-rate=90000,encoding-name=JPEG,framerate=30/1 ! rtpjpegdepay ! jpegdec ! videoconvert ! appsink", cv::CAP_GSTREAMER);
	
	while (!cap.isOpened())
	{
		std::cout << "VideoCapture or VideoWriter not opened" << std::endl;
		exit(-1);
	}

	cv::Mat frame;

	while (true) {
		cap.read(frame);
		if (frame.empty())
			break;
		//cout << "Width: " << frame.size().width << endl;
		//cout << "Height: " << frame.size().height << endl;

		int width = frame.size().width;
		int height = frame.size().height;
		int halfWidth = width / 2;

		cv::Mat leftCamera = frame(cv::Range::all(), cv::Range(0, halfWidth));
		cv::Mat rightCamera = frame(cv::Range::all(), cv::Range(halfWidth, width));

		//imshow(frame);
		//out.write(frame);
		//cvtColor(frame, frame, COLOR_BGR2RGB);
		cv::namedWindow("left");
		imshow("left", leftCamera);

		cv::namedWindow("right");
		imshow("right", rightCamera);

		//imshow("Sender", frame);
		if (cv::waitKey(1) == 's')
			break;

		myAppParams.num_frames = 2;
		
		// Stitch
		if (myApp.run(&myAppParams) != NVSTITCH_SUCCESS)
		{
			std::cout << "Stitching failed." << std::endl;
			return 1;
		}

	}
	cv::destroyAllWindows();

	return 0;
}