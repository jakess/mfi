/*
 * GccApplication5.c
 *
 * Created: 11/17/2013 11:46:39 AM
 *  Author: Jake Simon
 *
 * 
 */ 
/*INLCUDES*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>

//include for USI interface. requires USI_TWI_Master.c be in the same folder
#include "USI_TWI_Master.h"
//include for magnetometer control and reading data from 
#include "HMC_Control.h"
//include for accel/gyro control and reading data from
#include "MPU_Control.h"

#define timer_13ms 101
volatile char prescaler; //for use in timer
//goes XH, XL, YH, YL, ZH, ZL
unsigned char data_HMC[7] ={0, 0, 0, 0, 0, 0, 0};
//goes XH_acc, XL_acc, YH_acc, YL_acc, ZH_acc, ZL_acc
unsigned char data_MPU_a[7]={0, 0, 0, 0, 0, 0, 0};
//XH_gyro, XL_gyro, YH_gyro, YL_gyro, ZH_gyro, ZL_gyro
unsigned char data_MPU_g[7]={0, 0, 0, 0, 0, 0, 0};

void filter_MPU(double* previous_MPU[], double* result_MPU[]);
void position_MPU(double* position[],double* result_MPU[], double* previous_MPU[]);

#define MPU_sample_rate 0.013 //75 Hz, in seconds

int main()
{
	double position[]={1,2,3};
	//goes X_acc,Y_acc,Z_acc,X_gyro,Y_gyro,Z_gyro
	double result_MPU[3],previous_MPU[]={0,0,0};
	unsigned char send_data_USART [6];
	
	/**********SETUP*********/
	USI_TWI_Master_Initialise();
	HMC_Enable();
	MPU_Enable();
	
	//setting up timer for HMC
	DDRB = 0xFF;					   //set PORTB as output
	prescaler = 5;					  //divide by 1024
	TCNT0 = 0;
	TCCR0A = (1<<WGM01) | prescaler;  //set CTC bit and prescaler
	TIMSK0 = (1<<OCIE0A);			  //enable interrupt for timer0
	OCR0A = timer_13ms;				  //set timer to wait for 13ms. for how long it takes mag to update
	sei();
	
	/**********CODE********/
	while(1)
	{
		//take HMC data out of 2's comp form
		//while this process is ongoing no new HMC data is placed in data_HMC
		TIMSK0 = (0<<OCIE0A);
		for(int i=0; i<8;i++)
			{
				data_HMC[i] =~ data_HMC[i];
				data_HMC[i] = data_HMC[i]+1;
			}
		TIMSK0 = (1<<OCIE0A);
		
		//take MPU data and turn it into something usable
		filter_MPU(previous_MPU,result_MPU);
		//find the position
		position_MPU(position,result_MPU,previous_MPU);
		
		//go from 16 to 8 bit
		int x = result_MPU[0];
		send_data_USART[0]=x & 0x00FF;
		send_data_USART[1]=x>>8;
		x = result_MPU[1];
		send_data_USART[2]=x & 0x00FF;
		send_data_USART[3]=x>>8;
		x = result_MPU[2];
		send_data_USART[4]=x & 0x00FF;
		send_data_USART[5]=x>>8;
	}		
}

ISR(TIMER0_COMP_vect, ISR_BLOCK)
{
	HMC_Read(data_HMC);
	MPU_Read(data_MPU_a, data_MPU_g);	
}


void filter_MPU(double* previous_MPU[],double* result_MPU[])
{
	int t = MPU_sample_rate;
	//x,y,z
	int Rgyro[3], Agyro[3][2];
	//wGyro is a trust factor. Try values between 5 and 20
	int wGyro = 10;
	//x, y, z
	int data16_accel_MPU[3], data16_gyro_MPU[3];
	//make it so that no new accel/gryo data is received while doing this 
	TIMSK0 =(0<<OCIE0A);
	//take 2's comp
	for(int i=0; i <=7;i++)
	{
		data_MPU_a[i] =~ data_HMC[i];
		data_MPU_a[i] = data_HMC[i]+1;
		
		data_MPU_g[i] =~ data_HMC[i];
		data_MPU_g[i] = data_HMC[i]+1;
	}
	//convert from 8bit to 16bit
	data16_accel_MPU[0]=(data_MPU_a[1]<<8)|data_MPU_a[2];
	data16_accel_MPU[1]=(data_MPU_a[3]<<8)|data_MPU_a[4];
	data16_accel_MPU[2]=(data_MPU_a[5]<<8)|data_MPU_a[6];
	data16_gyro_MPU[0]=(data_MPU_g[1]<<8)|data_MPU_g[2];
	data16_gyro_MPU[1]=(data_MPU_a[3]<<8)|data_MPU_g[4];
	data16_gyro_MPU[2]=(data_MPU_a[5]<<8)|data_MPU_a[6];
	TIMSK0 =(1<<OCIE0A);

	//RAcc is data from accelerometer
	//REst is resultant vector 
	//RateA is data from gyro
	//T is sampling rate
	//if(RzEst(n-1)<0.00001){Rgyro = REst(n-1)}
	if (*previous_MPU[2]<0.001) {for(int i=0;i<3;i++){Rgyro[0] = *previous_MPU[0];}}
	else
	{
		//Axz(n-1)=atan2(RxEst(n-1), RzEst(n-1))
		Agyro[0][0]=atan2(*previous_MPU[0],*previous_MPU[2]);
		Agyro[1][0]=atan2(*previous_MPU[1],*previous_MPU[2]);
		Agyro[2][0]=atan2(*previous_MPU[0],*previous_MPU[1]);
		//Axz(n) = Axz(n-1)+RateAxzAvg*T
		Agyro[0][1]= Agyro[0][0]+data16_gyro_MPU[0]*t;
		Agyro[1][1]= Agyro[1][0]+data16_gyro_MPU[1]*t;
		Agyro[2][1]= Agyro[2][0]+data16_gyro_MPU[2]*t;
		//RxGyro = sin(Axz)/sqrt(1+cos(Axz)^2*tan(Ayz)^2);
		Rgyro[0]=sin(Agyro[0][1])/sqrt(1+((cos(Agyro[0][1])*cos(Agyro[0][1]))*(tan(Agyro[1][1])*tan(Agyro[1][1]))));
		Rgyro[1]=sin(Agyro[1][1])/sqrt(1+((cos(Agyro[1][1])*cos(Agyro[1][1]))*(tan(Agyro[0][1])*tan(Agyro[0][1]))));
		Rgyro[2]=sqrt(1-(Rgyro[0]*Rgyro[0])-(Rgyro[1]*Rgyro[1]));
	}
	//REst(n) = (RAcc +RGyro*wGyro)/(1+wGyro)
	*result_MPU[0]=(data16_accel_MPU[0]+(Rgyro[0]*wGyro))/(1+wGyro);
	*result_MPU[1]=(data16_accel_MPU[1]+(Rgyro[1]*wGyro))/(1+wGyro);
	*result_MPU[2]=(data16_accel_MPU[2]+(Rgyro[2]*wGyro))/(1+wGyro);
}

//finding current position using:
//x=v_0*t + 1/2*a[n]*t^2
//v_0=a[n-1]*t
//t is sampling rate
void position_MPU(double* position[], double* result_MPU[], double* previous_MPU[])
{
	int t = MPU_sample_rate;
	for (int i=0;i<3;i++)
	{
		*position[i]= *previous_MPU[i]*t + *result_MPU[i]*t*t*0.5;
		previous_MPU[i]=result_MPU[i];
	}	
}