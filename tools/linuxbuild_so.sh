PROJECT_DIR="$(pwd)/../"

echo ${PROJECT_DIR}

if ! [ -d "./build" ]; then
  mkdir ./build
fi
cd ./build/

cmake \
  -DSHARED_LIBRARY:STRING=True \
  -DCMAKE_EXE_LINKER_FLAGS="-Wl,-rpath='\$ORIGIN'" \
  -DTESTS:STRING=True \
  ..
  
cmake --build . --config Release

