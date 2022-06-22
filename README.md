![Linux](https://github.com/KNX-IOT/KNX-IOT-Virtual/actions/workflows/cmake-linux.yml/badge.svg)
![sync](https://github.com/KNX-IOT/KNX-IOT-Virtual/actions/workflows/gitlabsync.yml/badge.svg)

# 1. KNX-IOT-Virtual

<!-- TOC -->

- [1. KNX-IOT-Virtual](#1-knx-iot-virtual)
  - [1.1. The Applications](#11-the-applications)
    - [1.1.1. The knx_iot_virtual_pb Application](#111-the-knx_iot_virtual_pb-application)
    - [1.1.2. knx_iot_virtual_sa Application](#112-knx_iot_virtual_sa-application)
  - [1.2. Updating KNX-IOT-Virtual code base](#12-updating-knx-iot-virtual-code-base)
    - [GitHub access](#github-access)
    - [GitLab access](#gitlab-access)
  - [1.1.3. The Commandline applications](#113-the-commandline-applications)
  - [1.3. WxWidget GUI Applications (Windows)](#13-wxwidget-gui-applications-windows)
    - [1.3.1. Push Button wxWidget GUI](#131-push-button-wxwidget-gui)
    - [1.3.2. Switch Actuator wxWidget GUI](#132-switch-actuator-wxwidget-gui)
    - [1.3.3. Override the serial number (wxWidgets only)](#133-override-the-serial-number-wxwidgets-only)
    - [1.3.4. Building the applications on Windows](#134-building-the-applications-on-windows)
      - [1.3.4.1. Running the application on windows](#1341-running-the-application-on-windows)
        - [1.3.4.1.1. Python utility `restart_app.py`](#13411-python-utility-restart_apppy)
    - [1.3.5. Prerequisites windows](#135-prerequisites-windows)
      - [1.3.5.1. perl](#1351-perl)
      - [1.3.5.2. python](#1352-python)
      - [1.3.5.3. Installing wxWidgets on Windows](#1353-installing-wxwidgets-on-windows)
  - [1.4. Raspberry Pi Applications](#14-raspberry-pi-applications)
    - [1.4.1. Basic Thread Border Router Set-Up](#141-basic-thread-border-router-set-up)
    - [1.4.2. Accessing the Thread Border Router CLI](#142-accessing-the-thread-border-router-cli)
    - [1.4.3. Configuring the Raspberry Pi](#143-configuring-the-raspberry-pi)
    - [1.4.4. Building the Pi Applications](#144-building-the-pi-applications)
    - [1.4.5. Running the Pi applications](#145-running-the-pi-applications)

<!-- /TOC -->

## 1.1. The Applications

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

### 1.1.1. The knx_iot_virtual_pb Application

Push Button (PB)

- serial number :  0003000

Data points:

| url  | channel/usage       | instance |resource type | interface type | data type |
|------| --------------------| -------- | -------------| ---------------|-----------|
| "/p/o_1_1"  | OnOff_1 |  1 |urn:knx:dpa.421.61 | if.s |bool |
| "/p/o_2_2"  | InfoOnOff_1 |  1 |urn:knx:dpa.421.51 | if.a |bool |
| "/p/o_3_3"  | OnOff_2 |  2 |urn:knx:dpa.421.61 | if.s |bool |
| "/p/o_4_4"  | InfoOnOff_2 |  2 |urn:knx:dpa.421.51 | if.a |bool |
| "/p/o_5_5"  | OnOff_3 |  3 |urn:knx:dpa.421.61 | if.s |bool |
| "/p/o_6_6"  | InfoOnOff_3 |  3 |urn:knx:dpa.421.51 | if.a |bool |
| "/p/o_7_7"  | OnOff_4 |  4 |urn:knx:dpa.421.61 | if.s |bool |
| "/p/o_8_8"  | InfoOnOff_4 |  4 |urn:knx:dpa.421.51 | if.a |bool |

### 1.1.2. knx_iot_virtual_sa Application

Switch Actuator (SA)

- serial number : 0004000

Data points:

| url  | channel/usage       | instance |resource type | interface type | data type |
|------| --------------------| -------- | -------------| ---------------|-----------|
| "/p/o_1_1"  | OnOff_1 |  1 |urn:knx:dpa.417.61 | if.a |bool |
| "/p/o_2_2"  | InfoOnOff_1 |  1 |urn:knx:dpa.417.51 | if.s |bool |
| "/p/o_3_3"  | OnOff_2 |  2 |urn:knx:dpa.417.61 | if.a |bool |
| "/p/o_4_4"  | InfoOnOff_2 |  2 |urn:knx:dpa.417.51 | if.s |bool |
| "/p/o_5_5"  | OnOff_3 |  3 |urn:knx:dpa.417.61 | if.a |bool |
| "/p/o_6_6"  | InfoOnOff_3 |  3 |urn:knx:dpa.417.51 | if.s |bool |
| "/p/o_7_7"  | OnOff_4 |  4 |urn:knx:dpa.417.61 | if.a |bool |
| "/p/o_8_8"  | InfoOnOff_4 |  4 |urn:knx:dpa.417.51 | if.s |bool |

## 1.2. Updating KNX-IOT-Virtual code base

Please "touch" the `CMakeLists.txt` file, then the visual studio solution will see
that the file is outdated and will do a pull of the depended projects during
the next build of `KNX-IoT-Virtual` will use the latest code from `KNX-IoT-Stack`

Note: touch on windows: please add in the file a blank somewhere and save. e.g. the save date on disk will change.

### GitHub access

Obtain a clone of this repository using `git clone https://github.com/KNX-IOT/knx-iot-virtual`.
If you see a login prompt, use your GitHub username and a personal access token with "repo" access scope.

Since personal access tokens are difficult to keep track of, you may want to tell git to remember the access tokens you use using `git config global credential.helper store`. The credentials will be stored on the SD card in plaintext, within `~/.git-credentials`.

### GitLab access

Obtain a clone of this repository using `git clone https://github.com/KNX-IOT/knx-iot-virtual`.
If you see a login prompt, use your GitLab username and a personal access token with "repo" access scope.

Since personal access tokens are difficult to keep track of, you may want to tell git to remember the access tokens you use using `git config global credential.helper store`. The credentials will be stored on the SD card in plaintext, within `~/.git-credentials`.

## 1.1.3. The Commandline applications

The command line applications are:

- knx_iot_virtual_pb Application (Push button)
- knx_iot_virtual_sa Application (Switch Actuator)

The command line applications can run on Linux & Windows.
Both applications show the interaction with printfs.
The Push Button application has no means to fire a push button interaction.

## 1.3. WxWidget GUI Applications (Windows)

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

### 1.3.1. Push Button wxWidget GUI

The application implements:

- Button to send on/off, status bar shows what is being send (on=true or off=false)
- Toggle for InfoOnOff (readonly) shows the received info, the toggle is for viewing only

### 1.3.2. Switch Actuator wxWidget GUI

The application implements:

- Button (readonly) to visualize the received on/off
- Toggle for Info (readonly) shows the received info, the toggle is for viewing only
- Toggle for Fault info, e.g. allows sending the received status back or always false (e.g. not active).

### 1.3.3. Override the serial number (wxWidgets only)

The serial number can be overridden with the command line argument -s.

example (from the folder where the executable resides):

```bash
.\knx_iot_virtual_gui_pb.exe -s 0000333
.\knx_iot_virtual_gui_sa.exe -s 0000444
```

### 1.3.4. Building the applications on Windows

1. clone this repo

   ```powershell
   # clone the repo from GitHub
   git clone https://github.com/KNX-IOT/KNX-IOT-Virtual.git
   # go into the created folder
   cd KNX-IOT-Virtual
   ```

   ```powershell
   # clone the repo from GitLab
   git clone https://gitlab.knx.org/shared-projects/knx-iot-virtual.git
      # go into the created folder
   cd knx-iot-virtual
   cd 
   ```

1. Create the make infra structure.

   ```powershell
   mkdir build
   cd build
   cmake .. -DwxWidgets_ROOT_DIR=c:/wxWidgets-3.1.5
   ```

Note: The above set of commands is already in `build.sh`.

1. open solution (sln) created in the build folder with visual studio
  
   ```powershell
   # clone the repo
   cd build
   # open knx-virtual.sln in visual studio
   ```

1. build the applications in visual studio
  
   Note: build the release version.

To use knx gitlab as source of the KNX-IOT-STACK use the following command:

```powershell
cmake .. -DwxWidgets_ROOT_DIR=c:/wxWidgets-3.1.5 -DUSE_GITLAB=true
```

#### 1.3.4.1. Running the application on windows

The applications do not have a mechanims to restart themselves.
So if the applications are started from the commmand line, then the `restart` command from ETS will just shut down the application and will not restart the application.

##### 1.3.4.1.1. Python utility `restart_app.py`

This utility can start an application, and if the application quits it will restart the application automatically.

Note: Please stop the process by doing `Ctrl-C` in the window where python command is started.
The `Ctrl-C` is only handled when the application stops, e.g. `Ctrl-C` won't be honored when the application is running. 

### 1.3.5. Prerequisites windows

The prerequisites are the dependencies that are needed to build the applications on Windows:

- perl
- python
- wxWidgets

#### 1.3.5.1. perl

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

#### 1.3.5.2. python

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

#### 1.3.5.3. Installing wxWidgets on Windows

Download wxwidgets from (installer source code):
https://www.wxwidgets.org/downloads/

- Install the contents on the recommended folder (e.g. c:\wxWidgets-3.1.5)
- Build wxwidgets with visual studio:
  
  - Open c:\wxWidgets-3.1.5\build\msw\wx_vc16.sln (or take the highest number available)
  - Accept convert solution suggestion: convert solution to newer version studio
  - Build the solution :
    - static Win32 library for Debug & Release
    - static x64 library for Debug & Release

## 1.4. Raspberry Pi Applications

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

Info about Pimoroni Displayotron hat :

https://pinout.xyz/pinout/display_o_tron_hat?msclkid=02fa4484c6d511ecaaaf64d47a2d5e81

https://github.com/pimoroni/displayotron

The main differences wrt to wxWidget apps are:

- Different main application
- Interaction via python to listen to the buttons (pb)
  - Polling python interaction
- Interaction via python to turn on/off the leds (sa & pb)
  - via callback of the point api code (on CoAP POST)

### 1.4.1. Basic Thread Border Router Set-Up

Please flash an SD card with [the latest Border Router prebuilt image available
here](https://github.com/Cascoda/install-script/releases). Insert the SD
card into a Raspberry Pi 3 or 4, attach the Displayotron HAT and then connect
the Raspberry Pi to your wired via Ethernet. Finally, power on the Raspberry Pi by connecting the power supply cable.

### 1.4.2. Accessing the Thread Border Router CLI

You can configure the Thread Border Router via SSH, through the Ethernet connection. The default
hostname is `raspberrypi`. Alternatively, you may connect a keyboard & HDMI monitor to the
Raspberry Pi and access the terminal directly that way.

Once you have access to the terminal, log in as the user `pi` using the default password,
`raspberry`

### 1.4.3. Configuring the Raspberry Pi

Please access the configuration menu available through `sudo raspi-config`. Once you are in,
use the arrow keys and navigate to Advanced Options, then select Expand Filesystem. Once that is
complete, you may also want to change the Password & Hostname from within the System Options menu.
If you want to configure the Push Button application to toggle the Switch Actuator, I recommend
giving the two Raspberry Pis different Hostnames so that you can SSH into them easily.

These settings will take place upon reboot, so please reboot the device when prompted.

### 1.4.4. Building the Pi Applications

Obtain a clone of this repository using git.
e.g. the resulting action should be a folder `knx-iot-virtual`

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

The applications should be available in the `knx-iot-virtual/bin`` folder.

### 1.4.5. Running the Pi applications

The Raspberry Pi Push Button application is called `knx_iot_pb_pi`. You
should see the backlight of the Displayotron HAT turn on as soon as the
application starts. The KNX functionality is identical to the
`knx_iot_virtual_pb` application, except the push button resources correspond to
the touch buttons on the Displayotron HAT.

The Switching Actuator application is called `knx_iot_sa_pi`. The KNX
resources are equivalent to `knx_iot_virtual_sa` and likewise the actuators
control the status of the six LEDs on the side of the display.

Once one of the applications has been started, the device can commissioned.
