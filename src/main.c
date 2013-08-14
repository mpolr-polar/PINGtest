/**
 *****************************************************************************
 **
 **  File        : main.c
 **
 **  Abstract    : main function.
 **
 **  Functions   : main
 **
 **  Environment : Eclipse with Atollic TrueSTUDIO(R) Engine
 **                STMicroelectronics STM32F4xx Standard Peripherals Library
 **
 **
 **
 *****************************************************************************
 */

/* Includes */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "delay.h"

/* Private macro */
#define SENSITIVITY 8

/* Private variables */
GPIO_InitTypeDef 		GPIO_InitStructure;
GPIO_InitTypeDef		GPIO_TimeStructure;
TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;
USART_InitTypeDef		USART_InitStructure;
NVIC_InitTypeDef		NVIC_InitStructure;
uint16_t PING;

/* Private function prototypes */
/* Private functions */
void uart_putchar(const char c)
{
	/*Wait while there is the data to send*/
	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET){}

	/* Output the character data */
	USART_SendData(USART2,c);

	/* Wait while it have not finished sending */
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC) == RESET){}
}

void uart_putdec(uint16_t i)
{
	uart_putchar(i/1000 + '0');
	i %= 1000;
	uart_putchar(i/100 + '0');
	i %= 100;
	uart_putchar(i/10 + '0');
	i %= 10;
	uart_putchar(i + '0');
}

void uart_putstr(const char* str)
{
	char c;	//for output string

	/* Output the string data by one character */
	while(1)
	{
		c = *str++;
		if(c == 0)break;
		uart_putchar(c);
	}
}
/*
void TIM4_IRQHandler()
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update) != RESET)
	{
		PING++;
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	}
}
*/
/**
 **===========================================================================
 **
 **  Abstract: main program
 **
 **===========================================================================
 */
int main(void)
{
	/* initialize delay function */
	delay_init();

	/* supply clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

	/* initialize GPIO (for USART) */
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);

	/* initialize usart (only tx) */
	USART_InitStructure.USART_BaudRate				= 9600;
	USART_InitStructure.USART_WordLength			= USART_WordLength_8b;
	USART_InitStructure.USART_StopBits				= USART_StopBits_1;
	USART_InitStructure.USART_Parity				= USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode					= USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2,ENABLE);

	/* initialize GPIO (for timer)*/
	GPIO_TimeStructure.GPIO_Pin		= GPIO_Pin_0;
	GPIO_TimeStructure.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_TimeStructure.GPIO_Speed	= GPIO_Speed_100MHz;
	GPIO_TimeStructure.GPIO_OType	= GPIO_OType_PP;
	GPIO_TimeStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_TimeStructure);

	/* initialize timer */
	TIM_TimeBaseStructure.TIM_Period			= 3360000-1;
	TIM_TimeBaseStructure.TIM_Prescaler			= SENSITIVITY-1;
	TIM_TimeBaseStructure.TIM_ClockDivision		= 0;
	TIM_TimeBaseStructure.TIM_CounterMode		= TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter	= 0;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM4, TIM_IT_Update,ENABLE);

	/* initialize NVIC (for timer) */
/*	NVIC_InitStructure.NVIC_IRQChannel						= TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
*/
	while(1)
	{
		GPIO_TimeStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_Init(GPIOD, &GPIO_TimeStructure);
		GPIOD->ODR = 1;
		delay_us(4);
		GPIOD->ODR = 0;
		GPIO_TimeStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_Init(GPIOD, &GPIO_TimeStructure);
		TIM4->CNT = 0;
		while((GPIOD->IDR & 1)==0){}
		TIM_Cmd(TIM4,ENABLE);
		while((GPIOD->IDR & 1)==1){}
		TIM_Cmd(TIM4,DISABLE);
		uart_putdec( TIM4->CNT );
		uart_putstr("\n\r");
	}

	return 0;
}


void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){
	/* TODO, implement your code here */
	return;
}

/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
uint16_t EVAL_AUDIO_GetSampleCallBack(void){
	/* TODO, implement your code here */
	return -1;
}

