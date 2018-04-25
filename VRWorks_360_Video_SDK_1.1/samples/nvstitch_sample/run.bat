@ECHO OFF
SETLOCAL
SET PATH=%PATH%;..\..\nvcalib\binary;..\..\nvstitch\binary;..\..\external\cuda;..\..\external\fdkaac;..\..\external\opencv-3.2\binary
nvstitch_sample.exe  --input_dir_base ..\..\footage\ --stitcher_spec stitcher_spec_host_buffer.xml --rig_spec sample_calib_rig_spec.xml --video_input image_input.xml