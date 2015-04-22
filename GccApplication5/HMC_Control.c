/*
 * HMC_Control.c
 *
 * Created: 11/22/2013 11:46:39 AM
 *  Author: Jake Simon
 *	
 *	This header is for use with HMC5883L 3-axis magnetometer and the HMC_Control.h file
 */

/*INLCUDES*/
#include <avr/io.h>  //do I need this?
#include "HMC_Control.h"

void HMC_Selftest (void)
{
	
}

void HMC_Enable (void)
{
	unsigned char messageBuf[MESSAGEBUF_SIZE];
	unsigned char TWI_targetSlaveAddress = 0x1E;
	unsigned char temp;
	
	
	//**************Set up HMC5883***********//
	//config register A. setting 8 samples averaged per output, 75Hz output rate, normal measurement config 
	messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The zero byte must always consist of General Call code or the TWI slave address. if False have write
	messageBuf[1] = 0x00;            
	messageBuf[2] = 0b01111000;    
	temp = USI_TWI_Start_Transceiver_With_Data( messageBuf, 3);
	
	//config register B. setting sensor field range to +- 8.1 Ga
	messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The zero byte must always consist of General Call code or the TWI slave address. if False have write
	messageBuf[1] = 0x01;             
	messageBuf[2] = 0b11100000;
	temp = USI_TWI_Start_Transceiver_With_Data( messageBuf, 3);
	
	//mode register. setting continuous-measurement mode
	messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The zero byte must always consist of General Call code or the TWI slave address. if False have write
	messageBuf[1] = 0x02;            
	messageBuf[2] = 0x00;    
	temp = USI_TWI_Start_Transceiver_With_Data( messageBuf, 3);
}

void HMC_Read (unsigned char *data) 
{
	unsigned char TWI_targetSlaveAddress = 0x1E;
	unsigned char temp;
	
	//Tell the HMC5883 where to begin reading data. MSB of x-axis register
	data[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The zero byte must always consist of General Call code or the TWI slave address.
	data[1] = 0x03;			
	temp = USI_TWI_Start_Transceiver_With_Data( data, 2);
	
	//read data from all registers (there are six)
	data[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT);
	temp = USI_TWI_Start_Transceiver_With_Data( data, 7);
}
