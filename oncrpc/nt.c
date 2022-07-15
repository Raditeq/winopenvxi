/**********************************************************************
 * ONC RPC for WIN32.
 * 1997 by WD Klotz
 * ESRF, BP 220, F-38640 Grenoble, CEDEX
 * klotz-tech@esrf.fr
 *
 * SUN's ONC RPC for Windows NT and Windows 95. Ammended port from
 * Martin F.Gergeleit's distribution. This version has been modified
 * and cleaned, such as to be compatible with Windows NT and Windows 95. 
 * Compiler: MSVC++ version 4.2 and 5.0.
 *
 * RPC for the Windows NT Operating System COMES WITH ABSOLUTELY NO 
 * WARRANTY, NOR WILL I BE LIABLE FOR ANY DAMAGES INCURRED FROM THE 
 * USE OF. USE ENTIRELY AT YOUR OWN RISK!!!
 **********************************************************************/
/*********************************************************************
 * RPC for the Windows NT Operating System
 * 1993 by Martin F. Gergeleit
 *
 * RPC for the Windows NT Operating System COMES WITH ABSOLUTELY NO 
 * WARRANTY, NOR WILL I BE LIABLE FOR ANY DAMAGES INCURRED FROM THE 
 * USE OF. USE ENTIRELY AT YOUR OWN RISK!!!
 *********************************************************************/

#include "all_oncrpc.h"

WSADATA WSAData;

static int init = 0;

int rpc_nt_init(void)
{
    if (init++)
	return 0;
	
    if (WSAStartup(0x0101, &WSAData)) {
	init = 0;
	nt_rpc_report("WSAStartup failed\n");
	WSACleanup();
	return -1;
    }

    return 0;
}

int rpc_nt_exit(void)
{
    if (init == 0 || --init > 0)
	return 0;

    return WSACleanup();
}

VOID
nt_rpc_report(LPCSTR lpszMsg)
{
    WCHAR chMsg[256];
    LPCWSTR lpszStrings[2];
    CONST size_t wideMessageBytes = strlen(lpszMsg) * sizeof(WCHAR);
    WCHAR* wideMessage = (WCHAR *) malloc(wideMessageBytes);

    // Use event logging to log the error.
    //
    const HANDLE hEventSource = RegisterEventSource(NULL,
                                                    TEXT("rpc.dll"));

    swprintf_s(chMsg, sizeof chMsg / sizeof chMsg[0], L"sunrpc report: %d", GetLastError());
    if (wideMessage != NULL) {
        swprintf_s(wideMessage, (wideMessageBytes / sizeof wideMessage[0]), L"%hs", lpszMsg);
    }

    lpszStrings[0] = chMsg;
    lpszStrings[1] = wideMessage;

    if (hEventSource != NULL) {
        ReportEvent(hEventSource, // handle of event source
            EVENTLOG_WARNING_TYPE, // event type
            0,                    // event category
            0,                    // event ID
            NULL,                 // current user's SID
            2,                    // strings in lpszStrings
            0,                    // no bytes of raw data
            lpszStrings,          // array of error strings
            NULL);                // no raw data

        (VOID) DeregisterEventSource(hEventSource);
    }
    if (wideMessage != NULL) {
        free((void *)wideMessage);
    }
}

