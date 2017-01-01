#include <avr\io.h> 
   
         
#include <avr\interrupt.h>


    

void Int_init(void);
void ADC_init(void);
void delay(int _ms);
void tap_stop(int rotation, int round);
void tap_open(int rotation, int round);

double ADC_value;
float  t,initial_weight,limit;
int i,j,volume_option,check,k,r,flag,stop,belt,stop_flag,height_flag;//flag for tap condition , stop_flag-manual or auto stop ,belt- mood selection, stop-stop command

int main (void)
{
    initial_weight=5;
    r=0;
	flag=0;
	stop_flag=0;
	DDRB = 0x00;
	DDRD = 0xFF;
	DDRC=0x0F;
	

	ADC_init();
	
  // To Enable global Interrupt
	sei();
	
	
	for (;;)
	 {
   
	    volume_option=PINA & 0x1F;
	    check=PINB & 0x07;
		stop=PINC & 0x80;
		belt=PINB & 0x80;// mood select 
		height_flag=PINB & 0x08; 

	
		if ((flag==1)&&(stop!=0)) //stop button
		{
		ADCSRA &=~(1<<ADEN);//adc disable
		stop_flag=1;
		tap_stop(k+1,r);
		delay(100);   
       ADCSRA |=(1<<ADEN);
	   	ADCSRA |=(1<<ADSC);} 

		if (belt !=0) // free running mood
		{
		if (((check!=1)&&(check!=7))||( volume_option==0)) // if bottle is displaced or swich is off
		 flag=0;
		}

   
	   if( ( volume_option!=0)&&((check==1)||(check==7))&&(flag!=1)&&(((volume_option<=2)||(height_flag!=0))))
   
			   {
               if(stop_flag==0) // only for non-manual stop
			   initial_weight=t;
			   
			   r=volume_option;
				if( volume_option==1)
				limit=.30;
				else if( volume_option==2)
				     limit=.55;
				else if( volume_option==4)
				     limit=1.2;
				else if( volume_option==8)
				     limit=1.6;
			 	else if( volume_option==16)
				     limit=2.3;

			  // initial_weight=t;
			   tap_open(50,r );
			   	
			   
			    }
   
	   }
}


ISR(ADC_vect)
{
	
	unsigned int digit,Test;
	Test=0;
	
	ADC_value = ADCL;
	ADC_value += (ADCH<<8);
	ADC_value = ADC_value * 50;
	ADC_value = ADC_value/1024;
	t=ADC_value/10;       //this is input voltage
	/* following code for seven segment display*/
	digit = ADC_value / 10;
	Test = PORTD & 0x0F;
	Test |= (digit<<4);
	PORTD = Test;    //  Digit 3 (Higher digit)
	digit = ADC_value / 10;
	ADC_value = ADC_value - digit * 10;
	digit = ADC_value;
	Test = PORTD & 0xF0;
	Test |= digit;
	PORTD = Test;	

	check=PINB & 0x07;
		if (((t>=initial_weight+limit)||((belt==0)&&(check!=1)&&(check!=7)))&&(flag==1))//target reach or accidental bottle displacement in manual mood
		 {
		 initial_weight=5;
		 tap_stop(50,r);
		 PORTC=0;	    
		 }


	

	ADCSRA |=(1<<ADSC); // ADC Start Conversion.
	
}



void ADC_init(void)
{
	 ADMUX |=0x47;; // AVcc as Vref. // Channel Selected : ADC4
	ADCSRA |=(1<<ADEN); // ADC Enable.
	ADCSRA |=(1<<ADSC); // ADC Start Conversion.
	ADCSRA |=(1<<ADATE); // ADC Free Running.
	SFIOR=0x00;
	// Note ADFR is ADATE for other ATmega ICs.
	ADCSRA |=(1<<ADIE); // ADC Interrupt Enable.
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // ADC Prescaler Select.

}
void delay(int _ms)// delay

{
for(i=0;i<_ms*350;i++)
{
}

}

void tap_stop(int rotation, int round)
{
int p, n;
ADCSRA &=~(1<<ADEN);//adc disable
if (belt ==0) // for non-free running mood
flag=0;


p=16;

if (round==1)
    
   n=1;
   else if ((round==2)||(round==4))
    
   n=2;
    else if ((round==8)||(round==16))
    
   n=3;

	for(k=0;k<rotation;k++)
	{
	for(j=0;j<n;j++)
					   {
		
						delay(p);	
					    PORTC = 4;
	
						delay(p);
					    PORTC = 2;
	
						delay(p);
						PORTC = 1;
	
						delay(p);
						PORTC = 8;

					    }
		
		volume_option=PINA & 0x1F;  // for reopening after only for manual stopping
	    check=PINB & 0x07;
		if( ( volume_option!=0)&&((check==1)||(check==7))&&(flag!=1)&&(stop_flag==1)&&(((volume_option<=2)||(height_flag!=0))))
   
			   {
   
			   
			   	r=volume_option;
				if( volume_option==1)
				limit=.30;
				else if( volume_option==2)
				     limit=.55;
				else if( volume_option==4)
				     limit=1.2;
				else if( volume_option==8)
				     limit=1.6;
			 	else if( volume_option==16)
				     limit=2.3;

			   //initial_weight=t;
			   tap_open(k+1,r );
			   	
			   
			    break;
				}
   


		}
	delay(100);
	PORTC=0;   
ADCSRA |=(1<<ADEN);
ADCSRA |=(1<<ADSC); // ADC Start Conversion.
}


void tap_open(int rotation, int round)
{
int p,n;
ADCSRA &=~(1<<ADEN);
flag=1;
stop_flag=0;

p=16;

if (round==1)
    
   n=1;
   else if ((round==2)||(round==4))
    
   n=2;
    else if ((round==8)||(round==16))
    
   n=3;


for (k=0;k<rotation;k++)
         {
           for(j=0;j<n;j++)
	           {
		           delay(p);
				   PORTC =1;
	               delay(p);
				   PORTC = 2;
			       delay(p);
				   PORTC = 4;
				   delay(p);
				   PORTC = 8;

				}
				check=PINB & 0x07;
				stop=PINC & 0x80;
				if( ((check!=1)&&(check!=7))||((flag==1)&&(stop!=0)))
					{
					stop_flag=1;
					 tap_stop( k+1, round);
					 break;
					 }
		}
delay(p);
PORTC=0;
			   
delay(100);   
ADCSRA |=(1<<ADEN);
ADCSRA |=(1<<ADSC); // ADC Start Conversion.
	

}


