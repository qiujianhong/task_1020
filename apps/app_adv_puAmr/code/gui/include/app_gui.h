#ifndef	__APP_GUI_H_
#define __APP_GUI_H_

#ifdef __cplusplus
        extern "C" {
#endif

typedef struct
{
	OOP_ENERGY_T        penergy;         //�����й� 0x00100200
	OOP_ENERGYS_T       penergys;        //�����޹� 0x00300200
	OOP_ENERGY_T        nenergy;         //�����й� 0x00200200
	OOP_ENERGYS_T       nenergys;        //�����޹� 0x00400200
	OOP_ENERGY_T        quadrant1;       //��һ���� 0x00500200
	OOP_ENERGY_T        quadrant2;       //�ڶ����� 0x00600200
	OOP_ENERGY_T        quadrant3;       //�������� 0x00700200
	OOP_ENERGY_T        quadrant4;       //�������� 0x00800200
	OOP_WORD3V_T        vol;             //��ѹ       0x20000200
	OOP_INT3V_T         cur;             //����       0x20010200
	OOP_INT4V_T         ppower;          //�й�����     0x20040200
	OOP_INT4V_T         npower;          //�޹�����     0x20050200
	OOP_LONG4V_T        fpower;          //��������     0x200A0200
	OOP_DEMAND_T        pdemand;         //�����й����� 0x10100200
	OOP_DEMAND_T        ndemand;         //�����й����� 0x10200200
	OOP_HARMONIC_T      harmonicV[3];    //�����ѹг��������0x200D0200  0 x200D0400
    OOP_HARMONIC_T      harmonicC[3];    //�������г��������0x200E0200  0 x200E0400
    OOP_HARMONIC_U_VALUE_T      harmonicV_value[3];    //�����ѹг������0x20330200  0 x20330400
    OOP_HARMONIC_I_VALUE_T      harmonicC_value[3];    //�������г������0x20340200  0 x20340400
	OOP_CHINESEINFOS_T          chinese_info;
	OOP_SWITCHOUT_T     switchout;
	OOP_SECURITYFLAG_E  safemode;

    int                 retzeroVol;
    uint32              zeroVol;          //�����ѹ       0x20000600
    int                 retzeroCur;
	int                 zeroCur;          //���ߵ���       0x20010400
}DISPLAY_DATA_INFO;

typedef struct
{
	OOP_ENERGY_T        penergy;         //�����й� 0x00100200
	OOP_ENERGYS_T       penergys;        //�����޹� 0x00300200
	OOP_ENERGY_T        nenergy;         //�����й� 0x00200200
	OOP_ENERGYS_T       nenergys;        //�����޹� 0x00400200
	OOP_ENERGY_T        quadrant1;       //��һ���� 0x00500200
	OOP_ENERGY_T        quadrant2;       //�ڶ����� 0x00600200
	OOP_ENERGY_T        quadrant3;       //�������� 0x00700200
	OOP_ENERGY_T        quadrant4;       //�������� 0x00800200
}DISPLAY_DAY_FROZEN_INFO;

typedef struct
{
	OOP_HENERGY_T        penergy;         //�����й� 0x00100400
	OOP_HENERGYS_T       penergys;        //�����޹� 0x00300400
	OOP_HENERGY_T        nenergy;         //�����й� 0x00200400
	OOP_HENERGYS_T       nenergys;        //�����޹� 0x00400400
	OOP_HENERGY_T        quadrant1;       //��һ���� 0x00500400
	OOP_HENERGY_T        quadrant2;       //�ڶ����� 0x00600400
	OOP_HENERGY_T        quadrant3;       //�������� 0x00700400
	OOP_HENERGY_T        quadrant4;       //�������� 0x00800400
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

