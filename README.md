# LOLCODE Compiler for Parallel & Distributed Computing

This repository includes the source for the LOLCODE compiler (lcc) with extensions for parallel and distributed computing. Example codes demonstrating various features and code complexity may be found in the [./examples](./examples) directory.

## Dependencies

You need a C compiler and an OpenSHMEM implementation for your target platform. Three different platforms have been tested and general instructions for installing and using lcc.

1. Parallella, specifically, the many-core Epiphany coprocessor
2. x86 multi-core workstation, running Linux.
3. Cray XC40 supercomputer

## Install Instructions

Each platform has a some special configuration of compiler and OpenSHMEM library that is called in the back end that can be configured automatically with `--with-shmem=arlshmem|openshmem|crayshmem` at the configure script.

### For Parallella

You must download and install the [COPRTHR-2 SDK](http://www.browndeertechnology.com/coprthr2_download.htm) and follow the install instructions.  Then clone the ARL OpenSHMEM for Epiphany repository and build it.
```
git clone https://github.com/USArmyResearchLaboratory/openshmem-epiphany
```
Clone this repository, making sure to specify the install directories:
```
./configure --with-shmem=arlshmem --with-shmem-include-path=[SHMEM_INC_DIR] --with-shmem-lib-path=[SHMEM_LIB_PATH]
make
sudo make install
```

### For x86 multi-core workstation

The OpenSHMEM reference implementation based on GASNet (with libelf-dev dependency) has been tested. You should configure your GASNet and OpenSHMEM for symmetric multiprocessing (SMP).

```
git clone https://bitbucket.org/berkeleylab/gasnet.git
cd gasnet
./Bootstrap
./configure
make all
sudo make install
cd ..
sudo apt-get install libelf-dev
git clone https://github.com/openshmem-org/openshmem
cd openshmem
./configure --with-gasnet-root=/usr/local/gasnet --with-gasnet-conduit=smp
make
sudo make install
```

Then clone this repository, configure, build, and install it.

```
./configure --with-shmem=openshmem
make
sudo make install
```

### For Cray XC40 supercomputer

It's assumed that your Cray XC40 already has Cray OpenSHMEM installed.  If not, ask your sysadmin.  Then clone this repository, configure, build, and install it.

```
./configure --with-shmem=crayshmem
make
sudo make install
```

## Compiling LOLCODE

Compiling is analogous to using a C compiler. The LOLCODE compiler, lcc, has command line options which can be seen by running `lcc --help`.  You may compile programs in this manner.  The result of the compilation is a serial program which can be executed directly or called with a parallel job launcher described below.  By default, the executable is named a.out.

## Running

Each platform may have a different methods for launching the program across multiple processing elements.  For example, Parallella uses *coprsh*, x86 with the reference implementation of OpenSHMEM uses *oshrun*, and the Cray XC40 uses *aprun*.

For example, this will launch 16 parallel processing elements on the Parallella Epiphany cores:
```
coprsh -np 16 ./a.out
```
