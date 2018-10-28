# cmake -DEMSCRIPTEN=1 -DCMAKE_TOOLCHAIN_FILE=Emscripten.cmake -DEMSCRIPTEN_ROOT_PATH=/opt/emsdk/emscripten/1.38.12/  -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/opt/emsdk/emscripten/1.38.12/emcc
# make
source /opt/emsdk/emsdk_env.sh
PATH=$PATH:/opt/emsdk/emscripten/1.38.12/
emcc -w -std=c++1z -g4 -O1 -s DEMANGLE_SUPPORT=1 -DNO_ZLIB -DNO_READLINE src/*.cpp -o netbase.html
