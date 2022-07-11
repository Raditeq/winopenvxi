
Windows open source VXI library (only the client was tested).
This is a fork of the original [https://github.com/pocomane/winopenvxi](https://github.com/pocomane/winopenvxi)
project.
The main difference is that this project split up the code a bit more into
visual studio projects and allows for 64-bit compilation.

# Introduction

This library let you to communicate with any VXI-11 enabled device. The VXI is
a protocol for laboratory instrumentation based on the Sun ONC RPC (Open
Network Computing - Remote Procedure Call) system. More info are available at
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

# Build and Usage

This project uses visual studio solutions. So the easiest way to build the project
is to open it in visual studio (2019 has been tested).

The project supports 2 different configurations on 2 different platforms. There is
the Debug configuration and the Release configuration. The release configuration
is probably what most people will need. Then there are the x86 bit and x86_64 64 bit
platforms. You will need to build for the same platform that your application will use
DLL's are not backwards compatible.

## Projects

This project contains multiple other projects. The ones interesting for library use are,
`rpc`, `oncrpc`, and `winopenvxi`. The first two are part of the `oncrpc-windows` project
with minor changes and the `winopencvxi` project is the original code from the fork.

The other two projects `example` and `portmap` contain example code for using the vxi library
and `portmap` is a server that comes with `oncrpc-windows`.

## Command line

If you want to use the command line to build, for example on a build server, then you
will need to following commands.

**Note:** You will need to have `MSBuild.exe` in your path.

For 64 bit builds use

```ps
MSBuild.exe /p:Configuration=Release /p:Platform=x64
```

For 32 bit builds use

```ps
MSBuild.exe /p:Configuration=Release /p:Platform=x86
```

## Output files

The output files are in different folders depending on the configuration and architecture used.

|         | x86     | x86_64      |
|---------|---------|-------------|
| Debug   | Debug   | x64/Debug   |
| Release | Release | x64/Release |

The files needed are:

- oncrpc.dll
- winopenvxi.dll

If you are going to use [Implicit linking](https://docs.microsoft.com/en-us/cpp/build/linking-an-executable-to-a-dll?view=msvc-170#implicit-linking)
then you will also need (you will need to link against these libs):

- oncrpc.lib
- winopenvxi.lib

To make sure the correct methods and structures are available you will need the following header files in your project:

- winopenvxi/vxi11.h
- rpc/rpc/*.h (all the header files)

## Example

The example project opens a VXI connection to a device and sends some data. (*IDN? by default)

Usage is as follows `.\example.exe <ip> <data>`

Look to its source to get an idea of how the library works: there is no
additional documentation, sorry (except the one on the VXI and Sun ONC-RPC
websites). Just note that to deploy the library you just need to copy the
vxi11.h header and the winopenvxi.dll binary (plus the winopenvxi.lib if your
compiler generates/needs it).

The library contains also a VXI-11 server, however this has not been tested so its
functions/structures are not exposed in the vxi11.h. In order to use it you can
include the full header files in the "include" directory, or you can copy the
relevant definitions from there to your vxi11.h.

# License

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
