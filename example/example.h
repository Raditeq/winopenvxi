#pragma once

#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32

#ifdef NI_CVI

#define DllExport __export
#define DllImport __import 

#else // NI_CVI

#ifndef DllExport
#define DllExport	__declspec( dllexport )
#endif

#ifndef DllImport
#define DllImport   __declspec( dllimport )
#endif

#endif // NI_CVI

#else // _WIN32

#ifndef DllExport
#define DllExport	extern
#endif

#ifndef DllImport
#define DllImport   extern
#endif

#endif // _WIN32

