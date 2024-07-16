/*
*********************************************************************
×Ö¿â²Ù×÷
*********************************************************************
*/
#ifndef _FONT_H_
#define _FONT_H_


int font_gb2312_get(unsigned char *mchar, unsigned char *out, int *outlen);
int font_string_map(int line_space, char *mstr, int len, unsigned char *pMap, int max_x, int max_y);
void font_input_string(char *mstr, int len, int *outlen);

#endif