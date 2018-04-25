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

#ifndef FP32PCM_H_
#define FP32PCM_H_

#include <string>
#include <stdio.h>
#include <sys\stat.h>
#include <string>
#include <vector>

static uint64_t getFileSize(std::string filename)
{
	struct stat buf;
	if (0 != stat(filename.c_str(), &buf))
	{
		printf("Could not stat %s\n", filename.c_str());
		return 0;
	}
	return buf.st_size;
}

class fp32PCM
{
public:
	fp32PCM() : 
		m_deInterleavedBuffers(nullptr),
		m_numChannels(0),
		m_numSamples(0)
	{

	}

	~fp32PCM()
	{
		this->deallocate();
	}

	void allocateSilence(int numChannels, int numSamples)
	{
		if (m_deInterleavedBuffers != nullptr)
		{
			this->deallocate();
		}
		m_numChannels = numChannels;
		m_numSamples = numSamples;
		m_deInterleavedBuffers = new float*[m_numChannels];
		for (uint32_t ch = 0; ch < m_numChannels; ch++)
		{
			m_deInterleavedBuffers[ch] = new float[m_numSamples];
		}
	}

	void readFromFile(std::string filename, int numChannels)
	{
		uint32_t numSamples = getFileSize(filename.c_str()) / (sizeof(float)*numChannels);
		if (0 == numSamples)
		{
			printf("Error: Zero filesize for %s\n", filename.c_str());
			return;
		}
		FILE* infile = fopen(filename.c_str(), "rb");
		if (infile == nullptr)
		{
			printf("Error: Could not open file %s\n", filename.c_str());
		}
		float* inputBuffer = new float[numChannels*numSamples];
		uint32_t samplesRead = fread(inputBuffer, sizeof(float), numChannels*numSamples, infile);
		if (samplesRead != numChannels*numSamples)
		{
			printf("Error: Read %d samples from %s; %d expected\n", samplesRead, filename.c_str(), numChannels* numSamples);
		}
		else
		{
			m_numChannels = numChannels;
			m_numSamples = samplesRead;
			allocateSilence(m_numChannels, m_numSamples);
			uint32_t sampleOffset = 0;
			for (uint32_t s = 0; s < m_numSamples; s++)
			{
				for (uint32_t ch = 0; ch < m_numChannels; ch++)
				{
					m_deInterleavedBuffers[ch][s] = inputBuffer[sampleOffset++];
				}
			}
		}
		delete[] inputBuffer;
	}

	void writeToFile(std::string filename)
	{
		FILE* outfile = fopen(filename.c_str(), "wb");
		if (outfile == nullptr)
		{
			printf("Error: could not open %s for writing\n", filename.c_str());
			return;
		}
		for (uint32_t s = 0; s < m_numSamples; s++)
		{
			for (uint32_t ch = 0; ch < m_numChannels; ch++)
			{
				fwrite(&m_deInterleavedBuffers[ch][s], sizeof(float), 1, outfile);
			}
		}
	}

	void deallocate()
	{
		if (m_deInterleavedBuffers != nullptr)
		{
			for (uint32_t ch = 0; ch < m_numChannels; ch++)
			{
				if (m_deInterleavedBuffers[ch] != nullptr)
				{
					delete[] m_deInterleavedBuffers[ch];
				}
			}
			memset(m_deInterleavedBuffers, 0, m_numChannels * sizeof(float*));
			delete[] m_deInterleavedBuffers;
			m_deInterleavedBuffers = nullptr;
		}
		m_numSamples = 0;
		m_numChannels = 0;
	}
	float** m_deInterleavedBuffers;
	int     m_numSamples;
	int     m_numChannels;
};

#endif