#ifndef _GET_AUTH_DATA_H_
#define _GET_AUTH_DATA_H_

#ifdef __cplusplus
extern "C" {
#endif



//报文认证组件I接口
/*
data_in:报文数据               [IN]
data_len：报文长度             [IN]
appid_in:业务APP管理标识       [IN]
appid_len：标示长度            [IN]
ac_in：安全认证码              [IN]
ac_len：安全认证码长度         [IN]
auth_data：认证数据buf         [OUT]
auth_data_len：认证数据长度     [OUT]
time_out：时间戳    buf长度==8    [OUT]


拼接顺序 ： 报文 + 业务APP管理标识 + 安全认证码 + 认证数据 + 时间戳
总长度  ： 报文长度 + 标识长度 + 安全认证码长度 + 认证数据长度（当前为64B） + 时间戳长度（8B）

return 0 成功
        非 0 失败
*/


int sdt_msg_auth(unsigned char *data_in, unsigned int data_len, \
                 unsigned char *appid_in, unsigned int appid_len, \
                 unsigned char *ac_in, unsigned int ac_len, \
                 unsigned char *auth_data, unsigned int *auth_data_len,\
                 unsigned char *time_out);




#ifdef __cplusplus
}
#endif
#endif