# build script
# 2022-06-17 16:39:31.285243
mkdir -p build_unsecured
cd build_unsecured
cmake .. -DOC_OSCORE_ENABLED=OFF
# make with console
# cmake .. -DUSE_CONSOLE=true
# build step
cmake --build .
