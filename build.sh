# build script
# 2022-06-17 16:39:31.285243
mkdir -p build
cd build
cmake .. -DwxWidgets_ROOT_DIR=c:/wxWidgets-3.1.5
# make with console
# cmake .. -DwxWidgets_ROOT_DIR=c:/wxWidgets-3.1.5 -DUSE_CONSOLE=true
# TODO : add build step
