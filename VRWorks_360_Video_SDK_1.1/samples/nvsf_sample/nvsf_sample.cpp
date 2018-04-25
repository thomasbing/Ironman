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

#include <nvsf.h>
#include <stdio.h>
#include <memory.h>
#include <fp32pcm.hpp>

#include "CmdArgsMap.hpp"

#define NVSF_CALL(X) { nvstitchResult  t = (X); if(t!=NVSTITCH_SUCCESS){printf("Error at line %d\n", __LINE__); exit(-1);}}

void StereoNVSFDemo(uint32_t sampleRate,
				    float stereoSpread,
					float outputGain,	
					std::vector<std::string>& inputFiles,
					std::string& outputFile)

{
	const uint32_t processingSize = 1024;
	const uint32_t pushSize = 960;

	NVSF_CALL(nvsfInitialize());
	uint32_t version;
	NVSF_CALL(nvsfGetVersion(&version));

	nvsf_t context = nullptr;
	NVSF_CALL(nvsfCreateContext(&context, "", 0));
	NVSF_CALL(nvsfSetSampleRate(context, 48000u));
	NVSF_CALL(nvsfSetOutputFormat(context, NVSTITCH_AUDIO_OUTPUT_STEREO_MIXDOWN));
	NVSF_CALL(nvsfSetPullSize(context, processingSize));
	NVSF_CALL(nvsfSetAlgorithmParameter(context, NVSF_OUTPUT_GAIN, &outputGain));
	NVSF_CALL(nvsfSetAlgorithmParameter(context, NVSF_STEREO_SPREAD_MIX_COEFFICIENT, &stereoSpread));

	uint32_t numInputs = inputFiles.size();
	nvsfInputDescriptor_t* inputs = new nvsfInputDescriptor_t[numInputs];
	nvsfInput_t* inputHandles = new nvsfInput_t[numInputs];
	fp32PCM* inputPCMFiles = new fp32PCM[numInputs];
	uint32_t totalSamples = 0;
	bool* streamEnded = new bool[numInputs];
	for (int i = 0; i < numInputs; i++)
	{
		inputs[i].numChannels = 1;
		memset(&inputs[i].pose, 0, sizeof(nvstitchPose_t));
		inputs[i].type = NVSTITCH_AUDIO_INPUT_TYPE_OMNI;
		inputPCMFiles[i].readFromFile(inputFiles[i], 1);
		streamEnded[i] = false;
		totalSamples = totalSamples > inputPCMFiles[i].m_numSamples ? totalSamples : inputPCMFiles[i].m_numSamples;
		NVSF_CALL(nvsfAddInput(context, &inputHandles[i], &inputs[i]));
	}
	NVSF_CALL(nvsfCommitConfiguration(context));

	// outputSamples is totalSamples rounded up to the next multiple of processingSize
	uint32_t outputSamples = processingSize*((totalSamples + processingSize - 1) / processingSize);
	fp32PCM outputPCMFile;
	outputPCMFile.allocateSilence(2, outputSamples);
	float * outBuffers[2];
	outBuffers[0] = new float[processingSize];
	outBuffers[1] = new float[processingSize];
	uint64_t timestamp; // Timestamps are not used currently
	uint32_t pulledSamples = 0;
		
	for (uint32_t submittedSamples = 0; submittedSamples < totalSamples; submittedSamples += pushSize)
	{
		for (uint32_t inputIndex = 0; inputIndex < numInputs; inputIndex++)
		{
			if (!streamEnded[inputIndex])
			{
				float* dataPtr = &inputPCMFiles[inputIndex].m_deInterleavedBuffers[0][submittedSamples];
				if (submittedSamples + pushSize >= inputPCMFiles[inputIndex].m_numSamples)
				{
					uint32_t finalSampleCount = inputPCMFiles[inputIndex].m_numSamples - submittedSamples;
					NVSF_CALL(nvsfInputAddData(context, inputHandles[inputIndex], &dataPtr, finalSampleCount, 0));
					NVSF_CALL(nvsfInputEndOfStream(context, inputHandles[inputIndex]));
					streamEnded[inputIndex] = true;
				}
				else
				{
					NVSF_CALL(nvsfInputAddData(context, inputHandles[inputIndex], &dataPtr, pushSize, 0));
				}
			}
		}
		// Pull all valid data out of nvsf 
		while (NVSTITCH_SUCCESS == nvsfProcess(context, outBuffers, &timestamp))
		{
			memcpy(&outputPCMFile.m_deInterleavedBuffers[0][pulledSamples], outBuffers[0], processingSize * sizeof(float));
			memcpy(&outputPCMFile.m_deInterleavedBuffers[1][pulledSamples], outBuffers[1], processingSize * sizeof(float));
			pulledSamples += processingSize;
		}
		
	}
	NVSF_CALL(nvsfDestroyContext(context));
	NVSF_CALL(nvsfFinalize());

	fflush(stdout);
	
	outputPCMFile.writeToFile(outputFile);
	
	delete[] outBuffers[0];
	delete[] outBuffers[1];
	delete[] inputPCMFiles;
	delete[] inputHandles;
	delete[] inputs;
}

int main(int argc, char** argv)
{
	bool show_help = false;
	std::string numInputs_str;
	std::string contentDir_str;
	std::string sampleRate_str;
	std::string stereoSpread_str;
	std::string outputGain_str;
	std::string outputFile_str;

	// Process command line arguments
	CmdArgsMap cmdArgs = CmdArgsMap(argc, argv, "--")
		("help", "Produce help message", &show_help)
		("content_dir", "Directory containing input pcm files named input1.pcm, input2.pcm, ...", &contentDir_str, contentDir_str)
		("num_inputs", "number of input files (must be greater than zero)", &numInputs_str, numInputs_str)
		("sample_rate", "samples per second of input files", &sampleRate_str, sampleRate_str)
		("stereo_spread", "Stereo Spread effect from 0.0 (off) to 1.0 (max)", &stereoSpread_str, stereoSpread_str)
		("output_gain", "gain applied to output. From 0.0 (silence) and up higher", &outputGain_str, outputGain_str)
		("output_file", "output file. Will be 2 channel interleaved raw fp32 pcm", &outputFile_str, outputFile_str);

	if (show_help)
	{
		printf("Low-Level Sound Field Sample Application\n");
		printf("%s", cmdArgs.help().c_str());
		return 1;
	}

	uint32_t numInputs = atoi(numInputs_str.c_str());
	if (numInputs == 0)
	{
		printf("Error: Number of inputs must be > 0\n");
		return 0;
	}

	uint64_t sampleRate = atoi(sampleRate_str.c_str());
	if (sampleRate == 0)
	{
		printf("Error: Sample rate must be > 0\n");
		return 0;
	}
	float stereoSpread = atof(stereoSpread_str.c_str());
	if (stereoSpread < 0.0f || stereoSpread > 1.0f)
	{
		printf("Error: Stereo Spread (argument 3) must be between 0.0 and 1.0\n");
		return 0;
	}
	float outputGain = atof(outputGain_str.c_str());
	if (outputGain < 0.0)
	{
		printf("Error: output gain must be >= 0.0\n");
		return 0;
	}
  if (!contentDir_str.empty())
  {
    switch (contentDir_str[contentDir_str.size() - 1])
    {
    #ifdef _MSC_VER
    case '\\':
    #endif // _MSC_VER
    case '/':
      break;
    default:
      contentDir_str += '/';
      break;
    }
  }
  std::string outputFile(outputFile_str.c_str());
	std::vector<std::string> inputFiles;
	for (uint32_t i = 1; i <= numInputs; i++)
	{
		inputFiles.push_back(contentDir_str + "input" + std::to_string(i) + ".pcm");
	}

	StereoNVSFDemo(sampleRate, stereoSpread, outputGain, inputFiles, outputFile);
	printf("All Done. \nFile %s generated\n", outputFile.c_str());

	return 0;
}
