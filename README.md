# Silicium Compiler

Silicium Compiler (SiC) is a personal compiler project.

## Dependencies

**Sic** is written in C++, you will need :

- CMake 3.10
- Flex
- Bison
- LLVM

> [!TIP]
> On MacOS you may need to `brew link llvm --force` ans `brew link bison --force`

## Build

```bash
cmake -S src -B build
make -C build
```

## Run

```bash
./build/sic <file>
```

## Test

Once you have built the project, you can run the tests with :

```bash
make -C build test
```
