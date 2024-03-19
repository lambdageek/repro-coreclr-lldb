
This is a repro for https://github.com/dotnet/runtime/issues/99977

# Building

```console
$ cmake -B out -S .
$ cmake --build out
```

# Running

```console
$ lldb -b -o "plugin load ./out/libhihost.dylib"
(lldb) plugin load ./out/libhihost.dylib
Hello from C
hostfxr path is /Users/alklig/work/hihost/out/libhostfxr.dylib
running with dotnet root /Users/alklig/work/hihost/out/
coreclr initialized
zsh: killed     lldb -b -o "plugin load ./out/libhihost.dylib"

$ PAL_MachExceptionMode=7 lldb -b -o "plugin load ./out/libhihost.dylib"
(lldb) plugin load ./out/libhihost.dylib
Hello from C
hostfxr path is /Users/alklig/work/hihost/out/libhostfxr.dylib
running with dotnet root /Users/alklig/work/hihost/out/
coreclr initialized
host says: Failed to create CoreCLR, HRESULT: 0x8007000C
hostfxr_run_app finished
```

# Running in a non-LLDB app

Note `runmyself` is signed to use the hardened runtime, but is given the `com.apple.security.cs.allow-jit` and `com.apple.security.cs.disable-library-validation` entitlements (see CMakeLists.txt)

``` console
$ ./out/runmyself
Hello from C
hostfxr path is /Users/alklig/work/hihost/out/libhostfxr.dylib
running with dotnet root /Users/alklig/work/hihost/out/
coreclr initialized
Hello from C#
hostfxr_run_app finished
```
