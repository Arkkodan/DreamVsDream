# Build instructions for Dream vs. Dream

If you are a **User**, that is, someone who uses any of the Dream vs. Dream binaries, you should check if there are releases rather than build the binaries yourself.

If there are no such binaries available for you, please submit an issue.

You can also actively aid development by following the steps below.

## Preface

This repo fork of Dream vs. Dream aims to allow for cross-platform development with ease.
That is, allow developers to work on any system and users to use for any system.
Obviously, requiring all developers to create their own build systems is tedious and needlessly raises the bar for development.

Despite this, only some operating systems and build generators are actively supported.
Therefore, getting support may be more difficult if you are using a different operating system.
Regardless, if you have issues with the build instructions, you might receive support by contacting maintainers.

## Prerequisites

This project uses [CMake](https://cmake.org/) to generate build systems across many compilers and generators.
Please have the following installed on your computer and accessible by your terminal. You do not have to download them through these links. You might even have them already installed.

- [git](https://git-scm.com/downloads)
- [CMake](https://cmake.org/download/)
- [any generator supported by CMake](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)
- [a recent enough version of Python](https://www.python.org/downloads/) (for glad)

These steps are written for a novice to follow, just to build this project.

> Please make sure that the step that you are on passes without **error** before moving on.
> **Warnings** are ok and expected, but **errors** mean that something is wrong. If your error is reproducible, feel free to report it with detail (the command you ran, the error you got, your operating system and architecture, etc...).

If you know what you are doing, you don't have to follow these exact steps.

## 1: Clone the Repo

This step clones the remote repository onto a local copy on your computer using git.

In an accessible directory where you are able to identify your local git repo, open your terminal there.

In your terminal, type:

```console
git clone https://github.com/Arkkodan/DreamVsDream DreamVsDream
cd DreamVsDream
```

You can, of course, replace the output directory with whatever name you want.
After this step, you have successfully cloned the repo and are now located there.

## 2: Configure and Generate

This step configures and generates a build system for you using CMake and your generator.

> First, determine whether your build generator uses _single configuration_ or _multi-configuration_.

If you are unsure of which generator is your default, type in your terminal:

```console
cmake --help
```

For a baseline, Make-like generators (like MinGW) tend to be single configuration while IDE generators (like Visual Studio) tend to be multi-configuration.

### 2.SINGLE

If you have a single configuration generator, type in your terminal:

```console
cmake -S . -B build -DCMAKE_BUILD_TYPE=RELEASE
```

Keep your source directory here.
In this example, the build directory is set inside `build` and made the build type `RELEASE`.
You can change your build directory, build type, and generator (with the `-G` option).
If you don't specify the build type, the CMakeLists.txt script will automatically select `DEBUG`.

> You may need to run it twice, once to cache variables and another to finish generating.

### 2.MULTI

If you have a multi-configuration generator, type in your terminal:

```console
cmake -S . -B build
```

This is similar to the command for single except you don't specify the build type here.

> You may need to run it twice, once to cache variables and another to finish generating.

## 3. Build

This step builds the binaries and libraries using CMake and your generator. You can also manually use your generator if you know how to.

### 3.SINGLE

If you have a single configuration generator, type in your terminal:

```console
cmake --build build
```

Make sure that your build directory here is the same as the build directory you generated in step 2.

You can specify which target to build with the `--target` option if you don't want to build everything.

### 3.MULTI

If you have a multi-configuration generator, type in your terminal:

```console
cmake --build build --config Release
```

Make sure that your build directory here is the same as the build directory you generated in step 2.

This is similar to the command for single except you specify the configuration type here.
If you don't specify the configuration type, your generator will probably select `Debug`.

## End of Build instructions

You are technically done with build instructions.
In the cache directory of this repo, there are .char files already generated. Simply copy the .char files into the data/chars directory.
However, they may be incompatible with your DvD executable.

If you want to or need to make your .char files in order to play DvD, continue to step 4.

* * *

## 4. .char Creation

This step creates .char files for use with DvD.

Copy these files into the [COMPILE directory](./COMPILE):

- Atlas executable
- Compiler executable
- zlib shared library (if it exists)
- png shared library (if it exists)
- other runtime libraries as needed

Then run: [`./compile-all.sh`](./COMPILE/compile-all.sh)

If successful, there should be a directory called `DvD`.
Inside, there should be a `chars` directory.

Copy the `chars` directory inside the `data` directory for DvD.

## 5. Last step

This step places the data files in the correct location.

DvD, and all Dream vs. Dream binaries for that matter, treat its own directory as its working directory.

Therefore, copy the `data` directory into the same location that DvD is located.

The following should be in the same directory:

- data directory
- DvD executable
- SDL2 shared library (if it exists)
- glad shared library (if it exists)
- zlib shared library (if it exists)
- png shared library (if it exists)
- ogg shared library (if it exists)
- vorbis shared library (if it exists)
- vorbisenc shared library (if it exists)
- FLAC shared library (if it exists)
- opus shared library (if it exists)
- sndfile shared library (if it exists)
- other runtime libraries (if they exist)

You may have to rename some shared libraries (a known case for MinGW is libogg.dll -> ogg.dll).
Do not mix shared libraries from different generators.
