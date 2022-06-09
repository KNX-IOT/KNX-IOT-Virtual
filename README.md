# KNX-IOT-Virtual
<!-- TOC -->

- [KNX-IOT-Virtual](#knx-iot-virtual)
  - [The Applications](#the-applications)
    - [The knx_iot_virtual_pb Application](#the-knx_iot_virtual_pb-application)
    - [knx_iot_virtual_sa Application](#knx_iot_virtual_sa-application)
  - [Building the applications on Windows](#building-the-applications-on-windows)
    - [prerequisites windows](#prerequisites-windows)
      - [perl](#perl)
      - [python](#python)
      - [installing wxWidgets on Windows](#installing-wxwidgets-on-windows)
  - [updating KNX-IOT-Virtual code base](#updating-knx-iot-virtual-code-base)
  - [The Commandline applications](#the-commandline-applications)
  - [WxWidget GUI Applications](#wxwidget-gui-applications)
    - [Push Button wxWidget GUI](#push-button-wxwidget-gui)
    - [Switch Actuator wxWidget GUI](#switch-actuator-wxwidget-gui)
    - [Override the serial number (wxWidgets only)](#override-the-serial-number-wxwidgets-only)
  - [Raspberry Pi Applications](#raspberry-pi-applications)
    - [Basic Thread Border Router Set-Up](#basic-thread-border-router-set-up)
    - [Accessing the Thread Border Router CLI](#accessing-the-thread-border-router-cli)
    - [Configuring the Raspberry Pi](#configuring-the-raspberry-pi)
    - [Building the Pi Applications](#building-the-pi-applications)
    - [Running the Pi applications](#running-the-pi-applications)

<!-- /TOC -->
## The Applications

There are 2 applications in this repo:

- knx_iot_virtual_pb Application (Push button)
- knx_iot_virtual_sa Application (Switch Actuator)

Both applications are using the [KNX IoT Point API stack](https://github.com/KNX-IOT/KNX-IOT-STACK)

The general structure of these programs are:

```
   _________________
  |    GUI CODE     |   <---- WxWidget, Python, Printf
  |_________________|
  |  POINT API CODE |   <---- Generic code handling all Point API CoAP code
  |_________________|
  |      STACK      |   <---- Generic The KNX IoT Point API Stack (other repo)
  |_________________|

  General structure
```

The Point API Code is shared code that can be used:

- as commandline application (on Linux & Windows)
- GUI application with wxWidgets (on Windows)
- as raspberry Pi application (Linux) with an displayotron pi hat

The point code has an API so that one can:

- set/retrieve data from an URL
- callback on POST data changes
- functions to figure out what type of data the url conveys

### The knx_iot_virtual_pb Application

Push Button (PB)

- serial number :  0003000

Data points:

| url  | channel/usage       | resource type | interface type |
|------| --------------------| --------------| ---------------|
| /p/1  | channel1-OnOff      | dpa.421.61    | if.s |
| /p/2  | channel1-InfoOnOff  | dpa.421.51 |  if.a |
| /p/3  | channel2-OnOff      |  dpa.421.61    | if.s |
| /p/4  | channel2-InfoOnOff  | dpa.421.51 |  if.a |
| /p/5  | channel3-OnOff      | dpa.421.61    | if.s |
| /p/6  | channel3-InfoOnOff  | dpa.421.51 |  if.a |
| /p/7  | channel4-OnOff      | dpa.421.61    | if.s |
| /p/8  | channel4-InfoOnOff  | dpa.421.51 |  if.a |

### knx_iot_virtual_sa Application

Switch Actuator (SA)

- serial number : 0004000

Data points:

| url  | channel/usage       | resource type | interface type |
|------| --------------------| --------------| ---------------|
| /p/1  | channel1-OnOff      | dpa.417.61    | if.s |
| /p/2  | channel1-InfoOnOff  | dpa.417.51 |  if.a |
| /p/3  | channel2-OnOff      |  dpa.417.61    | if.s |
| /p/4  | channel2-InfoOnOff  | dpa.417.51 |  if.a |
| /p/5  | channel3-OnOff      | dpa.417.61    | if.s |
| /p/6  | channel3-InfoOnOff  | dpa.417.51 |  if.a |
| /p/7  | channel4-OnOff      | dpa.417.61    | if.s |
| /p/8  | channel4-InfoOnOff  | dpa.417.51 |  if.a |

## Building the applications on Windows

- clone this repo
- go to the repo (cd)
```powershell
mkdir build
cd build
cmake .. -DwxWidgets_ROOT_DIR=c:/wxWidgets-3.1.5
```
  - this command retrieves the dependencies from github
- open solution (sln) created in the build folder with visual studio
- build the applications in visual studio

To use knx gitlab as source of the KNX-IOT-STACK use the following command:

```powershell
cmake .. -DwxWidgets_ROOT_DIR=c:/wxWidgets-3.1.5 -DUSE_GITLAB=true
```

Note that one has to have access to knx gitlab.

### prerequisites windows

The prerequisites are the dependencies that are needed to build the applications on Windows:

- perl
- python
- wxWidgets

#### perl

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

#### python

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

#### installing wxWidgets on Windows

download wxwidgets from (installer source code):
https://www.wxwidgets.org/downloads/

- install the contents on the recommended folder (e.g. c:\wxWidgets-3.1.5)
- build wxwidgets with visual studio:
  
  - open c:\wxWidgets-3.1.5\build\msw\wx_vc16.sln (or take the highest number available)
  - accept convert solution suggestion: convert solution to newer version studio
  - build the solution :
    - static Win32 library for Debug & Release
    - static x64 library for Debug & Release

## updating KNX-IOT-Virtual code base

Please "touch" the `CMakeLists.txt` file, then the visual studio solution will see
that the file is outdated and will do a pull of the depended projects during
the next build of `KNX-IoT-Virtual` will use the latest code from `KNX-IoT-Stack`

Note: touch on windows: please add in the file a blank somewhere and save. e.g. the save date on disk will change.

## The Commandline applications

The command line applications are:

- knx_iot_virtual_pb Application (Push button)
- knx_iot_virtual_sa Application (Switch Actuator)

The command line applications can run on Linux & Windows.
Both applications show the interaction with printfs.
The Push Button application has no means to fire a push button interaction.

## WxWidget GUI Applications

```
   _________________
  | wxWidgets(C++)  |   <---- C++ code calling C
  |_________________|     
  |   POINT CODE    |   <---- Code handling all Point API CoAP code
  |_________________|
  |      STACK      |   <---- The KNX IoT Point API Stack (other repo)
  |_________________|

  Code on windows using WxWidgets as GUI framework
```

### Push Button wxWidget GUI

The application implements:

- Button to send on/off, status bar shows what is being send (on=true or off=false)
- Toggle for InfoOnOff (readonly) shows the received info, the toggle is for viewing only

### Switch Actuator wxWidget GUI

The application implements:

- Button (readonly) to visualize the received on/off
- Toggle for Info (readonly) shows the received info, the toggle is for viewing only
- Toggle for Fault info, e.g. allows sending the received status back or always false (e.g. not active).

### Override the serial number (wxWidgets only)

The serial number can be overridden with the command line argument -s.

example (from the folder where the executable resides):

```bash
.\knx_iot_virtual_gui_pb.exe -s 0000333
.\knx_iot_virtual_gui_sa.exe -s 0000444
```

## Raspberry Pi Applications

Versions of the Push Button and Switch Actuator applications are also available for the Cascoda Thread Border Router (Raspberry Pi).
These work alongside the Pimoroni Displayotron PiHat and take advantage of its six touch buttons, LEDs and LCD display.

The applications are:

- knx_iot_pb_pi.c (push button)
- knx_iot_sa_pi.c (switch actuator)

```
   _________________
  |   C  |  Python  |   <---- C code calling python
  |_________________|     
  |     POINT CODE  |   <---- Code handling all Point API CoAP code
  |_________________|
  |     STACK       |   <---- The KNX IoT Point API Stack (other repo)
  |_________________|

  Code on the Pi
```

info about Pimoroni Displayotron hat :

https://pinout.xyz/pinout/display_o_tron_hat?msclkid=02fa4484c6d511ecaaaf64d47a2d5e81

https://github.com/pimoroni/displayotron

The main differences wrt to wxWidget apps are:

- different main application
- interaction via python to listen to the buttons (pb)
  - polling python interaction
- interaction via python to turn on/off the leds (sa & pb)
  - via callback of the point api code (on CoAP POST)

### Basic Thread Border Router Set-Up

Please flash an SD card with [the latest Border Router prebuilt image available
here](https://github.com/Cascoda/install-script/releases). Insert the SD
card into a Raspberry Pi 3 or 4, attach the Displayotron HAT and then connect
the Raspberry Pi to your wired via Ethernet. Finally, power on the Raspberry Pi by connecting the power supply cable.

### Accessing the Thread Border Router CLI

You can configure the Thread Border Router via SSH, through the Ethernet connection. The default
hostname is `raspberrypi`. Alternatively, you may connect a keyboard & HDMI monitor to the
Raspberry Pi and access the terminal directly that way.

Once you have access to the terminal, log in as the user `pi` using the default password,
`raspberry`

### Configuring the Raspberry Pi

Please access the configuration menu available through `sudo raspi-config`. Once you are in,
use the arrow keys and navigate to Advanced Options, then select Expand Filesystem. Once that is
complete, you may also want to change the Password & Hostname from within the System Options menu.
If you want to configure the Push Button application to toggle the Switch Actuator, I recommend
giving the two Raspberry Pis different Hostnames so that you can SSH into them easily.

These settings will take place upon reboot, so please reboot the device when prompted.

### Building the Pi Applications

Obtain a clone of this repository using `git clone
https://github.com/KNX-IOT/knx-iot-virtual`. If you see a login prompt, use your
GitHub username and a personal access token with "repo" access scope.

Since personal access tokens are difficult to keep track of, you may want to
tell git to remember the access tokens you use using `git config --global
credential.helper store`. The credentials will be stored on the SD card in
plaintext, within `~/.git-credentials`.

Once you have obtained the repository, you must install the Python drivers
for the Displayotron HAT, by running `./displayotron-install.sh` within
the knx-iot-virtual folder.

In order to enable DNS-SD Discovery, you will also need to install the
`avahi-utils` package. `python3-dev` is needed for the communication between C
and Python. Obtain these dependencies using `sudo apt-get install avahi-utils
python3-dev`.

Finally, you may build the example applications from within the knx-iot-virtual
directory with the following set of commands:

```bash
mkdir build
cd build
cmake ../
make -j4 
```

### Running the Pi applications

The Raspberry Pi Push Button application is called `knx_iot_pb_pi`. You
should see the backlight of the Displayotron HAT turn on as soon as the
application starts. The KNX functionality is identical to the
`knx_iot_virtual_pb` application, except the push button resources correspond to
the touch buttons on the Displayotron HAT.

The Switching Actuator application is called `knx_iot_sa_pi`. The KNX
resources are equivalent to `knx_iot_virtual_sa` and likewise the actuators
control the status of the six LEDs on the side of the display.
