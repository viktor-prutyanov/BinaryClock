#define F_CPU 1000000UL 
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>

// Добавить обнуление при нажатии не только часов и минут, но и i и TCNT2.

uint8_t i = 0;
uint8_t m = 0;
uint8_t h = 0;
uint8_t t = 0;

ISR(TIMER2_COMPA_vect)
{
	TCNT2 = 0;  //v2
	cli();		//v2
	//_delay_ms(1000);
	i++;
	if(i == 10)
	{
		i = 0;
		m++;    
		if(m == 60)
		{
			m = 0;
			h++;
			if(h == 24)
			{
				h = 0;
			}
		}
		PORTC = m;
		PORTD = h + 0b11100000;
	}

	t = PIND;
	if ((t & 0xC0) == 0x00)
	{
		PORTC = 0b00111111;
		PORTD = 0xFF;
		i = 0; //v2
		m = 0;
		h = 0;
		_delay_ms(1000);
		while(1)
		{
			t = PIND;

			if ((t & 0xC0) == 0b10000000)
			{
				m++;
				if (m == 60)
				{
					m = 0;
				}
			}

			if ((t & 0xC0) == 0b01000000)
			{
				h++;
				if (h == 24)
				{
					h = 0;
				}
			}

			PORTC = m;
			PORTD = h + 0b11100000;

			if ((t & 0xC0) == 0x00)
			{	
				_delay_ms(1000);
				TCNT2 = 0; //v2
				break;
			}
			_delay_ms(150);
		}
	}

	sei();
	//sleep_cpu();
}

// ISR(INT0_vect)
// {
// 	PORTB ^= 0x02;
// }

int main(void)
{
	TIMSK2 = 0x00;
	ASSR   = 1<<AS2;
	TCNT2  = 0x00;
	OCR2A  = 0b11000000;
	TCCR2A = 0x00;
	TCCR2B = 0b00000111;
	TIMSK2 = 0b00000010;
	
	DDRC   = 0b00111111;
	PORTC  = 0x00;

	DDRB   = 0x00;
	PORTB  = 0xFF;

	DDRD   = 0b00011111;
	PORTD  = 0b11100000;

	power_adc_disable();
	power_usart0_disable();
	power_timer0_disable();
	power_timer1_disable();
	power_twi_disable();

	// MCUCR |= 0b00000011;
	// GICR |= 0b01000000;

	//set_sleep_mode(SLEEP_MODE_PWR_SAVE); // Set sleep mode to Power-save
    //sleep_enable(); 

	sei();

	for (int j = 1; j <= 32; j *= 2)
	{
		PORTC = j;
		_delay_ms(200);
	}
	PORTC = 0x00;
	for (int j = 1; j <= 16; j *= 2)
	{
		PORTD = j + 0b11100000;
		_delay_ms(200);
	}
	PORTD = 0b11100000;

	while(1)
	{
		//sleep_cpu();
	}
}
