@ECHO OFF
SETLOCAL
SET PATH=%PATH%;..\..\nvstitch\binary;..\..\external\cuda;..\..\external\opencv-3.2\binary
nvss_sample.exe --input_dir_base ..\..\footage\ --rig_spec sample_calib_rig_spec.xml --image_input image_input.xml --out_file out_stitched.jpg --stereo