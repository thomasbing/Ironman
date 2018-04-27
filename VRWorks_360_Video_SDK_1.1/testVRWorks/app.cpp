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

#include "app.h"

#include <iostream>
#include <chrono>
#include "cuda.h"
#include "cuda_runtime.h"
#include "vector_types.h"

#include "image_io_util.hpp"

using std::chrono::milliseconds;
using std::chrono::high_resolution_clock;

std::mutex frameMutex;

unsigned char* fakeImg = nullptr;
void initFake() {
	fakeImg = new unsigned char[1280 * 2560 * 4];
	for (int i = 0; i < 1280; i++) {
		for (int j = 0; j < 2560; j++) {
			int idx = 4 * (i * 2560 + j);
			fakeImg[idx] = (i/128)%2 * 255;
			fakeImg[idx + 1] = 0;
			fakeImg[idx + 2] = 0;
			fakeImg[idx + 3] = 0;
		}
	}
}

void pushImg
(unsigned char* FileMapping, char* camPos, unsigned char* framedata, int imageWidth, int imageHeight) {
	{
		std::unique_lock<std::mutex> lock(frameMutex);
		std::memcpy(FileMapping, framedata, imageHeight * imageWidth * 4);
		//initFake();
		//std::memcpy(FileMapping, fakeImg, imageHeight * imageWidth * 4);
		std:memcpy(FileMapping + 3276800 * 4, (unsigned char*)camPos, 25);
		std::cout << camPos[0] << camPos[1] << camPos[2] << camPos[3] << camPos[4] << camPos[5] << std::endl;

	}
}

nvstitchResult
app::run(appParams *params, cv::Mat leftCamera, cv::Mat rightCamera, HANDLE FileMappingHandle, unsigned char* FileMapping, char* orientation)
{
	int num_gpus;
	cudaGetDeviceCount(&num_gpus);

	std::vector<int> gpus;
	gpus.reserve(num_gpus);

	for (int gpu = 0; gpu < num_gpus; ++gpu)
	{
		cudaDeviceProp prop;
		cudaGetDeviceProperties(&prop, gpu);

		// Require minimum compute 5.2
		if (prop.major > 5 || (prop.major == 5 && prop.minor >= 2))
		{
			gpus.push_back(gpu);

			// Multi-GPU not yet supported for mono, so just take the first GPU
			if (!params->stereo_flag)
				break;
		}
	}

	nvssVideoStitcherProperties_t stitcher_props{ 0 };
	stitcher_props.version = NVSTITCH_VERSION;
	stitcher_props.pano_width = params->pano_width;
	stitcher_props.quality = params->quality;
	stitcher_props.num_gpus = gpus.size();
	stitcher_props.ptr_gpus = gpus.data();

	if (params->stereo_flag)
	{
		stitcher_props.pipeline = NVSTITCH_STITCHER_PIPELINE_STEREO;
		stitcher_props.stereo_ipd = 6.3f;
	}
	else
	{
		stitcher_props.pipeline = NVSTITCH_STITCHER_PIPELINE_MONO;
		stitcher_props.feather_width = 2.0f;
	}

	//std::cout << "part 1 finish." << std::endl;

	//stitcher_props.

	// Initialize stitcher instance
	nvssVideoHandle stitcher;
	RETURN_NVSS_ERROR(nvssVideoCreateInstance(&stitcher_props, &params->rig_properties, &stitcher));

	/*
	// Load image frames for each camera
	for (uint32_t camera = 0; camera < params->rig_properties.num_cameras; camera++)
	{

	std::string image_file_path = params->input_base_dir + params->filenames[camera];

	nvstitchImageBuffer_t input_image;
	RETURN_NVSS_ERROR(nvssVideoGetInputBuffer(stitcher, camera, &input_image));

	unsigned char* rgba_bitmap_ptr = nullptr;
	int image_width, image_height;
	if (getRgbaImage(image_file_path, &rgba_bitmap_ptr, image_width, image_height) == false)
	{
	std::cerr << "Error reading Image " << image_file_path << endl;
	return NVSTITCH_ERROR_MISSING_FILE;
	}

	if (nullptr == rgba_bitmap_ptr)
	{
	std::cout << "Error reading input image:" << image_file_path << std::endl;
	return  NVSTITCH_ERROR_NULL_POINTER;
	}

	// Ideally, we should verify image resolution matches rig descriptor. Now we just set
	if (image_width != input_image.width || image_height != input_image.height)
	{
	std::cout << "Error: resolution mismatch between input file " << image_file_path << " and rig descriptor\n";
	return NVSTITCH_ERROR_BAD_PARAMETER;
	}

	if (cudaMemcpy2D(input_image.dev_ptr, input_image.pitch,
	rgba_bitmap_ptr, input_image.row_bytes,
	input_image.row_bytes, input_image.height,
	cudaMemcpyHostToDevice) != cudaSuccess)
	{
	std::cout << "Error copying RGBA image bitmap to CUDA buffer" << std::endl;
	return NVSTITCH_ERROR_GENERAL;
	}
	}
	*/

	//***********************************************************************************
	// our code for stitching two cameras
	// left camera
	nvstitchImageBuffer_t input_image_left;
	uint32_t cameraLeft = 0;
	RETURN_NVSS_ERROR(nvssVideoGetInputBuffer(stitcher, cameraLeft, &input_image_left));

	unsigned char* rgba_bitmap_ptr_left = nullptr;
	int image_width_left, image_height_left;
	if (getRgbaImageAddress(leftCamera, &rgba_bitmap_ptr_left, image_width_left, image_height_left) == false) {
		std::cerr << "Error reading Image address " << endl;
		return NVSTITCH_ERROR_MISSING_FILE;
	}

	//std::cout << "part 2 finish" << std::endl;

	if (nullptr == rgba_bitmap_ptr_left)
	{
		std::cout << "Error reading left input image" << std::endl;
		return  NVSTITCH_ERROR_NULL_POINTER;
	}
	//std::cout << "part 2.1 finish" << std::endl;
	//std::cout << image_width_left << std::endl;
	//std::cout << input_image_left.width << std::endl;
	//std::cout << image_height_left << std::endl;
	//std::cout << input_image_left.height << std::endl;


	// Verify image resolution matches rig descriptor
	if (image_width_left != input_image_left.width || image_height_left != input_image_left.height)
	{
		std::cout << "Error: resolution mismatch between left input file and rig descriptor\n";
		return NVSTITCH_ERROR_BAD_PARAMETER;
	}


	if (cudaMemcpy2D(input_image_left.dev_ptr, input_image_left.pitch,
		rgba_bitmap_ptr_left, input_image_left.row_bytes,
		input_image_left.row_bytes, input_image_left.height,
		cudaMemcpyHostToDevice) != cudaSuccess)
	{
		std::cout << "Error copying RGBA image bitmap to CUDA buffer, left" << std::endl;
		return NVSTITCH_ERROR_GENERAL;
	}

	//std::cout << "part 3 finish" << std::endl;

	// right camera
	nvstitchImageBuffer_t input_image_right;
	uint32_t cameraRight = 1;
	RETURN_NVSS_ERROR(nvssVideoGetInputBuffer(stitcher, cameraRight, &input_image_right));

	unsigned char* rgba_bitmap_ptr_right = nullptr;
	int image_width_right, image_height_right;
	if (getRgbaImageAddress(rightCamera, &rgba_bitmap_ptr_right, image_width_right, image_height_right) == false) {
		std::cerr << "Error reading Image address " << endl;
		return NVSTITCH_ERROR_MISSING_FILE;
	}

	if (nullptr == rgba_bitmap_ptr_right)
	{
		std::cout << "Error reading right input image" << std::endl;
		return  NVSTITCH_ERROR_NULL_POINTER;
	}


	// Verify image resolution matches rig descriptor
	if (image_width_right != input_image_right.width || image_height_right != input_image_right.height)
	{
		std::cout << "Error: resolution mismatch between right input file and rig descriptor\n";
		return NVSTITCH_ERROR_BAD_PARAMETER;
	}


	if (cudaMemcpy2D(input_image_right.dev_ptr, input_image_right.pitch,
		rgba_bitmap_ptr_right, input_image_right.row_bytes,
		input_image_right.row_bytes, input_image_right.height,
		cudaMemcpyHostToDevice) != cudaSuccess)
	{
		std::cout << "Error copying RGBA image bitmap to CUDA buffer, right" << std::endl;
		return NVSTITCH_ERROR_GENERAL;
	}
	//**************************************************************************************

	// Synchronize CUDA before snapping start time
	cudaStreamSynchronize(cudaStreamDefault);

	const auto stitch_start = high_resolution_clock::now();

	// Stitch
	RETURN_NVSS_ERROR(nvssVideoStitch(stitcher));

	// Synchronize CUDA before snapping end time 
	cudaStreamSynchronize(cudaStreamDefault);

	// Report stitch time
	auto time = std::chrono::duration_cast<milliseconds>(high_resolution_clock::now() - stitch_start).count();
	std::cout << "Stitch Time: " << time << " ms" << std::endl;

	size_t out_offset = 0;
	unsigned char *out_stacked = nullptr;
	nvstitchImageBuffer_t output_image;
	int num_eyes = params->stereo_flag ? 2 : 1;
	for (int eye = 0; eye < num_eyes; eye++)
	{
		if (params->stereo_flag)
		{
			RETURN_NVSS_ERROR(nvssVideoGetOutputBuffer(stitcher, nvstitchEye(eye), &output_image));
		}
		else
		{
			RETURN_NVSS_ERROR(nvssVideoGetOutputBuffer(stitcher, NVSTITCH_EYE_MONO, &output_image));
		}

		if (out_stacked == nullptr)
			out_stacked = (unsigned char *)malloc(output_image.row_bytes * output_image.height * num_eyes);

		if (cudaMemcpy2D(out_stacked + out_offset, output_image.row_bytes,
			output_image.dev_ptr, output_image.pitch,
			output_image.row_bytes, output_image.height,
			cudaMemcpyDeviceToHost) != cudaSuccess)
		{
			std::cout << "Error copying output stacked panorama from CUDA buffer" << std::endl;
			return NVSTITCH_ERROR_GENERAL;
		}

		out_offset += output_image.height * output_image.row_bytes;
	}
	//putRgbaImage(params->out_file, out_stacked, output_image.width, output_image.height * num_eyes);
	float * camPos = new float[3];
	camPos[0] = 1.f;
	camPos[1] = 2.f;
	camPos[2] = 3.f;
	

	Mat img = cv::Mat(output_image.height * num_eyes, output_image.width, CV_8UC4, (void*)out_stacked);
	pushImg(FileMapping, orientation, img.data, output_image.width, output_image.height);

	//std::cout << "Length of output image: " << sizeof(out_stacked) / sizeof(*out_stacked) << std::endl;
	
	/*cv::namedWindow("result", WINDOW_NORMAL);
	cv::resizeWindow("result", 960, 480);
	Mat fake = cv::Mat(output_image.height * num_eyes, output_image.width, CV_8UC4, (void*)fakeImg);
	cv::imshow("result", fake);*/

	/*cv::namedWindow("result", WINDOW_NORMAL);
	cv::resizeWindow("result", 960, 480);
	cv::imshow("result", img);
	if (cv::waitKey(1) == 's') {
		std::cout << "finish playing" << endl;
		free(out_stacked);
		RETURN_NVSS_ERROR(nvssVideoDestroyInstance(stitcher));
		return NVSTITCH_ERROR_GENERAL;
	}*/

	free(out_stacked);
	delete[] camPos;

	// Clean up
	RETURN_NVSS_ERROR(nvssVideoDestroyInstance(stitcher));

	return NVSTITCH_SUCCESS;
}