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

#include <fstream>
#include <vector>
#include <iostream>
#include <chrono>
#include <string>

#include "image_io_util.hpp"

using std::chrono::milliseconds;
using std::chrono::high_resolution_clock;

void
NVSTITCHCALLBACK stitchCallBack(
	uint32_t packet_index,
	const nvstitchPayload_t *out_payload,
	const nvstitchAudioPayload_t *out_audio_payload,
	void *app_data) 
{

	// Write to file
	std::string outFileName = std::string("stitched_frame") + std::to_string(packet_index) + std::string(".jpg");

	if (putRgbaImage(outFileName, (unsigned char *)out_payload->payload.buffer.ptr, out_payload->image_size.x, out_payload->image_size.y) == false)
	{
		std::cerr << "Error Writing Image " << outFileName << endl;
	}
}

nvstitchResult
app::calibrate(appParams *params)
{
	nvstitchResult res = NVSTITCH_SUCCESS;

	// Create rig instance
	RETURN_NVSTITCH_ERROR(nvstitchCreateVideoRigInstance(&params->rig_properties, &params->stitcher_properties.video_rig));

	uint32_t frame_count = (uint32_t)params->calib_filenames.size();
	uint32_t camera_count = (uint32_t)params->calib_filenames.at(0).size();

	// Create calibration instance
	nvstitchCalibrationProperties_t calib_prop{};
	calib_prop.version = NVSTITCH_VERSION;
	calib_prop.frame_count = frame_count;
	calib_prop.input_form = nvstitchMediaForm::NVSTITCH_MEDIA_FORM_HOST_BUFFER;
	calib_prop.input_format = nvstitchMediaFormat::NVSTITCH_MEDIA_FORMAT_RGBA8UI;
	calib_prop.rig_estimate = params->stitcher_properties.video_rig;
	uint32_t input_image_channels = 4;

	nvstitchCalibrationInstanceHandle h_calib;
	RETURN_NVSTITCH_ERROR(nvstitchCreateCalibrationInstance(calib_prop, &h_calib));

	// Read input images for calibration
	uint32_t camera_num = params->rig_properties.num_cameras;

	const auto calibration_start = high_resolution_clock::now();

	for (uint32_t frame_index = 0; frame_index < 1; frame_index++)
	{
		for (uint32_t cam_index = 0; cam_index < camera_count; cam_index++)
		{
			std::string image_file_path = params->input_dir_base + params->calib_filenames[frame_index][cam_index];

			unsigned char* rgba_bitmap_ptr = nullptr;
			int image_width, image_height;
			if (getRgbaImage(image_file_path, &rgba_bitmap_ptr, image_width, image_height) == false)
			{
				std::cout << "Error reading calibration image " << image_file_path << endl;
				return NVSTITCH_ERROR_MISSING_FILE;
			}

			if (nullptr == rgba_bitmap_ptr)
			{
				std::cout << "Error reading input image:" << image_file_path << std::endl;
				return  NVSTITCH_ERROR_NULL_POINTER;
			}

			uint32_t width = params->rig_properties.cameras[cam_index].image_size.x;
			uint32_t height = params->rig_properties.cameras[cam_index].image_size.y;

			nvstitchPayload_t calib_payload = nvstitchPayload_t{ calib_prop.input_form,{ width, height } };

			calib_payload.payload.buffer.ptr = rgba_bitmap_ptr;

			calib_payload.payload.buffer.pitch = params->rig_properties.cameras[cam_index].image_size.x * input_image_channels;

			RETURN_NVSTITCH_ERROR(nvstitchFeedCalibrationInput(frame_index, h_calib, cam_index, &calib_payload));
		}
	}

	// Calibrate
	nvstitchVideoRigHandle h_calibrated_video_rig;
	RETURN_NVSTITCH_ERROR(nvstitchCalibrate(h_calib, &h_calibrated_video_rig));

	// Report calibration time.
	auto time = std::chrono::duration_cast<milliseconds>(high_resolution_clock::now() - calibration_start).count();
	std::cout << "Calibration Time: " << time << " ms" << std::endl;

	// Fetch calibrated rig properties
	RETURN_NVSTITCH_ERROR(nvstitchGetVideoRigProperties(h_calibrated_video_rig, &params->calibrated_rig_properties));

	// Destroy calibration instance
	RETURN_NVSTITCH_ERROR(nvstitchDestroyCalibrationInstance(h_calib));

	return NVSTITCH_SUCCESS;
}


nvstitchResult
app::run(appParams *params)
{
	nvstitchResult res = NVSTITCH_SUCCESS;

	// Create video rig instance
	if (params->calib_flag)
	{
		// Use calibrated rig properties
		RETURN_NVSTITCH_ERROR(nvstitchCreateVideoRigInstance(&params->calibrated_rig_properties, &params->stitcher_properties.video_rig));
	}
	else
	{
		// Use specified rig properties
		RETURN_NVSTITCH_ERROR(nvstitchCreateVideoRigInstance(&params->rig_properties, &params->stitcher_properties.video_rig));
	}

	// Create audio rig instance
	if (params->audio_flag)
	{
		RETURN_NVSTITCH_ERROR(nvstitchCreateAudioRigInstance(&params->audio_rig_properties, &params->stitcher_properties.audio_rig, nullptr));
	}

	if (params->stitcher_properties.output_payloads[0].payload_type == NVSTITCH_MEDIA_FORM_HOST_BUFFER)
	{
		// Create required output payload host buffers.
		for (uint32_t i{}; i < (uint32_t)params->stitcher_properties.num_output_payloads; ++i)
		{
			params->stitcher_properties.output_payloads[i].payload.buffer.ptr =
				malloc(params->stitcher_properties.output_payloads[0].image_size.x * 4 *
					params->stitcher_properties.output_payloads[0].image_size.y);
			params->stitcher_properties.output_payloads[i].payload.buffer.pitch =
				params->stitcher_properties.output_payloads[0].image_size.x * 4;
		}

		// Create stitcher instance
		nvstitchStitcherHandle stitcher{};
		RETURN_NVSTITCH_ERROR(nvstitchCreateStitcher(&params->stitcher_properties, &stitcher));

		// Start stitcher
		RETURN_NVSTITCH_ERROR(nvstitchStartStitcher(stitcher, &stitchCallBack, params));

		// Loop for every video input.  Load input images.
		for (uint32_t i{}; i < (uint32_t)params->rig_properties.num_cameras; ++i)
		{
			// Fix up the file names to include the full path
			const std::string filename(params->input_dir_base + params->payloads[i].payload.file.name);

			unsigned char *rgba_bitmap_ptr = nullptr;
			int image_width, image_height;
			if (getRgbaImage(filename, &rgba_bitmap_ptr, image_width, image_height) == false)
			{
				std::cerr << "Error reading Image " << filename << endl;
				return NVSTITCH_ERROR_MISSING_FILE;
			}

			if (nullptr == rgba_bitmap_ptr)
			{
				std::cout << "Error reading input image:" << filename << std::endl;
				return  NVSTITCH_ERROR_NULL_POINTER;
			}

			params->payloads[i].payload_type = NVSTITCH_MEDIA_FORM_HOST_BUFFER;
			params->payloads[i].payload.buffer.ptr = rgba_bitmap_ptr;
			params->payloads[i].payload.buffer.pitch = image_width * 4;
			params->payloads[i].image_size.x = image_width;
			params->payloads[i].image_size.y = image_height;
		}

		// Start timer
		const auto stitch_start = high_resolution_clock::now();

		// Stitch
		for (uint32_t i{}; i < (uint32_t)params->rig_properties.num_cameras; ++i)
		{
			RETURN_NVSTITCH_ERROR(nvstitchFeedStitcherVideo(0, stitcher, i, &params->payloads[i], false));
		}

		// Stop stitcher
		RETURN_NVSTITCH_ERROR(nvstitchStopStitcher(stitcher));

		// Report stitch time
		auto time = std::chrono::duration_cast<milliseconds>(high_resolution_clock::now() - stitch_start).count();
		std::cout << "Stitch Time: " << time << " ms" << std::endl;

		// Clean up
		nvstitchDestroyStitcher(stitcher);

		// Free input and output payload host buffers.
		for (uint32_t i{}; i < (uint32_t)params->stitcher_properties.num_output_payloads; ++i)
		{
			free(params->payloads[i].payload.buffer.ptr);
			free(params->stitcher_properties.output_payloads[i].payload.buffer.ptr);
		}
	}
	else
	{
		// Create stitcher instance
		nvstitchStitcherHandle stitcher{};
		RETURN_NVSTITCH_ERROR(nvstitchCreateStitcher(&params->stitcher_properties, &stitcher));

		// Start stitcher
		RETURN_NVSTITCH_ERROR(nvstitchStartStitcher(stitcher, nullptr, nullptr));

		const auto stitch_start = high_resolution_clock::now();

		// Loop for every video input.  Feed encoded audio and video.
		for (uint32_t i{}; i < (uint32_t)params->rig_properties.num_cameras; ++i)
		{
			// Fix up the file names to include the full path
			const std::string filename(params->input_dir_base + params->payloads[i].payload.file.name);
			params->payloads[i].payload.file.name = filename.c_str();

			if (params->audio_flag)
			{
				RETURN_NVSTITCH_ERROR(nvstitchFeedStitcherAudioVideo(0, stitcher, i, i, &params->payloads[i], false));
			}
			else
			{
				RETURN_NVSTITCH_ERROR(nvstitchFeedStitcherVideo(0, stitcher, i, &params->payloads[i], false));
			}
		}

		// Stop stitcher
		RETURN_NVSTITCH_ERROR(nvstitchStopStitcher(stitcher));

		// Report stitch time
		auto time = std::chrono::duration_cast<milliseconds>(high_resolution_clock::now() - stitch_start).count();
		std::cout << "Stitch Time: " << time << " ms" << std::endl;

		// Clean up
		nvstitchDestroyStitcher(stitcher);
	}

	return NVSTITCH_SUCCESS;
}