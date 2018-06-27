# Correlated OT

This is an implementation of the Correlated Oblivious Transfer protocol presented in Section 4 of [S18]. 

## Settings

Currently, our protocol supports only arithmetic modulo powers of 2 for which there exist native datatypes (e.g. 2^32, 2^64, 2^128).

## Dependencies

Our library requires [Boost](https://www.boost.org/) for the communication layer and [openssl](https://www.openssl.org/) for the cryptographic primitives (AES and SHA256).
It also requires [CMake](https://cmake.org/) for the generation of the Makefile.

## Installation

### Configuration

The customizable parameters for the compilation are
- `N`: This is the parameter n in the cited reference
- `K`: This is the parameter k in the cited reference
- `K_`: This is the parameter k' (k prime) in the cited reference
- `POWER`: The protocol will work over the ring of integers modulo 2^POWER
- `INT_TYPE`: This is the underlying representation of the integers. 
It must hold that `POWER/8 == sizeof(INT_TYPE)`

For security it is also required that `POWER*K_ >= K`. 
<!--Due to some implementation details, it is also required that `K_ + N` is divisible by 4.-->
To configure these parameters, access the file `CMakeLists.txt` and edit the first few lines.
```
set(N 999)
set(K 128)
set(K_ 1)
set(POWER 32)
set(INT_TYPE "unsigned int")
```

### Build (Linux only)

Our library assumes that Boost is installed in .
The best way to install Boost and comply with this requirement is by running the following (can be run from any directory)
```
# Download Boost (always check for last version)
wget boost_1_67_0.tar.bz2 https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.bz2
# Extract it in current folder
tar --bzip2 -xf boost_1_67_0.tar.bz2
# Move it to /usr/local
sudo mv boost_1_67_0 /usr/local
# Compile Boost
cd usr/local/boost_1_67_0/
./bootstrap.sh
./b2
```

Then download and build the corrOT library

```
git clone --recurse-submodules https://github.com/deescuderoo/corrOT.git
cd corrOT
cmake -H. -Bbuild
cd build
make
```

This will generate two folders in the main directory: `lib` and `bin`.
The former contains the library `libcorrOT.a` and the latter contains the executable `testOT` that tests the library. 
The source file of this executable (`test/test.cpp`) also shows how to use the library.


<!--
It uses [libscapi](https://github.com/cryptobiu/libscapi) for the communication layer, as well as for the base OT protocol (these are wrappers for [Boost](https://www.boost.org/)'s communication and 
[Simplest OT](https://eprint.iacr.org/2015/267.pdf), respectively)
-->

## Vector OLE

To get a vector Oblivious Linear Evaluation protocol from correlated OT, follow the guidelines presented in Appendix C of [CDESX18].

## TODOs

- Support other modulus
- Remove printing and add logging instead

---

> [S18] Peter Scholl: Extending Oblivious Transfer with Low Communication via Key-Homomorphic PRFs. Public Key Cryptography (1) 2018: 554-583

> [CDESX18] Ronald Cramer, Ivan Damgård, Daniel Escudero, Peter Scholl, and Chaoping Xing: 
SPDZ_2^k, Efficient MPC mod 2^k for Dishonest Majority
