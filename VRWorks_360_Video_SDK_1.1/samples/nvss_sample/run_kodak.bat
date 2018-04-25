@ECHO OFF
SETLOCAL
SET PATH=%PATH%;..\..\nvstitch\binary;..\..\external\cuda;..\..\external\opencv-3.2\binary
nvss_sample.exe --input_dir_base ..\..\kodak\ --rig_spec calib_rig_spec.xml --image_input image_input.xml --out_file kodak_stitched.jpg