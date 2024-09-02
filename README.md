# Tencent VectorDB C++ SDK

C++ SDK for Tencent Cloud VectorDB.

## Getting started

### Prerequisites
```
- c++ 17 or higher
```

### dependencies

- [protobuf](https://github.com/protocolbuffers/protobuf/releases?page=1)

version 27.0

如果protobuf版本不是27.0
需要重新生成olama.pb.h和olama.grpc.h文件

```
protoc -I=. --cpp_out=. olama.proto

protoc -I=. --grpc_out=. --plugin=protoc-gen-grpc=$(which grpc_cpp_plugin) olama.proto
```
使用grpc_cpp_plugin需要安装grpc

- [grpc](https://github.com/grpc/grpc/tree/master/src/cpp)

### CMake
```
mkdir build && cd build
cmake ..
make
```
