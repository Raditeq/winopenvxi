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

/* @(#)clnt_generic.c	2.2 88/08/01 4.0 RPCSRC */
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
static char sccsid[] = "@(#)clnt_generic.c 1.4 87/08/11 (C) 1987 SMI";
#endif
/*
 * Copyright (C) 1987, Sun Microsystems, Inc.
 */

#include "all_oncrpc.h"

/*
 * Generic client creation: takes (hostname, program-number, protocol) and
 * returns client handle. Default options are set, which the user can 
 * change using the rpc equivalent of ioctl()'s.
 */
CLIENT *
clnt_create(hostname, prog, vers, proto)
	char *hostname;
	unsigned prog;
	unsigned vers;
	char *proto;
{
	struct protoent *p;
	struct sockaddr_in sin;
	socket_t sock = { .fd = RPC_ANYSOCK };
	struct timeval tv;
	CLIENT *client;
	struct addrinfo* addressInfo = NULL;

	int error = getaddrinfo(hostname, NULL, NULL, &addressInfo);

	if (error != 0 || addressInfo == NULL) {
		rpc_createerr.cf_stat = RPC_UNKNOWNHOST;
		return (NULL);
	}
	if (addressInfo->ai_family != AF_INET) {
		/*
		 * Only support INET for now
		 */
		rpc_createerr.cf_stat = RPC_SYSTEMERROR;
#ifdef _WIN32
		rpc_createerr.cf_error.re_errno = WSAEAFNOSUPPORT; 
#else
		rpc_createerr.cf_error.re_errno = EAFNOSUPPORT; 
#endif
		return (NULL);
	}
	sin.sin_family = addressInfo->ai_family;
	sin.sin_port = 0;
	bzero(sin.sin_zero, sizeof(sin.sin_zero));
	struct sockaddr_in* address = (struct sockaddr_in*)addressInfo->ai_addr;
	bcopy((char*)&address->sin_addr, (char*)&sin.sin_addr, sizeof(IN_ADDR));
	p = getprotobyname(proto);
	if (p == NULL) {
		rpc_createerr.cf_stat = RPC_UNKNOWNPROTO;
#ifdef _WIN32
		rpc_createerr.cf_error.re_errno = WSAEPFNOSUPPORT;
#else
		rpc_createerr.cf_error.re_errno = EPFNOSUPPORT;
#endif 
		return (NULL);
	}
	sock.fd = RPC_ANYSOCK;
	switch (p->p_proto) {
	case IPPROTO_UDP:
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		client = clntudp_create(&sin, prog, vers, tv, &sock);
		if (client == NULL) {
			return (NULL);
		}
		tv.tv_sec = 25;
		clnt_control(client, CLSET_TIMEOUT, &tv);
		break;
	case IPPROTO_TCP:
		client = clnttcp_create(&sin, prog, vers, &sock, 0, 0);
		if (client == NULL) {
			return (NULL);
		}
		tv.tv_sec = 25;
		tv.tv_usec = 0;
		clnt_control(client, CLSET_TIMEOUT, &tv);
		break;
	default:
		rpc_createerr.cf_stat = RPC_SYSTEMERROR;
#ifdef _WIN32
		rpc_createerr.cf_error.re_errno = WSAEPFNOSUPPORT; 
#else
		rpc_createerr.cf_error.re_errno = EPFNOSUPPORT; 
#endif
		return (NULL);
	}
	return (client);
}

void
clnt_destroy(rh)
	CLIENT *rh;
{
  CLNT_DESTROY(rh);
}

