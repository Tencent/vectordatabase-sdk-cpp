# Tencent VectorDB C++ SDK

C++ SDK for Tencent Cloud VectorDB.

## Getting started

### Prerequisites
```
- c++ 17 or higher
```

### dependencies

#### [grpc](https://github.com/grpc/grpc/tree/master/src/cpp)

```
 git clone https://github.com/grpc/grpc
 cd grpc
 git submodule update --init

 mkdir -p cmake/build
 cd cmake/build
 cmake ../..
 make
 make install
```

#### [protobuf](https://github.com/protocolbuffers/protobuf/releases?page=1)

version 3.11.4

```
git clone https://github.com/protocolbuffers/protobuf.git
cd protobuf
git submodule update --init --recursive
cmake .
cmake --build . --parallel 10
sudo cmake --install .
```

生成olama.pb.cc和olama.grpc.pb.cc等文件

```
protoc -I=. --cpp_out=. olama.proto
protoc -I=. --grpc_out=. --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) olama.proto
```

重新生成后，需将olama.grpc.pb.cc中的SearchEngine_method_names修改为

```
static const char* SearchEngine_method_names[] = {
  "/alias/set",
  "/alias/get",
  "/alias/delete",
  "/collection/create",
  "/collection/drop",
  "/collection/truncate",
  "/collection/describe",
  "/collection/list",
  "/index/rebuild",
  "/document/upsert",
  "/document/update",
  "/document/query",
  "/document/search",
  "/document/delete",
  "/database/create",
  "/database/drop",
  "/database/list",
};
```

#### [googletest](https://github.com/google/googletest/tree/main/googletest)
```
git clone https://github.com/google/googletest.git
cd googletest
git submodule update --init
mkdir build         
cd build
cmake ..     
make
sudo make install 
```

### CMake
```
mkdir build && cd build
cmake ..
make
```
编译完成后在build文件夹下会得到libvectordb_sdk.a静态库

## Examples

See example/main.cpp about how to use this package to communicate with TencentCloud VectorDB.