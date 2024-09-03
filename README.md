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

安装完成grpc，protobuf会一并安装，无需额外安装

如果protobuf版本不是27.0
需要重新生成olama.pb.h和olama.pb.cc文件

```
protoc -I=. --cpp_out=. olama.proto
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

See [examples](https://git.woa.com/cloud_nosql/vectordb/vectordatabase-sdk-cpp/blob/master/example/main.cpp) about how to use this package to communicate with TencentCloud VectorDB