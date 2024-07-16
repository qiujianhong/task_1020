#ifndef COMMON_H
#define COMMON_H

#define  GUI_INI_FILE            "desktopGui.ini"
#define MINI_GUI_TASK_PATH       "desktopGui"

#ifdef __cplusplus
extern "C" {
#endif

#define GUI_PRIVATE_DATA    "^gui_paivate_file"

//私有文件偏移量
#define PRIVATE_FREEZE_TIME 0   // 存放输错密码后的冻结延时(长度4字节)
#define PRIVATE_PASSWORD    PRIVATE_FREEZE_TIME+4   // 存放密码偏移地址

//密码定义
#define SETPARAPASSWORD     "000000"  // 设置按键默认初始密码
#define PASSWORD_STR        "000000"  // 默认初始密码

#define PASSWORD_LEN        ((int)strlen(PASSWORD_STR))

extern int init_main(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif // COMMON_H
