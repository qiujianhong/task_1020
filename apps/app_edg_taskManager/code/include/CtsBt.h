#ifndef __CTSBT_H__
#define __CTSBT_H__

void* BtThread ( void* arg );
int bt_data_reading_normal(uint8 Pipe);
int bt_port_config(uint8 pipe,uint16 msindex);

#endif


