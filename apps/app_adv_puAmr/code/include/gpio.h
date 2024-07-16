

/*********************************************
  所属功能单元: gpio
  功能描述: 打开蜂鸣器
  输入参数: 无 	 
  输出参数: 无
  函数返回值: 无
  内部处理流程：LCD背光灯控制引脚送1
**********************************************/
int turnon_bell(void);

/*********************************************
  所属功能单元: gpio
  功能描述: 关闭蜂鸣器
  输入参数: 无 	 
  输出参数: 无
  函数返回值: 无
  内部处理流程：LCD背光灯控制引脚送1
**********************************************/
int turnoff_bell(void);

/*********************************************
  所属功能单元: gpio
  功能描述: 点亮LCD背景灯
  输入参数: 无 	 
  输出参数: 无
  函数返回值: 无
  内部处理流程：LCD背光灯控制引脚送1
**********************************************/
int lighton_lcd(void);

/*********************************************
  所属功能单元: gpio
  功能描述: 熄灭LCD背景灯
  输入参数: 无 	 
  输出参数: 无
  函数返回值: 无
  内部处理流程：LCD背光灯控制引脚送0
**********************************************/
int lightoff_lcd(void);

/**********************************************
  所属功能单元: gpio
  功能描述: LED灯亮灭控制
  输入参数: light_select 灯选择（6个灯0~5）
            light_switch 灯开关（亮1灭0）  	 
  输出参数: 无
  函数返回值: 无
  内部处理流程:
***********************************************/
int Out2Led(uint8 light_select,uint8 light_switch);

/**********************************************
  所属功能单元: gpio
  功能描述: 关语音功放
  输入参数: 无
  输出参数: 无
  函数返回值: 无
***********************************************/
int Close_Voice_Amp(void);




/**********************************************
  所属功能单元: gpio
  功能描述: 应用层设置GPI
  输入参数: 无
  输出参数: 无
  函数返回值: 返回GPIO状态
***********************************************/
int set_gpio_value(unsigned pin,unsigned int value);


#define PIN_BASE		0

#define MAX_GPIO_BANKS		5
#define GPIO_MAGIC	0xAA00
#define GPIO_IN     	(0<<4)
#define GPIO_OUT 	(1<<4)
#define PERIPH_A        (2<<4)
#define PERIPH_B        (3<<4)
#define GPIO_OUT_HIGH	GPIO_MAGIC | GPIO_OUT | 1
#define GPIO_OUT_LOW	GPIO_MAGIC | GPIO_OUT | 0
#define GPIO_IN_NUP	GPIO_MAGIC | GPIO_IN | 0
#define GPIO_IN_PULLUP	GPIO_MAGIC | GPIO_IN | 1
/* these pin numbers double as IRQ numbers, like AT91xxx_ID_* values */

#define	AT91_PIN_PA0	(PIN_BASE + 0x00 + 0)
#define	AT91_PIN_PA1	(PIN_BASE + 0x00 + 1)
#define	AT91_PIN_PA2	(PIN_BASE + 0x00 + 2)
#define	AT91_PIN_PA3	(PIN_BASE + 0x00 + 3)
#define	AT91_PIN_PA4	(PIN_BASE + 0x00 + 4)
#define	AT91_PIN_PA5	(PIN_BASE + 0x00 + 5)
#define	AT91_PIN_PA6	(PIN_BASE + 0x00 + 6)
#define	AT91_PIN_PA7	(PIN_BASE + 0x00 + 7)
#define	AT91_PIN_PA8	(PIN_BASE + 0x00 + 8)
#define	AT91_PIN_PA9	(PIN_BASE + 0x00 + 9)
#define	AT91_PIN_PA10	(PIN_BASE + 0x00 + 10)
#define	AT91_PIN_PA11	(PIN_BASE + 0x00 + 11)
#define	AT91_PIN_PA12	(PIN_BASE + 0x00 + 12)
#define	AT91_PIN_PA13	(PIN_BASE + 0x00 + 13)
#define	AT91_PIN_PA14	(PIN_BASE + 0x00 + 14)
#define	AT91_PIN_PA15	(PIN_BASE + 0x00 + 15)
#define	AT91_PIN_PA16	(PIN_BASE + 0x00 + 16)
#define	AT91_PIN_PA17	(PIN_BASE + 0x00 + 17)
#define	AT91_PIN_PA18	(PIN_BASE + 0x00 + 18)
#define	AT91_PIN_PA19	(PIN_BASE + 0x00 + 19)
#define	AT91_PIN_PA20	(PIN_BASE + 0x00 + 20)
#define	AT91_PIN_PA21	(PIN_BASE + 0x00 + 21)
#define	AT91_PIN_PA22	(PIN_BASE + 0x00 + 22)
#define	AT91_PIN_PA23	(PIN_BASE + 0x00 + 23)
#define	AT91_PIN_PA24	(PIN_BASE + 0x00 + 24)
#define	AT91_PIN_PA25	(PIN_BASE + 0x00 + 25)
#define	AT91_PIN_PA26	(PIN_BASE + 0x00 + 26)
#define	AT91_PIN_PA27	(PIN_BASE + 0x00 + 27)
#define	AT91_PIN_PA28	(PIN_BASE + 0x00 + 28)
#define	AT91_PIN_PA29	(PIN_BASE + 0x00 + 29)
#define	AT91_PIN_PA30	(PIN_BASE + 0x00 + 30)
#define	AT91_PIN_PA31	(PIN_BASE + 0x00 + 31)

#define	AT91_PIN_PB0	(PIN_BASE + 0x20 + 0)
#define	AT91_PIN_PB1	(PIN_BASE + 0x20 + 1)
#define	AT91_PIN_PB2	(PIN_BASE + 0x20 + 2)
#define	AT91_PIN_PB3	(PIN_BASE + 0x20 + 3)
#define	AT91_PIN_PB4	(PIN_BASE + 0x20 + 4)
#define	AT91_PIN_PB5	(PIN_BASE + 0x20 + 5)
#define	AT91_PIN_PB6	(PIN_BASE + 0x20 + 6)
#define	AT91_PIN_PB7	(PIN_BASE + 0x20 + 7)
#define	AT91_PIN_PB8	(PIN_BASE + 0x20 + 8)
#define	AT91_PIN_PB9	(PIN_BASE + 0x20 + 9)
#define	AT91_PIN_PB10	(PIN_BASE + 0x20 + 10)
#define	AT91_PIN_PB11	(PIN_BASE + 0x20 + 11)
#define	AT91_PIN_PB12	(PIN_BASE + 0x20 + 12)
#define	AT91_PIN_PB13	(PIN_BASE + 0x20 + 13)
#define	AT91_PIN_PB14	(PIN_BASE + 0x20 + 14)
#define	AT91_PIN_PB15	(PIN_BASE + 0x20 + 15)
#define	AT91_PIN_PB16	(PIN_BASE + 0x20 + 16)
#define	AT91_PIN_PB17	(PIN_BASE + 0x20 + 17)
#define	AT91_PIN_PB18	(PIN_BASE + 0x20 + 18)
#define	AT91_PIN_PB19	(PIN_BASE + 0x20 + 19)
#define	AT91_PIN_PB20	(PIN_BASE + 0x20 + 20)
#define	AT91_PIN_PB21	(PIN_BASE + 0x20 + 21)
#define	AT91_PIN_PB22	(PIN_BASE + 0x20 + 22)
#define	AT91_PIN_PB23	(PIN_BASE + 0x20 + 23)
#define	AT91_PIN_PB24	(PIN_BASE + 0x20 + 24)
#define	AT91_PIN_PB25	(PIN_BASE + 0x20 + 25)
#define	AT91_PIN_PB26	(PIN_BASE + 0x20 + 26)
#define	AT91_PIN_PB27	(PIN_BASE + 0x20 + 27)
#define	AT91_PIN_PB28	(PIN_BASE + 0x20 + 28)
#define	AT91_PIN_PB29	(PIN_BASE + 0x20 + 29)
#define	AT91_PIN_PB30	(PIN_BASE + 0x20 + 30)
#define	AT91_PIN_PB31	(PIN_BASE + 0x20 + 31)

#define	AT91_PIN_PC0	(PIN_BASE + 0x40 + 0)
#define	AT91_PIN_PC1	(PIN_BASE + 0x40 + 1)
#define	AT91_PIN_PC2	(PIN_BASE + 0x40 + 2)
#define	AT91_PIN_PC3	(PIN_BASE + 0x40 + 3)
#define	AT91_PIN_PC4	(PIN_BASE + 0x40 + 4)
#define	AT91_PIN_PC5	(PIN_BASE + 0x40 + 5)
#define	AT91_PIN_PC6	(PIN_BASE + 0x40 + 6)
#define	AT91_PIN_PC7	(PIN_BASE + 0x40 + 7)
#define	AT91_PIN_PC8	(PIN_BASE + 0x40 + 8)
#define	AT91_PIN_PC9	(PIN_BASE + 0x40 + 9)
#define	AT91_PIN_PC10	(PIN_BASE + 0x40 + 10)
#define	AT91_PIN_PC11	(PIN_BASE + 0x40 + 11)
#define	AT91_PIN_PC12	(PIN_BASE + 0x40 + 12)
#define	AT91_PIN_PC13	(PIN_BASE + 0x40 + 13)
#define	AT91_PIN_PC14	(PIN_BASE + 0x40 + 14)
#define	AT91_PIN_PC15	(PIN_BASE + 0x40 + 15)
#define	AT91_PIN_PC16	(PIN_BASE + 0x40 + 16)
#define	AT91_PIN_PC17	(PIN_BASE + 0x40 + 17)
#define	AT91_PIN_PC18	(PIN_BASE + 0x40 + 18)
#define	AT91_PIN_PC19	(PIN_BASE + 0x40 + 19)
#define	AT91_PIN_PC20	(PIN_BASE + 0x40 + 20)
#define	AT91_PIN_PC21	(PIN_BASE + 0x40 + 21)
#define	AT91_PIN_PC22	(PIN_BASE + 0x40 + 22)
#define	AT91_PIN_PC23	(PIN_BASE + 0x40 + 23)
#define	AT91_PIN_PC24	(PIN_BASE + 0x40 + 24)
#define	AT91_PIN_PC25	(PIN_BASE + 0x40 + 25)
#define	AT91_PIN_PC26	(PIN_BASE + 0x40 + 26)
#define	AT91_PIN_PC27	(PIN_BASE + 0x40 + 27)
#define	AT91_PIN_PC28	(PIN_BASE + 0x40 + 28)
#define	AT91_PIN_PC29	(PIN_BASE + 0x40 + 29)
#define	AT91_PIN_PC30	(PIN_BASE + 0x40 + 30)
#define	AT91_PIN_PC31	(PIN_BASE + 0x40 + 31)

#define	AT91_PIN_PD0	(PIN_BASE + 0x60 + 0)
#define	AT91_PIN_PD1	(PIN_BASE + 0x60 + 1)
#define	AT91_PIN_PD2	(PIN_BASE + 0x60 + 2)
#define	AT91_PIN_PD3	(PIN_BASE + 0x60 + 3)
#define	AT91_PIN_PD4	(PIN_BASE + 0x60 + 4)
#define	AT91_PIN_PD5	(PIN_BASE + 0x60 + 5)
#define	AT91_PIN_PD6	(PIN_BASE + 0x60 + 6)
#define	AT91_PIN_PD7	(PIN_BASE + 0x60 + 7)
#define	AT91_PIN_PD8	(PIN_BASE + 0x60 + 8)
#define	AT91_PIN_PD9	(PIN_BASE + 0x60 + 9)
#define	AT91_PIN_PD10	(PIN_BASE + 0x60 + 10)
#define	AT91_PIN_PD11	(PIN_BASE + 0x60 + 11)
#define	AT91_PIN_PD12	(PIN_BASE + 0x60 + 12)
#define	AT91_PIN_PD13	(PIN_BASE + 0x60 + 13)
#define	AT91_PIN_PD14	(PIN_BASE + 0x60 + 14)
#define	AT91_PIN_PD15	(PIN_BASE + 0x60 + 15)
#define	AT91_PIN_PD16	(PIN_BASE + 0x60 + 16)
#define	AT91_PIN_PD17	(PIN_BASE + 0x60 + 17)
#define	AT91_PIN_PD18	(PIN_BASE + 0x60 + 18)
#define	AT91_PIN_PD19	(PIN_BASE + 0x60 + 19)
#define	AT91_PIN_PD20	(PIN_BASE + 0x60 + 20)
#define	AT91_PIN_PD21	(PIN_BASE + 0x60 + 21)
#define	AT91_PIN_PD22	(PIN_BASE + 0x60 + 22)
#define	AT91_PIN_PD23	(PIN_BASE + 0x60 + 23)
#define	AT91_PIN_PD24	(PIN_BASE + 0x60 + 24)
#define	AT91_PIN_PD25	(PIN_BASE + 0x60 + 25)
#define	AT91_PIN_PD26	(PIN_BASE + 0x60 + 26)
#define	AT91_PIN_PD27	(PIN_BASE + 0x60 + 27)
#define	AT91_PIN_PD28	(PIN_BASE + 0x60 + 28)
#define	AT91_PIN_PD29	(PIN_BASE + 0x60 + 29)
#define	AT91_PIN_PD30	(PIN_BASE + 0x60 + 30)
#define	AT91_PIN_PD31	(PIN_BASE + 0x60 + 31)

#define	AT91_PIN_PE0	(PIN_BASE + 0x80 + 0)
#define	AT91_PIN_PE1	(PIN_BASE + 0x80 + 1)
#define	AT91_PIN_PE2	(PIN_BASE + 0x80 + 2)
#define	AT91_PIN_PE3	(PIN_BASE + 0x80 + 3)
#define	AT91_PIN_PE4	(PIN_BASE + 0x80 + 4)
#define	AT91_PIN_PE5	(PIN_BASE + 0x80 + 5)
#define	AT91_PIN_PE6	(PIN_BASE + 0x80 + 6)
#define	AT91_PIN_PE7	(PIN_BASE + 0x80 + 7)
#define	AT91_PIN_PE8	(PIN_BASE + 0x80 + 8)
#define	AT91_PIN_PE9	(PIN_BASE + 0x80 + 9)
#define	AT91_PIN_PE10	(PIN_BASE + 0x80 + 10)
#define	AT91_PIN_PE11	(PIN_BASE + 0x80 + 11)
#define	AT91_PIN_PE12	(PIN_BASE + 0x80 + 12)
#define	AT91_PIN_PE13	(PIN_BASE + 0x80 + 13)
#define	AT91_PIN_PE14	(PIN_BASE + 0x80 + 14)
#define	AT91_PIN_PE15	(PIN_BASE + 0x80 + 15)
#define	AT91_PIN_PE16	(PIN_BASE + 0x80 + 16)
#define	AT91_PIN_PE17	(PIN_BASE + 0x80 + 17)
#define	AT91_PIN_PE18	(PIN_BASE + 0x80 + 18)
#define	AT91_PIN_PE19	(PIN_BASE + 0x80 + 19)
#define	AT91_PIN_PE20	(PIN_BASE + 0x80 + 20)
#define	AT91_PIN_PE21	(PIN_BASE + 0x80 + 21)
#define	AT91_PIN_PE22	(PIN_BASE + 0x80 + 22)
#define	AT91_PIN_PE23	(PIN_BASE + 0x80 + 23)
#define	AT91_PIN_PE24	(PIN_BASE + 0x80 + 24)
#define	AT91_PIN_PE25	(PIN_BASE + 0x80 + 25)
#define	AT91_PIN_PE26	(PIN_BASE + 0x80 + 26)
#define	AT91_PIN_PE27	(PIN_BASE + 0x80 + 27)
#define	AT91_PIN_PE28	(PIN_BASE + 0x80 + 28)
#define	AT91_PIN_PE29	(PIN_BASE + 0x80 + 29)
#define	AT91_PIN_PE30	(PIN_BASE + 0x80 + 30)
#define	AT91_PIN_PE31	(PIN_BASE + 0x80 + 31)

#define	AT91_PIN_PF0	(PIN_BASE + 0xa0 + 0)
#define	AT91_PIN_PF1	(PIN_BASE + 0xa0 + 1)
#define	AT91_PIN_PF2	(PIN_BASE + 0xa0 + 2)
#define	AT91_PIN_PF3	(PIN_BASE + 0xa0 + 3)
#define	AT91_PIN_PF4	(PIN_BASE + 0xa0 + 4)
#define	AT91_PIN_PF5	(PIN_BASE + 0xa0 + 5)
#define	AT91_PIN_PF6	(PIN_BASE + 0xa0 + 6)
#define	AT91_PIN_PF7	(PIN_BASE + 0xa0 + 7)
#define	AT91_PIN_PF8	(PIN_BASE + 0xa0 + 8)
#define	AT91_PIN_PF9	(PIN_BASE + 0xa0 + 9)
#define	AT91_PIN_PF10	(PIN_BASE + 0xa0 + 10)
#define	AT91_PIN_PF11	(PIN_BASE + 0xa0 + 11)
#define	AT91_PIN_PF12	(PIN_BASE + 0xa0 + 12)
#define	AT91_PIN_PF13	(PIN_BASE + 0xa0 + 13)
#define	AT91_PIN_PF14	(PIN_BASE + 0xa0 + 14)
#define	AT91_PIN_PF15	(PIN_BASE + 0xa0 + 15)
#define	AT91_PIN_PF16	(PIN_BASE + 0xa0 + 16)
#define	AT91_PIN_PF17	(PIN_BASE + 0xa0 + 17)
#define	AT91_PIN_PF18	(PIN_BASE + 0xa0 + 18)
#define	AT91_PIN_PF19	(PIN_BASE + 0xa0 + 19)
#define	AT91_PIN_PF20	(PIN_BASE + 0xa0 + 20)
#define	AT91_PIN_PF21	(PIN_BASE + 0xa0 + 21)
#define	AT91_PIN_PF22	(PIN_BASE + 0xa0 + 22)
#define	AT91_PIN_PF23	(PIN_BASE + 0xa0 + 23)
#define	AT91_PIN_PF24	(PIN_BASE + 0xa0 + 24)
#define	AT91_PIN_PF25	(PIN_BASE + 0xa0 + 25)
#define	AT91_PIN_PF26	(PIN_BASE + 0xa0 + 26)
#define	AT91_PIN_PF27	(PIN_BASE + 0xa0 + 27)
#define	AT91_PIN_PF28	(PIN_BASE + 0xa0 + 28)
#define	AT91_PIN_PF29	(PIN_BASE + 0xa0 + 29)
#define	AT91_PIN_PF30	(PIN_BASE + 0xa0 + 30)
#define	AT91_PIN_PF31	(PIN_BASE + 0xa0 + 31)

#define	AT91_PIN_PG0	(PIN_BASE + 0xc0 + 0)
#define	AT91_PIN_PG1	(PIN_BASE + 0xc0 + 1)
#define	AT91_PIN_PG2	(PIN_BASE + 0xc0 + 2)
#define	AT91_PIN_PG3	(PIN_BASE + 0xc0 + 3)
#define	AT91_PIN_PG4	(PIN_BASE + 0xc0 + 4)
#define	AT91_PIN_PG5	(PIN_BASE + 0xc0 + 5)
#define	AT91_PIN_PG6	(PIN_BASE + 0xc0 + 6)
#define	AT91_PIN_PG7	(PIN_BASE + 0xc0 + 7)
#define	AT91_PIN_PG8	(PIN_BASE + 0xc0 + 8)
#define	AT91_PIN_PG9	(PIN_BASE + 0xc0 + 9)
#define	AT91_PIN_PG10	(PIN_BASE + 0xc0 + 10)
#define	AT91_PIN_PG11	(PIN_BASE + 0xc0 + 11)
#define	AT91_PIN_PG12	(PIN_BASE + 0xc0 + 12)
#define	AT91_PIN_PG13	(PIN_BASE + 0xc0 + 13)
#define	AT91_PIN_PG14	(PIN_BASE + 0xc0 + 14)
#define	AT91_PIN_PG15	(PIN_BASE + 0xc0 + 15)
#define	AT91_PIN_PG16	(PIN_BASE + 0xc0 + 16)
#define	AT91_PIN_PG17	(PIN_BASE + 0xc0 + 17)
#define	AT91_PIN_PG18	(PIN_BASE + 0xc0 + 18)
#define	AT91_PIN_PG19	(PIN_BASE + 0xc0 + 19)
#define	AT91_PIN_PG20	(PIN_BASE + 0xc0 + 20)
#define	AT91_PIN_PG21	(PIN_BASE + 0xc0 + 21)
#define	AT91_PIN_PG22	(PIN_BASE + 0xc0 + 22)
#define	AT91_PIN_PG23	(PIN_BASE + 0xc0 + 23)
#define	AT91_PIN_PG24	(PIN_BASE + 0xc0 + 24)
#define	AT91_PIN_PG25	(PIN_BASE + 0xc0 + 25)
#define	AT91_PIN_PG26	(PIN_BASE + 0xc0 + 26)
#define	AT91_PIN_PG27	(PIN_BASE + 0xc0 + 27)
#define	AT91_PIN_PG28	(PIN_BASE + 0xc0 + 28)
#define	AT91_PIN_PG29	(PIN_BASE + 0xc0 + 29)
#define	AT91_PIN_PG30	(PIN_BASE + 0xc0 + 30)
#define	AT91_PIN_PG31	(PIN_BASE + 0xc0 + 31)

#define	AT91_PIN_PH0	(PIN_BASE + 0xe0 + 0)
#define	AT91_PIN_PH1	(PIN_BASE + 0xe0 + 1)
#define	AT91_PIN_PH2	(PIN_BASE + 0xe0 + 2)
#define	AT91_PIN_PH3	(PIN_BASE + 0xe0 + 3)
#define	AT91_PIN_PH4	(PIN_BASE + 0xe0 + 4)
#define	AT91_PIN_PH5	(PIN_BASE + 0xe0 + 5)
#define	AT91_PIN_PH6	(PIN_BASE + 0xe0 + 6)
#define	AT91_PIN_PH7	(PIN_BASE + 0xe0 + 7)
#define	AT91_PIN_PH8	(PIN_BASE + 0xe0 + 8)
#define	AT91_PIN_PH9	(PIN_BASE + 0xe0 + 9)
#define	AT91_PIN_PH10	(PIN_BASE + 0xe0 + 10)
#define	AT91_PIN_PH11	(PIN_BASE + 0xe0 + 11)
#define	AT91_PIN_PH12	(PIN_BASE + 0xe0 + 12)
#define	AT91_PIN_PH13	(PIN_BASE + 0xe0 + 13)
#define	AT91_PIN_PH14	(PIN_BASE + 0xe0 + 14)
#define	AT91_PIN_PH15	(PIN_BASE + 0xe0 + 15)
#define	AT91_PIN_PH16	(PIN_BASE + 0xe0 + 16)
#define	AT91_PIN_PH17	(PIN_BASE + 0xe0 + 17)
#define	AT91_PIN_PH18	(PIN_BASE + 0xe0 + 18)
#define	AT91_PIN_PH19	(PIN_BASE + 0xe0 + 19)
#define	AT91_PIN_PH20	(PIN_BASE + 0xe0 + 20)
#define	AT91_PIN_PH21	(PIN_BASE + 0xe0 + 21)
#define	AT91_PIN_PH22	(PIN_BASE + 0xe0 + 22)
#define	AT91_PIN_PH23	(PIN_BASE + 0xe0 + 23)
#define	AT91_PIN_PH24	(PIN_BASE + 0xe0 + 24)
#define	AT91_PIN_PH25	(PIN_BASE + 0xe0 + 25)
#define	AT91_PIN_PH26	(PIN_BASE + 0xe0 + 26)
#define	AT91_PIN_PH27	(PIN_BASE + 0xe0 + 27)
#define	AT91_PIN_PH28	(PIN_BASE + 0xe0 + 28)
#define	AT91_PIN_PH29	(PIN_BASE + 0xe0 + 29)
#define	AT91_PIN_PH30	(PIN_BASE + 0xe0 + 30)
#define	AT91_PIN_PH31	(PIN_BASE + 0xe0 + 31)