#include "baseCommon.h"
#include "sm3sum.h"

static void lk_sm3_cf(lk_sm3_context_t *context)
{
	LK_MSG_CF(context)
}

void lk_sm3_update(lk_sm3_context_t *context, unsigned char *data, unsigned int len)
{
	int real_len, free, offset = 0;

	real_len = len + context->len;
	if (real_len < LK_GVALUE_LEN) {
		//本次数据不够一个分组大小,先缓存起来
		memcpy(context->data + context->len, data + offset, len);
		context->len = real_len;
		context->total += len;
		return;
	}
	free = LK_GVALUE_LEN - context->len;
	memcpy(context->data + context->len, data + offset, free);
	context->total += free;
	offset += free;
	len -= free;
	//进行迭代压缩
	lk_sm3_cf(context);

	while (1) {
		if (len < LK_GVALUE_LEN) {
			//本次数据不够一个分组大小,先缓存起来
			memcpy(context->data + context->len, data + offset, len);
			context->len = len;
			context->total += len;
			return;
		}
		memcpy(context->data + context->len, data + offset, LK_GVALUE_LEN);
		offset += LK_GVALUE_LEN;
		len -= LK_GVALUE_LEN;
		context->total += LK_GVALUE_LEN;
		//进行迭代压缩
		lk_sm3_cf(context);
	}
}

void lk_sm3_final(lk_sm3_context_t *context)
{
	unsigned int tk, k, i;
	//unsigned int free;
	unsigned long long len;
	//unsigned char tmp[LK_GVALUE_LEN] = { 0 };

	tk = context->total * 8 % 512;
	if (tk < 448) {
		k = 448 - tk;
	}
	else {
		k = 448 - tk + 512;
	}
	//计算需要填充的字节
	k = k / 8 + 8;
	//free = LK_GVALUE_LEN - context->len;
	k--;
	context->data[context->len] = 0x80;
	len = context->total * 8;
	for (i = context->len + 1; i < LK_GVALUE_LEN; i++, k--) {
		if (k != 8)
			context->data[i] = 0x00;
		else {
			bzero(context->data + i, 8);
			unsigned long long *pdata = (unsigned long long *)&(context->data[i]);
			*pdata = LK_GE_ONE(len);
			break;
		}
	}
	//进行迭代压缩
	lk_sm3_cf(context);
	if (64 == k) {
		for (i = 0; i < LK_GVALUE_LEN; i++, k--) {
			if (k != 8)
				context->data[i] = 0x00;
			else {
				bzero(context->data + i, 8);
				unsigned long long *pdata = (unsigned long long *)&(context->data[i]);
				*pdata = LK_GE_ONE(len);
				break;
			}
		}
		//进行迭代压缩
		lk_sm3_cf(context);
	}
	//get result
	LK_LE_ONE(context)
}

int calc_sm3(unsigned char *pBuf, int nSize, unsigned char *pOut)
{
	int nBlock = 1024; 
	int nMax = nSize/nBlock;
	int nLeft = nSize%nBlock;
	int nCpyLen = 0;
	
	unsigned char buffer[1024] = { 0 };

	lk_sm3_context_t context;
	//计算之前初始化
	LK_INIT_VALUE(&context);

	if( nLeft > 0 )
	{
		nMax++;
	}

	for( int i=0; i<nMax; ++i )
	{
		memset(buffer, 0x0, sizeof(buffer));

		if( i+1 >= nMax )
		{
			nCpyLen = nLeft;
		}
		else
		{
			nCpyLen = nBlock;
		}
		
		memcpy(buffer, &pBuf[i*nBlock], nCpyLen);
		
		//进行分组迭代压缩计算
		lk_sm3_update(&context, buffer, nCpyLen);
	}

	//进行最后的分组填充和迭代压缩计算
	lk_sm3_final(&context);

	memcpy(pOut, context.output, 32);

	return 0;
}

int cale_file_sm3(const char* filePath, unsigned char *pOut, int nSize)
{
	int rz = 0;

	FILE *fp;
	unsigned char buffer[1024] = { 0 };

	lk_sm3_context_t context;
	//计算之前初始化
	LK_INIT_VALUE(&context);

	if( filePath == NULL ) return -1;
	
	fp = fopen(filePath, "r+");
	if (!fp)
		return -1;
	while (!feof(fp)) {
		memset(buffer, 0x0, sizeof(buffer));
		rz = fread(buffer, 1, sizeof(buffer), fp);
		//进行分组迭代压缩计算
		lk_sm3_update(&context, buffer, rz);
	}
	//进行最后的分组填充和迭代压缩计算
	lk_sm3_final(&context);
	if (fp)
		fclose(fp);

	memcpy(pOut, context.output, 32);

	return 32;
}

/**********************************************************************
* @name      : GetFileSM3
* @brief     ：输入文件路径，获取SM3加密编码
* @param[in] ：const char   *filePath        待加密的文件
               unsigned int  sm3hexSize     *sm3hex的数组大小
* @param[out]：unsigned char *sm3hex         filepath计算得到的sm3编码
**********************************************************************/
int GetFileSM3(const char* filePath, unsigned int sm3hexSize, unsigned char *sm3hex)
{
	DIR *dir = NULL;
	int ret = 0;
	int i = 0;
	
	if(NULL == filePath || NULL == sm3hex || sm3hexSize < 32)
	{
		return -1;
	}

	if(0 != access(filePath, F_OK))
	{
		printf("sm3sum: %s: not exist\n", filePath);
		return -2;
	}

	dir = opendir(filePath);
	if(dir)
	{
		printf("sm3sum: %s: is a directory\n", filePath);
		closedir(dir);
		return -1;
	}

	ret = cale_file_sm3(filePath, sm3hex, 32);
	if(ret > 0)
	{
		for(i = 0; i < 32; i++)
		{
			printf("%02x", sm3hex[i]);
		}
		printf("  %s\n", filePath);
	}
	return 0;
}

//对文件中的某段数据区域计算sm3值,endOff为0xffffffff,表示文件末尾
int Sm3Region(char* pFileName, unsigned int startOff,unsigned int endOff,unsigned char *pOutSm3)
{
    int rz = 0;

    unsigned int len_sm3;
    unsigned char buffer[1024] = { 0 };

    lk_sm3_context_t context;
	//计算之前初始化
	LK_INIT_VALUE(&context);
    
    FILE *fp;
    if (!(fp=fopen(pFileName,"r+")))
    {
        printf("Can not open this file!\n");
        return 0;
    }

    fseek(fp, 0, SEEK_END);         //文件指针转到文件末尾
    if((len_sm3=ftell(fp))==-1)
    {
        printf("Sorry! Can not calculate files which larger than 2 GB!\n");
        fclose(fp);
        return 0;
    }

    //endOff为0xffffffff,表示文件末尾
    if(endOff == 0xffffffff)
    {
        endOff = len_sm3;
    }

    //判断所选数据段的合法性
    printf("文件大小: %d B sm3计算区域为: %d - %d B\n", len_sm3, startOff, endOff);
    if(startOff > len_sm3 || endOff > len_sm3 || startOff > endOff)
    {
        printf("Region selected is illegal !\n");
        return 0;
    }

    rewind(fp);                         //文件指针复位到文件头
    len_sm3 = endOff - startOff;        //所选区域长度
    fseek(fp, startOff, SEEK_SET);      //文件指针指向所选区域开始处
    while(len_sm3)
    {
        memset(buffer, 0x0, sizeof(buffer));
        rz = fread(buffer, 1, MIN(sizeof(buffer), len_sm3), fp);
        //进行分组迭代压缩计算
        lk_sm3_update(&context, buffer, rz);
        len_sm3 -= rz;
    }
    
    //进行最后的分组填充和迭代压缩计算
    lk_sm3_final(&context);
    if (fp)
        fclose(fp);

    memcpy(pOutSm3, context.output, 32);
    return 1;
}
