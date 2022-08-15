# unishox2-afl-harnesses

**WARNING: This README is a late-night, quick document (mostly for my future self), and may not adhere to fuzzing/AFL++ best practices.**

These are a few quick harnesses for testing [Unishox2](https://github.com/siara-cc/Unishox2) (a Unicode encoder which is *great* at compressing short strings), minimally modified from [moneromooo-monero's comment in Unishox2 issue #47](https://github.com/siara-cc/Unishox2/issues/47), but coupled with a bunch of documentation as I relearn some fuzzing skills.

Unishox2 has been found to crash if an untrusted input to its `unishox2_decompress` function, such as `NULL`. This would create an availability vulnerability in applications where the input to `unishox2_decompress` cannot be trusted. So, let's hammer Unishox2 and find out where its weak points are using [AFL++](https://github.com/AFLplusplus/AFLplusplus).

### Setup

Build and install AFL++ according to [AFL++ documentation](https://github.com/AFLplusplus/AFLplusplus/blob/stable/docs/INSTALL.md) - you will need `afl-fuzz` and `afl-gcc-fast` at minimum.

Clone Unishox2 into a local directory, and then place any harnesses from functions you want to test (the files that start with `harness-`) in the Unishox2 directory.

Change to your Unishox2 directory - we'll be working out of there from now on.

### Instrumentation

AFL++ can instrument on-the-fly (via QEMU) or during compile-time to help, but since Unishox2 can be easily instrumented at compile-time, we'll use that to retain better performance while fuzzing. To do this, harnesses needs to be compiled with `afl-gcc-fast` - for example, compiling `harness-unishox2-decompress-simple.c`:

```
afl-gcc-fast -o harness-unishox2-decompress-simple harness-unishox2-decompress-simple.c unishox2.c
```

### Fuzzing

```
mkdir -p fuzz-input && echo '       ' > fuzz-input/SEED
```

```
afl-fuzz -i fuzz-input -o fuzz-decompress-simple ./harness-unishox2-decompress-simple @@
```

### Results

