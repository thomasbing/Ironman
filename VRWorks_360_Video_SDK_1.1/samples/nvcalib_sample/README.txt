nvcalib Sample Application
---------------------------

Overview
--------

Sample application to demonstrate the use of the nvcalib API.  This sample application takes a collection of
images (Common image formats supported[jpeg, png, bmp]), an input camera rig estimates XML and outputs the calibrated camera rig parameter XML.

Usage
-----

nvcalib_sample --wd <path to rig, stitcher and footage specification XML files with trailing '\'>
               --in_xml <estimates rig specification XML file>
               --out_xml <output calibrated specification XML file>
               --trans < translation based calibration, 0=off(default), 1=on>

Example
-------

nvcalib_sample --wd Y:\ship\ --in_xml rig.xml --out_xml rig_calibrated.xml

run.bat
-------

Calibrates the sample footage in ..\..\footage directory.
The output xml will be stored as ..\..\footage\calib_rig_spec.xml