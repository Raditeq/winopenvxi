
Windows open source VXI library (only the client was tested).

Introduction
============

This library let you to communicate with any VXI-11 enabled device. The VXI is
a protocol for laboratory instrumentation based on the Sun ONC RPC (Open
Network Computing - Remote Procedure Call) system. More info are avaiable at
https://docs.oracle.com/cd/E53394_01/html/E54804/index.html.

The VXI 11 Specification (http://www.vxibus.org/specifications.html) contains a
description of the protocol that is compilable to c code by the means of
standard Sun tools.

This repository contain a Windows port of the Sun Library (found at
https://sourceforge.net/projects/oncrpc-windows) and the automatic generated
VXI code. Minor change was made in order to let the code to integrate in the a
framework I was working on.

The original library was distributed with LGPL licensing, so if you want to use
this code in commercial application, you have to compile it as a dll and link
it into your application. However the vxi11.h file is in the Public Domain, so
you can use and/or modify and/or redistribute it without any restriction.

Build and Usage
===============

In the build_mingw32 directory there are the binary files compiled with 32 bit
mingw; they were generated with the following command that should work on any
bash+mingw installation (e.g. msys2). You should adapt it to other
compilers/systems.

Make build dir:

mkdir -p build
cd build
rm -f winopenvxi.dll example.exe

Compiling the dll: 

shlc=$(ls ../*.c | grep -v example)
gcc -static-libgcc -include ../example.h -I../include -I../ $shlc shared -o winopenvxi.dll -lws2_32

Build the example application:

gcc -static-libgcc -I../include -I../ -o example.exe ../example.c winopenvxi.dll -lws2_32

The example will just send a string to an host and wait for a response (a new
line will be automatically appended at end of the string):

./example.exe 10.1.0.204 *IDN?

Look to its source to get an idea of how the library works: there is no
additional documentation, sorry (except the one on the VXI and Sun ONC-RPC
websites). Just note that to deploy the library you just need to copy the
vxi11.h header and the winopenvxi.dll binary (plus the winopenvxi.lib if your
compiler generates/needs it).

The library contains also a VXI-11 server, however I did not test it so its
functions/structures are not exposed in the vxi11.h. In order to use it you can
include the full header files in the "include" directory, or you can copy the
relevant definitions from there to your vxi11.h.

License
=======

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

