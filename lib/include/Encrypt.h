/*
 * Encrypt.h
 *
 *  Created on: Jul 3, 2020
 *      Author: Administrator
 */

#ifndef ENCRYPT_H_
#define ENCRYPT_H_

#define SEED_START  610

void String_Encrypt(unsigned char* pBuffer, unsigned short BufferLength);
void String_Decrypt(unsigned char* pBuffer, unsigned short BufferLength);


#endif /* ENCRYPT_H_ */
