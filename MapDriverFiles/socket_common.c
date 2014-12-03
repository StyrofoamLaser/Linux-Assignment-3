#include "socket_common.h"

int sizeofM(mapmsg_t msg)
{
	return sizeof(char) + sizeof(int) * 2 + strlen(msg.map);
}

int sizeofE(errmsg_t msg)
{
	return sizeof(char) + sizeof(int) + strlen(msg.errMsg);
}
