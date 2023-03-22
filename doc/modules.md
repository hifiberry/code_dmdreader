# Modules

DMDReader is completely modular. It does not have a pre-defined functionality. Everything is handled by 3 types of modules:

- Sources
- Processors
- Renderers

This documentation might not always be up-to-date. To check what sources, processors and renderers are available in the latest version, have a look at (../util/objectfactory.cpp)

## Sources

Sources are - as the name suggests - sources for DMD data. Your configuration have to define at least one source. If you define multiple sources, 
they will be processed sequentially: all frames from the firs sources will be uses, then it will start with the seconds source and so on with all sources

### dat

Reads frame from a DAT file. This has been used in initial stated of the development, but it's recommended to uses the txt source now.

|Parameter|Description|
|---|---|
|name|Name of the file to read (relative to the directory of the configuration file|

### txt

Reads the frames from a TXT file. This format is also used on VPins for colorisations.  

|Parameter|Description|
|---|---|
|name|Name of the file to read (relative to the directory of the configuration file|
|bitsperpixel|Bits/pixel. Can be 2,3 or 4 - depending on the source (e.g. 2 bit for WPC, 4 bit for SAM and Spike 1)
|use_timing_data|Sent frames according to the timestamps in the file. If this is set to false, frames are just being delivered as fast as possible
|frame_every_ms|Sent a frame every x milliseconds, even if there is no data in the file. If there is no data for the given timestamp, the previous frame is sent again

Note that when using "frame_every_ms", you should also set skip_unmodified_frames to false in the general settoings. Otherwise, 
the duplicate frames generated by this module will just be supressed again by the main processing loop.
### png

Reads PNG files, each representing a single frame

|Parameter|Description|
|---|---|
|files|A list of files (you can use wildcards) to use. Ideally all these have the same resolution. It's also recommended to only use files in usual DMD resolutions, e.g. 128x32)||


### spi

Reads frames directly from a pinball's DMD interface. An interface board is needed for this

|Parameter|Description|
|---|---|
|device|The SPI device file to use, e.g. /dev/spidev1.0|
|speed|SPI clock speed|1000000 = 1MHz should be enough|
|notify_gpio|GPIO pin the notify signal is connected to (e.g. 7)|

## null

A source that delivers no frames at all. Only useful for debugging purposes. 

## Processors

Processors are the backbone of the system. They can do all kinds of stuff. A processor just takes a frame, does something with in and returns a result frame. The result frame can be the same 
as the input frame (e.g. if it's just used for triggering some action), but it can also be totally different. Examples are resizing, re-coloring, but a processor isn't limited to this.

### frameinfo

Logs some information on the frame to the console. Doesn't change the frame.

### pupcapture

Uses pupcapture files (usually used in VPins) to send triggers.

TODO: Document usage

### serum

Uses a SERUM colorisation and apply it to the frames.

|Parameter|Description|
|---|---|
|file|The colorisation file. Can be an uncomrpessed .cROM file or a compressed .cRZ file|
|ignoreUnknownFramesTimesout|Timeout (ins ms) after that frames will not be colored if there is no matching colorisation|

The coloriser will only process uncolored frames (max 8bits/pixel). If a frame can't be colored (e.g. if therer is no matching colorisation for it), it will be returned unprocessed.
To deal with these you can e.g. add a "palette" colorizer after applying serum.

#### About "ignoreUnknownFramesTimesout"

Some colorisations don't colorise each available frame, but repeat one frame multiple times. This works as follows: A colorisation is looked up. If a match is found, 
the frame is being colorised. If no match is found, the previous frame is just repeated for a maximum of ignoreUnknownFramesTimesout milliseconds. After this timeout has passed, it won't
return colored frames until a new match has been found.

### palette

Color the frame with a fixed palette. Default is some orange color gradient that is similar to old-school DMDs.

|Parameter|Description|
|---|---|
|colors|Number of colors - set this to the number of colors your source provides, e.g. 4 for WPC or 16 for SAM|
|red|Red component of the brightest color|
|green|Green component of the brightest color|
|blue|Blue component of the brightest color|

### txtwriter

Doesn't process the frame, but write it to a TXT file. This processor only handled uncolored frames up to 4bit/pixel as the TXT format is only designed for this. 

|Parameter|Description|
|---|---|
|filename|file to write|
|ignore_duplicates|Only include a frame once. If a frame has been seen before, it won't be written into the file again|
|async|Do not write the frames to the file when they are received, but when program finishes (either the source has no more frames or the program is gracefully terminated). This is recommended especially on the Raspberry Pi as I/O performance might be |

### pngwriter

TODO 
 
### patterndetector

TODO 

### statedetector

TODO 

### upscale

Increases the resolution of a DMDFrame. A usual use case is displaying 128x32 DMD content on a 256x64 display - upscaling it by factor 2
It uses XBR or HQ upscaling - these algorithms are specifically designed for line art.

|Parameter|Description|
|---|---|
|upscaler|Upscaler to use. This can by hq2x, hq3x, hqx4, xbr2x, xbr3x, xbr4x|


## Renderers

### null

Does nothing, just discards frames

### opengl
|Parameter|Description|
|---|---|
|width|width of the screen or windows to display on|
|height|height of the screen or windows to display on|
|dmd_x|x coordinate of the top-left corner of the DMD display (default 0)|
|dmd_y|y coordinate of the top-left corner of the DMD display (default 0)|
|dmd_width|width of the DMD display|
|dmd_height|width of the DMD display|
|overlay_texture|overlay a texture on each pixel (e.g. to render round pixels)|
|scale_linear|use bilinear upscaling of the DMD image|
|fragment_shader|fragment shader to use, check out the shader directory for the available fragment shaders|

### ledmatrix

Control an LED matrix connected to the DMDReader hardware. This is based on https://github.com/hzeller/rpi-rgb-led-matrix
Note that dmdreader have to run as root to use this.

|Parameter|Description|
|---|---|
|width|width of the LED matrix (columns)|
|height|height of the LED matrix (rows)|
|pwm_bits|PWM bits per pixel (default 8, can be 1-11)|

