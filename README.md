# Hashmap
This module is meant to be used as a shared library. building this creates a `libhashmap.so` file at location `build/src/libhashmap.so`.
This should be linked using CMake library `hashmap`

## Building
First step make sure `buildall.sh` is executable. 
```sh
chmod +x buildall.sh
```

There is only a single flag for clean building which will delete the build directory and start from scratch
```sh
./buildall.sh -c
```

This can be built without clean flag if not wanted
### NOTE: v1.1 contains a pthread_mutex and any executable using it must be linked with -pthread during compilation if running Ubuntu version prior to 22.04