#include "miniguidef.h"

/*-页面全局变量-*/
static int nSelete = 0; // 当前所选中的行号:nSelete=0~4
static char manufacturer[16];
static char tsn[20+1]; // tsn: terminal serial num
static char sv[4+1]; // sv: software version
static char sv_inn[4+1]; // 内部版本号
static char plcconf[5]; // plcconf: PLC Conf(PLC点抄配置)
#if	FAC_MODE == 1		//出厂模式
S_FacMode g_DefaultSet;
const char* g_DefaultFlag[] = {"出厂模式", "送检模式"};
#endif
uint8 eth_gprs_auto_enable = 0;		// 是否启用以太网GPRS自动切换  0xaa-不启用 0x55-启用
const char* g_EnableFlag[] = {"启用", "停用"};

/*-页面子函数声明-*/
static void InitLayer(DWORD dwPluginID);
static void UninitLayer(void);
static void Draw(HDC hdc);
static BOOL KeyMsg(int nKey);

/*-页面-*/
PLUGIN_INFO g_layerHiddenLayer = {
	0xffffffff, // HiddenLayer不需要ID
	InitLayer,
	UninitLayer,
	Draw,
	NULL, // Timer
	KeyMsg,
	NULL, // IPC
};

/*-页面子函数-*/
static void InitLayer(DWORD dwPluginID)
{
	int nRet = 0;
	#if BEIJING_JZQ_EN == 1
	char tsn2[10];
	#endif
	nSelete = 0;
	g_guiComm.bCanTrunDisp = FALSE;
	g_guiComm.bTrundispTally = FALSE;

	// 厂商代号
	memset(manufacturer, '\0', sizeof(manufacturer));
	//nRet = ReadXPara(TMN_RUN_PARA, FAC_CODE_ID, manufacturer, 0);
	nRet = -1;
	if (nRet < 0 || manufacturer[0]==NONE_CHAR)
	{
		// 厂商代号无效时显示默认值
		strcpy(manufacturer, MANUFACTURER);
	}
	manufacturer[4] = '\0';

	// 设备编号
	#if BEIJING_JZQ_EN == 1
	nRet = ReadXPara(TMN_PARA_EXTEND, AREA_SOFT_VERSION_ID, tsn2, 0);
	sprintf(tsn, "%02x%02x%02x%02x",tsn2[0],tsn2[1],tsn2[2],tsn2[3]);
	nRet *= 2;
	#else
	//nRet = commfun_ReadTmnSerialNum(tsn);
	#endif

	if (nRet < 0 || tsn[0]==NONE_CHAR)
	{
		memset(tsn, NONE_CHAR, sizeof(tsn));
	}
	else
	{
		tsn[nRet] = '\0';
	}

	// 终端软件版本号
	char sv_reversed[4] = {0}; // 注:数据字典中软件版本号一直是倒序排列的
	//nRet = ReadXPara(TMN_RUN_PARA, SOFTWARE_VERSION_ID, sv_reversed, 0);
	nRet = -1;
	if (nRet < 0)
	{
		memset(sv, NONE_CHAR, sizeof(sv));
	}
	else
	{
		memcpy(sv, sv_reversed, 4);
		sv[4] = '\0';
	}

	//程序内部版本号
	memset(sv_inn, 0x0, sizeof(sv_inn));
	//nRet = ReadXPara(TMN_RUN_PARA, SOFTWARE_VERSION_INN_ID, sv_inn, 0);
	nRet = -1;
	if (nRet < 0)
	{
		memset(sv_inn, NONE_CHAR, sizeof(sv_inn));
	}
	commfun_toupper(sv_inn);		//将内部版本信息转换为大写字母

	// PLC点抄配置
	{
		uint16 conf;
		conf = NONE_WORD;
		//nRet = ReadXPara(TMN_RUN_PARA, PLC_READ_RELAY, (char*)(&conf), 0);
		nRet = -1;
        #if HUNAN_FUN == 1
        if (nRet > 0 && conf == 0x00AA)
            strcpy(plcconf, "停用");
        else
            strcpy(plcconf, "启用");
        #else
		if (nRet > 0 && conf == 0x00AA)
			strcpy(plcconf, "启用");
		else
			strcpy(plcconf, "停用");
        #endif
	}

	#if	FAC_MODE == 1		//出厂模式
	//commfun_ReadFacMode(&g_DefaultSet);
	#endif

	//nRet = ReadXPara(TMN_RUN_PARA, ETH_GPRS_SWITCH_AUTO, (char *)&eth_gprs_auto_enable, TMN_ID);
	nRet = -1;
	if(nRet > 0 && (eth_gprs_auto_enable == 0xaa))
		eth_gprs_auto_enable = 0xaa;		//是否启用以太网GPRS自动切换  0xaa-不启用 0x55-启用（默认开启）
	else
		eth_gprs_auto_enable = 0x55;
}

static void UninitLayer(void)
{
	g_guiComm.bTrundispTally = TRUE;
}

static void Draw(HDC hdc)
{
	int nLeft1, nLeft2, nAdd, y;
	#if BEIJING_JZQ_EN == 1
	char tsn2[21];
	#endif
	nLeft1 = 10;
	nLeft2 = 85;
	nAdd = 15;
	y = 20;

	SelectFont(hdc, g_guiComm.fontSmall);
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "厂商代号");
	if (nSelete == 0)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2, y, (((manufacturer[0]==NONE_CHAR)||(manufacturer[0]==0x00&&manufacturer[1]==0x00&&manufacturer[2]==0x00&&manufacturer[3]==0x00)||(manufacturer[0]=='x'&&manufacturer[1]=='x'))? "----":manufacturer));

	y += nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "终端编号");
	if (nSelete == 1)
	{
		ITEM_SELECT;
	}
#if BEIJING_JZQ_EN == 1
	if((tsn[0]==NONE_CHAR)||(tsn[0]==0x00&&tsn[1]==0x00&&tsn[2]==0x00&&tsn[3]==0x00))
		sprintf(tsn2, "----");
	else
	{
		sprintf(tsn2, "%c%c%c%c%c%c%c%c",tsn[0],tsn[1],tsn[2],tsn[3],tsn[4],tsn[5],tsn[6],tsn[7]);
	}
	TextOut(hdc, nLeft2, y, tsn2);
#else
	if ((tsn[0]>=20)||(tsn[0]==0))
		TextOut(hdc, nLeft2, y,"----");
	else
	{
		char tmpBuff[40];
		int i;
		for (i=0;i<tsn[0];i++)
		{
			tmpBuff[(tsn[0]-i-1)<<1]=((tsn[i+1]>>4) &0x0F)+0x30;//8
			tmpBuff[((tsn[0]-i-1)<<1)+1]=(tsn[i+1] &0x0F)+0x30;//9
		}
		tmpBuff[tsn[0]<<1] = '\0';
		//if (tmpBuff[0]==0x30)	//oct必定为双位的
		//	TextOut(hdc, nLeft2, y,tmpBuff+1);
		//else
			TextOut(hdc, nLeft2, y,tmpBuff);
	}
#endif
	y += nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "软件版本");
	if (nSelete == 2)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2, y, (((sv[0]==NONE_CHAR)||(sv[0]==0x00&&sv[1]==0x00&&sv[2]==0x00&&sv[3]==0x00))?"----":sv));

	y += nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "内部版本");
	if (nSelete == 3)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2, y, (((sv_inn[0]==NONE_CHAR)||(sv_inn[0]==0x00&&sv_inn[1]==0x00&&sv_inn[2]==0x00&&sv_inn[3]==0x00))?"----":sv_inn));

	y += nAdd; // 载波测量点点抄：
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "载波点抄");
	if (nSelete == 4)//
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2, y, plcconf);

	y += nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "抄表设置");
	if (nSelete == 5)//
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2, y, (g_DefaultSet.FacMode == 0x55)?g_DefaultFlag[0]:g_DefaultFlag[1]);

	y += nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "信道切换");
	if (nSelete == 6)//
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2, y, (eth_gprs_auto_enable == 0xaa)?g_EnableFlag[1]:g_EnableFlag[0]);

	ITEM_NOT_SELECT;
	if (nSelete == 7)//
	{	ITEM_SELECT;
	}
	#if HARDWARE_TYPE == 1
	TextOut(hdc, nLeft2 + 80 , y, "保存");
	#else
	y += nAdd;
	TextOut(hdc, nLeft2 , y, "保存");
	#endif

	ITEM_NOT_SELECT;
	PopListDraw(hdc);
}

void KeyMsgReturnGetManufacturer(char* pNewStr, void* pPara)
{
	if (pNewStr!=NULL && strlen(pNewStr)>0)
	{
		manufacturer[0] = pNewStr[0];
		manufacturer[1] = pNewStr[1];
		manufacturer[2] = pNewStr[2];
		manufacturer[3] = pNewStr[3];
		manufacturer[4] = '\0';
		if (strlen(pNewStr)>4)
		{
			MessageBox("警告:超过4字节的部分无效");
		}
	}
}

void KeyMsgReturnGetTerminalSN(char* pNewStr, void* pPara)
{
	if (pNewStr!=NULL && strlen(pNewStr)>0)
	{
		memset(tsn, '\0', sizeof(tsn));

		if (strlen(pNewStr)>20)
		{
			pNewStr[20] = '\0';
			MessageBox("警告:超过20字节的部分无效");
		}
		#if BEIJING_JZQ_EN == 1
		memcpy(tsn, pNewStr, strlen(pNewStr));
		#endif
	}
	else
	{
		memset(tsn, NONE_CHAR, sizeof(tsn));
	}
	tsn[20] = '\0';
}

void KeyMsgReturnGetSoftwareVer(char* pNewStr, void* pPara)
{
	if (pNewStr!=NULL && strlen(pNewStr)>0)
	{
		sv[0] = pNewStr[0];
		sv[1] = pNewStr[1];
		sv[2] = pNewStr[2];
		sv[3] = pNewStr[3];
		sv[4] = '\0';
	}
}

void KeyMsgReturnGetSoftwareVerInn(char* pNewStr, void* pPara)
{
	if (pNewStr!=NULL && strlen(pNewStr)>0)
	{
		if (strlen(pNewStr)>2)
		{
			pNewStr[2] = '\0';
			MessageBox("警告:超过2字节的部分无效");
		}
		sv_inn[0] = pNewStr[0];
		sv_inn[1] = pNewStr[1];
		sv_inn[2] = 0;
		sv_inn[3] = 0;
		sv_inn[4] = 0;
	}
}

static BOOL KeyMsg(int nKey)
{
	if (g_guiComm.bPopList == TRUE)
	{
		int nSel;
		PopListKeyMsg(nKey, &nSel);
		if(nKey == KEY_OK)
		{
			switch (nSelete)
			{
			case 0:
				break;

			case 1:
				break;

			case 2:
				break;

			case 3:
				break;

			case 4:
				if (nSel == 0)
					strcpy(plcconf, "启用");
				else
					strcpy(plcconf, "停用");
				break;

			case 5:
				if(nSel ==0)
					g_DefaultSet.FacMode = 0x55;
				else
					g_DefaultSet.FacMode = 0xAA;
				break;

			case 6:
				if (nSel == 0)
					eth_gprs_auto_enable = 0x55;
				else
					eth_gprs_auto_enable = 0xaa;
				break;

			case 7:
				break;

			default:
				break;
			}
		}
		g_bRunPaint = TRUE;
		return (TRUE);
	}
	switch (nKey)
	{
	case KEY_UP:
		if(nSelete == 0)
			nSelete = 7;
		else
			nSelete--;
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return (TRUE);

	case KEY_DOWN:
		if (nSelete == 7)
			nSelete = 0;
		else
			nSelete++;
		UpdateWindow(g_guiComm.hMainWnd, TRUE);
		return (TRUE);

	case KEY_OK:
		switch (nSelete)
		{
		case 0:
			InputBox(manufacturer, KeyMsgReturnGetManufacturer, NULL, 0x1D, 0x04);
			break;

		case 1:
			if (tsn[0]==NONE_CHAR)
			{	tsn[0] = '\0';
			}
			InputBox(tsn, KeyMsgReturnGetTerminalSN, NULL, 0x1D, 0x04);
			break;

		case 2:
			if (sv[0]==NONE_CHAR)
			{	sv[0] = '\0';
			}
			InputBox(sv, KeyMsgReturnGetSoftwareVer, NULL, 0x1D, 0x04);
			break;

		case 3:
			if (sv_inn[0]==NONE_CHAR)
			{	sv_inn[0] = '\0';
			}
			InputBox(sv_inn, KeyMsgReturnGetSoftwareVerInn, NULL, 0x20, 0x20);
			break;

		case 4:				// 载波抄表配置
			{
				const char * List[] = {"启用","停用"};
				PopList(List, 2, plcconf, 90, 80, FALSE);
			}
			break;

		case 5:				// 抄表设置
			PopList(g_DefaultFlag, 2, (g_DefaultSet.FacMode == 0x55)?g_DefaultFlag[0]:g_DefaultFlag[1],90, 95, FALSE);
			break;

		case 6:				// 以太网、GPRS自动切换
			PopList(g_EnableFlag, 2, (eth_gprs_auto_enable == 0xaa)?g_EnableFlag[1]:g_EnableFlag[0],90, 95, FALSE);
			break;

		case 7: // 保存以上参数
			if (manufacturer[0] != NONE_CHAR)
			{
				//WriteXPara(TMN_RUN_PARA, FAC_CODE_ID, manufacturer, 0);
			}
			if (tsn[0] != NONE_CHAR)
			{
				#if BEIJING_JZQ_EN == 1
				int  i;
				char tmp[3];
				char tsn2[4];
				tmp[2] = '\0';
				for(i=0; i<4; i++)
				{
				    memcpy(tmp, &tsn[i*2], 2);
				    tsn2[i] = (char)hexasc2int(tmp);
				    printf("tsn2[%d] = %02x\n",i,tsn2[i]);
				}
				//WriteXPara(TMN_PARA_EXTEND, AREA_SOFT_VERSION_ID, tsn2, 0);
				#else
				//commfun_WriteTmnSerialNum(tsn);
				#endif
			}
			if (sv[0]!=NONE_CHAR && sv[1]!=NONE_CHAR && sv[2]!=NONE_CHAR && sv[3]!=NONE_CHAR)
			{
				//WriteXPara(TMN_RUN_PARA, SOFTWARE_VERSION_ID, sv, 0);
			}
			if (sv_inn[0]!=NONE_CHAR && sv_inn[1]!=NONE_CHAR && sv_inn[2]!=NONE_CHAR && sv_inn[3]!=NONE_CHAR)
			{
				commfun_toupper(sv_inn);		//将内部版本信息转换为大写字母
				//WriteXPara(TMN_RUN_PARA, SOFTWARE_VERSION_INN_ID, sv_inn, 0);
			}

			// 载波抄表配置
			{
				/*uint16 conf;
                #if HUNAN_FUN == 1
				if (strcmp(plcconf, "启用")==0)
					conf = 0xFFFF;
				else
					conf = 0x00AA;
                #else
				if (strcmp(plcconf, "启用")==0)
					conf = 0x00AA;
				else
					conf = 0xFFFF;
                #endif
				WriteXPara(TMN_RUN_PARA, PLC_READ_RELAY, (char*)(&conf), 0);*/
			}
			#if	FAC_MODE == 1		//出厂模式
			//commfun_WriteFacMode(&g_DefaultSet);
			#endif
			//WriteXPara(TMN_RUN_PARA, ETH_GPRS_SWITCH_AUTO, (char *)&eth_gprs_auto_enable, TMN_ID);
			MessageBox("参数已保存");
			break;

		default:
			break;
		}
		return (TRUE);
		break;
	default:
		return (FALSE);
		break;
	}
}


/************************************************************************/
/* 产品特性配置                                                */
/************************************************************************/
static int nSelete2 = 0; // 当前所选中的行号:nSelete2=0~4
static char acconf[15]; // acconf: AC Conf(交采电源配置)
static char accprecisionLevel[4];//交采有功精度等级
static uint32 pulseconst = 0;  // 交采脉冲常数
const char* g_pulseconst[] = {"3200","6400","8000","10000","20000"};
char nState;
unsigned char nState_pulse;
// 2013 COMMON BUG
const char *RS[]={"无RS编码", "RS11/15", "RS9/15"};
unsigned char rscode;
char PDCollrctMode;
// 2013 COMMON BUG END
char meterWriteLogEN = 0;
char  realdataToPointData = 0;
void ProductFeatureCfgInit(DWORD dwPluginId)
{
	uint8 precisionLevel = 0;
	int nRet;

	nSelete2 = 0;
	g_guiComm.bCanTrunDisp = FALSE;
	g_guiComm.bTrundispTally = FALSE;

	// 交采电源配置
	{
		uint8 nType = AC_LINE_MODE_ERR;  //  1字节，交采的接线方式，当前通道的类型 1-三相三线 0-三相四线 0xFF-无效
		uint8 nTypeEx = AC_VOLT_TYPE_ERR;// 1字节,手动选择三相四线终端额定电压是220V还是57.7V：在已确定交采硬件为三相四线的前提下，配置为0x55时表示57.7V，否则按默认220V处理；当确定交采硬件为三相三线时,一律按100V处理，忽略AC_Config_577_ID

		//nRet = ReadFromEEPROM((char*)&nType, FM_AC_POWER_TYPE_ADDR, 1);
		nRet = -1;
		if (nRet > 0 && nType != AC_LINE_MODE_ERR)
		{
			if (nType == AC_LINE_MODE_3P3L)
			{
				strcpy(acconf, "三相三线100V");
			}
			else
			{
				//ReadFromEEPROM((char*)&nTypeEx, FM_AC_Config_577_ADDR, 1);
				if (nTypeEx == AC_VOLT_TYPE_577)
				{
					strcpy(acconf, "三相四线57.7V");
				}
				else
				{
					strcpy(acconf, "三相四线220V");
				}
			}
		}
		else
		{
			strcpy(acconf, "(无内置交采)");
		}
	}

	//交采有功精度等级
	//nRet = read_pdata_xram(USER_DEFINED_DATA_FILENAME,(char*)&precisionLevel,0,1);
	if (nRet < 0 || precisionLevel == 0xFF)
	{
		strcpy(accprecisionLevel, "0.5");
	}
	else
	{
		if (precisionLevel == 0x01)
		{
			strcpy(accprecisionLevel, "1.0");
		}
		else
		{
			strcpy(accprecisionLevel, "0.5");
		}
	}

    // 从铁电读取脉冲常数
    //pulseconst = commfun_ReadACPulseConst();
    if (pulseconst == 0)
    {
        pulseconst = 6400;
    }
	else if (pulseconst == 3200)
	{
		nState_pulse = 0;
	}
	else if (pulseconst == 6400)
	{
		nState_pulse = 1;
	}
	else if (pulseconst == 8000)
	{
		nState_pulse = 2;
	}
	else if (pulseconst == 10000)
	{
		nState_pulse = 3;
	}
	else if(pulseconst == 20000)
	{
		nState_pulse = 4;
	}
	else
    {
    	pulseconst = 6400; // 防止将三相四线57.7的脉冲常数保存给三相四线220
		nState_pulse = 1;
    }

	// 2013 COMMON BUG
	//if (ReadXPara(TMN_RUN_PARA, TMN_RADIO_RS_EN, (char *)&rscode, TMN_ID)<0 || (rscode>=3))
	{
		rscode = 0;
	}
	// 2013 COMMON BUG END

	// 485日志使能开关
    //if ((ReadXPara(TMN_PARA_EXTEND, METER_485_LOG_EN, (char *)&meterWriteLogEN, 0x0) < 0) || (meterWriteLogEN != 0xaa))
	{
		meterWriteLogEN = 0x55;
	}

	// 实时数据转存冻结数据使能
    //if ((ReadXPara(TMN_PARA_EXTEND, DATA_POINT_COPY_FROM_REALDATA, (char *)&realdataToPointData, 0x0) < 0) || (realdataToPointData != 0x55))
	{
		realdataToPointData = 0;
	}
    PDCollrctMode = 0;
    //if(ReadXPara(TMN_PARA_EXTEND,AMR07_DAY_DATA_SAVE_TYPE,(char*)&PDCollrctMode,0) < 0)
        PDCollrctMode = 0;

	return;
}

void ProductFeatureCfgDraw(HDC hdc)
{
	char czbuf[10];
	char CheckSumFlag=0;
	int nLeft1, nLeft2, nAdd, y;
	nLeft1 = 10;
	nLeft2 = 90;
	nAdd = 13;
	y = 20;

	SelectFont(hdc,g_guiComm.fontSmall);

	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "交采电源");
	if (nSelete2 == 0)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2-15, y, acconf);

	y+=nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "交采有功精度");
	if (nSelete2 == 1)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2, y, accprecisionLevel);

	y+=nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "交采脉冲常数");
	if (nSelete2 == 2)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2, y, g_pulseconst[nState_pulse]);

	y+=nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "校验和验证");
	CheckSumFlag = 0;
	//ReadFromEEPROM(&CheckSumFlag, FM_CheckSumOKFlag_ADDR, 1);
	if(CheckSumFlag==0x55)
	{
		sprintf(czbuf,"不一致");
	}
	else if(CheckSumFlag==0xcc)
	{
		sprintf(czbuf,"一致");
	}
	else
	{
		sprintf(czbuf,"未知");
	}
	TextOut(hdc, nLeft2, y, czbuf);
	// 2013 COMMON BUG
	y+=nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "RS编码");// 2013 COMMON BUG
	if (nSelete2 == 3)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2, y, RS[rscode]);

	// 485日志使能
	y+=nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "485日志使能");
	if (nSelete2 == 4)
	{	ITEM_SELECT;
	}

	if (meterWriteLogEN == 0x55)
		TextOut(hdc, nLeft2, y, "打开");
	else
		TextOut(hdc, nLeft2, y, "关闭");

	// 实时数据转存使能
	y+=nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "485表实时转存");
	if (nSelete2 == 5)
	{	ITEM_SELECT;
	}

	if (realdataToPointData == 0x55)
		TextOut(hdc, nLeft2, y, "转存");
	else
		TextOut(hdc, nLeft2, y, "不转存");

	y+=nAdd;
	ITEM_NOT_SELECT;


    TextOut(hdc, nLeft1, y, "载波表07转存");
    if(nSelete2 == 6)
    {
        ITEM_SELECT;
	}
    if(PDCollrctMode == 0xAA)
        TextOut(hdc, nLeft2, y, "转存");
    else
        TextOut(hdc, nLeft2, y, "不转存");

    ITEM_NOT_SELECT;
    if (nSelete2 == 7)// 2013 COMMON BUG
    {
        ITEM_SELECT;
    }
#if HARDWARE_TYPE == 1
    TextOut(hdc, nLeft2 + 80 , y, "保存");
#else
    y += nAdd;
	TextOut(hdc, nLeft2, y, "保存");
#endif

	// 2013 COMMON BUG END

	ITEM_NOT_SELECT;
	PopListDraw(hdc);
}

BOOL ProductFeatureCfgKeyMsg(int nKey)
{
	// 弹出List选择处理
	if(g_guiComm.bPopList == TRUE)
	{
		int nSel=0;

		PopListKeyMsg(nKey, &nSel);
		if(nKey == KEY_OK)
		{
			if (nSelete2==0 && nSel>=0 && nSel<=1)		// 交采电源配置
			{
				int nRet;
				uint8 nType = AC_LINE_MODE_ERR; // 1字节，交采的接线方式

				//nRet = ReadFromEEPROM((char*)&nType, FM_AC_POWER_TYPE_ADDR, 1);
				nRet = -1;
				if (nRet > 0 && nType != AC_LINE_MODE_ERR)
				{
					if (nType == AC_LINE_MODE_3P4L)
					{
						if (nSel == 0)
						{
							strcpy(acconf, "三相四线220V");
						}
						if (nSel == 1)
						{
							strcpy(acconf, "三相四线57.7V");
						}
					}
				}
			}
			if (nSelete2==1 && nSel>=0 && nSel<=1)
			{
				if (nSel == 0)
				{
					strcpy(accprecisionLevel, "1.0");
				}
				else
				{
					strcpy(accprecisionLevel, "0.5");
				}
			}
			if (nSelete2==2 )
			{
				if(nSel == 0)
				{
					nState_pulse = 0;
					pulseconst = 3200;
				}
				else
				{
					nState_pulse = 1;
					pulseconst = 6400;
				}
			}
			// 2013 COMMON BUG
			if (nSelete2 == 3)
			{
				rscode = nSel;
			}
			// 2013 COMMON BUG END
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
		}
		return TRUE;
	}
	else
	{
		switch(nKey)
		{
		case KEY_OK:
			switch (nSelete2)
			{
			case 0:			// 交采电源配置
				{
					int nRet;
					uint8 nType = AC_LINE_MODE_ERR; // 1字节，交采的接线方式
					const char * List33[] = {"三相三线100V"};
					const char * List34[] = {"三相四线220V","三相四线57.7V"};

					//nRet = ReadFromEEPROM((char*)&nType, FM_AC_POWER_TYPE_ADDR, 1);
					nRet = -1;
					if (nRet > 0 && nType != AC_LINE_MODE_ERR)
					{
						if (nType == AC_LINE_MODE_3P3L)
						{
							PopList(List33, 1, "三相三线100V", 75, 20, FALSE);
						}
						else
						{
							PopList(List34, 2, acconf, 75, 20, FALSE);
						}
					}
					else
					{
					}
				}
				break;

			case 1:			//交采有功精度等级
				{
					const char *ListLevel[] = {"1.0", "0.5"};
					PopList(ListLevel,2, accprecisionLevel, 90, 36, FALSE);
				}
				break;

			case 2:
				PopList(g_pulseconst, 2, g_pulseconst[nState_pulse], 90, 52, FALSE);
				break;

			case 3: // 2013 COMMON BUG END
				PopList(RS, 3, RS[rscode], 90, 82, FALSE);
				break;

			case 4:
				if (meterWriteLogEN != 0x55)
					meterWriteLogEN = 0x55;
				else
					meterWriteLogEN = 0xaa;
				break;

			case 5:
				if (realdataToPointData != 0x55)
					realdataToPointData = 0x55;
				else
					realdataToPointData = 0x00;
				break;

            case 6:
                if(PDCollrctMode == 0xAA)
                    PDCollrctMode = 0;
                else
                    PDCollrctMode = 0xAA;
                UpdateWindow(g_guiComm.hMainWnd, TRUE);
                break;
                case 7: // 保存以上参数// 2013 COMMON BUG
				{
					uint8 nTypeEx = AC_VOLT_TYPE_ERR;
					// 交采电源配置
					{
						if (strcmp(acconf, "三相四线57.7V")==0)
						{
							nTypeEx = AC_VOLT_TYPE_577;
							//WriteToEEPROM((char*)&nTypeEx, FM_AC_Config_577_ADDR, 1);
						}
						else if (strcmp(acconf, "三相四线220V")==0)
						{
							nTypeEx = AC_VOLT_TYPE_220;
							//WriteToEEPROM((char*)&nTypeEx, FM_AC_Config_577_ADDR, 1);
						}
						else
						{
							nTypeEx = AC_VOLT_TYPE_ERR;
							//WriteToEEPROM((char*)&nTypeEx, FM_AC_Config_577_ADDR, 1);
						}
					}

					//交采有功精度等级
					{
						/*uint8 precisionLevel;
						if (strcmp(accprecisionLevel, "1.0") == 0)
						{
							precisionLevel = 0x01;
						}
						else
						{
							precisionLevel = 0x02;
						}
						write_pdata_xram(USER_DEFINED_DATA_FILENAME,(char*)&precisionLevel, 0, 1);*/
					}
					//WriteXPara(TMN_RUN_PARA, TMN_RADIO_RS_EN, (char *)&rscode, TMN_ID);// 2013 COMMON BUG

					if (nTypeEx == AC_VOLT_TYPE_220) // 只在三相四线220V下保存脉冲常数
					{
                    	//commfun_WriteACPulseConst(pulseconst); // 保存脉冲常数至铁电
					}

					// 保存日志使能开关状态
   					//WriteXPara(TMN_PARA_EXTEND, METER_485_LOG_EN, (char *)&meterWriteLogEN, 0x0);

					// 保存实时数据转存使能开关状态
   					//WriteXPara(TMN_PARA_EXTEND, DATA_POINT_COPY_FROM_REALDATA, (char *)&realdataToPointData, 0x0);
                    //WriteXPara(TMN_PARA_EXTEND,AMR07_DAY_DATA_SAVE_TYPE,(char *)&PDCollrctMode,0);

					MessageBox("参数保存成功");
					break;
				}
			default: break;
			}
			break;
		case KEY_DOWN:
            if (nSelete2 == 7)// 2013 COMMON BUG
				nSelete2 = 0;
			else
				nSelete2++;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return (TRUE);

		case KEY_UP:
			if(nSelete2 == 0)
                nSelete2 = 7;// 2013 COMMON BUG
			else
				nSelete2--;
			UpdateWindow(g_guiComm.hMainWnd, TRUE);
			return (TRUE);
		default:
			return (FALSE);
			break;
		}
		return (TRUE);
	}
}

PLUGIN_INFO g_layerProductFeatureCfg = {
	0x20B70001/*id*/,
	ProductFeatureCfgInit/*InitLayer*/,
	NULL/*UninitLayer*/,
	ProductFeatureCfgDraw/*Draw*/,
	NULL/*Timer*/,
	ProductFeatureCfgKeyMsg/*keymsg*/,
	NULL/*ipc*/
};

/************************************************************************/
/* 产品特性配置结束													*/
/************************************************************************/

