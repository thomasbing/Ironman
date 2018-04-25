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

#ifndef __NVSF_H__
#define __NVSF_H__

#include <stdlib.h>
#include <stdint.h>
#include <nvstitch_common.h>
#include <nvstitch_common_audio.h>

#ifdef __cplusplus
extern "C" {
#endif	

/*
=== Overview  ==== 
	The NVIDIA Sound Field (NVSF) API is a low-level audio processing API for combining several different audio
inputs such as a microphone array into a single sound field. NVSF is part of the NVIDIA 360 Video SDK.

	NVSF require that all inputs have a uniform sample rate and that all inputs are in 32-bit floating point
pulse code modulation (PCM) format. The client application is responsible for all sample rate and format
conversions.

	NVSF operates on a push/pull model. Audio data from inputs are *pushed* into NVSF as they become available. 
The client application can then request a *pull* operation to get a sound field out. NVSF buffers the input and will
only generate an output on a pull request if sufficient data from each input has been presented. 


==== Using NVSF: Setup ====
	To use NVSF, the library must first be setup. nvsfInitialize() must be called to create the library before any
other calls can be executed. After initialization, an application must call nvsfCreateContext to create a processing 
context that holds all the state information for a sound field. Processing contexts may be created with a name. If
an application creates a context with the same name as a context that exists, including the empty name, a handle
to the existing context is returned and a reference cound for that context is incremented. This is usfeul if the input
and output components of the client application is structured such that the input and output components cannot
easily communicate directly. 

	Before the NVSF context can be used for processing, inputs must be declared, parameters set, and then the configuration
must be committed. Inputs are specified to nvsf with the nvsfAddInput() function. Other functions, documented below, exist
to set the sample rate, output format, and pull size. 

	Once all inputs are specified and parameters are set, the client must call nvsfCommitConfiguration() to create the
internal processing pipeline and ready NVSF for data processing. The configuration of a context cannot be changed after
nvsfCommitConfiguration() has been called. After nvsfCommitConfiguration() has been called, attempting to add inputs or
setting parameters other than the algorithm parameters is an error.


==== Using NVSF: Operations ==== 

	During operation, the client application will feed audio data to NVSF using nvsfInputAddData(). The client will request
data from NVSF using nvsfProcess(). When nvsfProcess() returns NVSTITCH_SUCCESS, it has generated N samples for each channel
of the output, where N is the pull size set with nvsfSetPullSize(). If any of the inputs do not have enough data to generate
N output samples, nvsfProcess() will return NVSTITCH_AUDIO_OUT_OF_DATA. 

	When an input stream has ended, the client application should call nvsfInputEndOfStream() to indicate that no more data will
come from this stream. If other streams are still active, the ended stream will behave as though it is being fed silence. 
Once all streams have ended and all of the data which was pushed to NVSF via nvsfInputAddData() has been consumed, 
calls to nvsfProcess() will return NVSTITCH_AUDIO_ALL_STREAMS_ENDED.

	At any time during operation, including after nvsfCommitConfiguration() has been called, the client application can call
nvsfSetAlgorithmParameter()  to adjust  parameters to the algorthm which generates output audio from the inputs. Changes
to parameters will take effect on the subsequent call to nvsfProcess().


Using NVSF: Cleanup
	Once the client application is done with a context, it must call nvsfDestroyContext() to destroy the context. If
that context was created more than once, a reference counter is decremented. The context is deallocated and destroyed
ocne the reference count reaches zero.
	Once the application is done with NVSF, it should call nvsfFinalize() to clean up any remaining API state. Any calls
to NVSF after nvsfFinalize() has been called are errors.
*

*/


/*
	nvsfAlgortihmParameter_t enumerates parameters for the sound field creation algorithm. Some parameters may or may not
 be applicable, depending on the selected output format.
 NVSF_STEREO_SPREAD_MIX_COEFFICIENT - Controls how much of the stereo spread effect is applied in NVSF_OUTPUT_STEREO_MIXDOWN.
                                      Parameter value is a 32bit float in the range [0.0, 1.0]. A value of 0.0 disables the 
									  effect and L and R output channels will be identical. A value of 1.0 has the effect turned
									  up to it's maximum effect. Default value NVSF_DEFAULT_STEREO_SPREAD_COEFFICIENT is used
									  if this parameter is not set.
 
 NVSF_OUTPUT_GAIN -                   Sets a uniform gain applied to all channels of the output. Parameter is a 32 bit float in the
									  range [0.0 - Inf). This has the effect of multiply each output sample by the provided gain.
									  Default value of NVSF_DEFAULT_OUTPUT_GAIN is used if this parameter is not set.
*/
typedef enum NVSF_ALGORITHM_PARAMETERS_enum
{
	NVSF_STEREO_SPREAD_MIX_COEFFICIENT,
	NVSF_OUTPUT_GAIN,
	NVSF_NUM_ALGORITHM_PARAMETERS
} nvsfAlgorithmParameter_t;


/*
nvsfInputDescriptr_t contains information describing a single input. 
	numChannels - the number of channels in this input
	type        - the type of input (e.g. microphone)
	pose        - information about the location and orientation of the input.
*/
typedef struct NVSF_INPUT_DESCRIPTOR_struct
{
	uint32_t         numChannels;
    nvstitchAudioInputType  type;
	nvstitchPose_t   pose;
} nvsfInputDescriptor_t;

/* Default values which will be used if no other values are specified */
const uint32_t NVSF_DEFAULT_SAMPLE_RATE = 48000;
const uint32_t NVSF_DEFAULT_PULL_SIZE = 1024;
const uint32_t NVSF_DEFAULT_OUTPUT_FORMAT = NVSTITCH_AUDIO_OUTPUT_STEREO_MIXDOWN;
const float    NVSF_DEFAULT_OUTPUT_GAIN = 1.0f;
const float    NVSF_DEFAULT_STEREO_SPREAD_COEFFICIENT = 0.45f;

/* Opaque Types for handles in the user application */
typedef uint64_t nvsfInput_t;
typedef void* nvsfOutput_t;
typedef void* nvsf_t;

/* Initialize the API. Must be called before any other routines are called

Returns:
 NVSTITCH_SUCCESS on success
 */
nvstitchResult nvsfInitialize();

/* Finalize the API. No other routines may be called after this has been called 

Returns:
  NVSTITCH_SUCCESS on success
*/

nvstitchResult nvsfFinalize();

/* Returns the Version number as MMNN where MM is major version number and NN is minor version number

Returns
  NVSTITCH_SUCCESS on success
*/
nvstitchResult nvsfGetVersion(uint32_t* pVersion);

/* Create a named context. If a context of this name exists, a handle to the existing context will be 
   returned and the reference count of that context incremented.


Returns:
   NVSTITCH_SUCCESS on success
   NVSTITCH_ERROR_BAD_STATE - NVSF is not initialized
   NVSTITCH_ERROR_NULL_POINTER if pContext is NULL.
*/
nvstitchResult nvsfCreateContext(nvsf_t* pContext, const char name[], const size_t nameLength);


/* Decrement the reference count of the context and destroy it if reference count has reached zero 

Returns:
  NVSTITCH_SUCCESS on success
  NVSTITCH_ERROR_BAD_STATE - NVSF is not initialized 
  NVSTITCH_ERROR_BAD_PARAMETER - if the context is not valid
*/
nvstitchResult nvsfDestroyContext(nvsf_t context);

/* Tells NVSF about a new input. Must be called before nvsfCommitConfiguration() 

Returns:
  NVSTITCH_SUCCESS on success
  NVSTITCH_ERROR_BAD_STATE - NVSF is not initialized or the context is invalid
  NVSTITCH_AUDIO_CONFIGURATION_COMMITTED - properties cannot be changed because configuration has already been committed.
  NVSTITCH_ERROR_NULL_POINTER - pInputHandle or pDescriptor are NULL
*/
nvstitchResult nvsfAddInput(nvsf_t context, nvsfInput_t* pInputHandle, const nvsfInputDescriptor_t* pDescriptor);

/* Change the input descriptor for an existing input. Must be called before nvsfCommitCofiguration() 

Returns:
  NVSTITCH_SUCCESS on success
  NVSTITCH_ERROR_BAD_STATE - NVSF is not initialized or the context is invalid
  NVSTITCH_AUDIO_CONFIGURATION_COMMITTED - properties cannot be changed because configuration has already been committed.
  NVSTITCH_BAD_PARAMETER - The specified input is not part of this context.
*/
nvstitchResult nvsfSetInputDescriptor(nvsf_t context, nvsfInput_t inputHandle, nvsfInputDescriptor_t* pDescriptor);

/* Returns the input descriptor for an input for an existing input.

Returns:
	NVSTITCH_SUCCESS on success
	NVSTITCH_ERROR_BAD_STATE - NVSF is not initialized or the context is invalid
	NVSTITCH_AUDIO_CONFIGURATION_COMMITTED - properties cannot be changed because configuration has already been committed.
	NVSTITCH_BAD_PARAMETER - The specified input is not part of this context.
	NVSTITCH_ERROR_NULL_POINTER - pDescriptor is NULL
*/
nvstitchResult nvsfGetInputDescriptor(nvsf_t context, nvsfInput_t inputHandle, nvsfInputDescriptor_t * pDescriptor);

/* Set the universal sample rate for all inputs and output of NVSF. Must be aclled before  
   nvsfCommitConfiguration() 

 Returns:
   NVSTITCH_SUCCESS on success
   NVSTITCH_ERROR_BAD_STATE - NVSF is not initialized or the context is invalid
   NVSTITCH_AUDIO_CONFIGURATION_COMMITTED - properties cannot be changed because configuration has already been committed.
   NVSTITCH_BAD_PARAMETER - The requested sample rate is outside the valid range [32000-192000]
*/
nvstitchResult nvsfSetSampleRate(nvsf_t context, uint32_t sampleRate);

/* Returns the universal sample rate for all inputs and output of NVSF.

Returns:
  NVSTITCH_SUCCESS on success
  NVSTITCH_ERROR_BAD_STATE - NVSF is not initialized or the context is invalid
  NVSTITCH_ERROR_NULL_POINTER if pSampleRate is NULL.
*/
nvstitchResult nvsfGetSampleRate(nvsf_t context, uint32_t* pSampleRate);

/* Set the number of samples which will be generated by nvsfProcess() each time. Must be called before 
   nvsfCommitConfiguration(). numSamples must be greater than or equal to 32.
 

Returns:
   NVSTITCH_SUCCESS on success
   NVSTITCH_ERROR_BAD_STATE - NVSF is not initialized or the context is invalid
   NVSTITCH_AUDIO_CONFIGURATION_COMMITTED - properties cannot be changed because configuration has already been committed.
   NVSTITCH_BAD_PARAMETER - The requested sample rate is outside the valid range [32 - MAX_INT]     
*/
nvstitchResult nvsfSetPullSize(nvsf_t context, uint32_t numSamples);


/* Returns the number of samples which will be generated by nvsfProcess() each time.
Returns:
  NVSTITCH_SUCCESS on success
  NVSTITCH_ERROR_BAD_STATE - NVSF is not initialized or the context is invalid

*/
nvstitchResult nvsfGetPullSize(nvsf_t context, uint32_t* pNumSamples);

/* Sets the output format which will be generated by this NVSF context. Must be called before 
   nvsfCommitConfiguration()
 
 Returns:
   NVSTITCH_SUCCESS on success
   NVSTITCH_ERROR_BAD_STATE - NVSF is not initialized or the context is invalid
   NVSTITCH_AUDIO_CONFIGURATION_COMMITTED - properties cannot be changed because configuration has already been committed.
   NVSTITCH_BAD_PARAMETER - The requested sample rate is outside the valid range [32 - MAX_INT]
   */
nvstitchResult nvsfSetOutputFormat(nvsf_t context, nvstitchAudioOutputType  type);


/* Returns the output format this NVSF context is set to generate 

Returns: 
  NVSTITCH_SUCCESS on success
  NVSTITCH_ERROR_BAD_STATE - NVSF is not initialized or the context is invalid
  NVSTITCH_ERROR_NULL_POINTER - pType is NULL
*/
nvstitchResult nvsfGetOutputFormat(nvsf_t context, nvstitchAudioOutputType* pType);

/* Commit configuration of NVSF and prepare it for processing

Returns:
NVSTITCH_ERROR_NOT_IMPLEMENTED - Output format is not supported
NVSTITCH_ERROR_GENERAL - An error occurred setting up internal processing components
*/
nvstitchResult nvsfCommitConfiguration(nvsf_t context);

/* Change the value of thet algorithm parameter specified by parameter to the contets 
   of the memory value points to.
   
  Valid parameter values                 value type       valid range 
   NVSF_STEREO_SPREAD_MIX_COEFFICIENT       float         [0.0 - 1.0]
   NVSF_OUTPUT_GAIN                         float         [0.0 - Inf)
   
  Returns:
   NVSTITCH_SUCCESS - The call succeeded
   NVSTITCH_ERROR_BAD_PARAMETER - Parameter is not a recognized algorithm parameter enum 
                                  or the value passed i is not in the valid range for that parameter.
   NVSTITCH_ERROR_NULL_POINTER  - value is NULL.
   */
nvstitchResult nvsfSetAlgorithmParameter(nvsf_t context, nvsfAlgorithmParameter_t parameter, void* value);


/* Push numSamples for all channels of a specified input into NVSF. pData is an array of N pointers, where N
   is the number of channels in the specified input.  The pointers in pData point to contiguous (not-interleaved) 
   arrays of numSamples floats for each channel. Timestamp is an optional timestamp in nanoseconds. 
   
   Returns:
   NVSTITCH_SUCCESS - The call succeeded
   NVSTITCH_AUDIO_CONFIGURATION_NOT_COMMITTED - Illegal call because configuration has not been committed 
   NVSTITCH_ERROR_BAD_STATE - The requested input is not part of this context 
   NVSTITCH_ERROR_NULL_POINTER if pData is NULL.
   */
nvstitchResult nvsfInputAddData(nvsf_t context, nvsfInput_t inputIdentifier, float** pData, uint32_t numSamples, uint64_t timeStamp);

/* Indicates that no more data will come from this stream. After this call, the input will behave as though
   silence (zeros) have been pushed into the buffer until all streams end and all data pushed into all streams
   has been consumed.
   
   Returns:
   NVSTITCH_SUCCESS - The call succeeded
   NVSTITCH_ERROR_BAD_STATE - The requested input is not part of this context
*/
nvstitchResult nvsfInputEndOfStream(nvsf_t context, nvsfInput_t inputIdentifier);

/* Request one set of output buffers. pData will be an array of N pointers, where N is the number of channels 
   dictated by the output format. Each pointer must point to a contiguous (non-interleaved) array where NVSF
   will put the results. If NVSF cannot generate the output due to lack of data, either NVSTITCH_AUDIO_OUT_OF_DATA
   or NVSTITCH_AUDIO_ALL_STREAMS_ENDED will be returned and the contents of pData arrays will not be valid. 
   If the process succeeds, NVSTITCH_SUCCESS is returned and the output is in the arrays indicated in pData.
   
   Returns: 
   NVSTITCH_SUCCESS - Processing succeeded and data buffers have been filled
   NVSTITCH_AUDIO_CONFIGURATION_NOT_COMMITTED - Illegal call because configuration has not been committed
   NVSTITCH_AUDIO_OUT_OF_DATA - Insufficient data from at least one input to generate an output
   NVSTITCH_AUDIO_ALL_STREAMS_ENDED - All streams have ended and all buffered data has been consumed
   NVSTITCH_ERROR_BAD_STATE - The internal processing structures encountered an unexpected state
*/
nvstitchResult nvsfProcess(nvsf_t context, float ** pData, uint64_t* pTimeStamp);


#ifdef __cplusplus
}
#endif

#endif /* __NVSF_H__ */
