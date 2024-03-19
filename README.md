
# Building

```console
$ cmake -B out -S .
$ cmake --build out
```

# Running

```console
$ lldb -b -o "plugin load ./out/libhihost.dylib"
```
