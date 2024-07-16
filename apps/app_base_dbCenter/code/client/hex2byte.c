#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "baseType.h"

#include <ctype.h>

void Hex2Str(const sint8 *sSrc, sint8 *sDest, sint32 nSrcLen)
{
    sint32 i;
    sint8 szTmp[3];

    for (i = 0; i < nSrcLen; i++)
    {
        sprintf(szTmp, "%02X", (uint8)sSrc[i]);
        memcpy(&sDest[i * 2], szTmp, 2);
    }
    return;
}

void HexStrToByte(const sint8 *source, uint8 *dest, sint32 sourceLen)
{
    sint16 i;
    uint8 highByte, lowByte;

    for (i = 0; i < sourceLen; i += 2)
    {
        highByte = (uint8)toupper(source[i]);
        lowByte = (uint8)toupper(source[i + 1]);

        if (highByte > 0x39)
            highByte -= 0x37;
        else
            highByte -= 0x30;

        if (lowByte > 0x39)
            lowByte -= 0x37;
        else
            lowByte -= 0x30;

        dest[i / 2] = (highByte << 4) | lowByte;
    }
    return;
}


sint32 str2intbuf(sint8 *s, sint8 *d, uint32 *data, sint32 dataMax)
{

    sint8 *token;
    sint32 i = 0;
    for (token = strtok(s, d); token != NULL; token = strtok(NULL, d))
    {
        // printf(token);
        // printf("+");
        //data[i] = atoi(token);
        data[i] = strtoul(token, NULL,0);
        i++;
    }
    // printf("\n");}
    return i;
}

sint32 hexStr2U32buf(sint8 *s, sint8 *d, uint32 *data, sint32 dataMax)
{
    sint8 *token;
    sint32 i = 0;
    for (token = strtok(s, d); token != NULL; token = strtok(NULL, d))
    {
        // printf(token);
        // printf("+");
        // data[i] = atoi(token);
        sscanf(token,"%x",&data[i]);
        i++;
    }
    // printf("\n");}
    return i;
}

sint32 str2shortbuf(sint8 *s, sint8 *d, uint16 *data, sint32 dataMax)
{

    sint8 *token;
    sint32 i = 0;
    for (token = strtok(s, d); token != NULL; token = strtok(NULL, d))
    {
        // printf(token);
        // printf("+");
        data[i] = atoi(token);
        i++;
    }
    // printf("\n");}
    return i;
}