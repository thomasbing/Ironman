nvstitch Sample Application
---------------------------

Overview
--------

Sample application to demonstrate the use of the nvstitch API. This sample application takes a collection of
MP4 videos and creates a single stitched MP4 360 video panorama or takes a collection of images in host buffers
and stitches a single stereo panorama that is saved from a host buffer in a callback function.


Usage
-----

nvstitch_sample --help
                --rig_spec <rig specification XML file>
                --audio_rig_spec <audio rig specification XML file>
                --calib_rig_spec <updated rig specification XML file resulting from calibration>
                --stitcher_spec <stitcher specification XML file>
                --video_input <XML file containing video input specification>
                --audio_input <XML file containing audio input specification>
                --input_dir_base <path to rig, stitcher and footage specification XML files with trailing '\'>
                --calib
                --audio

Examples
--------

1) Stitch Video & Audio

nvstitch_sample.exe --input_dir_base ..\..\footage\ --rig_spec sample_calib_rig_spec.xml --video_input video_input.xml  --audio --audio_rig_spec audio_rig_spec.xml --audio_input audio_rig_spec.xml --stitcher_spec stitcher_spec.xml

2) Calibrate First, Then Stitch Video & Audio

nvstitch_sample.exe --input_dir_base ..\..\footage\ --calib --rig_spec rig_spec.xml --video_input video_input.xml --audio --audio_rig_spec audio_rig_spec.xml --audio_input audio_rig_spec.xml --stitcher_spec stitcher_specs.xml

3) Stitch Single Frame From Host Buffers, Then Use Callback Function To Save Result

nvstitch_sample.exe  --input_dir_base ..\..\footage\ --stitcher_spec stitcher_spec_host_buffer.xml --rig_spec sample_calib_rig_spec.xml --video_input image_input.xml 

run.bat
-------

Creates a 360 stereo panorama MP4 video using the sample footage in ..\..\footage directory with the high level SDK. 
The result will be available to view in ..\..\samples\nvstitch_sample\out_stitched.mp4


