#define F_CPU 8000000UL    // clock
#include <avr/io.h>
#include <avr/interrupt.h>	// interrupts
#include <util/delay.h>		//delays


#define  Trigger_pin_1	PB7	/* Trigger pins */

#define  Echo_pin_1	PB6

volatile int TimerOverflow = 0;

ISR(TIMER0_OVF_vect)
{
	TimerOverflow++;

}

int main(void){

	double distance_1;
	double distance_2;
	DDRB =0xA8;
	DDRC =0xff;

	/*Calculate width of Echo by Getting TCNT1 Timer values */

	while (1){	
	
		DDRA =1<<5;
		PORTA = 1<<5;
		_delay_us(1000);
		PORTA =0<<5;
		_delay_us(1000);
		
		distance_1 = distance_cal(Trigger_pin_1,Echo_pin_1);
		
		if (distance_1<=10){
			PORTC |= 1<<PC3;
			_delay_us(1000);
			PORTC &= ~(1<<PC3);
			_delay_us(1000);
		}else{
		
			PORTC &= ~(1<<PC3);
		}
		
	
	}
	
}

int distance_cal(int trig_pin, int echo_pin){

		int count_1=0;
		int overflow =0;
		double distance_value =0;
			
		//setting up TIMER0 For time measurements
		TCNT0 = 0;
		TCCR0 |= (1<<CS01); // PRESCALER 8
		TIMSK = (1<<TOIE0);// enable TIMER) overflow Interrupt
		sei();// enable global Interrupts
		_delay_ms(50);

		/* Make a 15us trigger pulse on trig pin_1 to HC-SR05 */
		PORTB =0x00;
		_delay_us(10);
		PORTB |= 1<<trig_pin;
		_delay_us(10);
		PORTB &= ~(1<<trig_pin);

		//6cm-->> 176us
		//600cm-->>17647us

		/* Wait for rising edge */
		while(1){
			if(!(PINB & (1<<echo_pin))){continue;}
			else{
				TCNT0 =0;
				TimerOverflow=0;
				break;}
			}
		
		/* Wait for falling edge */
		while (1){
			if(PINB & (1<<echo_pin)){continue;}
			else{
				count_1 = TCNT0 ;
				overflow = TimerOverflow;
				break;}
			}

		cli();//disable global Interrupts

		count_1 = count_1+ 256*(overflow);

		distance_value = count_1/58;// distance calculation  : distance in Centimeters

		return distance_value;
}
