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

## Raspberry Pi Applications

Versions of the Push Button and Switch Actuator applications are also available
for the Cascoda Border Router. These work alongside the Pimoroni Displayotron
PiHat and take advantage of its six touch buttons, LEDs and LCD display.

### Basic Border Router Set-Up

Please flash an SD card with [the latest Border Router prebuilt image available
here](https://github.com/Cascoda/install-script/releases). Insert the SD
card into a Raspberry Pi 3 or 4, attach the Displayotron HAT and then connect
the Raspberry Pi to your wired via Ethernet. Finally, power on the Raspberry Pi
by connecting the power supply cable.

### Accessing the Border Router CLI

You can configure the Border Router via SSH, through the Ethernet connection. The default
hostname is `raspberrypi`. Alternatively, you may connect a keyboard & HDMI monitor to the
Raspberry Pi and access the terminal directly that way.

Once you have access to the terminal, log in as the user `pi` using the default password,
`raspberry`

### Configuring the Raspberry Pi.

Please access the configuration menu available through `sudo raspi-config`. Once you are in,
use the arrow keys and navigate to Advanced Options, then select Expand Filesystem. Once that is
complete, you may also want to change the Password & Hostname from within the System Options menu.
If you want to configure the Push Button application to toggle the Switch Actuator, I recommend
giving the two Raspberry Pis different Hostnames so that you can SSH into them easily.

These settings will take place upon reboot, so please reboot the device when prompted.

### Building the Demo Applications

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
`avahi-utils` package using `sudo apt-get install avahi-utils`.

Finally, you may build the example applications from within the knx-iot-virtual
directory with the following set of commands:

```bash
mkdir build
cd build
cmake ../
make -j4 
```

### Running the demo applications

The Raspberry Pi Push Button application is called `knx_iot_pb_pi`. You
should see the backlight of the Displayotron HAT turn on as soon as the
application starts. The KNX functionality is identical to the
`knx_iot_virtual_pb` application, except the push button resources correspond to
the touch buttons on the Displayotron HAT.

The Switching Actuator application is called `knx_iot_sa_pi`. The KNX
resources are equivalent to `knx_iot_virtual_sa` and likewise the actuators
control the status of the six LEDs on the side of the display.
