@ECHO OFF
SETLOCAL
SET PATH=%PATH%;..\nvstitch\binary;..\external\cuda;..\external\opencv-3.2\binary;..\external\memtest;
testVRWorks.exe --input_dir_base .\ --rig_spec calib_rig_spec.xml --image_input image_input.xml --out_file out_stitched.jpg