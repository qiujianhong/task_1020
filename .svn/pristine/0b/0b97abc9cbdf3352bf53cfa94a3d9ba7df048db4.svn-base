#ifndef __PDSMQTT_H__
#define __PDSMQTT_H__


 int mqtt_init(char* ip,int port);
 void* mqtt_thread(void* arg);
int mqtt_sendtotaskManager(uint16 iop,uint8* payload,uint16 len,uint16* pindex);
int mqtt_sendtrans(uint8* buf,uint16 len,uint32 port,uint8 baud,uint32 overtime,uint16 *msgindex);
void mqtt_query_app_info(char* appname);

#endif

