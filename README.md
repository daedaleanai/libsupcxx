
libsupcxx
=========

`libsupcxx` is an implementation of the [freestanding C++][frstd] runtime on top
of bare metal. Most of it, is derived from the GNU standard C++ library and is,
therefore distributed under the terms of the [GNU GPLv3][gpl] with the
[runtime exception][gcc-rt-exp]. The GNU library has been cleaned-up from all
the code that was not compatible with C++14 and all the code supporting the
previous versions of the standard has been converted to the form idiomatic to
C++14. All the code meant specifically for compilers other than GCC or for the
platforms we do not want to support has been removed as well.

The directory structure is as follows:

 * **boot** - bootup code for the supported architectures including program
   startup and linker scripts
 * **cmake** - CMake utility scripts
 * **io** - printing utilities (`printf`) and other IO-related code
 * **libruncxx** - parts of the runtime that were either not provided (dependent
   on the OS or libc) or that had to be rewritten because of the target platform
   considerations
 * **libsupcxx** - refactored and cleaned-up version of GCC's libsupc++
 * **tests** - a bunch of simple tests kernels that test some of the
   functionality provided by the library


Building instructions
---------------------

First of all, you will need a cross-compiler targeting bare metal. To build one,
you will need a couple of utilities and libraries. These may be installed on
Ubuntu using the following command:

    sudo apt-get install build-essential libgmp-dev libmpc-dev libmpfr-dev

This distribution provides a convenience shell script that can help with the
building and installation process of the cross-compiler itself. Run something
like the command below to build it for the desired platform.

    ./build-cross-compiler.sh <installation-path> <platform>

Currently supported platforms are:

 * i686-elf

After the installation process is complete, you will need to add the
installation binary path to your shell's search path:

    export PATH=<installation-path>/bin:$PATH

Finally, you can build the whole thing using CMake:

    mkdir build
    cd build
    cmake .. -DTOOLCHAIN_NAME=<platform>
    make

where platform is the same as above.

Running and debugging
---------------------

The binaries built this way are meant for bare metal and thus are not runnuble
under an operating system. They are however, multiboot compatible kernels and,
therefore, can be run using `qemu`. To install `qemu` on Ubuntu, type:

    sudo apt-get install qemu-system-x86

To run the tests, type:

    qemu-system-i386 -kernel tests/test-07-throw-clean-up-rethrow.elf

The binaries are built in debug mode by default and may be debugged using GDB.
Run `qemu` in one terminal window:

    qemu-system-i386 -S -s -kernel tests/test-07-throw-clean-up-rethrow.elf

The `-s` parameter is a shorthand for `-gdb tcp::1234` and will start a GDB
server at `localhost:1234`, '-S' tells `qemu` to not start the virtual CPU,
but to wait for the monitor (GDB) to start it. You can then run gdb in another
terminal window:

    gdb tests/test-07-throw-clean-up-rethrow.elf
    (gdb) target remote localhost:1234
    Remote debugging using localhost:1234
    0x0000fff0 in ?? ()
    (gdb) b main
    Breakpoint 1 at 0x10057a: file /home/lj/Project/libsupcxx/tests/test-07-throw-clean-up-rethrow.cc, line 62.
    (gdb) c
    Continuing.

    Breakpoint 1, main (cmdline=0x100043 <_start+40> "tests/test-07-throw-clean-up-rethrow.elf") at /home/lj/Project/libsupcxx/tests/test-07-throw-clean-up-rethrow.cc:62
    62        int test = one();
    (gdb) p cmdline
    $1 = 0x100043 <_start+40> "tests/test-07-throw-clean-up-rethrow.elf"

[frstd]: https://en.cppreference.com/w/cpp/freestanding
[gpl]: https://www.gnu.org/licenses/gpl-3.0.en.html
[gcc-rt-exp]: https://www.gnu.org/licenses/gcc-exception-3.1.en.html
