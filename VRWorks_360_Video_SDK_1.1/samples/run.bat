@ECHO OFF
SETLOCAL
SET PATH=%PATH%;..\nvcalib\binary;..\nvstitch\binary;..\external\cuda;..\external\fdkaac;..\external\opencv-3.2\binary;nvcalib_sample;nvss_sample
nvcalib_sample.exe --wd ..\footage\ --in_xml rig_spec.xml --out_xml calib_rig_spec.xml
nvss_sample.exe --input_dir_base ..\footage\ --rig_spec calib_rig_spec.xml --image_input image_input.xml --out_file out_stitched.jpg --stereo