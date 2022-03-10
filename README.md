# KNX-IOT-Virtual

## virtual applications

## installing wxWidgets on Windows

download wxwidgets from (installer source code):
https://www.wxwidgets.org/downloads/

- install the contents on the recommended folder (e.g. c:\wxWidgets-3.1.5)
- build wxwidgets with visual studio:
  
  - open c:\wxWidgets-3.1.5\build\msw\wx_vc16.sln (or take the highest number available)
  - accept convert solution suggestion: convert solution to newer version studio
  - build the solution

## building the applications on Windows

- clone this repo
- go to the repo (cd)
- mkdir build
- cmake .. -DwxWidgets_ROOT_DIR=c:/wxWidgets-3.1.5
- build with visual studio

### knx_iot_virtual_pb

push button

- serial number :  0003000

### knx_iot_virtual_sa

switch actuator

- serial number : 0004000