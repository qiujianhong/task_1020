#include "miniguidef.h"

/*-ҳ��ȫ�ֱ���-*/
static int nSelete = 0; // ��ǰ��ѡ�е��к�:nSelete=0~4
static char manufacturer[16];
static char tsn[20+1]; // tsn: terminal serial num
static char sv[4+1]; // sv: software version
static char sv_inn[4+1]; // �ڲ��汾��
static char plcconf[5]; // plcconf: PLC Conf(PLC�㳭����)
#if	FAC_MODE == 1		//����ģʽ
S_FacMode g_DefaultSet;
const char* g_DefaultFlag[] = {"����ģʽ", "�ͼ�ģʽ"};
#endif
uint8 eth_gprs_auto_enable = 0;		// �Ƿ�������̫��GPRS�Զ��л�  0xaa-������ 0x55-����
const char* g_EnableFlag[] = {"����", "ͣ��"};

/*-ҳ���Ӻ�������-*/
static void InitLayer(DWORD dwPluginID);
static void UninitLayer(void);
static void Draw(HDC hdc);
static BOOL KeyMsg(int nKey);

/*-ҳ��-*/
PLUGIN_INFO g_layerHiddenLayer = {
	0xffffffff, // HiddenLayer����ҪID
	InitLayer,
	UninitLayer,
	Draw,
	NULL, // Timer
	KeyMsg,
	NULL, // IPC
};

/*-ҳ���Ӻ���-*/
static void InitLayer(DWORD dwPluginID)
{
	int nRet = 0;
	#if BEIJING_JZQ_EN == 1
	char tsn2[10];
	#endif
	nSelete = 0;
	g_guiComm.bCanTrunDisp = FALSE;
	g_guiComm.bTrundispTally = FALSE;

	// ���̴���
	memset(manufacturer, '\0', sizeof(manufacturer));
	//nRet = ReadXPara(TMN_RUN_PARA, FAC_CODE_ID, manufacturer, 0);
	nRet = -1;
	if (nRet < 0 || manufacturer[0]==NONE_CHAR)
	{
		// ���̴�����Чʱ��ʾĬ��ֵ
		strcpy(manufacturer, MANUFACTURER);
	}
	manufacturer[4] = '\0';

	// �豸���
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

	// �ն�����汾��
	char sv_reversed[4] = {0}; // ע:�����ֵ�������汾��һֱ�ǵ������е�
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

	//�����ڲ��汾��
	memset(sv_inn, 0x0, sizeof(sv_inn));
	//nRet = ReadXPara(TMN_RUN_PARA, SOFTWARE_VERSION_INN_ID, sv_inn, 0);
	nRet = -1;
	if (nRet < 0)
	{
		memset(sv_inn, NONE_CHAR, sizeof(sv_inn));
	}
	commfun_toupper(sv_inn);		//���ڲ��汾��Ϣת��Ϊ��д��ĸ

	// PLC�㳭����
	{
		uint16 conf;
		conf = NONE_WORD;
		//nRet = ReadXPara(TMN_RUN_PARA, PLC_READ_RELAY, (char*)(&conf), 0);
		nRet = -1;
        #if HUNAN_FUN == 1
        if (nRet > 0 && conf == 0x00AA)
            strcpy(plcconf, "ͣ��");
        else
            strcpy(plcconf, "����");
        #else
		if (nRet > 0 && conf == 0x00AA)
			strcpy(plcconf, "����");
		else
			strcpy(plcconf, "ͣ��");
        #endif
	}

	#if	FAC_MODE == 1		//����ģʽ
	//commfun_ReadFacMode(&g_DefaultSet);
	#endif

	//nRet = ReadXPara(TMN_RUN_PARA, ETH_GPRS_SWITCH_AUTO, (char *)&eth_gprs_auto_enable, TMN_ID);
	nRet = -1;
	if(nRet > 0 && (eth_gprs_auto_enable == 0xaa))
		eth_gprs_auto_enable = 0xaa;		//�Ƿ�������̫��GPRS�Զ��л�  0xaa-������ 0x55-���ã�Ĭ�Ͽ�����
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
	TextOut(hdc, nLeft1, y, "���̴���");
	if (nSelete == 0)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2, y, (((manufacturer[0]==NONE_CHAR)||(manufacturer[0]==0x00&&manufacturer[1]==0x00&&manufacturer[2]==0x00&&manufacturer[3]==0x00)||(manufacturer[0]=='x'&&manufacturer[1]=='x'))? "----":manufacturer));

	y += nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "�ն˱��");
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
		//if (tmpBuff[0]==0x30)	//oct�ض�Ϊ˫λ��
		//	TextOut(hdc, nLeft2, y,tmpBuff+1);
		//else
			TextOut(hdc, nLeft2, y,tmpBuff);
	}
#endif
	y += nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "����汾");
	if (nSelete == 2)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2, y, (((sv[0]==NONE_CHAR)||(sv[0]==0x00&&sv[1]==0x00&&sv[2]==0x00&&sv[3]==0x00))?"----":sv));

	y += nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "�ڲ��汾");
	if (nSelete == 3)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2, y, (((sv_inn[0]==NONE_CHAR)||(sv_inn[0]==0x00&&sv_inn[1]==0x00&&sv_inn[2]==0x00&&sv_inn[3]==0x00))?"----":sv_inn));

	y += nAdd; // �ز�������㳭��
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "�ز��㳭");
	if (nSelete == 4)//
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2, y, plcconf);

	y += nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "��������");
	if (nSelete == 5)//
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2, y, (g_DefaultSet.FacMode == 0x55)?g_DefaultFlag[0]:g_DefaultFlag[1]);

	y += nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "�ŵ��л�");
	if (nSelete == 6)//
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2, y, (eth_gprs_auto_enable == 0xaa)?g_EnableFlag[1]:g_EnableFlag[0]);

	ITEM_NOT_SELECT;
	if (nSelete == 7)//
	{	ITEM_SELECT;
	}
	#if HARDWARE_TYPE == 1
	TextOut(hdc, nLeft2 + 80 , y, "����");
	#else
	y += nAdd;
	TextOut(hdc, nLeft2 , y, "����");
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
			MessageBox("����:����4�ֽڵĲ�����Ч");
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
			MessageBox("����:����20�ֽڵĲ�����Ч");
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
			MessageBox("����:����2�ֽڵĲ�����Ч");
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
					strcpy(plcconf, "����");
				else
					strcpy(plcconf, "ͣ��");
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

		case 4:				// �ز���������
			{
				const char * List[] = {"����","ͣ��"};
				PopList(List, 2, plcconf, 90, 80, FALSE);
			}
			break;

		case 5:				// ��������
			PopList(g_DefaultFlag, 2, (g_DefaultSet.FacMode == 0x55)?g_DefaultFlag[0]:g_DefaultFlag[1],90, 95, FALSE);
			break;

		case 6:				// ��̫����GPRS�Զ��л�
			PopList(g_EnableFlag, 2, (eth_gprs_auto_enable == 0xaa)?g_EnableFlag[1]:g_EnableFlag[0],90, 95, FALSE);
			break;

		case 7: // �������ϲ���
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
				commfun_toupper(sv_inn);		//���ڲ��汾��Ϣת��Ϊ��д��ĸ
				//WriteXPara(TMN_RUN_PARA, SOFTWARE_VERSION_INN_ID, sv_inn, 0);
			}

			// �ز���������
			{
				/*uint16 conf;
                #if HUNAN_FUN == 1
				if (strcmp(plcconf, "����")==0)
					conf = 0xFFFF;
				else
					conf = 0x00AA;
                #else
				if (strcmp(plcconf, "����")==0)
					conf = 0x00AA;
				else
					conf = 0xFFFF;
                #endif
				WriteXPara(TMN_RUN_PARA, PLC_READ_RELAY, (char*)(&conf), 0);*/
			}
			#if	FAC_MODE == 1		//����ģʽ
			//commfun_WriteFacMode(&g_DefaultSet);
			#endif
			//WriteXPara(TMN_RUN_PARA, ETH_GPRS_SWITCH_AUTO, (char *)&eth_gprs_auto_enable, TMN_ID);
			MessageBox("�����ѱ���");
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
/* ��Ʒ��������                                                */
/************************************************************************/
static int nSelete2 = 0; // ��ǰ��ѡ�е��к�:nSelete2=0~4
static char acconf[15]; // acconf: AC Conf(���ɵ�Դ����)
static char accprecisionLevel[4];//�����й����ȵȼ�
static uint32 pulseconst = 0;  // �������峣��
const char* g_pulseconst[] = {"3200","6400","8000","10000","20000"};
char nState;
unsigned char nState_pulse;
// 2013 COMMON BUG
const char *RS[]={"��RS����", "RS11/15", "RS9/15"};
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

	// ���ɵ�Դ����
	{
		uint8 nType = AC_LINE_MODE_ERR;  //  1�ֽڣ����ɵĽ��߷�ʽ����ǰͨ�������� 1-�������� 0-�������� 0xFF-��Ч
		uint8 nTypeEx = AC_VOLT_TYPE_ERR;// 1�ֽ�,�ֶ�ѡ�����������ն˶��ѹ��220V����57.7V������ȷ������Ӳ��Ϊ�������ߵ�ǰ���£�����Ϊ0x55ʱ��ʾ57.7V������Ĭ��220V������ȷ������Ӳ��Ϊ��������ʱ,һ�ɰ�100V��������AC_Config_577_ID

		//nRet = ReadFromEEPROM((char*)&nType, FM_AC_POWER_TYPE_ADDR, 1);
		nRet = -1;
		if (nRet > 0 && nType != AC_LINE_MODE_ERR)
		{
			if (nType == AC_LINE_MODE_3P3L)
			{
				strcpy(acconf, "��������100V");
			}
			else
			{
				//ReadFromEEPROM((char*)&nTypeEx, FM_AC_Config_577_ADDR, 1);
				if (nTypeEx == AC_VOLT_TYPE_577)
				{
					strcpy(acconf, "��������57.7V");
				}
				else
				{
					strcpy(acconf, "��������220V");
				}
			}
		}
		else
		{
			strcpy(acconf, "(�����ý���)");
		}
	}

	//�����й����ȵȼ�
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

    // �������ȡ���峣��
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
    	pulseconst = 6400; // ��ֹ����������57.7�����峣���������������220
		nState_pulse = 1;
    }

	// 2013 COMMON BUG
	//if (ReadXPara(TMN_RUN_PARA, TMN_RADIO_RS_EN, (char *)&rscode, TMN_ID)<0 || (rscode>=3))
	{
		rscode = 0;
	}
	// 2013 COMMON BUG END

	// 485��־ʹ�ܿ���
    //if ((ReadXPara(TMN_PARA_EXTEND, METER_485_LOG_EN, (char *)&meterWriteLogEN, 0x0) < 0) || (meterWriteLogEN != 0xaa))
	{
		meterWriteLogEN = 0x55;
	}

	// ʵʱ����ת�涳������ʹ��
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
	TextOut(hdc, nLeft1, y, "���ɵ�Դ");
	if (nSelete2 == 0)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2-15, y, acconf);

	y+=nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "�����й�����");
	if (nSelete2 == 1)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2, y, accprecisionLevel);

	y+=nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "�������峣��");
	if (nSelete2 == 2)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2, y, g_pulseconst[nState_pulse]);

	y+=nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "У�����֤");
	CheckSumFlag = 0;
	//ReadFromEEPROM(&CheckSumFlag, FM_CheckSumOKFlag_ADDR, 1);
	if(CheckSumFlag==0x55)
	{
		sprintf(czbuf,"��һ��");
	}
	else if(CheckSumFlag==0xcc)
	{
		sprintf(czbuf,"һ��");
	}
	else
	{
		sprintf(czbuf,"δ֪");
	}
	TextOut(hdc, nLeft2, y, czbuf);
	// 2013 COMMON BUG
	y+=nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "RS����");// 2013 COMMON BUG
	if (nSelete2 == 3)
	{	ITEM_SELECT;
	}
	TextOut(hdc, nLeft2, y, RS[rscode]);

	// 485��־ʹ��
	y+=nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "485��־ʹ��");
	if (nSelete2 == 4)
	{	ITEM_SELECT;
	}

	if (meterWriteLogEN == 0x55)
		TextOut(hdc, nLeft2, y, "��");
	else
		TextOut(hdc, nLeft2, y, "�ر�");

	// ʵʱ����ת��ʹ��
	y+=nAdd;
	ITEM_NOT_SELECT;
	TextOut(hdc, nLeft1, y, "485��ʵʱת��");
	if (nSelete2 == 5)
	{	ITEM_SELECT;
	}

	if (realdataToPointData == 0x55)
		TextOut(hdc, nLeft2, y, "ת��");
	else
		TextOut(hdc, nLeft2, y, "��ת��");

	y+=nAdd;
	ITEM_NOT_SELECT;


    TextOut(hdc, nLeft1, y, "�ز���07ת��");
    if(nSelete2 == 6)
    {
        ITEM_SELECT;
	}
    if(PDCollrctMode == 0xAA)
        TextOut(hdc, nLeft2, y, "ת��");
    else
        TextOut(hdc, nLeft2, y, "��ת��");

    ITEM_NOT_SELECT;
    if (nSelete2 == 7)// 2013 COMMON BUG
    {
        ITEM_SELECT;
    }
#if HARDWARE_TYPE == 1
    TextOut(hdc, nLeft2 + 80 , y, "����");
#else
    y += nAdd;
	TextOut(hdc, nLeft2, y, "����");
#endif

	// 2013 COMMON BUG END

	ITEM_NOT_SELECT;
	PopListDraw(hdc);
}

BOOL ProductFeatureCfgKeyMsg(int nKey)
{
	// ����Listѡ����
	if(g_guiComm.bPopList == TRUE)
	{
		int nSel=0;

		PopListKeyMsg(nKey, &nSel);
		if(nKey == KEY_OK)
		{
			if (nSelete2==0 && nSel>=0 && nSel<=1)		// ���ɵ�Դ����
			{
				int nRet;
				uint8 nType = AC_LINE_MODE_ERR; // 1�ֽڣ����ɵĽ��߷�ʽ

				//nRet = ReadFromEEPROM((char*)&nType, FM_AC_POWER_TYPE_ADDR, 1);
				nRet = -1;
				if (nRet > 0 && nType != AC_LINE_MODE_ERR)
				{
					if (nType == AC_LINE_MODE_3P4L)
					{
						if (nSel == 0)
						{
							strcpy(acconf, "��������220V");
						}
						if (nSel == 1)
						{
							strcpy(acconf, "��������57.7V");
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
			case 0:			// ���ɵ�Դ����
				{
					int nRet;
					uint8 nType = AC_LINE_MODE_ERR; // 1�ֽڣ����ɵĽ��߷�ʽ
					const char * List33[] = {"��������100V"};
					const char * List34[] = {"��������220V","��������57.7V"};

					//nRet = ReadFromEEPROM((char*)&nType, FM_AC_POWER_TYPE_ADDR, 1);
					nRet = -1;
					if (nRet > 0 && nType != AC_LINE_MODE_ERR)
					{
						if (nType == AC_LINE_MODE_3P3L)
						{
							PopList(List33, 1, "��������100V", 75, 20, FALSE);
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

			case 1:			//�����й����ȵȼ�
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
                case 7: // �������ϲ���// 2013 COMMON BUG
				{
					uint8 nTypeEx = AC_VOLT_TYPE_ERR;
					// ���ɵ�Դ����
					{
						if (strcmp(acconf, "��������57.7V")==0)
						{
							nTypeEx = AC_VOLT_TYPE_577;
							//WriteToEEPROM((char*)&nTypeEx, FM_AC_Config_577_ADDR, 1);
						}
						else if (strcmp(acconf, "��������220V")==0)
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

					//�����й����ȵȼ�
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

					if (nTypeEx == AC_VOLT_TYPE_220) // ֻ����������220V�±������峣��
					{
                    	//commfun_WriteACPulseConst(pulseconst); // �������峣��������
					}

					// ������־ʹ�ܿ���״̬
   					//WriteXPara(TMN_PARA_EXTEND, METER_485_LOG_EN, (char *)&meterWriteLogEN, 0x0);

					// ����ʵʱ����ת��ʹ�ܿ���״̬
   					//WriteXPara(TMN_PARA_EXTEND, DATA_POINT_COPY_FROM_REALDATA, (char *)&realdataToPointData, 0x0);
                    //WriteXPara(TMN_PARA_EXTEND,AMR07_DAY_DATA_SAVE_TYPE,(char *)&PDCollrctMode,0);

					MessageBox("��������ɹ�");
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
/* ��Ʒ�������ý���													*/
/************************************************************************/

