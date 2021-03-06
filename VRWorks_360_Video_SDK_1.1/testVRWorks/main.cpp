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
#include <string>
#include <tchar.h>

#include "CmdArgsMap.hpp"

#include "app.h"

#include "xml_util/xml_utility_video.h"
#include <opencv2/opencv.hpp>
#include <thread>
#include <chrono>

// socket include
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 1024
#define SERVER_IP "192.168.0.144"
#define DEFAULT_PORT "5001"

uint32_t
main(int argc, char *argv[])
{
	app myApp;
	appParams myAppParams;

	bool show_help = false;
	std::string rig_spec_name;
	std::string image_input_name;
	//myAppParams.pano_width = 3840;
	myAppParams.pano_width = 2560;
	myAppParams.quality = NVSTITCH_STITCHER_QUALITY_HIGH;
	myAppParams.out_file = "stacked_360.bmp";
	myAppParams.stereo_flag = false;

	int pano_width_arg = myAppParams.pano_width;
	int quality_arg = myAppParams.quality;
	// Process command line arguments
	CmdArgsMap cmdArgs = CmdArgsMap(argc, argv, "--")
		("help", "Produce help message", &show_help)
		("input_dir_base", "Base directory for input MP4 files", &myAppParams.input_base_dir, myAppParams.input_base_dir)
		("rig_spec", "XML file containing rig specification", &rig_spec_name, rig_spec_name)
		("image_input", "XML file containing footage files", &image_input_name, image_input_name)
		("pano_width", "Width of the output panorama", &pano_width_arg, pano_width_arg)
		("quality", "Stitch quality (0=high, 1=medium, 2=low)", &quality_arg, quality_arg)
		("out_file", "Stacked output panorama", &myAppParams.out_file, myAppParams.out_file)
		("stereo", "Stereo flag", &myAppParams.stereo_flag);

	if (show_help || rig_spec_name.empty())
	{
		std::cout << "Low-Level Video Stitch Sample Application" << std::endl;
		std::cout << cmdArgs.help();
		return 1;
	}

	if (image_input_name.empty())
	{
		image_input_name = rig_spec_name;
	}

	if (pano_width_arg > 0)
	{
		myAppParams.pano_width = pano_width_arg;
	}
	else
	{
		std::cout << "Invalid panorama width - must be greater than zero.\n";
		exit(0);
	}

	if (quality_arg < 0 || quality_arg > 2)
	{
		std::cout << "Invalid quality_preset: 0=high(default), 1=medium, 2=low\n";
		std::cout << cmdArgs.help();
		exit(0);
	}
	myAppParams.quality = (nvstitchStitcherQuality)quality_arg;

	if (!myAppParams.input_base_dir.empty())
	{
		switch (myAppParams.input_base_dir[myAppParams.input_base_dir.size() - 1])
		{
#ifdef _MSC_VER
		case '\\':
#endif // _MSC_VER
		case '/':
			break;
		default:
			myAppParams.input_base_dir += '/';
			break;
		}
	}

	// Fetch rig parameters from XML file.
	if (!xmlutil::readCameraRigXml(myAppParams.input_base_dir + rig_spec_name, myAppParams.cam_properties, &myAppParams.rig_properties))
	{
		std::cout << std::endl << "Failed to retrieve rig paramters from XML file." << std::endl;
		return 1;
	}

	// Fetch input media feeds from XML file.
	if (!xmlutil::readInputMediaFeedFilenamesXml(myAppParams.input_base_dir + image_input_name, myAppParams.filenames))
	{
		std::cout << std::endl << "Failed to retrieve input media feeds from XML file." << std::endl;
		return 1;
	}

	/*
	if (myApp.run(&myAppParams) != NVSTITCH_SUCCESS)
	{
	std::cout << "Stitcher failure." << std::endl;
	return 1;
	}
	*/
	//cv::VideoCapture cap("udpsrc port=5000 ! application/x-rtp,media=video,payload=26,clock-rate=90000,encoding-name=JPEG,framerate=30/1 ! rtpjpegdepay ! jpegdec ! videoconvert ! appsink", cv::CAP_GSTREAMER);
	cv::VideoCapture cap("udpsrc port=5000 ! application/x-rtp,media=video,payload=96,clock-rate=90000,encoding-name=H264,framerate=30/1 ! rtph264depay ! decodebin ! videoconvert ! appsink", cv::CAP_GSTREAMER);

	while (!cap.isOpened())
	{
		std::cout << "VideoCapture or VideoWriter not opened" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	cv::Mat frame;
	HANDLE FileMappingHandle;
	unsigned char* FileMapping;

	if ((FileMappingHandle = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, sizeof(unsigned char) * 4 * 3276800 + 19, "mapping")) == 0)
	{
		return -12;
	}

	if ((FileMapping = (unsigned char*)MapViewOfFile(FileMappingHandle, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(unsigned char) * 4 * 3276800 + 19)) == 0)
	{
		return -13;
	}

	printf("file mapping created");

	// Initial Socket
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	const char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult = 0;
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(SERVER_IP, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	while (true) {
		// receive IMU data from socket
		// Attempt to connect to an address until one succeeds
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

			// Create a SOCKET for connecting to server
			ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
				ptr->ai_protocol);
			if (ConnectSocket == INVALID_SOCKET) {
				printf("socket failed with error: %ld\n", WSAGetLastError());
				WSACleanup();
				return 1;
			}

			// Connect to server.
			iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				closesocket(ConnectSocket);
				ConnectSocket = INVALID_SOCKET;
				continue;
			}
			break;
		}

		
		// Send an initial buffer
		iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}

		printf("Bytes Sent: %ld\n", iResult);
		

		// shutdown the connection since no more data will be sent
		iResult = shutdown(ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			printf("Bytes received: %d\n", iResult);
			recvbuf[iResult] = 0;
			fputs(recvbuf, stdout);
			//std::this_thread::sleep_for(std::chrono::milliseconds(30));
		}
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

		// receive captured images from gstreamer
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

		//cv::namedWindow("left");
		//imshow("left", leftCamera);

		//cv::namedWindow("right");
		//imshow("right", rightCamera);

		//imshow("Sender", frame);
		//if (cv::waitKey(1) == 's')
		//break;


		// Stitch
		if (myApp.run(&myAppParams, leftCamera, rightCamera, FileMappingHandle, FileMapping, recvbuf) != NVSTITCH_SUCCESS)
		{
			std::cout << "Stitching failed." << std::endl;
			return 1;
		}

	}
	cv::destroyAllWindows();

	// cleanup socket
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}