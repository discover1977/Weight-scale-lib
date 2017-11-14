/*
 * ��������!!!
 * ������� ���������� ���������� SPI
 */
#ifndef _MAX72xx_
#define _MAX72xx_

#include <stdint.h>
#include "compilers_4.h"

/*********************** ��������� ***********************/
/* ���������� ��������� */
#define MAX72XX_NUMBERS		1

/* ��������� ��������� ��������(������ �����) � ������� ������ ����� */
// 0 - �������� ���������
// 1 - ��������� ��������
#define SYMBOL_INCLUDE		1

/* ��������� SPI �����*/
#define SPI_PORT			PORTB
#define SPI_DDR				DDRB
#define SPI_MOSI			3
#define SPI_MISO			4
#define SPI_SCK				5
#define SPI_CS				2

/***************** ����� ������� MAX72xx *****************/
#define NO_OP			0x00
#define DIGIT_0			0x01
#define DIGIT_1			0x02
#define DIGIT_2			0x03
#define DIGIT_3			0x04
#define DIGIT_4			0x05
#define DIGIT_5			0x06
#define DIGIT_6			0x07
#define DIGIT_7			0x08
#define DECODE_MODE		0x09
#define INTENSITY		0x0A
#define SCAN_LIMIT		0x0B
#define SHUTDOWN		0x0C
#define DISPLAY_TEST	0x0F
/*********************************************************/

/****************** Code B Font MAX72xx ******************/
#define SYMBOL_MINUS	0x0A
#define SYMBOL_E		0x0B
#define SYMBOL_H		0x0C
#define SYMBOL_L		0x0D
#define SYMBOL_P		0x0E
#define SYMBOL_BLANK	0x0F
/*********************************************************/

/* ���������������� ������� */

/***********************************************************************************
*   Function name :   MAX72xx_Init
*   Returns :         ���
*   Parameters :      intensity - �������������/�������/��� ��������,
*   Purpose :         ������������� ����������, ��������� ������� ����������.
***********************************************************************************/
void MAX72xx_Init(uint8_t intensity);
/***********************************************************************************
*   Function name :   MAX72xx_Clear
*   Returns :         ���
*   Parameters :      ic_addr - ����� ����������
*   Purpose :         ������� ����������
***********************************************************************************/
void MAX72xx_Clear(uint8_t ic_addr);
/***********************************************************************************
*   Function name :   MAX72xx_Send
*   Returns :         ���
*   Parameters :      ic_addr - ����� ����������,
*       			  reg - ������� MAX72xx,
*       			  data - ������,
*   Purpose :         �������� ������ �� MAX72xx
***********************************************************************************/
void MAX72xx_Send(int8_t ic_addr, uint8_t reg, uint8_t data );
/***********************************************************************************
*   Function name :   MAX72xx_OutInt
*   Returns :         ���
*   Parameters :      ic_addr - ����� ����������/����������,
*       			  value - �����,
*       			  comma_pos - ������� �����,
*   Purpose :         ����� ��������� ����� �� ��������� �� ������
*   				  ����� ��������� � ������ ������
***********************************************************************************/
void MAX72xx_OutInt(uint8_t ic_addr, int32_t value, uint8_t comma_pos);
/***********************************************************************************
*   Function name :   MAX72xx_OutIntFormat
*   Returns :         ���
*   Parameters :      value - �����,
*   				  start_pos - ��������� ������� ������ �����
*   				  stop_pos - �������� ������� ������ �����
*       			  comma_pos - ������� �����,
*   Purpose :         ��������� ����� ��������� ����� �� ���������.
*   				  ����� ���������� � ������� start_pos �� stop_pos.
*   				  ��� �������������� ��������� - ��������� ������� �������,
*   				  ��� ���� ��������������� ���������� �����������,
*					  ���������/���������� � ������� "0" ����� ��������� �������� 1-8,
*					  ���������/���������� � ������� "1" ����� ��������� �������� 9-16,
*					  � ��� �����.
***********************************************************************************/
void MAX72xx_OutIntFormat(int32_t value, uint8_t start_pos, uint8_t stop_pos, uint8_t comma_pos);
/***********************************************************************************
*   Function name :   MAX72xx_SetComma
*   Returns :         ���
*   Parameters :      pos - �������,
*   				  val - ��������: 1 - ������� �����, 0 - �������,
*   Purpose :         ���������/�������� ����� � �������
***********************************************************************************/
void MAX72xx_SetComma(uint8_t pos, uint8_t val);
/***********************************************************************************
*   Function name :   MAX72xx_SetCommaMask
*   Returns :         ���
*   Parameters :      ic_addr - ����� ����������/����������,
*   				  comma_mask - �����,
*   Purpose :         ����� �����/����� �� �����
***********************************************************************************/
void MAX72xx_SetCommaMask(uint8_t ic_addr, uint8_t comma_mask);
void MAX72xx_SetIntensity(uint8_t ic_addr, uint8_t val);
#if SYMBOL_INCLUDE
/***********************************************************************************
*   Function name :   MAX72xx_OutSym
*   Returns :         ���
*   Parameters :      *str - ��������� �� ������,
*   				  pos - �������,
*   Purpose :         ����� �������/������ ������ ������� � ��������� �������.
***********************************************************************************/
void MAX72xx_OutSym(char *str, int8_t pos);
#endif

/***********************************************************
//BITS MACROS
//PASHGAN 2009
//CHIPENABLE.RU
//
//reg : ��� ����������, ��������
//bit : ������� ����
//val : 0 ��� 1
************************************************************/
#define SUFIX	UL
#define Bit(bit)  (1<<(bit))

#define ClearBit(reg, bit)       reg &= (~(1UL<<(bit)))
//������: ClearBit(PORTB, 1); //�������� 1-� ��� PORTB

#define SetBit(reg, bit)          reg |= (1UL<<(bit))
//������: SetBit(PORTB, 3); //���������� 3-� ��� PORTB

#define SetBitVal(reg, bit, val) do{if ((val&1)==0) reg &= (~(1<<(bit)));\
                                  else reg |= (1<<(bit));}while(0)
//������: SetBitVal(PORTB, 3, 1); //���������� 3-� ��� PORTB
//	  SetBitVal(PORTB, 2, 0); //�������� 2-� ��� PORTB

#define BitIsClear(reg, bit)    ((reg & (1<<(bit))) == 0)
//������: if (BitIsClear(PORTB,1)) {...} //���� ��� ������

#define BitIsSet(reg, bit)       ((reg & (1<<(bit))) != 0)
//������: if(BitIsSet(PORTB,2)) {...} //���� ��� ����������

#define InvBit(reg, bit)	  reg ^= (1<<(bit))
//������: InvBit(PORTB, 1); //������������� 1-� ��� PORTB

#endif

