libscep - A Client Side SCEP Library
====================================
[![Build Status](https://travis-ci.org/Javex/libscep.svg?branch=develop)](https://travis-ci.org/Javex/libscep)

`libscep` implements the 
[SCEP protocol](http://tools.ietf.org/html/draft-nourse-scep-23) as a C library
and additionally provides a command line client that allows to operate the 
program without need to write an implementation of this library.

The client is based on the classic [sscep](https://github.com/certnanny/sscep)
client and follows the same syntax. Internally it just wraps the library.

The library itself is pretty basic. It gets initialized, then one or multiple
operations can be executed and then it gets destroyed again.

*Note*: This is currently in development phase und not yet ready.

Compiling
----------

Dependencies:

    `openssl`, `cmake`, `pkg-config`, `check` (unit test framework for C),
    `uriparser` (liburiparser1, liburiparser-dev on debian),
    `libcurl4-openssl-dev`

    mkdir build
    cmake ..
    make
    make test

Version Bump
-------------

The major and monir version numbers are tracked in:

    CMakeLists.txt
    src/clients/perl/Crypt-LibSCEP/MYMETA.json
    src/clients/perl/Crypt-LibSCEP/MYMETA.yml
    src/clients/perl/Crypt-LibSCEP/lib/Crypt/LibSCEP.pm

Building Debian Package
------------------------

The Debian packaging is done via Vagrant. To create the libscep and Perl
library packages, run the following:

    vagrant up
    vagrant ssh --command /vagrant/package/make-deb.sh

The resulting debian package files will be copied to the current directory.

NOTE: The current support for building packages is proof-of-concept. The
package versions must be set manually in the debian/\* files.

