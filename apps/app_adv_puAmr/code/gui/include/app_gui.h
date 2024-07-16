#ifndef	__APP_GUI_H_
#define __APP_GUI_H_

#ifdef __cplusplus
        extern "C" {
#endif

typedef struct
{
	OOP_ENERGY_T        penergy;         //正向有功 0x00100200
	OOP_ENERGYS_T       penergys;        //正向无功 0x00300200
	OOP_ENERGY_T        nenergy;         //反向有功 0x00200200
	OOP_ENERGYS_T       nenergys;        //反向无功 0x00400200
	OOP_ENERGY_T        quadrant1;       //第一象限 0x00500200
	OOP_ENERGY_T        quadrant2;       //第二象限 0x00600200
	OOP_ENERGY_T        quadrant3;       //第三象限 0x00700200
	OOP_ENERGY_T        quadrant4;       //第四象限 0x00800200
	OOP_WORD3V_T        vol;             //电压       0x20000200
	OOP_INT3V_T         cur;             //电流       0x20010200
	OOP_INT4V_T         ppower;          //有功功率     0x20040200
	OOP_INT4V_T         npower;          //无功功率     0x20050200
	OOP_LONG4V_T        fpower;          //功率因数     0x200A0200
	OOP_DEMAND_T        pdemand;         //正向有功需量 0x10100200
	OOP_DEMAND_T        ndemand;         //反向有功需量 0x10200200
	OOP_HARMONIC_T      harmonicV[3];    //三相电压谐波含有率0x200D0200  0 x200D0400
    OOP_HARMONIC_T      harmonicC[3];    //三相电流谐波含有率0x200E0200  0 x200E0400
    OOP_HARMONIC_U_VALUE_T      harmonicV_value[3];    //三相电压谐波含量0x20330200  0 x20330400
    OOP_HARMONIC_I_VALUE_T      harmonicC_value[3];    //三相电流谐波含量0x20340200  0 x20340400
	OOP_CHINESEINFOS_T          chinese_info;
	OOP_SWITCHOUT_T     switchout;
	OOP_SECURITYFLAG_E  safemode;

    int                 retzeroVol;
    uint32              zeroVol;          //零序电压       0x20000600
    int                 retzeroCur;
	int                 zeroCur;          //零线电流       0x20010400
}DISPLAY_DATA_INFO;

typedef struct
{
	OOP_ENERGY_T        penergy;         //正向有功 0x00100200
	OOP_ENERGYS_T       penergys;        //正向无功 0x00300200
	OOP_ENERGY_T        nenergy;         //反向有功 0x00200200
	OOP_ENERGYS_T       nenergys;        //反向无功 0x00400200
	OOP_ENERGY_T        quadrant1;       //第一象限 0x00500200
	OOP_ENERGY_T        quadrant2;       //第二象限 0x00600200
	OOP_ENERGY_T        quadrant3;       //第三象限 0x00700200
	OOP_ENERGY_T        quadrant4;       //第四象限 0x00800200
}DISPLAY_DAY_FROZEN_INFO;

typedef struct
{
	OOP_HENERGY_T        penergy;         //正向有功 0x00100400
	OOP_HENERGYS_T       penergys;        //正向无功 0x00300400
	OOP_HENERGY_T        nenergy;         //反向有功 0x00200400
	OOP_HENERGYS_T       nenergys;        //反向无功 0x00400400
	OOP_HENERGY_T        quadrant1;       //第一象限 0x00500400
	OOP_HENERGY_T        quadrant2;       //第二象限 0x00600400
	OOP_HENERGY_T        quadrant3;       //第三象限 0x00700400
	OOP_HENERGY_T        quadrant4;       //第四象限 0x00800400
}HIGH_PRECISION_DATA_INFO;

typedef uint16 (*APP_MODE_INFO)(uint32, uint8*, uint16);

extern DB_CLIENT   g_gui_db_client;

extern BOOLEAN advance_mqtt_guiok();

void advance_gui_infonum(uint16 infonum);

int update_ac_data(int clientid, uint8 logicId, uint16 infoNum, uint32 value);


#ifdef __cplusplus
}
#endif

#endif

