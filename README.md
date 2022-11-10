![Linux](https://github.com/KNX-IOT/KNX-IOT-Virtual/actions/workflows/cmake-linux.yml/badge.svg)
![sync](https://github.com/KNX-IOT/KNX-IOT-Virtual/actions/workflows/gitlabsync.yml/badge.svg)

# 1. KNX-IOT-Virtual

<!-- TOC -->

- [1. KNX-IOT-Virtual](#1-knx-iot-virtual)
  - [1.1. The Applications](#11-the-applications)
    - [1.1.1. The knx_iot_virtual_pb Application](#111-the-knx_iot_virtual_pb-application)
      - [1.1.1.1. MetaData push button](#1111-metadata-push-button)
    - [1.1.2. knx_iot_virtual_sa Application](#112-knx_iot_virtual_sa-application)
      - [1.1.2.1. MetaData switch actuator](#1121-metadata-switch-actuator)
  - [1.2. Updating KNX-IOT-Virtual code base](#12-updating-knx-iot-virtual-code-base)
    - [1.2.1. GitHub access](#121-github-access)
    - [1.2.2. GitLab access](#122-gitlab-access)
  - [1.3. The Commandline applications](#13-the-commandline-applications)
  - [1.4. WxWidget GUI Applications (Windows)](#14-wxwidget-gui-applications-windows)
    - [1.4.1. Push Button wxWidget GUI](#141-push-button-wxwidget-gui)
    - [1.4.2. Switch Actuator wxWidget GUI](#142-switch-actuator-wxwidget-gui)
    - [1.4.3. Override the serial number (wxWidgets GUI apps only)](#143-override-the-serial-number-wxwidgets-gui-apps-only)
    - [1.4.4. Building the applications on Windows](#144-building-the-applications-on-windows)
      - [1.4.4.1. Running the application on windows](#1441-running-the-application-on-windows)
        - [1.4.4.1.1. Python utility `restart_app.py`](#14411-python-utility-restart_apppy)
    - [1.4.5. Prerequisites windows](#145-prerequisites-windows)
      - [1.4.5.1. Visual Studio](#1451-visual-studio)
      - [1.4.5.1. git](#1451-git)
      - [1.4.5.2. perl](#1452-perl)
      - [1.4.5.3. python](#1453-python)
      - [1.4.5.4. Cmake](#1454-cmake)
      - [1.4.5.5. Installing wxWidgets on Windows](#1455-installing-wxwidgets-on-windows)
  - [1.5. Installation tools](#16-installation-tools)

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

The point code has an API so that one can:

- set/retrieve data from an URL
- callback on POST data changes
- functions to figure out what type of data the url conveys

### 1.1.1. The knx_iot_virtual_pb Application

Push Button (PB)

- serial number :  00FA10010400

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

#### 1.1.1.1. MetaData push button

The mandatory metadata parameters per data points implemented:

- id (9) - the unique url
- rt - the resource type
- if - the interface type
- dpt - the data point type
- ga - the array of group addresses (if initialized)

next to the mandatory metadata fields the following datapoint specific metadata fields are implemented

| url  | name   | metadata tag | metadata value |
|------| ----------------| ---------| --------- |
| "/p/o_1_1"  | OnOff_1 |  desc |On/Off push button 1 |
| "/p/o_2_2"  | InfoOnOff_1 |  desc |Feedback 1 |
| "/p/o_3_3"  | OnOff_2 |  desc |On/Off push button 2 |
| "/p/o_4_4"  | InfoOnOff_2 |  desc |Feedback 2 |
| "/p/o_5_5"  | OnOff_3 |  desc |On/Off push button 3 |
| "/p/o_6_6"  | InfoOnOff_3 |  desc |Feedback 3 |
| "/p/o_7_7"  | OnOff_4 |  desc |On/Off push button 4 |
| "/p/o_8_8"  | InfoOnOff_4 |  desc |Feedback 4 |

for querying the metadata items implemented one can use the following commands:

```
  GET /p/o_1_1?m=*
  GET /p/o_2_2?m=*
  GET /p/o_3_3?m=*
  GET /p/o_4_4?m=*
  GET /p/o_5_5?m=*
  GET /p/o_6_6?m=*
  GET /p/o_7_7?m=*
  GET /p/o_8_8?m=*
```

### 1.1.2. knx_iot_virtual_sa Application

Switch Actuator (SA)

- serial number : 00FA10010700

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

#### 1.1.2.1. MetaData switch actuator

The mandatory metadata parameters per data points implemented:

- id (9) - the unique url
- rt - the resource type
- if - the interface type
- dpt - the data point type
- ga - the array of group addresses (if initialized)

next to the mandatory metadata fields the following datapoint specific metadata fields are implemented

| url  | name   | metadata tag | metadata value |
|------| ----------------| ---------| --------- |
| "/p/o_1_1"  | OnOff_1 |  desc |On/Off switch 1 |
| "/p/o_2_2"  | InfoOnOff_1 |  desc |Feedback 1 |
| "/p/o_3_3"  | OnOff_2 |  desc |On/Off switch 2 |
| "/p/o_4_4"  | InfoOnOff_2 |  desc |Feedback 2 |
| "/p/o_5_5"  | OnOff_3 |  desc |On/Off switch 3 |
| "/p/o_6_6"  | InfoOnOff_3 |  desc |Feedback 3 |
| "/p/o_7_7"  | OnOff_4 |  desc |On/Off switch 4 |
| "/p/o_8_8"  | InfoOnOff_4 |  desc |Feedback 4 |

for querying the metadata items implemented one can use the following commands:

```
  GET /p/o_1_1?m=*
  GET /p/o_2_2?m=*
  GET /p/o_3_3?m=*
  GET /p/o_4_4?m=*
  GET /p/o_5_5?m=*
  GET /p/o_6_6?m=*
  GET /p/o_7_7?m=*
  GET /p/o_8_8?m=*
```

## 1.2. Updating KNX-IOT-Virtual code base

Please "touch" the `CMakeLists.txt` file, then the visual studio solution will see
that the file is outdated and will do a pull of the depended projects during
the next build of `KNX-IoT-Virtual` will use the latest code from `KNX-IoT-Stack`

Note: touch on windows: please add in the file a blank somewhere and save. e.g. the save date on disk will change.

### 1.2.1. GitHub access

Obtain a clone of this repository using `git clone https://github.com/KNX-IOT/knx-iot-virtual`.
If you see a login prompt, use your GitHub username and a personal access token with "repo" access scope.

Since personal access tokens are difficult to keep track of, you may want to tell git to remember the access tokens you use using `git config global credential.helper store`. The credentials will be stored on the SD card in plaintext, within `~/.git-credentials`.

### 1.2.2. GitLab access

Obtain a clone of this repository using `git clone https://github.com/KNX-IOT/knx-iot-virtual`.
If you see a login prompt, use your GitLab username and a personal access token with "repo" access scope.

Since personal access tokens are difficult to keep track of, you may want to tell git to remember the access tokens you use using `git config global credential.helper store`. The credentials will be stored on the SD card in plaintext, within `~/.git-credentials`.

## 1.3. The Commandline applications

The command line applications are:

- knx_iot_virtual_pb Application (Push button)
- knx_iot_virtual_sa Application (Switch Actuator)

The command line applications can run on Linux & Windows.
Both applications show the interaction with printfs.
The Push Button application has no means to fire a push button interaction.

## 1.4. WxWidget GUI Applications (Windows)

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

### 1.4.1. Push Button wxWidget GUI

The application implements:

- Button to send on/off, status bar shows what is being send (on=true or off=false)
- Toggle for InfoOnOff (readonly) shows the received info, the toggle is for viewing only

### 1.4.2. Switch Actuator wxWidget GUI

The application implements:

- Button (readonly) to visualize the received on/off
- Toggle for Info (readonly) shows the received info, the toggle is for viewing only
- Toggle for Fault info, e.g. allows sending the received status back or always false (e.g. not active).

### 1.4.3. Override the serial number (wxWidgets GUI apps only)

The serial number can be overridden with the command line argument -s.

Example (from the folder where the executable resides):

```bash
.\knx_iot_virtual_gui_pb.exe -s 00FA10010401
.\knx_iot_virtual_gui_sa.exe -s 00FA10010401
```

### 1.4.4. Building the applications on Windows

1. Install all prerequisits:
   - visual studio (C++)
   - git
   - python
   - perl
   - cmake
   - wxwidgets

2. Steps using GitHub

   - Clone the repo
   - Go to the created folder
   - Make build folder
   - Issue cmake command to create solution

   ```powershell
   # clone the repo from GitHub
   git clone https://github.com/KNX-IOT/KNX-IOT-Virtual.git
   # go into the created folder
   cd KNX-IOT-Virtual
   mkdir build
   cd build
   # use the wxWidgets folder on your machine
   cmake .. -DwxWidgets_ROOT_DIR=c:/wxWidgets-3.1.5
   ```

   Note: The above set of commands is already in `build.sh`.

3. Steps using GitLab

   - Clone the repo
   - Go to the created folder
   - Make build folder
   - Issue cmake command to create solution

   ```powershell
   # clone the repo from GitLab
   git clone https://gitlab.knx.org/shared-projects/knx-iot-virtual.git
      # go into the created folder
   cd knx-iot-virtual
   mkdir build
   cd build 
   # use the wxWidgets folder on your machine
   cmake .. -DwxWidgets_ROOT_DIR=c:/wxWidgets-3.1.5 -DUSE_GITLAB=true
   ```

4. open solution (sln) created in the build folder with visual studio
  
   ```powershell
   # show the contents of the build dir
   ls
   # open knx-virtual.sln in visual studio by double clicking it.
   
   ```

5. Build the applications in visual studio
  
   Note: build the release version.

Note: The above set of commands will build the executables without security. The security is turned on/off during compile time.
If one wants to build the executables with security: add the option -DUSE_OSCORE=true in the cmake command:

   ```powershell
   cmake .. -DwxWidgets_ROOT_DIR=c:/wxWidgets-3.1.5 -DOC_OSCORE_ENABLED=ON
   or
   cmake .. -DwxWidgets_ROOT_DIR=c:/wxWidgets-3.1.5 -DUSE_GITLAB=true -DOC_OSCORE_ENABLED=ON
   ```

#### 1.4.4.1. Running the application on windows

The applications do not have a mechanims to restart themselves.
So if the applications are started from the commmand line, then the `restart` command from ETS will just shut down the application and will not restart the application.

##### 1.4.4.1.1. Python utility `restart_app.py`

This utility can start an application, and if the application quits it will restart the application automatically.

Note: Please stop the process by doing `Ctrl-C` in the window where python command is started.
The `Ctrl-C` is only handled when the application stops, e.g. `Ctrl-C` won't be honored when the application is running. 

### 1.4.5. Prerequisites windows

The prerequisites are the dependencies that are needed to build the applications on Windows:

- visual studio (C++)
- git
- cmake
  - perl
  - python
- wxWidgets

#### 1.4.5.1. Visual Studio

download visual studio from :

https://visualstudio.microsoft.com/downloads/

Install the package, including C++ (Desktop development with C++)

#### 1.4.5.1. git

git can be obtained from:

https://git-scm.com/download/win

- Download the file that is appropriate for your machine

  example: `64-bit Git for Windows Setup`
- Install the software (e.g. run the downloaded installer)

one should now have windows explorer integration to:

- Git Gui Here (to push data)
- Git Bash Here (a bash shell for commandline git)

#### 1.4.5.2. perl

Building (e.g. configuring with Cmake) requires perl.
If perl is not installed then install it via a Windows installer available at:

https://www.perl.org/get.html

To check if perl is installed:

```bash
# do in a new bash window
which perl
# result should be
# /usr/bin/perl
```

Note: needed by Cmake

#### 1.4.5.3. python

Building (e.g. configuring with CMake) requires python.

If python is not installed then install it via a Windows installer available at:

https://www.python.org/downloads/

To check if python is installed:

```bash
# do in a new bash window
which python
# result should be
# <some path>/python
```

Note: needed by Cmake

#### 1.4.5.4. Cmake

Configuring makefiles, solutions are done via CMake.

If cmake is not installed then install it via a Windows installer available at:

https://cmake.org/download/

Note: During installation, select that cmake is installed in path.

```bash
# do in a new bash window
which cmake 
# result should be
# <some path>/cmake
```

#### 1.4.5.5. Installing wxWidgets on Windows

Download wxwidgets from (installer source code):
https://www.wxwidgets.org/downloads/

- Install the contents on the recommended folder (e.g. c:\wxWidgets-3.1.5)
- Build wxwidgets with visual studio:
  
  - Open c:\wxWidgets-3.1.5\build\msw\wx_vc16.sln (or take the highest number available)
  - Accept convert solution suggestion: convert solution to newer version studio
  - Build the solution :
    - static Win32 library for Debug & Release
    - static x64 library for Debug & Release

## 1.5. Installation tools

The installation tools in a zip called `app_release_no_security_win32.zip` can be found in the release page of
[GitHub](https://github.com/KNX-IOT/KNX-IOT-STACK/releases) or in the release page of [GitLab](https://gitlab.knx.org/shared-projects/knx-iot-point-api-public-stack/-/releases).

- Download the zip file
- unzip the file on your local (windows machine)
- read the README.md file located in the created folder
