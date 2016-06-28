#include <stdio.h>
#include <rpc/rpc.h>
#include <rpc/pmap_cln.h>
#include "vxi11_prot.h"

static void device_async_1();
static void device_core_1();

void vxi11_svc_main()
{
	SVCXPRT *transp;

#ifdef WIN32
	rpc_nt_init();
#endif

	(void)pmap_unset(DEVICE_ASYNC, DEVICE_ASYNC_VERSION);
	(void)pmap_unset(DEVICE_CORE, DEVICE_CORE_VERSION);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		(void)fprintf(stderr, "cannot create udp service.\n");
#ifdef WIN32
		rpc_nt_exit();
#endif
		exit(1);
	}
	if (!svc_register(transp, DEVICE_ASYNC, DEVICE_ASYNC_VERSION, device_async_1, IPPROTO_UDP)) {
		(void)fprintf(stderr, "unable to register (DEVICE_ASYNC, DEVICE_ASYNC_VERSION, udp).\n");
#ifdef WIN32
		rpc_nt_exit();
#endif
		exit(1);
	}
	if (!svc_register(transp, DEVICE_CORE, DEVICE_CORE_VERSION, device_core_1, IPPROTO_UDP)) {
		(void)fprintf(stderr, "unable to register (DEVICE_CORE, DEVICE_CORE_VERSION, udp).\n");
#ifdef WIN32
		rpc_nt_exit();
#endif
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		(void)fprintf(stderr, "cannot create tcp service.\n");
#ifdef WIN32
		rpc_nt_exit();
#endif
		exit(1);
	}
	if (!svc_register(transp, DEVICE_ASYNC, DEVICE_ASYNC_VERSION, device_async_1, IPPROTO_TCP)) {
		(void)fprintf(stderr, "unable to register (DEVICE_ASYNC, DEVICE_ASYNC_VERSION, tcp).\n");
#ifdef WIN32
		rpc_nt_exit();
#endif
		exit(1);
	}
	if (!svc_register(transp, DEVICE_CORE, DEVICE_CORE_VERSION, device_core_1, IPPROTO_TCP)) {
		(void)fprintf(stderr, "unable to register (DEVICE_CORE, DEVICE_CORE_VERSION, tcp).\n");
#ifdef WIN32
		rpc_nt_exit();
#endif
		exit(1);
	}
	svc_run();
	(void)fprintf(stderr, "svc_run returned\n");
#ifdef WIN32
	rpc_nt_exit();
#endif
	exit(1);
}

#ifdef MULTITHREAD
struct call_params {
	struct svc_req *rqstp;
	SVCXPRT *transp;
	void *arg;
	char *(*local)();
	bool_t (*xdr_argument)(), (*xdr_result)();
};
static void device_async_1_a ();
#endif

static void
device_async_1(rqstp, transp)
	struct svc_req *rqstp;
	SVCXPRT *transp;
{
	union {
		Device_Link device_abort_1_arg;
	} argument;
	char *result;
	bool_t (*xdr_argument)(), (*xdr_result)();
	char *(*local)();


#ifdef MULTITHREAD
	DWORD TID = 0;
	HANDLE threadHandle = NULL;
	struct call_params	*params;
#endif
	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void)svc_sendreply(transp, xdr_void, (char *)NULL);
		return;

	case device_abort:
		xdr_argument = xdr_Device_Link;
		xdr_result = xdr_Device_Error;
		local = (char *(*)()) device_abort_1;
		break;

	default:
		svcerr_noproc(transp);
		return;
	}
	bzero((char *)&argument, sizeof(argument));
	if (!svc_getargs(transp, xdr_argument, &argument)) {
		svcerr_decode(transp);
		return;
	}

#ifdef MULTITHREAD
	params = (struct call_params*) malloc (sizeof(struct call_params));

	params->rqstp = (struct svc_req*) malloc (sizeof(struct svc_req));
	bcopy(rqstp, params->rqstp, sizeof(struct svc_req));

	params->transp = transp;

	params->arg = malloc (sizeof(argument));
	bcopy(&argument, params->arg, sizeof(argument));

	params->local = local;

	params->xdr_argument = xdr_argument;
	params->xdr_result = xdr_result;

	threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)device_async_1_a, params, 0, &TID);
	if (!threadHandle)
		device_async_1_a(&params);
}

static void
device_async_1_a(params)
struct call_params *params;
{
	struct svc_req *rqstp;
	SVCXPRT *transp;
	void *argument;
	void (*destroy_proc)();

	char *result;
	bool_t (*xdr_argument)(), (*xdr_result)();
	char *(*local)();


	argument = params->arg;
	rqstp = params->rqstp;
	transp = params->transp;
	xdr_argument = params->xdr_argument;
	xdr_result = params->xdr_result;
	local = params->local;
	destroy_proc = transp->xp_ops->xp_destroy;
	transp->xp_ops->xp_destroy = xprt_unregister;
#endif

	result = (*local)(&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, xdr_result, result)) {
		svcerr_systemerr(transp);
	}
#ifdef MULTITHREAD
	free(params->rqstp);
	free(params);
#endif
	if (!svc_freeargs(transp, xdr_argument, &argument)) {
		(void)fprintf(stderr, "unable to free arguments\n");
#ifdef WIN32
		rpc_nt_exit();
#endif
		exit(1);
	}
#ifdef MULTITHREAD
	free(argument);
	transp->xp_ops->xp_destroy = destroy_proc;
#endif
}


#ifdef MULTITHREAD
struct call_params {
	struct svc_req *rqstp;
	SVCXPRT *transp;
	void *arg;
	char *(*local)();
	bool_t (*xdr_argument)(), (*xdr_result)();
};
static void device_core_1_a ();
#endif

static void
device_core_1(rqstp, transp)
	struct svc_req *rqstp;
	SVCXPRT *transp;
{
	union {
		Create_LinkParms create_link_1_arg;
		Device_WriteParms device_write_1_arg;
		Device_ReadParms device_read_1_arg;
		Device_GenericParms device_readstb_1_arg;
		Device_GenericParms device_trigger_1_arg;
		Device_GenericParms device_clear_1_arg;
		Device_GenericParms device_remote_1_arg;
		Device_GenericParms device_local_1_arg;
		Device_LockParms device_lock_1_arg;
		Device_Link device_unlock_1_arg;
		Device_EnableSrqParms device_enable_srq_1_arg;
		Device_DocmdParms device_docmd_1_arg;
		Device_Link destroy_link_1_arg;
		Device_RemoteFunc create_intr_chan_1_arg;
	} argument;
	char *result;
	bool_t (*xdr_argument)(), (*xdr_result)();
	char *(*local)();


#ifdef MULTITHREAD
	DWORD TID = 0;
	HANDLE threadHandle = NULL;
	struct call_params	*params;
#endif
	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void)svc_sendreply(transp, xdr_void, (char *)NULL);
		return;

	case create_link:
		xdr_argument = xdr_Create_LinkParms;
		xdr_result = xdr_Create_LinkResp;
		local = (char *(*)()) create_link_1;
		break;

	case device_write:
		xdr_argument = xdr_Device_WriteParms;
		xdr_result = xdr_Device_WriteResp;
		local = (char *(*)()) device_write_1;
		break;

	case device_read:
		xdr_argument = xdr_Device_ReadParms;
		xdr_result = xdr_Device_ReadResp;
		local = (char *(*)()) device_read_1;
		break;

	case device_readstb:
		xdr_argument = xdr_Device_GenericParms;
		xdr_result = xdr_Device_ReadStbResp;
		local = (char *(*)()) device_readstb_1;
		break;

	case device_trigger:
		xdr_argument = xdr_Device_GenericParms;
		xdr_result = xdr_Device_Error;
		local = (char *(*)()) device_trigger_1;
		break;

	case device_clear:
		xdr_argument = xdr_Device_GenericParms;
		xdr_result = xdr_Device_Error;
		local = (char *(*)()) device_clear_1;
		break;

	case device_remote:
		xdr_argument = xdr_Device_GenericParms;
		xdr_result = xdr_Device_Error;
		local = (char *(*)()) device_remote_1;
		break;

	case device_local:
		xdr_argument = xdr_Device_GenericParms;
		xdr_result = xdr_Device_Error;
		local = (char *(*)()) device_local_1;
		break;

	case device_lock:
		xdr_argument = xdr_Device_LockParms;
		xdr_result = xdr_Device_Error;
		local = (char *(*)()) device_lock_1;
		break;

	case device_unlock:
		xdr_argument = xdr_Device_Link;
		xdr_result = xdr_Device_Error;
		local = (char *(*)()) device_unlock_1;
		break;

	case device_enable_srq:
		xdr_argument = xdr_Device_EnableSrqParms;
		xdr_result = xdr_Device_Error;
		local = (char *(*)()) device_enable_srq_1;
		break;

	case device_docmd:
		xdr_argument = xdr_Device_DocmdParms;
		xdr_result = xdr_Device_DocmdResp;
		local = (char *(*)()) device_docmd_1;
		break;

	case destroy_link:
		xdr_argument = xdr_Device_Link;
		xdr_result = xdr_Device_Error;
		local = (char *(*)()) destroy_link_1;
		break;

	case create_intr_chan:
		xdr_argument = xdr_Device_RemoteFunc;
		xdr_result = xdr_Device_Error;
		local = (char *(*)()) create_intr_chan_1;
		break;

	case destroy_intr_chan:
		xdr_argument = xdr_void;
		xdr_result = xdr_Device_Error;
		local = (char *(*)()) destroy_intr_chan_1;
		break;

	default:
		svcerr_noproc(transp);
		return;
	}
	bzero((char *)&argument, sizeof(argument));
	if (!svc_getargs(transp, xdr_argument, &argument)) {
		svcerr_decode(transp);
		return;
	}

#ifdef MULTITHREAD
	params = (struct call_params*) malloc (sizeof(struct call_params));

	params->rqstp = (struct svc_req*) malloc (sizeof(struct svc_req));
	bcopy(rqstp, params->rqstp, sizeof(struct svc_req));

	params->transp = transp;

	params->arg = malloc (sizeof(argument));
	bcopy(&argument, params->arg, sizeof(argument));

	params->local = local;

	params->xdr_argument = xdr_argument;
	params->xdr_result = xdr_result;

	threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)device_core_1_a, params, 0, &TID);
	if (!threadHandle)
		device_core_1_a(&params);
}

static void
device_core_1_a(params)
struct call_params *params;
{
	struct svc_req *rqstp;
	SVCXPRT *transp;
	void *argument;
	void (*destroy_proc)();

	char *result;
	bool_t (*xdr_argument)(), (*xdr_result)();
	char *(*local)();


	argument = params->arg;
	rqstp = params->rqstp;
	transp = params->transp;
	xdr_argument = params->xdr_argument;
	xdr_result = params->xdr_result;
	local = params->local;
	destroy_proc = transp->xp_ops->xp_destroy;
	transp->xp_ops->xp_destroy = xprt_unregister;
#endif

	result = (*local)(&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, xdr_result, result)) {
		svcerr_systemerr(transp);
	}
#ifdef MULTITHREAD
	free(params->rqstp);
	free(params);
#endif
	if (!svc_freeargs(transp, xdr_argument, &argument)) {
		(void)fprintf(stderr, "unable to free arguments\n");
#ifdef WIN32
		rpc_nt_exit();
#endif
		exit(1);
	}
#ifdef MULTITHREAD
	free(argument);
	transp->xp_ops->xp_destroy = destroy_proc;
#endif
}

