#include "example.h"

#include <stdint.h>
#include <string.h>
#include <vxi11.h>
#include <synchapi.h>


CLIENT* CreateClient(char* host) {
	CLIENT* client = clnt_create(host, DEVICE_CORE, DEVICE_CORE_VERSION, "tcp");
	if (client == NULL) {
		printf("Error creating rpc/vxi client\n");
	}
	return client;
}

Device_Link CreateLink(CLIENT* client, const char* deviceID) {
	Create_LinkParms cpar;
	cpar.clientId = 0; // Not used
	cpar.lockDevice = 0; // No exclusive access
	cpar.lock_timeout = 0;
	cpar.device = deviceID; // Logical device name

	Create_LinkResp* linkResponse = create_link_1(&cpar, client);
	if (linkResponse == NULL || linkResponse->error != 0) {
		printf("Error linking to vxi instrument\n");
		return 0;
	}
	return linkResponse->lid;
}

int DeviceWrite(CLIENT* client, const Device_Link link, const char* data, const size_t dataSize) {
	Device_WriteParms wpar;
	wpar.lid = link;
	wpar.flags = 0;
	wpar.io_timeout = 10000; // in ms
	wpar.lock_timeout = 10000; // in ms
	wpar.data.data_val = data;
	wpar.data.data_len = (uint32_t) dataSize;

	Device_WriteResp* wresp = device_write_1(&wpar, client);

	if (wresp == NULL || wresp->error != 0) {
		printf("Error writing to vxi instrument\n");
		return -1;
	}
	return 0;
}

int DeviceRead(CLIENT* client, const Device_Link link, char* readBuffer, const size_t readBufferSize) {
	Device_ReadParms rpar;
	rpar.lid = link;
	rpar.flags = 0;
	rpar.io_timeout = 10000;
	rpar.lock_timeout = 10000;
	rpar.termChar = '\n';

	Device_ReadResp* rresp = device_read_1(&rpar, client);
	if (rresp == NULL || rresp->error != 0) {
		printf("Error reading from vxi instrument\n");
		return -1;
	}
	strncpy_s(readBuffer, readBufferSize, rresp->data.data_val, rresp->data.data_len);
	readBuffer[rresp->data.data_len] = 0;
	return 0;
}

int main(const int argc, char** argv)
{
	Device_Link deviceLink = 0;
	char dataWrite[200] = "*IDN?\n";
	char dataRead[200] = "";
	char host[19] = "127.0.0.1";
	int returnValue = 0;
	CLIENT* client = NULL;

	if (argc > 1) {
		strncpy_s(host, sizeof(host), argv[1], strlen(argv[1]));
	}
	for (size_t n = 0, a = 2; a < (size_t)argc && n < sizeof(dataWrite) - 2; a += 1) {
		snprintf(dataWrite + n, sizeof(dataWrite) - n - 2, " %s\n", argv[a]);
		n += strlen(argv[a]) + 1;
	}
	dataWrite[sizeof(dataWrite) - 2] = '\n';
	dataWrite[sizeof(dataWrite) - 1] = '\0';
	printf("%s< %s", host, dataWrite);

	WSADATA wsaData;
	returnValue = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (returnValue != 0) {
		printf("Winsock intialization failed\n");
		returnValue = -1;
		goto End;
	}

	client = CreateClient(host);
	if (client == 0) {
		returnValue = -1;
		goto End;
	}

	deviceLink = CreateLink(client, "inst0");
	if (deviceLink == 0) {
		returnValue = -1;
		goto End;
	}

	returnValue = DeviceWrite(client, deviceLink, dataWrite, strlen(dataWrite));
	if (returnValue != 0) {
		goto End;
	}

	Sleep(1000);

	returnValue = DeviceRead(client, deviceLink, dataRead, sizeof dataRead);
	if (returnValue != 0) {
		goto End;
	}
	printf("%s> %s\n", host, dataRead);

	printf("Opening yet another client\n");

	if (destroy_link_1(&deviceLink, client) == NULL) {
		printf("Error closing communication with vxi instrument\n");
		returnValue = -1;
		goto End;
	}
	if (client != NULL) {
		clnt_destroy(client);
		client = NULL;
	}

	client = CreateClient(host);
	if (client == NULL) {
		returnValue = -1;
		goto End;
	}

	deviceLink = CreateLink(client, "inst0");
	if (deviceLink == 0) {
		returnValue = -1;
		goto End;
	}

	returnValue = DeviceWrite(client, deviceLink, dataWrite, strlen(dataWrite));
	if (returnValue != 0) {
		goto End;
	}

	Sleep(1000);

	returnValue = DeviceRead(client, deviceLink, dataRead, sizeof dataRead);
	if (returnValue != 0) {
		goto End;
	}
	printf("%s> %s\n", host, dataRead);

End:
	if (deviceLink != 0 && client != NULL) {
		if (destroy_link_1(&deviceLink, client) == NULL) {
			printf("Error closing communication with vxi instrument\n");
			returnValue = -1;
		}
	}
	if (client != NULL) {
		clnt_destroy(client);
	}
	WSACleanup();

	return returnValue;

}
