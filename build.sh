# build script
# 2022-06-17 16:39:31.285243
mkdir -p build
cd build
cmake .. -DOC_OSCORE_ENABLED=ON
# build step
cmake --build .

# make with console
# cmake .. -DUSE_CONSOLE=true
