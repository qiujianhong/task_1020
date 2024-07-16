/*-
****************************************************************************************
 * Copyright (c) 2004,南京新联电子仪器有限责任公司
 *            All rights reserved.
 * 程序名称：get.c
 * 版本号：1.0
 * 功能：面向对象协议栈-应用层-数据编码接口
 * 开发人：qxc
 * 开发时间：2016-06-30
 * 修改者:
 * 修改时间:
 * 修改简要说明:
 * 其他： 
*****************************************************************************************
-*/
#include "baseCommon.h"


/****************************************************************************
*模块编号：                                                                  
*名称：                                                                      
*功能：数据编码，按照X-ADR标准编码                                                                     
*输入参数：                                                                  
*输出参数：                                                                  
*处理：                                                                      
*返回：无                                                                    
****************************************************************************/

void OopDataEncode(enum eDatatype dataType,uint16 dataLen,uint8 *pSrcBuff,uint8 *pDesBuff,uint16 *pOffset)
{
	uint16 offset = *pOffset;
	uint16 len = dataLen;
	pDesBuff[offset++] = dataType;
	if(dataType==NULL_DATA)
	{
		*pOffset = offset;
		return;
	}
	if((dataType == STRUCTURE)

		||(dataType == ARRAY)

		||(dataType == OCTET_STRING)

		||(dataType == VISIBLE_STRING)

		||(dataType == UTF8_STRIGN)

		||(dataType == BIT_STRING)

		||(dataType == eMAC)

		||(dataType == eRAND)

		||(dataType == eTSA))
	{
		if(dataType == BIT_STRING)

			len = dataLen *8;

		if(dataLen<0x80)
		{

			pDesBuff[offset++] = (uint8)len;

		}
		else
		{

			pDesBuff[offset++] = 0x82;

			pDesBuff[offset++] = (uint8)(len>>8) ;

			pDesBuff[offset++] = (uint8)len ;

		}
	}
	memcpy(pDesBuff+offset,pSrcBuff,dataLen);
	offset += dataLen;
	*pOffset = offset;
}

/****************************************************************************
*模块编号：                                                                  
*名称：                                                                      
*功能：                                                                      
*输入参数：                                                                  
*输出参数：                                                                  
*处理：                                                                      
*返回：无                                                                    
****************************************************************************/
uint8 getDataTypeLength(enum eDatatype dataType)
{
  switch(dataType)
  {
  		case NULL_DATA:
            return 0;
        case TBOOLEAN:
        case UNSIGNED:
        case INTEGER:
		case INTEGER_CUSTOM:
        case ENUM:
            return 1;

        case LONG_SIGNED:
		case LONG_SIGNED_CUSTOM:
        case LONG_UNSIGNED:
        case eOI:
        case eSCALER_UNIT:
            return 2;
			
		case TIME:
		case TIMEBCD:
            return 3;
        //4
        case DOUBLE_LONG:
		case DOUBLE_LONG_CUSTOM:
        case DOUBLE_LONG_UNSIGNED:
        case FLOAT32:
        case eOAD:
        case eOMD:
		case DATEBCD:
        case e645DATETIME_M:
        case e645DATETIME_W:
            return 4;
        //5
        case TDATE:
		case eCOMDCB:
            return 5;
        case DATETIMEBCD_S:
        case e645DATETIME:
            return 6;
        //7
        case DATE_TIME_S:
            return 7;
        //8
        case LONG64_SIGNED:
		case LONG64_SIGNED_CUSTOM:
        case LONG64_UNSIGNED:
        case FLOAT64:
            return 8;
        //9
        case DATETIMEBCD_H:
            return 9;
        //10
        case DATE_TIME:
            return 10;
        default:
            return 0xFF;
  }
}
/****************************************************************************
*模块编号：                                                                  
*名称：                                                                      
*功能：                                                                      
*输入参数：                                                                  
*输出参数：                                                                  
*处理：                                                                      
*返回：无                                                                    
****************************************************************************/
BOOL getNextReadData(const uint8 *inData,uint16 *offset,uint16 inDataLen,enum eDatatype DataType,BOOL bWithType)
{
	uint16 dataLen;
	uint8 nchioce;
	uint8 dataNum;
	uint8 i;
	uint16 offSet=*offset;
	enum eDatatype dataType;
    
	if(bWithType)
    {   
		dataType = DataType;
    }
	else
    {   
		dataType = (enum eDatatype)inData[offSet++];
    }
        
	if(dataType == NULL_DATA) 
    {   
	    *offset = offSet;
		return TRUE;
    }
	if((dataType == STRUCTURE)
		||(dataType == ARRAY)
		||(dataType == OCTET_STRING)
		||(dataType == VISIBLE_STRING)
		||(dataType == UTF8_STRIGN)
		||(dataType == BIT_STRING)
		||(dataType == eMAC)
		||(dataType == eRAND)
		||(dataType == eTSA))
	{
		dataLen = inData[offSet++];
		if(dataLen == 0x81)
			dataLen = inData[offSet++];
		else if(dataLen == 0x82)
		{
			dataLen = inData[offSet++];
			dataLen <<= 8;
			dataLen += inData[offSet++];
		}
		else if(dataLen>0x82||dataLen==0x80)
        {      
            return FALSE;
        }
		if((dataType != ARRAY) && (dataType != OCTET_STRING) && (dataType != VISIBLE_STRING) \
            && (dataType != eMAC) &&(dataLen == 0)) 
        {      
            return FALSE;
        }
        
		if(dataType == BIT_STRING)
        {      
			dataLen /= 8;
        }
        
		if((dataType == STRUCTURE)
			||(dataType == ARRAY))
		{
			uint16 eleNum = dataLen;
			while(eleNum--)
			{
				if(FALSE == getNextReadData(inData,&offSet,inDataLen,NULL_DATA,FALSE))
                {         
					return FALSE;
                }
			}	
		}
		else
		{
			offSet+=dataLen;
		}
	}
	else if(dataType == eRSD)
	{
		uint8 eleNum=0;
		nchioce = inData[offSet++];
		switch(nchioce)
		{
			case 0://
				break;
			case 1:
				if(FALSE == getNextReadData(inData,&offSet,inDataLen,eOAD,TRUE))
					return FALSE;
				if(FALSE == getNextReadData(inData,&offSet,inDataLen,NULL_DATA,FALSE))
					return FALSE;
				break;
			case 2:
				if(FALSE == getNextReadData(inData,&offSet,inDataLen,eOAD,TRUE))
					return FALSE;
				if(FALSE == getNextReadData(inData,&offSet,inDataLen,NULL_DATA,FALSE))
					return FALSE;
				if(FALSE == getNextReadData(inData,&offSet,inDataLen,NULL_DATA,FALSE))
					return FALSE;
				if(FALSE == getNextReadData(inData,&offSet,inDataLen,NULL_DATA,FALSE))
					return FALSE;
				break;	
			case 3:
				eleNum = inData[offSet++];
				for(i=0;i<eleNum;i++)
				{
					if(FALSE == getNextReadData(inData,&offSet,inDataLen,eOAD,TRUE))
						return FALSE;
					if(FALSE == getNextReadData(inData,&offSet,inDataLen,NULL_DATA,FALSE))
						return FALSE;
					if(FALSE == getNextReadData(inData,&offSet,inDataLen,NULL_DATA,FALSE))
						return FALSE;
					if(FALSE == getNextReadData(inData,&offSet,inDataLen,NULL_DATA,FALSE))
						return FALSE;
				}
				break;
			case 4:
			case 5:
				if(FALSE == getNextReadData(inData,&offSet,inDataLen,DATE_TIME_S,TRUE))
					return FALSE;
				if(FALSE == getNextReadData(inData,&offSet,inDataLen,eMS,TRUE))
					return FALSE;
				break;
			case 6:
			case 7:
			case 8:
				if(FALSE == getNextReadData(inData,&offSet,inDataLen,DATE_TIME_S,TRUE))
					return FALSE;
				if(FALSE == getNextReadData(inData,&offSet,inDataLen,DATE_TIME_S,TRUE))
					return FALSE;
				if(FALSE == getNextReadData(inData,&offSet,inDataLen,eTI,TRUE))
					return FALSE;
				if(FALSE == getNextReadData(inData,&offSet,inDataLen,eMS,TRUE))
					return FALSE;
				break;
			case 9:
				if(FALSE == getNextReadData(inData,&offSet,inDataLen,UNSIGNED,TRUE))
					return FALSE;
				break;
			case 10:
				if(FALSE == getNextReadData(inData,&offSet,inDataLen,UNSIGNED,TRUE))
					return FALSE;
				if(FALSE == getNextReadData(inData,&offSet,inDataLen,eMS,TRUE))
					return FALSE;
				break;
				
		}
	}
	else if(dataType == eCSD)
	{
		nchioce = inData[offSet++];
		if(nchioce == 0)
		{
			dataLen = getDataTypeLength(eOAD);
			offSet +=dataLen;
		}
		else if(nchioce == 1)
		{
			dataLen = getDataTypeLength(eOAD);
			offSet +=dataLen;
			dataNum = inData[offSet++];
			offSet +=dataLen*dataNum;
		}
		else 
			return FALSE;

	}
	else if(dataType == eROAD)
	{
		dataLen = getDataTypeLength(eOAD);
		offSet +=dataLen;
		dataNum = inData[offSet++];
		offSet +=dataLen*dataNum;
		printf("ROAD LEN = %d\n",dataLen+1+dataLen*dataNum);
	}
	else if(dataType == eRCSD)
	{
		dataNum = inData[offSet++];
		for(i=0;i<dataNum;i++)
		{
			if(FALSE == getNextReadData(inData,&offSet,inDataLen,eCSD,TRUE))
				return FALSE;
		}
	}
	else if(dataType == eMS)
	{
		nchioce = inData[offSet++];
		if((nchioce == 0)||(nchioce == 1))
		{
		
		}
		else if(nchioce == 2)//一组用户类型
		{
			dataNum = inData[offSet++];
			dataLen = getDataTypeLength(UNSIGNED);
			offSet +=dataLen*dataNum;
		}
		else if(nchioce == 3)
		{
			dataNum = inData[offSet++];
			while(dataNum--)
			{
				dataLen = inData[offSet++];
				if((dataLen>8)||(dataLen==0))
					return FALSE;
				offSet += dataLen;
			}
		}
		else if(nchioce == 4)
		{
			dataNum = inData[offSet++];
			dataLen = getDataTypeLength(LONG_UNSIGNED);
			offSet +=dataLen*dataNum;
		}
		else if(nchioce == 5)//一组用户类型区间
		{
			dataNum = inData[offSet++];
			
			while(dataNum--)
			{
			    if(inData[offSet++]>3)
				    return FALSE;
				if(inData[offSet++]!=UNSIGNED)
					return FALSE;
				dataLen = getDataTypeLength(UNSIGNED);
				offSet +=dataLen;
				if(inData[offSet++]!=UNSIGNED)
					return FALSE;
				dataLen = getDataTypeLength(UNSIGNED);
				offSet +=dataLen;
			}
		}
		else if(nchioce == 6)//一组用户地址区间
		{
			dataNum = inData[offSet++];			
			while(dataNum--)
			{
			    if(inData[offSet++]>3)
				    return FALSE;
				if(inData[offSet++]!=eTSA)
					return FALSE;
				dataLen = inData[offSet++];
				if((dataLen>8)||(dataLen==0))
					return FALSE;
				offSet +=dataLen;
				if(inData[offSet++]!=eTSA)
					return FALSE;
				dataLen = inData[offSet++];
				if((dataLen>8)||(dataLen==0))
					return FALSE;
				offSet +=dataLen;
			}
		}
		else if(nchioce == 7)//一组配置序号区间
		{
			dataNum = inData[offSet++];
			
			while(dataNum--)
			{
			    if(inData[offSet++]>3)
				    return FALSE;
				if(inData[offSet++]!=LONG_UNSIGNED)
					return FALSE;
				dataLen = getDataTypeLength(LONG_UNSIGNED);
				offSet +=dataLen;
				if(inData[offSet++]!=LONG_UNSIGNED)
					return FALSE;
				dataLen = getDataTypeLength(LONG_UNSIGNED);
				offSet +=dataLen;
			}
		}
		else
			return FALSE;

	}
	else if(dataType == eREGION)
	{
		if(inData[offSet++]>3)
			return FALSE;
		if(FALSE == getNextReadData(inData,&offSet,inDataLen,NULL_DATA,FALSE))
			return FALSE;
		if(FALSE == getNextReadData(inData,&offSet,inDataLen,NULL_DATA,FALSE))
			return FALSE;
	}
	else if(dataType == eSID)
	{
		if(FALSE == getNextReadData(inData,&offSet,inDataLen,DOUBLE_LONG_UNSIGNED,TRUE))
			return FALSE;
		if(FALSE == getNextReadData(inData,&offSet,inDataLen,OCTET_STRING,TRUE))
			return FALSE;	
	}
	else if(dataType == eSID_MAC)
	{
		if(FALSE == getNextReadData(inData,&offSet,inDataLen,eSID,TRUE))
			return FALSE;
		if(FALSE == getNextReadData(inData,&offSet,inDataLen,eMAC,TRUE))
			return FALSE;
	}
	else if(dataType == eTI)
	{
		if(inData[offSet++]>5)
			return FALSE;
		offSet += 2;
	}
	else
	{
		
		dataLen = getDataTypeLength(dataType);
		if(0xFF != dataLen)	
			offSet +=dataLen;
		else
			return FALSE;
	}
	if(offSet>inDataLen)
		return FALSE;
	*offset = offSet;
	return TRUE;
}

