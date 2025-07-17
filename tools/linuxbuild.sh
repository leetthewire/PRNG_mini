PROJECT_DIR="$(dirname "$0")"/../
echo ${PROJECT_DIR}
cd ${PROJECT_DIR}

if ! [ -d "./libs/blake3" ]; then
	git submodule update --init
fi

if ! [ -d "./build" ]; then
	mkdir ./build
fi

cd ./build/

cmake -DSHARED_LIBRARY:STRING=False -DTESTS:STRING=True ..
cmake --build . --config Release
