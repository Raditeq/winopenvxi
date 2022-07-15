#include <rpc/Rpc.h>
#include "vxi11_prot.h"


bool_t
xdr_Device_Link(XDR* xdrs, Device_Link* objp)
{
	if (!xdr_long(xdrs, objp)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_Device_AddrFamily(XDR* xdrs, Device_AddrFamily* objp)
{
	if (!xdr_enum(xdrs, (enum_t *)objp)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_Device_Flags(XDR* xdrs, Device_Flags* objp)
{
	if (!xdr_long(xdrs, objp)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_Device_ErrorCode(XDR* xdrs, Device_ErrorCode* objp)
{
	if (!xdr_long(xdrs, objp)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_Device_Error(XDR* xdrs, Device_Error* objp)
{
	if (!xdr_Device_ErrorCode(xdrs, &objp->error)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_Create_LinkParms(XDR* xdrs, Create_LinkParms* objp)
{
	if (!xdr_long(xdrs, &objp->clientId)) {
		return (FALSE);
	}
	if (!xdr_bool(xdrs, &objp->lockDevice)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->lock_timeout)) {
		return (FALSE);
	}
	const size_t length = strlen(objp->device);
	char* device = (char *) malloc(length);
	if (device == NULL) {
		return (FALSE);
	}
	strcpy_s(device, length, objp->device);
	const bool_t result = xdr_string(xdrs, &device, ~(u_int)0);
	free(device);
	return result;
}




bool_t
xdr_Create_LinkResp(XDR* xdrs, Create_LinkResp* objp)
{
	if (!xdr_Device_ErrorCode(xdrs, &objp->error)) {
		return (FALSE);
	}
	if (!xdr_Device_Link(xdrs, &objp->lid)) {
		return (FALSE);
	}
	if (!xdr_u_short(xdrs, &objp->abortPort)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->maxRecvSize)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_Device_WriteParms(XDR* xdrs, Device_WriteParms* objp)
{
	if (!xdr_Device_Link(xdrs, &objp->lid)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->io_timeout)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->lock_timeout)) {
		return (FALSE);
	}
	if (!xdr_Device_Flags(xdrs, &objp->flags)) {
		return (FALSE);
	}
	if (!xdr_bytes(xdrs, (char **)&objp->data.data_val, &objp->data.data_len, ~(u_int)0)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_Device_WriteResp(XDR* xdrs, Device_WriteResp* objp)
{
	if (!xdr_Device_ErrorCode(xdrs, &objp->error)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->size)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_Device_ReadParms(XDR* xdrs, Device_ReadParms* objp)
{
	if (!xdr_Device_Link(xdrs, &objp->lid)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->requestSize)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->io_timeout)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->lock_timeout)) {
		return (FALSE);
	}
	if (!xdr_Device_Flags(xdrs, &objp->flags)) {
		return (FALSE);
	}
	if (!xdr_char(xdrs, &objp->termChar)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_Device_ReadResp(XDR* xdrs, Device_ReadResp* objp)
{
	if (!xdr_Device_ErrorCode(xdrs, &objp->error)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->reason)) {
		return (FALSE);
	}
	if (!xdr_bytes(xdrs, &objp->data.data_val, &objp->data.data_len, ~(u_int)0)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_Device_ReadStbResp(XDR* xdrs, Device_ReadStbResp* objp)
{
	if (!xdr_Device_ErrorCode(xdrs, &objp->error)) {
		return (FALSE);
	}
	if (!xdr_u_char(xdrs, &objp->stb)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_Device_GenericParms(XDR* xdrs, Device_GenericParms* objp)
{
	if (!xdr_Device_Link(xdrs, &objp->lid)) {
		return (FALSE);
	}
	if (!xdr_Device_Flags(xdrs, &objp->flags)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->lock_timeout)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->io_timeout)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_Device_RemoteFunc(XDR* xdrs, Device_RemoteFunc* objp)
{
	if (!xdr_u_long(xdrs, &objp->hostAddr)) {
		return (FALSE);
	}
	if (!xdr_u_short(xdrs, &objp->hostPort)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->progNum)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->progVers)) {
		return (FALSE);
	}
	if (!xdr_Device_AddrFamily(xdrs, &objp->progFamily)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_Device_EnableSrqParms(XDR* xdrs, Device_EnableSrqParms* objp)
{
	if (!xdr_Device_Link(xdrs, &objp->lid)) {
		return (FALSE);
	}
	if (!xdr_bool(xdrs, &objp->enable)) {
		return (FALSE);
	}
	if (!xdr_bytes(xdrs, (char **)&objp->handle.handle_val, &objp->handle.handle_len, 40)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_Device_LockParms(XDR* xdrs, Device_LockParms* objp)
{
	if (!xdr_Device_Link(xdrs, &objp->lid)) {
		return (FALSE);
	}
	if (!xdr_Device_Flags(xdrs, &objp->flags)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->lock_timeout)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_Device_DocmdParms(XDR* xdrs, Device_DocmdParms* objp)
{
	if (!xdr_Device_Link(xdrs, &objp->lid)) {
		return (FALSE);
	}
	if (!xdr_Device_Flags(xdrs, &objp->flags)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->io_timeout)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->lock_timeout)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->cmd)) {
		return (FALSE);
	}
	if (!xdr_bool(xdrs, &objp->network_order)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->datasize)) {
		return (FALSE);
	}
	if (!xdr_bytes(xdrs, &objp->data_in.data_in_val, &objp->data_in.data_in_len, ~(size_t)0)) {
		return (FALSE);
	}
	return (TRUE);
}




bool_t
xdr_Device_DocmdResp(XDR* xdrs, Device_DocmdResp* objp)
{
	if (!xdr_Device_ErrorCode(xdrs, &objp->error)) {
		return (FALSE);
	}
	if (!xdr_bytes(xdrs, &objp->data_out.data_out_val, &objp->data_out.data_out_len, ~(size_t)0)) {
		return (FALSE);
	}
	return (TRUE);
}
