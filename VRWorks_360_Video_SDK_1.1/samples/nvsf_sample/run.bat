@ECHO OFF
SETLOCAL
SET PATH=%PATH%;..\..\nvstitch\binary
nvsf_sample.exe --content_dir ..\..\footage\ --num_inputs 2 --sample_rate 48000 --stereo_spread 0.75 --output_gain 1.5 --output_file output.pcm