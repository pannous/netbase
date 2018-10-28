# cmake -DEMSCRIPTEN=1 -DCMAKE_TOOLCHAIN_FILE=Emscripten.cmake -DEMSCRIPTEN_ROOT_PATH=/opt/emsdk/emscripten/1.38.12/  -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/opt/emsdk/emscripten/1.38.12/emcc
# make
source /opt/emsdk/emsdk_env.sh
PATH=$PATH:/opt/emsdk/emscripten/1.38.12/
EMCC_DEBUG=1
# "--ignore-dynamic-linking "--embed-file

options="--memoryprofiler --emit-symbol-map --tracing  --profiling --emrun --memory-init-file 1 -v -s ASSERTIONS=1 " 
options="$options -s TOTAL_MEMORY=0x30000000"
emcc -w $options -std=c++1z -g4 -O0 -s DEMANGLE_SUPPORT=1 -DWASM -DNO_ZLIB -DNO_READLINE src/*.cpp -o netbase.html

# wasm-gc netbase.wasm # optimize and demangle

cat glue.js >> netbase.js