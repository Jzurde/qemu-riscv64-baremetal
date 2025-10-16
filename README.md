# BAREMETAL PRINTF
This project provides simple printf function that works on baremetal(without os system). We expect to run this on QEMU, but slight modify on UART address and other link discriptions will allow execution on other environments.

## How things work
```
$ ./s-boot.sh // execute via OPEN SBI(S-mode)
```
or
```
$ ./m-boot.sh // execute without OPEN SBI(M-mode)
```
Note, this will require `riscv64-unknown-elf-g++` to be build and install in advance with --march: imac, with designated PATH added, and off course `qemu-system-riscv64`.

`printf` functions are located in `baremetal_printfs.h` header file. Including the `baremetal_printfs.h` header file will allow you to use printf. printf functions are directly declared inside headerfile, which will result in single file, but are also declared inline, which will affect in someways.

## Limitation
Printfs will only allow, **int** and **strings**(char*, not std::string). **float** cann**not** be handdled in printfs. You are asked to use print_float function indivisually when printing float variables.