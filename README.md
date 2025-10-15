# PulseMaker
### Arduino pseudorandom simulation of counts from a Geiger counter

## How (I think) it works
At fixed intervals, rapid w.r.t. the sampling period, the main loop generates the next pseudorandom 32-bit integer _p_ and compares it to 
a 32-bit threshold integer _q_. 
If _p_ _<_ _q_, then fire a pulse. 
_q_ is chosen empirically to give an average number of pulses. A larger _q_ produces more frequent pulses (exact relationship TBD). 
The generation is continuous, as if the counts are from a sample with a large number of atoms and a half-life much longer than the 
observation period. For consistency in testing, the averaging period should be 1 minute.

The 1ms wide pulses are H-L-H, same as the pulses from the high voltage module in Safecast bGeigie instruments (Safepulse or Medcom Intl. iRover). 
Along with the pulse, the onboard Arduino LED (typically on pin D13) is flashed briefly.

The momentary contact pushbutton toggles between low and high pulse rate. Eventually, this should be a more versatile control.

## Building
This should build on 32-bit Arduino platforms. This version was specifically tested on Adafruit ItsyBitsy M4 Express (ATSAMD51) with 
Visual Studio Code and the PlatformIO environment for embedded systems. 
Changing the pin numbering should be straightforward. The SFMT documentation warns that the CPU must be little-endian. Although 32-bit ARM
CPUs can be configured either little- or big-endian, it seems that Arduino uses little-endian.

The pseudorandom implementation is on branch `prandom`. The main branch (for now, at least) has the previous fixed interval pulse rate.

## Credits
* SFMT - SIMD-oriented Fast Mersenne Twister implementation by Mutsuo Saito and Makoto Matsumoto in 2006 (BSD License).
  https://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/SFMT/index.html (See subdirectory sfmt.)
* RBD_Timer - Arduino library by Alex Taujenis (2015, MIT License).
* Debouncer - Algorithm by Jack Ganssle (https://www.ganssle.com/debouncing.htm), implementation by Trent Cleghorn (2014, MIT license).

## Licence

With the exceptions of the packages mentioned above, this software is licensed under the 2-clause BSD Licence (a.k.a. Simplified BSD Licence).

<blockquote>Copyright (c) 2025 Louis Bertrand

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
</blockquote>
