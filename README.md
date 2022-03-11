# KNX-IOT-Virtual

## virtual applications

These applications are meant to run on Windows 10.

## installing wxWidgets on Windows

download wxwidgets from (installer source code):
https://www.wxwidgets.org/downloads/

- install the contents on the recommended folder (e.g. c:\wxWidgets-3.1.5)
- build wxwidgets with visual studio:
  
  - open c:\wxWidgets-3.1.5\build\msw\wx_vc16.sln (or take the highest number available)
  - accept convert solution suggestion: convert solution to newer version studio
  - build the solution

Note: using wxWidgets requires perl
If not installed then install it via a Windows installer available at:

https://www.perl.org/get.html

## building the applications on Windows

- clone this repo
- go to the repo (cd)
- mkdir build
- cmake .. -DwxWidgets_ROOT_DIR=c:/wxWidgets-3.1.5
- build with visual studio

### knx_iot_virtual_pb

Push Button

- serial number :  0003000

### knx_iot_virtual_sa

Switch Actuator

- serial number : 0004000