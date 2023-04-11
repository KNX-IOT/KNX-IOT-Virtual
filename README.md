![Linux](https://github.com/KNX-IOT/KNX-IOT-Virtual/actions/workflows/cmake-linux.yml/badge.svg)
![Windows](https://github.com/KNX-IOT/KNX-IOT-Virtual/actions/workflows/cmake-windows.yml/badge.svg)
![sync](https://github.com/KNX-IOT/KNX-IOT-Virtual/actions/workflows/gitlabsync.yml/badge.svg)

<!-- TOC -->

- [1. The Applications](#1-the-applications)
  - [1.1. The knx\_iot\_virtual\_pb Application](#11-the-knx_iot_virtual_pb-application)
    - [1.1.1. MetaData push button](#111-metadata-push-button)
  - [1.2. knx\_iot\_virtual\_sa Application](#12-knx_iot_virtual_sa-application)
    - [1.2.1. MetaData switch actuator](#121-metadata-switch-actuator)
- [2. Updating KNX-IOT-Virtual code base](#2-updating-knx-iot-virtual-code-base)
  - [2.1. GitHub access](#21-github-access)
  - [2.2. GitLab access](#22-gitlab-access)
- [3. The Commandline applications](#3-the-commandline-applications)
- [4. WxWidget GUI Applications (Windows)](#4-wxwidget-gui-applications-windows)
  - [4.1. Push Button wxWidget GUI](#41-push-button-wxwidget-gui)
  - [4.2. Switch Actuator wxWidget GUI](#42-switch-actuator-wxwidget-gui)
  - [4.3. Override the serial number (wxWidgets GUI apps only)](#43-override-the-serial-number-wxwidgets-gui-apps-only)
  - [4.4 Options (windows application)](#44-options-windows-application)
  - [4.4.1 Display option](#441-display-option)
    - [4.4.1.1 Group Address](#4411-group-address)
    - [4.4.1.1 GRPID](#4411-grpid)
    - [4.4.1.1 Installation ID](#4411-installation-id)
  - [4.4.1 Sleepy option](#441-sleepy-option)
  - [4.4. Building the applications on Windows](#44-building-the-applications-on-windows)
    - [4.4.1. Running the application on windows](#441-running-the-application-on-windows)
      - [4.4.1.1. Python utility `restart_app.py`](#4411-python-utility-restart_apppy)
  - [4.5. Prerequisites windows](#45-prerequisites-windows)
    - [4.5.1. Visual Studio](#451-visual-studio)
    - [4.5.2. git](#452-git)
    - [4.5.3. python](#453-python)
    - [4.5.4. Cmake](#454-cmake)
- [5. Installation tools](#5-installation-tools)

<!-- /TOC -->

## 1. The Applications

There are 2 applications in this repo:

- knx_iot_virtual_pb Application (Push button)
- knx_iot_virtual_sa Application (Switch Actuator)

Both applications are using the [KNX IoT Point API stack](https://github.com/KNX-IOT/KNX-IOT-STACK).

The general structure of these programs are:

```
   _________________
  |    GUI CODE     |   <---- WxWidget, Printf
  |_________________|
  |  POINT API CODE |   <---- Generic code handling all Point API CoAP code
  |_________________|
  |      STACK      |   <---- Generic The KNX IoT Point API Stack (other repo)
  |_________________|

  General structure
```

Note: In the structure diagram above, the "GUI CODE" can be replaced with something else like a CLI.
This is possible because that upper layer is decoupled from the layers below it.
 The "POINT API CODE" layer is completely generic and does not need to be modified.

The Point API Code is shared code that can be used:

- as commandline application (on Linux & Windows)
- GUI application with wxWidgets (on Windows)

The point code has an API so that one can:

- set/retrieve data from an URL
- callback on PUT data changes
- functions to figure out what type of data the url conveys

Note: Although the applications are called virtual, they are real KNX IOT Applications.
They are only running on a PC instead of on actual hardware.

### 1.1. The knx_iot_virtual_pb Application

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

#### 1.1.1. MetaData push button

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

### 1.2. knx_iot_virtual_sa Application

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

#### 1.2.1. MetaData switch actuator

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

## 2. Updating KNX-IOT-Virtual code base

Please "touch" the `CMakeLists.txt` file, then the visual studio solution will see
that the file is outdated and will do a pull of the depended projects during
the next build of `KNX-IoT-Virtual` will use the latest code from `KNX-IoT-Stack`

Note: touch on windows: please add in the file a blank somewhere and save. e.g. the save date on disk will change.

### 2.1. GitHub access

Obtain a clone of this repository using `git clone https://github.com/KNX-IOT/knx-iot-virtual`.
If you see a login prompt, use your GitHub username and a personal access token with "repo" access scope.

Since personal access tokens are difficult to keep track of, you may want to tell git to remember the access tokens you use using `git config global credential.helper store`. The credentials will be stored on the SD card in plaintext, within `~/.git-credentials`.

### 2.2. GitLab access

Obtain a clone of this repository using `git clone https://github.com/KNX-IOT/knx-iot-virtual`.
If you see a login prompt, use your GitLab username and a personal access token with "repo" access scope.

Since personal access tokens are difficult to keep track of, you may want to tell git to remember the access tokens you use using `git config global credential.helper store`. The credentials will be stored on the SD card in plaintext, within `~/.git-credentials`.

## 3. The Commandline applications

The command line applications are:

- knx_iot_virtual_pb Application (Push button)
- knx_iot_virtual_sa Application (Switch Actuator)

The command line applications can run on Linux & Windows.
Both applications show the interaction with printfs.
The Push Button application has no means to fire a push button interaction.

## 4. WxWidget GUI Applications (Windows)

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

### 4.1. Push Button wxWidget GUI

The application implements:

- Button to send on/off, status bar shows what is being send (on=true or off=false)
- Toggle for InfoOnOff (readonly) shows the received info, the toggle is for viewing only

### 4.2. Switch Actuator wxWidget GUI

The application implements:

- Button (readonly) to visualize the received on/off
- Toggle for Info (readonly) shows the received info, the toggle is for viewing only
- Toggle for Fault info, e.g. allows sending the received status back or always false (e.g. not active).

### 4.3. Override the serial number (wxWidgets GUI apps only)

The serial number can be overridden with the command line argument -s.

Example (from the folder where the executable resides):

```bash
.\knx_iot_virtual_gui_pb.exe -s 00FA10010401
.\knx_iot_virtual_gui_sa.exe -s 00FA10010401
```

### 4.4 Options (windows application)

### 4.4.1 Display option

The Individual address is always shown in:

- decimal integer
- 3 level address, similar as ETS is using in the UI

#### 4.4.1.1 Group Address

The Group address (GA) can be shown in the UI as:

- decimal integer
- 3 level address, similar as ETS is using in the UI

#### 4.4.1.1 GRPID

The Group ID (recipient/publisher table) can be shown in the UI as:

- decimal integer
- 2 level notation, similar as ETS is using in the UI

#### 4.4.1.1 Installation ID

The Installation ID (IID) can be shown in the UI as:

- decimal integer
- 2 level notation, similar as ETS is using in the UI

### 4.4.1 Sleepy option

The windows device can simulate the sleepy device.
When enabled the device interacts with the network at 20 seconds intervals.
The when the programming mode is enabled then the device will temporarily disable the sleep interval.

### 4.4. Building the applications on Windows

1. Install all prerequisits:
   - visual studio (C++)
   - git
   - python
   - cmake

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
   # create the msdev solution
   cmake ..
   ```

   The above sequence of commands are also available via shell scripts that creates the solution AND create the executables
   - build.sh for building the secure devices with mDNS
   - build_unsecured.sh  for building the same devices with security turned off (no mDNS).

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
   # secure, no mDNS
   mkdir build
   cd build 
   cmake .. -DUSE_GITLAB=true
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
   cmake .. -DOC_OSCORE_ENABLED=ON
   or
   cmake .. -DUSE_GITLAB=true -DOC_OSCORE_ENABLED=ON
   ```

#### 4.4.1. Running the application on windows

The applications do not have a mechanims to restart themselves.
So if the applications are started from the commmand line, then the `restart` command from ETS will just shut down the application and will not restart the application.

##### 4.4.1.1. Python utility `restart_app.py`

This utility can start an application, and if the application quits it will restart the application automatically.

Note: Please stop the process by doing `Ctrl-C` in the window where python command is started.
The `Ctrl-C` is only handled when the application stops, e.g. `Ctrl-C` won't be honored when the application is running.

Note: according to the specifications the restart is not an actual restart of the device, but just a partial restart of the stack.
Hence to simulate a full restart this utility can be used.

### 4.5. Prerequisites windows

The prerequisites are the dependencies that are needed to build the applications on Windows:

- visual studio (C++)
- git
- cmake
  - python

#### 4.5.1. Visual Studio

download visual studio from :

https://visualstudio.microsoft.com/downloads/

Install the package, including C++ (Desktop development with C++)

#### 4.5.2. git

git can be obtained from:

https://git-scm.com/download/win

- Download the file that is appropriate for your machine

  example: `64-bit Git for Windows Setup`
- Install the software (e.g. run the downloaded installer)

one should now have windows explorer integration to:

- Git Gui Here (to push data)
- Git Bash Here (a bash shell for commandline git)

#### 4.5.3. python

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

#### 4.5.4. Cmake

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

## 5. Installation tools

please download the x-copy of ETS 6.
