/*
 Name:		Arduino_Stepper_7Segment_USART.c
 Created:	09.06.2020 16:32:33
 Author:	TsgabiXD
*/

#include <avr/io.h>
#include "Wait.h"
#include "USART.h"

//static declaration of display segments
__attribute__((unused)) static uint8_t segm[16] =
/* 0,    1,    2,    3,    4,     5,    6,    7,    8,    9,    A,    b,    C     d     E     F */
{ 0x3F, 0x06, 0x5B, 0x4F, 0x66,  0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71 };

void motor(char direction, int multiplier, int clockspeed, char clock)
{
	int a = 0, b = 0;
	PORTF = segm[b];

	if (direction == 'b')		//backward
	{
		for (; a <= multiplier; a++)
		{
			PORTB = (1 << 4);
			WaitMs(clockspeed);
			PORTB = (1 << 5);
			WaitMs(clockspeed);
			PORTB = (1 << 6);
			WaitMs(clockspeed);
			PORTB = (1 << 7);
			WaitMs(clockspeed);

			if (clock == 'y')
			{
				int rest = a % 512;

				if (rest == 0 && a > 0)
				{
					b++;

					if (b > 9)
					{
						b = 0;
					}

					PORTF = segm[b];
				}
			}
		}
	}
	else if (direction == 'f')	//forward
	{
		for (; a <= multiplier; a++)
		{
			PORTB = (1 << 7);
			WaitMs(clockspeed);
			PORTB = (1 << 6);
			WaitMs(clockspeed);
			PORTB = (1 << 5);
			WaitMs(clockspeed);
			PORTB = (1 << 4);
			WaitMs(clockspeed);

			if (clock == 'y')
			{
				int rest = a % 512;

				if (rest == 0 && a > 0)
				{
					b++;

					if (b > 9)
					{
						b = 0;
					}

					PORTF = segm[b];
				}
			}
		}
	}

	PORTB = 0;						//for no current consumption during stop
}

int16_t main(void)
{

	DDRB = 0xFF;		//motor
	PORTB = 0;
	DDRF = 0xFF;		//7-Segment
	PORTF = segm[0];
	DDRA = 0;			//button up/down
	PORTA = 255;

	int i = 0, ntimes, speed, steps;
	char direction = '0', directionfb, ntimestp, cyclecounteryn;

	USARTInit(0, 115200, 1, 0, 1, 0);

	WaitMs(2000);
	printf("\nInitialized\n");
	printf("\e[1;1H\e[2J");		//init done clear screen

	while (1)
	{

		if (USARTCharReceived(0))			//char recived from USART
		{
			direction = USARTReceive(0);
			USARTTransmit(0, direction);	//back to serialmonitor
			printf("\n");
		}

		if (direction == 'u')		//7segment count up
		{
			i++;
			if (i > 9)
			{
				i = 0;
			}

			PORTF = segm[i];
			WaitMs(500);
		}
		else if (direction == 'd')	//7segment count down
		{
			i--;
			if (i < 0)
			{
				i = 9;
			}

			PORTF = segm[i];
			WaitMs(500);
		}
		else if (direction == 'f')	//clockwise 1 revolution
		{
			motor('f', 512, 2, 0);
			direction = '0';
		}
		else if (direction == 'b')	//counterclockwise 1 revolution
		{
			motor('b', 512, 2, 0);
			direction = '0';
		}
		else if (direction == 'c')	//"custom" modify yourself during programm
		{
			printf("\nDirection (f[forward] / b[backward]): ");
			scanf(" %c", &directionfb);
			printf("%c", directionfb);

			if (directionfb == 'f' || directionfb == 'b')			//forward | backward | cancel
			{
				printf("\nRevolution Mod (t[total] / p[partial]): ");
				scanf(" %c", &ntimestp);
				printf("%c", ntimestp);

				if (ntimestp == 't')
				{
					printf("\nRevolutions: ");
					scanf("%d", &ntimes);
					printf("%d", ntimes);

					if (ntimes >= 1)
					{
						ntimes = ntimes * 512;

						printf("\nSpeed\n 1 [seconds for one revolution: 4,096]\n 2 [seconds for one revolution: 8,192]\n 3 [seconds for one revolution: 16,384]\n ");
						scanf("%d", &speed);
						printf("%d", speed);

						if (speed == 1)
						{
							speed = 2;

							printf("\nCount Revolutions? y/n\n ");
							scanf(" %c", &cyclecounteryn);
							printf("%c", cyclecounteryn);


							if (cyclecounteryn == 'y' || cyclecounteryn == 'n')
							{
								motor(directionfb, ntimes, speed, cyclecounteryn);
								direction = '0';
							}
							else
							{
								direction = '0';
							}
						}
						else if (speed == 2)
						{
							speed = 4;

							printf("\nCount Revolutions? y/n\n");
							scanf(" %c", &cyclecounteryn);
							printf("%c", cyclecounteryn);

							if (cyclecounteryn == 'y' || cyclecounteryn == 'n')
							{
								motor(directionfb, ntimes, speed, cyclecounteryn);
								direction = '0';
							}
							else
							{
								direction = '0';
							}
						}
						else if (speed == 3)
						{
							speed = 8;

							printf("\nCount Revolutions? y/n\n");
							scanf(" %c", &cyclecounteryn);
							printf("%c", cyclecounteryn);

							if (cyclecounteryn == 'y' || cyclecounteryn == 'n')
							{
								motor(directionfb, ntimes, speed, cyclecounteryn);
								direction = '0';
							}
							else
							{
								direction = '0';
							}
						}
						else
						{
							direction = '0';
						}
					}
					else
					{
						direction = '0';
					}
				}
				else if (ntimestp == 'p')
				{
					printf("\n[512 Steps = one revolution]\n[256 Steps = a half of a revolution]\n[128 Steps = a quarter of a revolution]\nSteps: ");
					scanf("%d", &steps);
					printf("%d\n", steps);

					printf("\nSpeed\n 1 [seconds for one revolution: 4,096]\n 2 [seconds for one revolution: 8,192]\n 3 [seconds for one revolution: 16,384]\n ");
					scanf("%d", &speed);
					printf("%d", speed);

					if (speed == 1)
					{
						speed = 2;

						printf("\nCount Revolutions? y/n\n ");
						scanf(" %c", &cyclecounteryn);
						printf("%c", cyclecounteryn);


						if (cyclecounteryn == 'y' || cyclecounteryn == 'n')
						{
							motor(directionfb, steps, speed, cyclecounteryn);
							direction = '0';
						}
						else
						{
							direction = '0';
						}
					}
					else if (speed == 2)
					{
						speed = 4;

						printf("\nCount Revolutions? y/n\n");
						scanf(" %c", &cyclecounteryn);
						printf("%c", cyclecounteryn);

						if (cyclecounteryn == 'y' || cyclecounteryn == 'n')
						{
							motor(directionfb, steps, speed, cyclecounteryn);
							direction = '0';
						}
						else
						{
							direction = '0';
						}
					}
					else if (speed == 3)
					{
						speed = 8;

						printf("\nCount Revolutions? y/n\n");
						scanf(" %c", &cyclecounteryn);
						printf("%c", cyclecounteryn);

						if (cyclecounteryn == 'y' || cyclecounteryn == 'n')
						{
							motor(directionfb, steps, speed, cyclecounteryn);
							direction = '0';
						}
						else
						{
							direction = '0';
						}
					}
					else
					{
						direction = '0';
					}
				}
			}
			else
			{
				direction = '0';
			}
		}
		else if (direction == 't')	//switch to "button only mode"
		{
			break;
		}
		else
		{
			direction = '0';
		}
	}

	printf("Press the reset button at the Arduinoboard to\n return to USART-mode");

	while (1)	//button only mode
	{
		uint8_t up = !(PINA & 1), down = !((PINA & (1 << 1)) >> 1), lastup, lastdown;

		if (up && !down && i < 9 && !lastup)
		{
			lastup = 1;
		}
		else if (down && !up && i > 0 && !lastdown)
		{
			lastdown = 1;
		}

		if (!up && lastup)
		{
			i++;
			lastup = 0;
		}
		else if (!down && lastdown)
		{
			i--;
			lastdown = 0;
		}

		PORTF = segm[i];
		WaitMs(50);
	}
}