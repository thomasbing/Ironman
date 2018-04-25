
Folder contents
---------------

run.bat : calibrates and stitches the sample footage with low level video SDK. The results will be available to view in ..\samples\

nvcalib_sample : demonstrates the use of the nvcalib (camera calibration) API. This sample application takes a collection of image, an input camera rig estimates XML and outputs the calibrated camera rig parameter XML

nvsf_sample : demonstrates the use of the nvsf (low level audio) API. This sample application mixes a collection of 32 bit float mono PCM input audio files into a single stereo spread 32 bit float output audio file

nvss_sample : demonstrates the use of the nvss (low level video) API. This sample application stitches a collection of images specified in the footage XML file into a single mono or stereo panorama BMP image

nvstitch_sample: demonstrates the use of the nvstitch (high level video and audio) API. This sample application takes a collection of MP4 videos and creates a single stitched MP4 360 video panorama

To build the sample applications, use CMake 3.2 or higher to generate a Visual Studio 2015 x64 solution. The solution will contain a project for each of the samples listed above.
