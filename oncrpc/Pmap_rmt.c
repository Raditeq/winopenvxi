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
/* @(#)pmap_rmt.c	2.2 88/08/01 4.0 RPCSRC */
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
static char sccsid[] = "@(#)pmap_rmt.c 1.21 87/08/27 Copyr 1984 Sun Micro";
#endif

/*
 * pmap_rmt.c
 * Client interface to pmap rpc service.
 * remote call and broadcast service
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

#include "all_oncrpc.h"
#include "pmap_tmt.h"

#define MAX_BROADCAST_SIZE 1400

#ifndef _WIN32
extern int errno;
#endif

/*
 * pmapper remote-call-service interface.
 * This routine is used to call the pmapper remote call service
 * which will look up a service program in the port maps, and then
 * remotely call that routine with the given parameters.  This allows
 * programs to do a lookup and call in one step.
*/
enum clnt_stat
pmap_rmtcall(struct sockaddr_in* addr, const u_long prog, const u_long vers, const u_long proc, xdrproc_t xdrargs, caddr_t argsp, xdrproc_t xdrres, caddr_t resp, const struct timeval tout, u_long* port_ptr)
{
	socket_t socket;
	register CLIENT *client;
	struct rmtcallargs a;
	struct rmtcallres r;
	enum clnt_stat stat;

	addr->sin_port = htons(PMAPPORT);
	client = clntudp_create(addr, PMAPPROG, PMAPVERS, pmap_clientTimeout, &socket);
	if (client != (CLIENT *)NULL) {
		a.prog = prog;
		a.vers = vers;
		a.proc = proc;
		a.args_ptr = argsp;
		a.xdr_args = xdrargs;
		r.port_ptr = port_ptr;
		r.results_ptr = resp;
		r.xdr_results = xdrres;
		stat = CLNT_CALL(client, PMAPPROC_CALLIT, xdr_rmtcall_args, &a,
		    xdr_rmtcallres, &r, tout);
		CLNT_DESTROY(client);
	} else {
		stat = RPC_FAILED;
	}
#ifdef _WIN32
	(void)closesocket(socket.socket);
#else
	(void)close(socket.socket);
#endif
	addr->sin_port = 0;
	return (stat);
}


/*
 * XDR remote call arguments
 * written for XDR_ENCODE direction only
 */
bool_t
xdr_rmtcall_args(register XDR* xdrs, register struct rmtcallargs* cap)
{
	size_t lenposition, argposition, position;

	if (xdr_u_long(xdrs, &(cap->prog)) &&
	    xdr_u_long(xdrs, &(cap->vers)) &&
	    xdr_u_long(xdrs, &(cap->proc))) {
		lenposition = XDR_GETPOS(xdrs);
		if (! xdr_u_long(xdrs, &(cap->arglen)))
		    return (FALSE);
		argposition = XDR_GETPOS(xdrs);
		if (! (*(cap->xdr_args))(xdrs, cap->args_ptr))
		    return (FALSE);
		position = XDR_GETPOS(xdrs);
		cap->arglen = position - argposition;
		XDR_SETPOS(xdrs, lenposition);
		if (! xdr_u_long(xdrs, &(cap->arglen)))
		    return (FALSE);
		XDR_SETPOS(xdrs, position);
		return (TRUE);
	}
	return (FALSE);
}

/*
 * XDR remote call results
 * written for XDR_DECODE direction only
 */
bool_t
xdr_rmtcallres(register XDR* xdrs, register struct rmtcallres* crp)
{
	caddr_t port_ptr;

	port_ptr = (caddr_t)crp->port_ptr;
	if (xdr_reference(xdrs, &port_ptr, sizeof (u_long),
	    xdr_u_long) && xdr_u_long(xdrs, (u_long *) &crp->resultslen)) {
		crp->port_ptr = (u_long*)port_ptr;
		return ((*(crp->xdr_results))(xdrs, crp->results_ptr));
	}
	return (FALSE);
}


/*
 * The following is kludged-up support for simple rpc broadcasts.
 * Someday a large, complicated system will replace these trivial
 * routines which only support udp/ip .
 */

/**
 * \param addrs
 * \param sock any valid socket will do
 * \param buf why allocxate more when we can use existing...
 */
static int
getbroadcastnets(struct in_addr* addrs, socket_t sock, char *buf)
{
#ifdef _WIN32
	/* try to do a global broadcast, this is not a clean solution */
	addrs[0].s_addr = INADDR_BROADCAST;
	return 1;

	/* was return 0 in version 1.04 */

#else
	struct ifconf ifc;
        struct ifreq ifreq, *ifr;
	struct sockaddr_in *sin;
        int n, i;

        ifc.ifc_len = UDPMSGSIZE;
        ifc.ifc_buf = buf;
        if (ioctl(sock.socket, SIOCGIFCONF, (char *)&ifc) < 0) {
                perror("broadcast: ioctl (get interface configuration)");
                return (0);
        }
        ifr = ifc.ifc_req;
        for (i = 0, n = ifc.ifc_len/sizeof (struct ifreq); n > 0; n--, ifr++) {
                ifreq = *ifr;
                if (ioctl(sock.socket, SIOCGIFFLAGS, (char *)&ifreq) < 0) {
                        perror("broadcast: ioctl (get interface flags)");
                        continue;
                }
                if ((ifreq.ifr_flags & IFF_BROADCAST) &&
		    (ifreq.ifr_flags & IFF_UP) &&
		    ifr->ifr_addr.sa_family == AF_INET) {
			sin = (struct sockaddr_in *)&ifr->ifr_addr;
#ifdef SIOCGIFBRDADDR   /* 4.3BSD */
			if (ioctl(sock.socket, SIOCGIFBRDADDR, (char *)&ifreq) < 0) {
				addrs[i++] = inet_makeaddr(inet_netof
			    (sin->sin_addr.s_addr), INADDR_ANY);
			} else {
				addrs[i++] = ((struct sockaddr_in*)
				  &ifreq.ifr_addr)->sin_addr;
			}
#else /* 4.2 BSD */
			addrs[i++] = inet_makeaddr(inet_netof
			  (sin->sin_addr.s_addr), INADDR_ANY);
#endif
		}
	}
	return (i);
#endif
}

/**
 * \brief Send a broadacst message on the local network.
 * \param prog program number
 * \param vers version number
 * \param proc procedure number
 * \param xargs xdr routine for args
 * \param argsp pointer to args
 * \param xresults xdr routine for results
 * \param resultsp pointer to results
 * \param eachresult call with each result obtained
 * \return The status of the call. will return RPC_SUCCESS if everything went correctly
 */
enum clnt_stat
clnt_broadcast(u_long prog, u_long vers, u_long proc, xdrproc_t xargs, caddr_t argsp, xdrproc_t xresults, caddr_t resultsp, resultproc_t eachresult)
{
	enum clnt_stat stat;
	AUTH *unix_auth = authunix_create_default();
	XDR xdr_stream;
	register XDR *xdrs = &xdr_stream;
	size_t outlen, nets;
	int inlen, fromlen;
	socket_t sock;
	int on = 1;
#ifdef FD_SETSIZE
	fd_set mask;
	fd_set readfds;
#else
	int readfds;
	register int mask;
#endif /* def FD_SETSIZE */
	register int i;
	bool_t done = FALSE;
	register u_long xid;
	u_long port;
	struct in_addr addrs[20];
	struct sockaddr_in baddr, raddr; /* broadcast and response addresses */
	struct rmtcallargs a;
	struct rmtcallres r;
	struct rpc_msg msg;
	struct timeval t;
	char outbuf[MAX_BROADCAST_SIZE], inbuf[UDPMSGSIZE];

	/*
	 * initialization: create a socket, a broadcast address, and
	 * preserialize the arguments into a send buffer.
	 */
#ifdef _WIN32
	if ((sock.socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
#else
	if ((sock.socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
#endif
		perror("Cannot create socket for broadcast rpc");
		stat = RPC_CANTSEND;
		goto done_broad;
	}
#ifdef SO_BROADCAST
	if (setsockopt(sock.socket, SOL_SOCKET, SO_BROADCAST, (const char*)&on, sizeof (int)) < 0) {
		perror("Cannot set socket option SO_BROADCAST");
		stat = RPC_CANTSEND;
		goto done_broad;
	}
#endif /* def SO_BROADCAST */
#ifdef FD_SETSIZE
	FD_ZERO(&mask);
	FD_SET(sock.socket, &mask);
#else
	mask = (1 << sock);
#endif /* def FD_SETSIZE */
	nets = getbroadcastnets(addrs, sock, inbuf);
	bzero((char *)&baddr, sizeof (baddr));
	baddr.sin_family = AF_INET;
	baddr.sin_port = htons(PMAPPORT);
	baddr.sin_addr.s_addr = htonl(INADDR_ANY);
/*	baddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY); */
	(void)gettimeofday(&t, (struct timezone *)0);
#ifdef _WIN32
	msg.rm_xid = xid = _getpid() ^ t.tv_sec ^ t.tv_usec;
#else
	msg.rm_xid = xid = getpid() ^ t.tv_sec ^ t.tv_usec;
#endif
	t.tv_usec = 0;
	msg.rm_direction = CALL;
	msg.rm_call.cb_rpcvers = RPC_MSG_VERSION;
	msg.rm_call.cb_prog = PMAPPROG;
	msg.rm_call.cb_vers = PMAPVERS;
	msg.rm_call.cb_proc = PMAPPROC_CALLIT;
	msg.rm_call.cb_cred = unix_auth->ah_cred;
	msg.rm_call.cb_verf = unix_auth->ah_verf;
	a.prog = prog;
	a.vers = vers;
	a.proc = proc;
	a.xdr_args = xargs;
	a.args_ptr = argsp;
	r.port_ptr = &port;
	r.xdr_results = xresults;
	r.results_ptr = resultsp;
	xdrmem_create(xdrs, outbuf, MAX_BROADCAST_SIZE, XDR_ENCODE);
	if ((! xdr_callmsg(xdrs, &msg)) || (! xdr_rmtcall_args(xdrs, &a))) {
		stat = RPC_CANTENCODEARGS;
		goto done_broad;
	}
	outlen = (int)xdr_getpos(xdrs);
	xdr_destroy(xdrs);
	/*
	 * Basic loop: broadcast a packet and wait a while for response(s).
	 * The response timeout grows larger per iteration.
	 */
	for (t.tv_sec = 4; t.tv_sec <= 14; t.tv_sec += 2) {
		for (i = 0; i < nets; i++) {
			baddr.sin_addr = addrs[i];
			if (sendto(sock.socket, outbuf, outlen, 0,
				(struct sockaddr *)&baddr,
				sizeof (struct sockaddr)) != outlen) {
				perror("Cannot send broadcast packet");
				stat = RPC_CANTSEND;
				goto done_broad;
			}
		}
		if (eachresult == NULL) {
			stat = RPC_SUCCESS;
			goto done_broad;
		}
	recv_again:
		msg.acpted_rply.ar_verf = _null_auth;
		msg.acpted_rply.ar_results.where = (caddr_t)&r;
                msg.acpted_rply.ar_results.proc = xdr_rmtcallres;
		readfds = mask;
#ifdef _WIN32
		switch (select(0 /* unused in winsock */, &readfds, NULL,
			       NULL, &t)) {
#else
		switch (select(_rpc_dtablesize(), &readfds, (int *)NULL, 
			       (int *)NULL, &t)) {
#endif
		case 0:  /* timed out */
			stat = RPC_TIMEDOUT;
			continue;

		case -1:  /* some kind of error */
#ifdef _WIN32
			if (WSAerrno == EINTR)
#else
			if (errno == EINTR)
#endif
				goto recv_again;
			perror("Broadcast select problem");
			stat = RPC_CANTRECV;
			goto done_broad;

		}  /* end of select results switch */
	try_again:
		fromlen = sizeof(struct sockaddr);
		inlen = recvfrom(sock.socket, inbuf, UDPMSGSIZE, 0,
			(struct sockaddr *)&raddr, &fromlen);
		if (inlen < 0) {
#ifdef _WIN32
			if (WSAerrno == EINTR)
#else
			if (errno == EINTR)
#endif
				goto try_again;
			perror("Cannot receive reply to broadcast");
			stat = RPC_CANTRECV;
			goto done_broad;
		}
		if (inlen < sizeof(u_long))
			goto recv_again;
		/*
		 * see if reply transaction id matches sent id.
		 * If so, decode the results.
		 */
		xdrmem_create(xdrs, inbuf, (u_int)inlen, XDR_DECODE);
		if (xdr_replymsg(xdrs, &msg)) {
			if ((msg.rm_xid == xid) &&
				(msg.rm_reply.rp_stat == MSG_ACCEPTED) &&
				(msg.acpted_rply.ar_stat == SUCCESS)) {
				raddr.sin_port = htons((u_short)port);
				done = (*eachresult)(resultsp, &raddr);
			}
			/* otherwise, we just ignore the errors ... */
		} else {
#ifdef notdef
			/* some kind of deserialization problem ... */
			if (msg.rm_xid == xid)
#ifdef _WIN32
				nt_rpc_report("Broadcast deserialization problem");
#else
				fprintf(stderr, "Broadcast deserialization problem");
#endif
			/* otherwise, just random garbage */
#endif
		}
		xdrs->x_op = XDR_FREE;
		msg.acpted_rply.ar_results.proc = xdr_void;
		(void)xdr_replymsg(xdrs, &msg);
		(void)(*xresults)(xdrs, resultsp);
		xdr_destroy(xdrs);
		if (done) {
			stat = RPC_SUCCESS;
			goto done_broad;
		} else {
			goto recv_again;
		}
	}
done_broad:
#ifdef _WIN32
	(void)closesocket(sock.socket);
#else
	(void)close(sock.socket);
#endif
	AUTH_DESTROY(unix_auth);
	return (stat);
}

