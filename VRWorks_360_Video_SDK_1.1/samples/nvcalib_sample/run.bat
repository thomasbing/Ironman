@ECHO OFF
SETLOCAL
SET PATH=%PATH%;..\..\nvcalib\binary;..\..\nvstitch\binary;..\..\external\cuda;..\..\external\fdkaac;..\..\external\opencv-3.2\binary
nvcalib_sample.exe --wd ..\..\kodak\ --in_xml rig_spec.xml --out_xml calib_rig_spec.xml