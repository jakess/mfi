/*
 * MPU_Control.h
 *
 * Created: 11/22/2013 11:46:39 AM
 *  Author: Jake Simon
 *	
 *	This header is for use with MPU6050 3-axis accel and 3-axis gryo and the MPU_Control.c file
 */

//********** Defines **********//
#define MPU_ADDR 0xD0 
#define MESSAGEBUF_SIZE 7	  
#define TWI_READ_BIT  0       // Bit position for R/W bit in "address byte".
#define TWI_ADR_BITS  1       // Bit position for LSB of the slave address bits in the init byte.
#define TWI_NACK_BIT  0       // Bit position for (N)ACK bit.

// General defines
#define TRUE  1
#define FALSE 0


//********** Prototypes **********//
void MPU_Enable (void);
void MPU_Read (unsigned char *, unsigned char *); 
void MPU_Selftest (void);