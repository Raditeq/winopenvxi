
// This file is in the Public Domain:
// - You can do anything you want with it.
// - You have no legal obligation to do anything else.
// - There is NO WARRANTY of any kind.

#ifndef _VXI11_H_
#define _VXI11_H_

#define	bool_t int
typedef char *caddr_t;
typedef unsigned int u_int;
typedef unsigned long u_long;
typedef unsigned short u_short;

typedef struct CLIENT CLIENT;
CLIENT *clnt_create(char *hostname, unsigned prog, unsigned vers, char *proto);
void clnt_destroy(CLIENT* rh);

#define DEVICE_CORE ((u_long)0x0607AF)
#define DEVICE_CORE_VERSION ((u_long)1)

typedef long Device_Link;
typedef long Device_Flags;
typedef long Device_ErrorCode;

typedef struct {
	Device_ErrorCode error;
	Device_Link lid;
	u_short abortPort;
	u_long maxRecvSize;
} Create_LinkResp;

typedef struct {
	Device_ErrorCode error;
	u_long size;
} Device_WriteResp;

typedef struct {
	Device_ErrorCode error;
	long reason;
	struct {
		u_int data_len;
		char *data_val;
	} data;
} Device_ReadResp;

struct Create_LinkParms {
	long clientId;
	bool_t lockDevice;
	u_long lock_timeout;
	char *device;
};
typedef struct Create_LinkParms Create_LinkParms;

typedef struct {
	Device_Link lid;
	u_long io_timeout;
	u_long lock_timeout;
	Device_Flags flags;
	struct {
		u_int data_len;
		char *data_val;
	} data;
} Device_WriteParms	;

typedef struct {
	Device_Link lid;
	u_long requestSize;
	u_long io_timeout;
	u_long lock_timeout;
	Device_Flags flags;
	char termChar;
} Device_ReadParms;

typedef struct {
	Device_ErrorCode error;
} Device_Error;

Create_LinkResp * create_link_1(Create_LinkParms *argp, CLIENT *clnt);
Device_WriteResp * device_write_1(Device_WriteParms *argp, CLIENT *clnt);
Device_ReadResp * device_read_1(Device_ReadParms *argp, CLIENT *clnt);
Device_Error *destroy_link_1(Device_Link *argp, CLIENT *clnt);

#endif // _VXI11_H_

