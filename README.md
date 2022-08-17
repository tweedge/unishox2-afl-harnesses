# unishox2-afl-harnesses

These are a few quick harnesses for testing [Unishox2](https://github.com/siara-cc/Unishox2) (a Unicode encoder which is *great* at compressing short strings), minimally modified from [moneromooo-monero's comment in Unishox2 issue #47](https://github.com/siara-cc/Unishox2/issues/47).

**WARNING:** This README is intentionally specific to using AFL++ with Unishox2, a concise but narrow example of how to use AFL++. It is not intended as a general guide and may not adhere to fuzzing/AFL++ best practices. This is tested and working on Ubuntu 22.04 on aarch64, and the same commands (particularly, the instrumentation chosen) could/should be changed on other platforms.

### Background

**Problem:** Unishox2 has been found to crash if an untrusted input to its `unishox2_decompress` function, such as `NULL`. This would create an availability vulnerability in applications where the input to `unishox2_decompress` cannot be trusted.

**Solution:** So, let's hammer Unishox2 and find out where its weak points are using [AFL++](https://github.com/AFLplusplus/AFLplusplus). For those unfamiliar with fuzzing or AFL++, here is some suggested reading material, in order:

* [OWASP's "what is fuzzing" page](https://owasp.org/www-community/Fuzzing)
* [AFL's homepage and "sales pitch"](https://lcamtuf.coredump.cx/afl/) (precursor to AFL++)
* [AFL++'s README and graphical guide](https://github.com/AFLplusplus/AFLplusplus/blob/stable/docs/README.md)
* [Michael Macnair's "fuzzing with AFL" workshop](https://github.com/mykter/afl-training)

### Setup

Build and install AFL++ according to [AFL++ documentation](https://github.com/AFLplusplus/AFLplusplus/blob/stable/docs/INSTALL.md) - you will need `afl-fuzz` and `afl-gcc-fast` at minimum.

Clone Unishox2 into a local directory, and then place any harnesses from functions you want to test (the files that start with `harness-`) in the Unishox2 directory.

Change to your Unishox2 directory - we'll be working out of there from now on.

### Instrumentation

AFL++ can instrument on-the-fly (via QEMU) or during compile-time to help, but since Unishox2 can be easily instrumented at compile-time, we'll use that to retain better performance while fuzzing. To do this, harnesses needs to be compiled with `afl-gcc-fast` - for example, compiling `harness-unishox2-decompress-simple.c`:

```
afl-gcc-fast -o harness-unishox2-decompress-simple harness-unishox2-decompress-simple.c unishox2.c
```

This uses AFL's GCC plugin ([documentation](https://github.com/AFLplusplus/AFLplusplus/blob/stable/instrumentation/README.gcc_plugin.md)) and was chosen for simplicity. AFL's clang-based instrumentation is generally recommended where available.

### Fuzzing

The seed we'll start fuzzing with is a series of spaces, and we'll let AFL++ do the heavy lifting of generating random inputs and finding edge cases through millions of tests.

```
mkdir -p fuzz-input && echo '       ' > fuzz-input/SEED
```

In future fuzzing campaigns, it would be sensible to have more test cases, including both valid examples and cases that Unishox2 had potentially failed to compress/decompress previously.

Now, we'll start a *very barebones* fuzzing campaign with one of the harnesses. This is single-threaded, and a new AFL++ thread needs to be started for each harness we want to test.

```
afl-fuzz -i fuzz-input -o fuzz-decompress-simple ./harness-unishox2-decompress-simple @@
```

If AFL++ throws any runtime errors (ex. about CPU performance or crash handling), those can either be resolved, or the given flag can be prepended to the `afl-fuzz` command to ignore that error.

Rapid improvements can/should be made to improve effectiveness:

* AFL++ can be [parallelized](https://aflplus.plus/docs/parallel_fuzzing/) for more coverage.
* AFL++ should use an available [dictionary](https://github.com/AFLplusplus/AFLplusplus/blob/stable/dictionaries/README.md) to use available grammar (ex. in UTF-8) without needing to discover it.

### Results

Running AFL++ against Unishox2 at commit [44780fac6e](https://github.com/siara-cc/Unishox2/tree/44780fac6e93e543a88e56d4d958bfee9c21b707) netted the following results within minutes:

* `unishox2_compress_simple`: 0 distinct crashes, 5 distinct hangs
* `unishox2_decompress_simple`: 39 distinct crashes, 0 distinct hangs

Unishox2 is a small program, and coverage improvements leveled off rapidly. Unishox2 could be a fantastic candidate for fuzzing in CI.