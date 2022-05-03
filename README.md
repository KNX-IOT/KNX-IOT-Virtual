# KNX-IOT-Virtual

## virtual applications

These applications are meant to run on Windows 10.

## building the applications on Windows

- clone this repo
- go to the repo (cd)
- mkdir build
- cmake .. -DwxWidgets_ROOT_DIR=c:/wxWidgets-3.1.5
- open solution (sln) created in the build folder with visual studio
- build the applications in visual studio

## prerequisites

The prerequisites are the dependencies that are needed to build the applications on windows:

- perl
- python
- wxWidgets

### perl

Building (e.g. configuring wxWidgets with Cmake) requires perl
If perl is not installed then install it via a Windows installer available at:

https://www.perl.org/get.html

To check if perl is installed:

```bash
# do in a bash window
which perl
# result should be
# /usr/bin/perl
```

### python

Building (e.g. configuring wxWidgets with CMake) requires python

If python is not installed then install it via a Windows installer available at:

https://www.python.org/downloads/

To check if python is installed:

```bash
# do in a bash window
which python
# result should be
# <some path>/python
```

### installing wxWidgets on Windows

download wxwidgets from (installer source code):
https://www.wxwidgets.org/downloads/

- install the contents on the recommended folder (e.g. c:\wxWidgets-3.1.5)
- build wxwidgets with visual studio:
  
  - open c:\wxWidgets-3.1.5\build\msw\wx_vc16.sln (or take the highest number available)
  - accept convert solution suggestion: convert solution to newer version studio
  - build the solution :
    - static Win32 library for Debug & Release
    - static x64 library for Debug & Release

## Building the applications on Windows

- clone this repo
- go to the repo (cd)
- mkdir build
- cmake .. -DwxWidgets_ROOT_DIR=c:/wxWidgets-3.1.5
  - this command retrieves the dependencies from github
- open solution (sln) created in the build folder with visual studio
- build the applications in visual studio

### updating KNX-IOT-Virtual

please use recursive pull request:

git pull --recurse-submodules

updating the knx-iot-stack in knx-iot virtual:

- close all windows, including visual studio
- remove (or clean) the folder build
- redo the cmake command listed above

## knx_iot_virtual_pb

Push Button (PB)

- serial number :  0003000

Data points:

| url  | channel/usage       | resource type | interface type |
|------| --------------------| --------------| ---------------|
| p/1  | channel1-OnOff      | dpa.421.61    | if.s |
| p/2  | channel1-InfoOnOff  | dpa.421.51 |  if.a |
| p/3  | channel2-OnOff      |  dpa.421.61    | if.s |
| p/4  | channel2-InfoOnOff  | dpa.421.51 |  if.a |
| p/5  | channel3-OnOff      | dpa.421.61    | if.s |
| p/6  | channel3-InfoOnOff  | dpa.421.51 |  if.a |
| p/7  | channel4-OnOff      | dpa.421.61    | if.s |
| p/8  | channel4-InfoOnOff  | dpa.421.51 |  if.a |

### Push Button GUI

The application implements:

- Button to send on/off, status bar shows what is being send (on=true or off=false)
- Toggle for Info (readonly) shows the received info, the toggle is for viewing only

## knx_iot_virtual_sa

Switch Actuator (SA)

- serial number : 0004000

Data points:

| url  | channel/usage       | resource type | interface type |
|------| --------------------| --------------| ---------------|
| p/1  | channel1-OnOff      | dpa.417.61    | if.s |
| p/2  | channel1-InfoOnOff  | dpa.417.51 |  if.a |
| p/3  | channel2-OnOff      |  dpa.417.61    | if.s |
| p/4  | channel2-InfoOnOff  | dpa.417.51 |  if.a |
| p/5  | channel3-OnOff      | dpa.417.61    | if.s |
| p/6  | channel3-InfoOnOff  | dpa.417.51 |  if.a |
| p/7  | channel4-OnOff      | dpa.417.61    | if.s |
| p/8  | channel4-InfoOnOff  | dpa.417.51 |  if.a |

### Switch Actuator GUI

The application implements:

- Button (readonly) to visualize the received on/off
- Toggle for Info (readonly) shows the received info, the toggle is for viewing only
- Toggle for Fault info, e.g. allows sending the received status back or always false (e.g. not active).

## override the serial number

the serial number can be overridden with the command line argument -s.

example (from the folder where the executable resides):

```bash
.\knx_iot_virtual_gui_pb.exe -s 0000333
```
