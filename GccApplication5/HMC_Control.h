/*
 * HMC_Control.h
 *
 * Created: 11/22/2013 11:46:39 AM
 *  Author: Jake Simon
 *	
 *	This header is for use with HMC5883L 3-axis magnetometer and the HMC_Control.c file
 */

//********** Defines **********//
#define MAG_ADDR 0x1E 
#define MESSAGEBUF_SIZE 7 
#define TWI_READ_BIT  0       // Bit position for R/W bit in "address byte".
#define TWI_ADR_BITS  1       // Bit position for LSB of the slave address bits in the init byte.
#define TWI_NACK_BIT  0       // Bit position for (N)ACK bit.

// General defines
#define TRUE  1
#define FALSE 0


//********** Prototypes **********//
void HMC_Enable (void);
void HMC_Read (unsigned char *); 
void HMC_Selftest (void);