/*
*    Filename: tea.c
* Description: TEA 암호화 모듈
*
*      Author: 김한주 (aka. 비엽, Cronan), 송영진 (aka. myevan, 빗자루)
*/
#include "StdAfx.h"
#include "tea.h"
#include <memory.h>

/*
* TEA Encryption Module Instruction
*					Edited by 김한주 aka. 비엽, Cronan
*
* void tea_code(const unsigned long sz, const unsigned long sy, const unsigned long *key, unsigned long *dest)
* void tea_decode(const unsigned long sz, const unsigned long sy, const unsigned long *key, unsigned long *dest)
*   8바이트를 암호/복호화 할때 사용된다. key 는 16 바이트여야 한다.
*   sz, sy 는 8바이트의 역순으로 대입한다. 
* 
* int tea_decrypt(unsigned long *dest, const unsigned long *src, const unsigned long *key, int size);
* int tea_encrypt(unsigned long *dest, const unsigned long *src, const unsigned long *key, int size);
*   한꺼번에 8 바이트 이상을 암호/복호화 할때 사용한다. 만약 size 가
*   8의 배수가 아니면 8의 배수로 크기를 "늘려서" 암호화 한다. 
*
* ex. tea_code(pdwSrc[1], pdwSrc[0], pdwKey, pdwDest);
*     tea_decrypt(pdwDest, pdwSrc, pdwKey, nSize);
*/

#define TEA_ROUND		32		// 32 를 권장하며, 높을 수록 결과가 난해해 진다.
#define DELTA			0x9E3779B9	// DELTA 값 바꾸지 말것.

void tea_code(const unsigned long sz, const unsigned long sy, const unsigned long *key, unsigned long *dest)
{
	register unsigned long y = sy, z = sz, sum = 0;
	unsigned long		n = TEA_ROUND;
	
	while (n-- > 0)
	{
		y	+= ((z << 4 ^ z >> 5) + z) ^ (sum + key[sum & 3]);
		sum	+= DELTA;
		z	+= ((y << 4 ^ y >> 5) + y) ^ (sum + key[sum >> 11 & 3]);
	}
	
	*(dest++)	= y;
	*dest	= z;
}

void tea_decode(const unsigned long sz, const unsigned long sy, const unsigned long *key, unsigned long *dest)
{
#pragma warning(disable:4307)
	register unsigned long y = sy, z = sz, sum = DELTA * TEA_ROUND;
#pragma warning(default:4307)

	unsigned long		n = TEA_ROUND;
	
	while (n-- > 0)
	{
		z -= ((y << 4 ^ y >> 5) + y) ^ (sum + key[sum >> 11 & 3]);
		sum -= DELTA;
		y -= ((z << 4 ^ z >> 5) + z) ^ (sum + key[sum & 3]);
	}
	
	*(dest++)	= y;
	*dest	= z;
}

int tea_encrypt(unsigned long *dest, const unsigned long *src, const unsigned long * key, int size)
{
	int		i;
	int		resize;
	
	if (size % 8 != 0)
	{
		resize = size + 8 - (size % 8);
		memset((char *) src + size, 0, resize - size);
	}
	else
		resize = size;
	
	for (i = 0; i < resize >> 3; i++, dest += 2, src += 2)
		tea_code(*(src + 1), *src, key, dest);
	
	return (resize);
}

int tea_decrypt(unsigned long *dest, const unsigned long *src, const unsigned long * key, int size)
{
	int		i;
	int		resize;
	
	if (size % 8 != 0)
		resize = size + 8 - (size % 8);
	else
		resize = size;
	
	for (i = 0; i < resize >> 3; i++, dest += 2, src += 2)
		tea_decode(*(src + 1), *src, key, dest);
	
	return (resize);
}

