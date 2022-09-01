/*
 * table2.cpp
 *
 * Created: 12-08-2014 15:43:18
 *  Author: Urvashi
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#define USART_BAUDRATE 9600
#define PS ((F_CPU/(16UL*USART_BAUDRATE))-1)
void lcd_init();
void disp_cmd(unsigned int cmd);
void disp_data(int cmd);
void disp_num(unsigned int num);
void disp_string(char *str);
int adc_read(int channel);
//#define LCD_DATA PORTA // LCD data port
//#define ctrl PORTB
//#define en PB2 // enable signal
//#define rw PB1 // read/write signal
//#define rs PB0 // register select signal
void usart_init();
unsigned int usart_getch();
char card[12],correctcard[14] = "18008907FB6D";
unsigned int i=0,j=0,k=0,e=100,f=150,g=200,h=250,bal=1000;
void getcard_id(void);
//void getccard_id(void);
void LCD_display(void);
//void touchscreen();
void station();

void lcd_init()
{
	disp_cmd(0X02);
	disp_cmd(0X28);
	disp_cmd(0X0C);
	disp_cmd(0X06);
}

void disp_cmd(unsigned int cmd)
{
	PORTB=((cmd)& 0XF0);
	PORTB=(PORTB>>2) + 0X02;
	_delay_ms(2);
	PORTB=PORTB - 0X02;
	PORTB=((cmd<<4)& 0XF0);
	PORTB=(PORTB>>2) + 0X02;
	_delay_ms(2);
	PORTB=PORTB - 0X02;
	
	
}
void disp_data( int cmd)
{
	PORTB=(cmd  & 0XF0);
	PORTB=(PORTB>>2)+ 0X03;
	_delay_ms(2);
	PORTB=PORTB - 0X03;
	PORTB=((cmd<<4)& 0XF0);
	PORTB=(PORTB>>2)+ 0X03;
	_delay_ms(2);
	PORTB=PORTB - 0X03;
}
void usart_init()
{
	UCSRB =(1<<RXEN) | (1<<TXEN);
	UCSRC =(1<<URSEL) | (1<<USBS) | (1<<UCSZ1) | (1<<UCSZ0);
	UBRRL=PS;
	UBRRH=(PS>>8);
}

void usart_tx(unsigned char data)
{
	while((UCSRA & 0x20)==0);
	UDR=data;
}
void usart_txstring(char *str)
{
	int i=0;
	while (str[i]!='\0')
	{
		usart_tx(str[i]);
		_delay_ms(100);
		i++;
	}
}

unsigned int usart_rx()
{
	while((UCSRA & 0x80)==0);
	return UDR;
}
void disp_string( char *str)
{
	int i=0;
	while (str[i]!='\0')
	{
		disp_data (str[i]);
		i++;
	}
}

void disp_num(unsigned int num)
{
	int a=0;
	if (num==0)
	{
		disp_data(48);
	}
	disp_cmd (0X04);
	while (num!=0)
	{
		a=num % 10;
		disp_data(a+48);
		num=num/10;
	}
	disp_cmd (0X06);
}

unsigned int usart_getch()
{
	while ((UCSRA & (1 << RXC)) == 0);
	return(UDR);
}


int main(void)
{
	DDRB=0XFF; 
	DDRC=0XFF;
	//DDRB=0x07; 
	lcd_init();
    _delay_ms(2);
	usart_init(); 
	
	//getccard_id(); 
	disp_cmd(0x80);
	disp_string("Welcome to Metro"); 
	disp_cmd(0xc0);
	disp_string("Swap your card"); 
	while(1)
	{
		getcard_id(); 
		_delay_ms(2);
		disp_cmd(0x01);
		/*disp_cmd(0XC0);
		LCD_display();
		_delay_ms(100);*/
		
	   int a=strncmp(card,correctcard,12);
	    if (a<=0)
	    {
			disp_cmd(0X01);
			disp_cmd(0xC0);
			disp_string("ACCESS GRANTED");
			 _delay_ms(3000);
			 disp_cmd(0x01);
			 disp_cmd(0x80);
			 disp_string("OPENING GATE");
			 PORTC = 0b00000001;
			 _delay_ms(20000);
			 PORTC = 0X00;
			 _delay_ms(100);
			  disp_cmd(0x80);
			  disp_string("CLOSING GATE");
			 PORTC=  0b00000010;
			  _delay_ms(20000);
			 /* _delay_ms(1000);
			  PORTC = 0b00000010;
			   _delay_ms(1000);
			    _delay_ms(1000);
						
			*/	
		}			 		 
	   else
	    {
			
			 		   disp_cmd(0x80);
			 		   disp_string("Access Denied");
		}						
			   		    
	 
	}
	  }	
			  
		

void station()
{
 _delay_ms(100);
 disp_cmd(0x01);
 disp_cmd(0x80);
 disp_string("choose your station");
 _delay_ms(1000);
 disp_cmd(0x01);
 _delay_ms(2);
 disp_cmd(0x80);
 disp_string("1. a    2. b");
 disp_cmd(0xc0);
 disp_string("3. c    4. d");
 _delay_ms(1000);
 //touchscreen();
} 

void getcard_id(void) 
{
	for(i=0;i<12;i++)
	{
		card[i]= usart_getch();
	}
	//card[i]='\0';
}
/*void getccard_id(void)
{
	for(i=0;i<12;i++)
	{
		correctcard[i]=usart_getch();
	}
}*/
void LCD_display(void) 
{
	
	for(i=0;i<12;i++)
	{
	
	   disp_data(card[i]);
	}
	
}

void touchscreen()
{
	
	DDRC= 0b11111010;
	PORTC=0b00001100;
	disp_cmd(0x01);
	_delay_ms(100);
	int x=adc_read(0);
	
	DDRC= 0b11110101;
	PORTC=0b00001001;
	int y=adc_read(3);
	/*disp_cmd(0x85);
	disp_num(x);
	disp_cmd(0xC5);
	disp_num(y);
	_delay_ms(1000);*/
		
		if ((x>1000)&&(x<1050) && (y>390)&&(y<500))
		{
	disp_cmd(0x80);
	disp_string("1. a    2. b");
	disp_cmd(0xc0);
	disp_string("3. c    4. d");
	_delay_ms(1000);
		}
		while (1)
		{
		
			
	if ((x>130)&&(x<300) && (y>100)&&(y<250))
	{
		 if(bal>e)
		 {
			 bal=bal-e;
			 disp_cmd(0x01);
			 _delay_ms(2);
			 disp_cmd(0x80);
			 disp_string("CONFIRMED");
			 _delay_ms(2);
			 disp_cmd(0xc0);
			 disp_string("Avail Bal=");
			 disp_cmd(0xcb);
			 disp_num(bal);
			 _delay_ms(1000);
			 //break;
		 }
		 
		 else
		 {
			 disp_cmd(0x01);
			 disp_cmd(0x80);
			 disp_string("Please Recharge");
			 disp_cmd(0xc0);
			 disp_string("Avail Bal=");
			 disp_cmd(0xcb);
			 disp_num(bal);
			 _delay_ms(1000);
			//break;
		 }
		 
	 }
		
	
	else if ((x>500)&&(x<860) && (y>100)&&(y<250))
	{
		 if(bal>=f)
		 {
			 bal=bal-f;
			 disp_cmd(0x01);
			 _delay_ms(2);
			 disp_cmd(0x80);
			 disp_string("CONFIRMED");
			 _delay_ms(2);
			 disp_cmd(0xc0);
			 disp_string("Avail Bal=");
			 disp_cmd(0xcb);
			 disp_num(bal);
			 _delay_ms(1000);
			 //break;
		 }
		 
		 else
		 {
			 disp_cmd(0x01);
			 disp_cmd(0x80);
			 disp_string("Please Recharge");
			 disp_cmd(0xc0);
			 disp_string("Avail Bal=");
			 disp_cmd(0xcb);
			 disp_num(bal);
			 _delay_ms(1000);
			 //break;
		 }
	}
	else if ((x>200)&&(x<300) &&(y>850)&& (y<950))
	{
		 if(bal>=g)
		 {
			 bal=bal-g;
			 disp_cmd(0x01);
			 _delay_ms(2);
			 disp_cmd(0x80);
			 disp_string("CONFIRMED");
			 _delay_ms(2);
			 disp_cmd(0xc0);
			 disp_string("Avail Bal=");
			 disp_cmd(0xcb);
			 disp_num(bal);
			 _delay_ms(1000);
			 //break;
		 }
		 
		 else
		 {
			 disp_cmd(0x01);
			 disp_cmd(0x80);
			 disp_string("Please Recharge");
			 disp_cmd(0xc0);
			 disp_string("Avail Bal=");
			 disp_cmd(0xcb);
			 disp_num(bal);
			 _delay_ms(1000);
			 //break;
		 }
	}
	else if ((x<600)&&(x>900) && (y>600)&&(y<900))
	{
		 if(bal>=h)
		 {
			 bal=bal-h;
			 disp_cmd(0x01);
			 _delay_ms(2);
			 disp_cmd(0x80);
			 disp_string("CONFIRMED");
			 _delay_ms(2);
			 disp_cmd(0xc0);
			 disp_string("Avail Bal= ");
			 disp_cmd(0xcb);
			 disp_num(bal);
			 _delay_ms(1000);
			 //break;
		 }
		 
		 else
		 {
			 disp_cmd(0x01);
			 disp_cmd(0x80);
			 disp_string("Please Recharge");
			 disp_cmd(0xc0);
			 disp_string("Avail Bal=");
			 disp_cmd(0xcb);
			 disp_num(bal);
			 _delay_ms(1000);
			 //break;
		 }
	}
		}	
	
	}

int adc_read(int channel)
{
	
	ADMUX=0X40 + channel;
	ADCSRA=0XD7;
	while((ADCSRA & 0X10) ==0);
	return ADC;
}


