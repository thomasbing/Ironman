nvsf Sample Application
-----------------------------

Overview
--------

Mixes a collection of 32 bit float mono PCM input audio files into a single stereo 32 bit float
output audio file. All the input files should have same sample rate; the output will have the same
sample rate. The amount of stereo spread effect is controlled with the "stereSpread" parameter
and a gain can be applied on the mixed output using "outputGain" parameter.

Usage
-----

nvsf_sample.exe --help
                --content_dir <Directory containing input pcm files named input1.pcm, input2.pcm,... with trailing '\'>
                --num_inputs <number of input files (must be greater than zero)>
                --sample_rate <samples per second of input files>
                --stereo_spread <Stereo Spread effect from 0.0 (off) to 1.0 (max)>
                --output_gain <gain applied to output. From 0.0 (silence) and up higher>
                --output_file <output file name>

Example
-------

nvsf_sample.exe --content_dir ..\..\footage\ --num_inputs 2 --sample_rate 48000 --stereo_spread 0.75 --output_gain 1.5 --output_file output.pcm

run.bat
-------

Creates a stereo spread output audio file using the sample input audio files in ..\..\footage directory with the low level audio SDK. 
The result will be available in ..\..\samples\nvsf_sample\output.pcm