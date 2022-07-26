#include "example.h"
#include <string.h>
#include <vxi11.h>

int main(int argc, char** argv)
{
    Create_LinkParms cpar;
    Create_LinkResp* MyCreate_LinkResp;
    Device_Link lpar;
    Device_WriteParms wpar;
    Device_WriteResp* wresp;
    Device_ReadParms rpar;
    Device_ReadResp* rresp;
    char DataWrite[200] = "*IDN?\n";
    char DataRead[200] = "";
    char host[19] = "127.0.0.1";
    int toret = 0;
    CLIENT* VXI11Client = 0;

    if (argc > 1) {
        strcpy_s(host, sizeof(host), argv[1]);
    }
    int a = 2;
    for (size_t n = 0; a < argc && n < sizeof(DataWrite) - 2; a += 1) {
        snprintf(DataWrite + n, sizeof(DataWrite) - n - 2, " %s\n", argv[a]);
        n += strlen(argv[a]) + 1;
    }
    DataWrite[sizeof(DataWrite) - 2] = '\n';
    DataWrite[sizeof(DataWrite) - 1] = '\0';
    printf("%s< %s", host, DataWrite);

    WSADATA wsaData;
    int wret = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wret != 0) {
        printf("Winsock intialization failed\n");
        toret = -1;
        goto End;
    }
    if (argc < 2) VXI11Client = clnt_create(host, DEVICE_CORE, DEVICE_CORE_VERSION, "tcp");
    else VXI11Client = clnt_create(argv[1], DEVICE_CORE, DEVICE_CORE_VERSION, "tcp");
    if (VXI11Client == 0) {
        printf("Error creating rpc/vxi client\n");
        toret = -1;
        goto End;
    }
    cpar.clientId = 0; // Not used
    cpar.lockDevice = 0; // No exclusive access
    cpar.lock_timeout = 0;
    cpar.device = "inst1"; // Logical device name
    if ((MyCreate_LinkResp = create_link_1(&cpar, VXI11Client)) == NULL) {
        printf("Error linking to vxi instrument\n");
        toret = -1;
        goto End;
    }

    lpar = MyCreate_LinkResp->lid; // Save link ID for further use
    wpar.lid = lpar;
    wpar.io_timeout = 10000; // in ms
    wpar.lock_timeout = 10000; // in ms
    wpar.flags = 0;
    wpar.data.data_val = DataWrite;
    wpar.data.data_len = (u_int) strlen(DataWrite);
    if ((wresp = device_write_1(&wpar, VXI11Client)) == NULL) {
        printf("Error writing to vxi instrument\n");
        toret = -1;
        goto End;
    }

    rpar.lid = lpar;
    rpar.requestSize = sizeof(DataRead);
    rpar.io_timeout = 10000;
    rpar.lock_timeout = 10000;
    rpar.flags = 0;
    rpar.termChar = '\n';
    if ((rresp = device_read_1(&rpar, VXI11Client)) == NULL) {
        printf("Error reading from vxi instrument\n");
        toret = -1;
        goto End;
    }
    strncpy_s(DataRead, sizeof(DataRead), rresp->data.data_val, rresp->data.data_len);
    DataRead[rresp->data.data_len] = 0;

    printf("%s> %s\n", host, DataRead);

    if (destroy_link_1(&lpar, VXI11Client) == NULL) {
        printf("Error closing communication with vxi instrument\n");
        toret = -1;
        goto End;
    }
    clnt_destroy(VXI11Client);
    WSACleanup();

End:
    //while(1);	
    return toret;

}
