/*
 * Utility.h
 *
 *  Created on: 8 de jul de 2023
 *      Author: Prof. Dr. Fagner de Araujo Pereira
 */

#ifndef UTILITY_H_
#define UTILITY_H_

//Declarações de funções úteis
void Utility_Init(void);			//inicialização de funções da biblioteca
void Configure_Clock(void);			//configuração do sistema de clock
void TIM2_Setup(void);				//configuração do Timer2 como base de tempo de 1us
void Delay_us(uint32_t delay);		//atraso em us
void Delay_ms(uint32_t delay);		//atraso em ms
void USART1_Init(void);				//configuração da USART1 para debug com printf


//Inicialização de funções da biblioteca
void Utility_Init(void)
{
	Configure_Clock();	//inicializa o sistema de clock
	TIM2_Setup();		//configura o Timer2 como base de tempo de 1us
}

//Configuração do sistema de clock para velocidade máxima em todos os barramentos
//cristal externo de 8MHz e HCLK de 168 MHz
void Configure_Clock(void)
{
	//Parâmetros do PLL principal
	#define PLL_M	4
	#define PLL_N	168
	#define PLL_P	2
	#define PLL_Q	7

	//Reseta os registradores do módulo RCC para o estado inicial
	RCC->CIR = 0;				//desabilita todas as interrupções de RCC
	RCC->CR |= RCC_CR_HSION;	//liga o oscilador HSI
	RCC->CFGR = 0;				//reseta o registrador CFGR
	//Desliga HSE, CSS e o PLL e o bypass de HSE
	RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_CSSON |
			   RCC_CR_PLLON | RCC_CR_HSEBYP);
	RCC->PLLCFGR = 0x24003010;	//reseta o registrador PLLCFGR

	//Configura a fonte de clock (HSE), os parâmetros do PLL,
	//prescalers dos barramentos AHB, APB
	RCC->CR |= RCC_CR_HSEON;				//habilita HSE
	while(!((RCC->CR) & RCC_CR_HSERDY));	//espera HSE ficar pronto
    RCC->CFGR |= 0x9400;	//HCLK = SYSCLK/1, PCLK2 = HCLK/2, PCLK1 = HCLK/4

    //Configura a fonte de clock e os parâmetros do PLL principal
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
                   (0x400000)           | (PLL_Q << 24);

    RCC->CR |= RCC_CR_PLLON;			//habilita o PLL
    while(!(RCC->CR & RCC_CR_PLLRDY));	//espera o PLL ficar pronto verificando a flag PLLRDY

    RCC->CFGR |= 0x2;					//seleciona o PLL como fonte de SYSCLK
    while((RCC->CFGR & 0xC) != 0x8);	//espera o PLL ser a fonte de SYSCLK
}

//Configura o timer 2 como base de tempo de 1us
void TIM2_Setup(void)
{
	//O modo padrão do contador é com contagem crescente
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;	//liga o clock do Timer2
	TIM2->PSC = 83;						//prescaler para incrementos a cada 1uS
	TIM2->EGR = TIM_EGR_UG;				//update event para escrever o valor do prescaler
	TIM2->CR1 |= TIM_CR1_CEN;			//habilita o timer
}

//Criação de atraso em us
void Delay_us(uint32_t delay)
{
	TIM2->CNT = 0;				//inicializa o contador com 0
	while(TIM2->CNT < delay);	//aguarda o tempo passar
}

//Criação de atraso em ms
void Delay_ms(uint32_t delay)
{
	uint32_t max = 1000*delay;
	TIM2->CNT = 0;				//inicializa o contador com 0
	while(TIM2->CNT < max);		//aguarda o tempo passar
}

#endif /* UTILITY_H_ */