#ifndef __CLIENTTOOL_H__
#define __CLIENTTOOL_H__
#include "optparse.h"
#include "baseType.h"

#ifdef __cplusplus
extern "C"
{
#endif

sint32 udp_recv(void *buf, sint32 bufMaxLen);
sint32 udp_send(void *buf, sint32 buflen);

#ifdef __cplusplus
}
#endif

#endif