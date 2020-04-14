# Learning-OpenGL
So basically - I can't OpenGL

# How to run it?
You'll need VCPKG from Microsoft. You can get it here - https://github.com/microsoft/vcpkg
Then, set environment variable VCPKG_ROOT pointing to the repository you just downloaded.
Once you're done, just regular CMake:
```
mkdir build; cd build; cmake ..; cmake --build .;
```

There are convenient tasks defined for Visual Studio Code for generating CMake for both debug and release. Also, debug Launch Tasks for Windows (CL) and Linux (LLDB).