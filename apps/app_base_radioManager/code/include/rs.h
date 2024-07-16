#ifndef _RS_H
#define _RS_H

#define MSK_RS_EN     1

#if MSK_RS_EN == 1

void rs_encode_9(unsigned char *src,unsigned char *dest);	/* length is always 9->15 */
void rs_decode_9(unsigned char *src,unsigned char *dest);	/* length is always 15->9 */
unsigned int rs_code_unpack_9(unsigned char *pbuff, unsigned int len);
unsigned int rs_code_pack_9(unsigned char *pbuff, unsigned int len);

void rs_encode_11(unsigned char *src,unsigned char *dest);	/* length is always 9->15 */
void rs_decode_11(unsigned char *src,unsigned char *dest);	/* length is always 15->9 */
unsigned int rs_code_unpack_11(unsigned char *pbuff, unsigned int len);
unsigned int rs_code_pack_11(unsigned char *pbuff, unsigned int len);

//RS编码标识	0 标识无RS编码  1 表示RS11/15  2 表示RS9/15
void rs_encode(unsigned char *src,unsigned char *dest, uint8 rs_code);	/* length is always 9->15 */
void rs_decode(unsigned char *src,unsigned char *dest, uint8 rs_code);	/* length is always 15->9 */
unsigned int rs_code_unpack(unsigned char *pbuff, unsigned int len, uint8 rs_code);
unsigned int rs_code_pack(unsigned char *pbuff, unsigned int len, uint8 rs_code);

#else

#define rs_9_2_4	0	//-9+2+4->15-
#define rs_11_0_4	1	//-11+0+4->15-

void rs_encode(unsigned char *src,unsigned char *dest);	/* length is always 9->15 */
void rs_decode(unsigned char *src,unsigned char *dest);	/* length is always 15->9 */
unsigned int rs_code_unpack(unsigned char *pbuff, unsigned int len);
unsigned int rs_code_pack(unsigned char *pbuff, unsigned int len);

#endif

#endif /* _RS_H */
