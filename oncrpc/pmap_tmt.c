#include "all_oncrpc.h"

struct timeval pmap_clientTimeout = { 5, 0 };
struct timeval pmap_totTimeout = { 60, 0 };

void pmap_settimeout(struct timeval* timeout) {
	pmap_totTimeout.tv_sec = timeout->tv_sec;
	pmap_totTimeout.tv_usec = timeout->tv_usec;
}
const struct timeval* pmap_gettimeout() {
	return &pmap_totTimeout;
}