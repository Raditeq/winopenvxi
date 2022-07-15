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
 * Users may use, copy or modify Sun RPC for the Windows NT Operating 
 * System according to the Sun copyright below.
 * RPC for the Windows NT Operating System COMES WITH ABSOLUTELY NO 
 * WARRANTY, NOR WILL I BE LIABLE FOR ANY DAMAGES INCURRED FROM THE 
 * USE OF. USE ENTIRELY AT YOUR OWN RISK!!!
 **********************************************************************/
/*********************************************************************
 * RPC for the Windows NT Operating System
 * 1993 by Martin F. Gergeleit
 * Users may use, copy or modify Sun RPC for the Windows NT Operating 
 * System according to the Sun copyright below.
 *
 * RPC for the Windows NT Operating System COMES WITH ABSOLUTELY NO 
 * WARRANTY, NOR WILL I BE LIABLE FOR ANY DAMAGES INCURRED FROM THE 
 * USE OF. USE ENTIRELY AT YOUR OWN RISK!!!
 *********************************************************************/

/* @(#)clnt_perror.c	2.1 88/07/29 4.0 RPCSRC */
/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 *
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */
#if !defined(lint) && defined(SCCSIDS)
static char sccsid[] = "@(#)clnt_perror.c 1.15 87/10/07 Copyr 1984 Sun Micro";
#endif

/*
 * clnt_perror.c
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 *
 */
#include "all_oncrpc.h"

#ifndef _WIN32
extern char *sys_errlist[];
extern char *sprintf();
#endif
static char *auth_errmsg();

//extern char *strcpy();

static char *buf = NULL;
#define BUFFER_SIZE (256)

static char *
_buf()
{

	if (buf == NULL) {
		buf = (char*)malloc(BUFFER_SIZE);
	}
	return (buf);
}

/*
 * Print reply error info
 */
const char *
clnt_sperror(CLIENT* client, char* msg)
{
	struct rpc_err e;
	void clnt_perrno();
	char *err;
	char *str = _buf();
	char *strstart = str;

	if (str == 0) {
		return (0);
	}
	CLNT_GETERR(client, &e);

#ifdef _WIN32
	sprintf_s(str, BUFFER_SIZE, "%s: ", msg);
#else
	(void) sprintf(str, "%s: ", msg);
#endif
	str += strlen(str);

#ifdef _WIN32
	strcpy_s(str, BUFFER_SIZE, clnt_sperrno(e.re_status));
#else
	(void) strcpy(str, clnt_sperrno(e.re_status));
#endif
	str += strlen(str);

	switch (e.re_status) {
	case RPC_SUCCESS:
	case RPC_CANTENCODEARGS:
	case RPC_CANTDECODERES:
	case RPC_TIMEDOUT:
	case RPC_PROGUNAVAIL:
	case RPC_PROCUNAVAIL:
	case RPC_CANTDECODEARGS:
	case RPC_SYSTEMERROR:
	case RPC_UNKNOWNHOST:
	case RPC_UNKNOWNPROTO:
	case RPC_PMAPFAILURE:
	case RPC_PROGNOTREGISTERED:
	case RPC_FAILED:
		break;

	case RPC_CANTSEND:
	case RPC_CANTRECV:
#ifdef _WIN32
		; //empty statement because declaration is not allowed after a label
		char errorBuffer[BUFFER_SIZE];
		if (strerror_s(errorBuffer, BUFFER_SIZE, e.re_errno) == 0) {
			sprintf_s(str,  BUFFER_SIZE, "; errno = %s",
				errorBuffer);
		}
		else {
			(void)sprintf_s(str, BUFFER_SIZE, "Error %d, ", e.re_errno);
		}
#else
		(void)sprintf(str, "; errno = %s",
			sys_errlist[e.re_errno]);
#endif
		str += strlen(str);
		break;

	case RPC_VERSMISMATCH:
#ifdef _WIN32
		 sprintf_s(str, BUFFER_SIZE,
			"; low version = %lu, high version = %lu",
			e.re_vers.low, e.re_vers.high);
#else
		(void) sprintf(str,
			"; low version = %lu, high version = %lu",
			e.re_vers.low, e.re_vers.high);
#endif
		str += strlen(str);
		break;

	case RPC_AUTHERROR:
		err = auth_errmsg(e.re_why);
#ifdef _WIN32
		sprintf_s(str, BUFFER_SIZE, "; why = ");
#else
		(void) sprintf(str,"; why = ");
#endif
		str += strlen(str);
		if (err != NULL) {
#ifdef _WIN32
			sprintf_s(str, BUFFER_SIZE, "%s", err);
#else
			(void) sprintf(str, "%s",err);
#endif
		} else {
#ifdef _WIN32
			sprintf_s(str, BUFFER_SIZE,
				"(unknown authentication error - %d)",
				(int)e.re_why);
#else
			(void) sprintf(str,
				"(unknown authentication error - %d)",
				(int) e.re_why);
#endif
		}
		str += strlen(str);
		break;

	case RPC_PROGVERSMISMATCH:
#ifdef _WIN32
		sprintf_s(str, BUFFER_SIZE,
			"; low version = %lu, high version = %lu",
			e.re_vers.low, e.re_vers.high);
#else
		(void) sprintf(str,
			"; low version = %lu, high version = %lu",
			e.re_vers.low, e.re_vers.high);
#endif
		str += strlen(str);
		break;

	default:	/* unknown */
#ifdef _WIN32
		sprintf_s(str, BUFFER_SIZE,
			"; s1 = %lu, s2 = %lu",
			e.re_lb.s1, e.re_lb.s2);
#else
		(void) sprintf(str,
			"; s1 = %lu, s2 = %lu",
			e.re_lb.s1, e.re_lb.s2);
#endif
		str += strlen(str);
		break;
	}
#ifdef _WIN32
	sprintf_s(str, BUFFER_SIZE, "\n");
#else
	(void) sprintf(str, "\n");
#endif
	return(strstart) ;
}

void
clnt_perror(CLIENT* client, char* msg)
{
#ifdef _WIN32
	nt_rpc_report(clnt_sperror(client, msg));
#else
	(void) fprintf(stderr,"%s",clnt_sperror(client, msg));
#endif
}


struct rpc_errtab {
	enum clnt_stat status;
	const char *message;
};

static struct rpc_errtab  rpc_errlist[] = {
	{ RPC_SUCCESS,
		"RPC: Success" },
	{ RPC_CANTENCODEARGS,
		"RPC: Can't encode arguments" },
	{ RPC_CANTDECODERES,
		"RPC: Can't decode result" },
	{ RPC_CANTSEND,
		"RPC: Unable to send" },
	{ RPC_CANTRECV,
		"RPC: Unable to receive" },
	{ RPC_TIMEDOUT,
		"RPC: Timed out" },
	{ RPC_VERSMISMATCH,
		"RPC: Incompatible versions of RPC" },
	{ RPC_AUTHERROR,
		"RPC: Authentication error" },
	{ RPC_PROGUNAVAIL,
		"RPC: Program unavailable" },
	{ RPC_PROGVERSMISMATCH,
		"RPC: Program/version mismatch" },
	{ RPC_PROCUNAVAIL,
		"RPC: Procedure unavailable" },
	{ RPC_CANTDECODEARGS,
		"RPC: Server can't decode arguments" },
	{ RPC_SYSTEMERROR,
		"RPC: Remote system error" },
	{ RPC_UNKNOWNHOST,
		"RPC: Unknown host" },
	{ RPC_UNKNOWNPROTO,
		"RPC: Unknown protocol" },
	{ RPC_PMAPFAILURE,
		"RPC: Port mapper failure" },
	{ RPC_PROGNOTREGISTERED,
		"RPC: Program not registered"},
	{ RPC_FAILED,
		"RPC: Failed (unspecified error)"}
};


/*
 * This interface for use by clntrpc
 */
const char * clnt_sperrno(enum clnt_stat stat)
{
	int i;

	for (i = 0; i < sizeof(rpc_errlist)/sizeof(struct rpc_errtab); i++) {
		if (rpc_errlist[i].status == stat) {
			return (rpc_errlist[i].message);
		}
	}
	return ("RPC: (unknown error code)");
}

void
clnt_perrno(enum clnt_stat num)
{
#ifdef _WIN32
	nt_rpc_report(clnt_sperrno(num));
#else
	(void) fprintf(stderr,"%s",clnt_sperrno(num));
#endif
}


const char * clnt_spcreateerror(const char* s)
{
	extern int sys_nerr;
#ifndef _WIN32
	extern char *sys_errlist[];
#endif
	char *str = _buf();

	if (str == 0)
		return(0);
#ifdef _WIN32
	sprintf_s(str, BUFFER_SIZE, "%s: ", s);
	strcat_s(str, BUFFER_SIZE, clnt_sperrno(rpc_createerr.cf_stat));
#else
	(void) sprintf(str, "%s: ", s);
	(void) strcat(str, clnt_sperrno(rpc_createerr.cf_stat));
#endif
	switch (rpc_createerr.cf_stat) {
	case RPC_PMAPFAILURE:
#ifdef _WIN32
		strcat_s(str, BUFFER_SIZE, " - ");
		strcat_s(str, BUFFER_SIZE,
			clnt_sperrno(rpc_createerr.cf_error.re_status));
#else
		(void) strcat(str, " - ");
		(void) strcat(str,
		    clnt_sperrno(rpc_createerr.cf_error.re_status));
#endif
		break;

	case RPC_SYSTEMERROR:
#ifdef _WIN32
		strcat_s(str, BUFFER_SIZE, " - ");
#else
		(void) strcat(str, " - ");
#endif
		if (rpc_createerr.cf_error.re_errno > 0
			&& rpc_createerr.cf_error.re_errno < sys_nerr) {
#ifdef _WIN32
			strcat_s(str, BUFFER_SIZE,
				"internal rpc error");
#else
			(void)strcat(str,
				sys_errlist[rpc_createerr.cf_error.re_errno]);
#endif
		}
		else {
#ifdef _WIN32
			size_t length = strlen(str);
			sprintf_s(&str[length], BUFFER_SIZE - length,  "Error %d",
				rpc_createerr.cf_error.re_errno);
#else
			(void)sprintf(&str[strlen(str)], "Error %d",
				rpc_createerr.cf_error.re_errno);
#endif
		}
		break;
	}
#ifdef _WIN32
	strcat_s(str, BUFFER_SIZE, "\n");
#else
	(void) strcat(str, "\n");
#endif
	return (str);
}

void
clnt_pcreateerror(const char* s)
{
#ifdef _WIN32
	nt_rpc_report(clnt_spcreateerror(s));
#else
	(void) fprintf(stderr,"%s",clnt_spcreateerror(s));
#endif
}

struct auth_errtab {
	enum auth_stat status;
	char *message;
};

static struct auth_errtab auth_errlist[] = {
	{ AUTH_OK,
		"Authentication OK" },
	{ AUTH_BADCRED,
		"Invalid client credential" },
	{ AUTH_REJECTEDCRED,
		"Server rejected credential" },
	{ AUTH_BADVERF,
		"Invalid client verifier" },
	{ AUTH_REJECTEDVERF,
		"Server rejected verifier" },
	{ AUTH_TOOWEAK,
		"Client credential too weak" },
	{ AUTH_INVALIDRESP,
		"Invalid server verifier" },
	{ AUTH_FAILED,
		"Failed (unspecified error)" },
};

static char *
auth_errmsg(stat)
	enum auth_stat stat;
{
	int i;

	for (i = 0; i < sizeof(auth_errlist)/sizeof(struct auth_errtab); i++) {
		if (auth_errlist[i].status == stat) {
			return(auth_errlist[i].message);
		}
	}
	return(NULL);
}
