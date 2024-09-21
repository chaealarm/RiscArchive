/*
 * Example for using AFIO to remap peripheral outputs to alternate configuration
 * 06-01-2023 B. Roy, based on previous work by:
 * 03-28-2023 E. Brombaugh
 * 05-29-2023 recallmenot adapted from Timer1 to Timer2
 *
 * Usage: 
 * Connect LEDs between PD3 and GND, PD4 and GND, PC1 and GND, and PC7 and GND
 * Observe activity on PD3 and PD4, then activity on PC1 and PC7, and back
 *
 * Nutshell:
 * 1. Ensure you're providing a clock to the AFIO peripheral! Save yourself an 
 * 	hour of troubleshooting!
 *	RCC->APB2PCENR |= RCC_APB2Periph_AFIO
 * 2. Apply the remapping configuration bits to the AFIO register:
 * 	AFIO->PCFR1 |= AFIO_PCFR1_TIM2_REMAP_FULLREMAP
 * 3. Go on about your business.
 *
 * /


Timer 2 pin mappings by AFIO->PCFR1
	00	AFIO_PCFR1_TIM2_REMAP_NOREMAP
		D4		T2CH1ETR
		D3		T2CH2
		C0		T2CH3
		D7		T2CH4  --note: requires disabling nRST in opt
	01	AFIO_PCFR1_TIM2_REMAP_PARTIALREMAP1
		C5		T2CH1ETR_
		C2		T2CH2_
		D2		T2CH3_
		C1		T2CH4_
	10	AFIO_PCFR1_TIM2_REMAP_PARTIALREMAP2
		C1		T2CH1ETR_
		D3		T2CH2
		C0		T2CH3
		D7		T2CH4  --note: requires disabling nRST in opt
	11	AFIO_PCFR1_TIM2_REMAP_FULLREMAP
		C1		T2CH1ETR_
		C7		T2CH2_
		D6		T2CH3_
		D5		T2CH4_
*/

#include "ch32v003fun.h"
#include <stdio.h>
#include "ch32v003_GPIO_branchless.h"
/* some bit definitions for systick regs */
#define SYSTICK_SR_CNTIF ( 1 << 0 )
#define SYSTICK_CTLR_STE ( 1 << 0 )
#define SYSTICK_CTLR_STIE ( 1 << 1 )
#define SYSTICK_CTLR_STCLK ( 1 << 2 )
#define SYSTICK_CTLR_STRE ( 1 << 3 )
#define SYSTICK_CTLR_SWIE ( 1 << 31 )

#define MC3 0
#define MCS3 1
#define MD3 2
#define MDS3 3
#define ME3 4
#define MF3 5
#define MFS3 6
#define MG3 7
#define MGS3 8
#define MA3 9
#define MAS3 10
#define MB3 11
#define MC4 12
#define MCS4 13
#define MD4 14
#define MDS4 15
#define ME4 16
#define MF4 17
#define MFS4 18
#define MG4 19
#define MGS4 20
#define MA4 21
#define MAS4 22
#define MB4 23
#define MC5 24
#define MCS5 25
#define MD5 26
#define MDS5 27
#define ME5 28
#define MF5 29
#define MFS5 30
#define MG5 31
#define MGS5 32
#define MA5 33
#define MAS5 34
#define MB5 35
#define MC6 36
#define MCS6 37
#define MD6 38
#define MDS6 39
#define ME6 40
#define MF6 41
#define MFS6 42
#define MG6 43
#define MGS6 44
#define MA6 45
#define MAS6 46
#define MB6 47
#define MC7 48
#define MCS7 49
#define MD7 50
#define MDS7 51
#define ME7 52
#define MF7 53
#define MFS7 54
#define MG7 55
#define MGS7 56
#define MA7 57
#define MAS7 58
#define MB7 59
#define MT 60
#define MDRUM 4000
#define MSNEAR 1200
#define DEF_VOL 150
// 8 32 50 58
//12

uint16_t Melody[61] = { 3822, 3608, 3405, 3214, 3034, 2864, 2703, 2551, 2408, 2273, 2145, 2025, 1911, 1804, 1703, 1607,
	1517, 1432, 1351, 1276, 1204, 1136, 1073, 1012, 956, 902, 851, 804, 758, 716, 676, 638, 602, 568, 536, 506, 478,
	451, 426, 402, 379, 358, 338, 319, 301, 284, 268, 253, 239, 225, 213, 201, 190, 179, 169, 159, 150, 142, 134, 127, 0 };

uint8_t Aoharu1[] = {MD7, MCS7, MA6, MFS6,MFS6,MFS6,MFS6,MFS6, MT, MT, MT,MT, MT,MT,MT,MT,MD7, MCS7, MA6, ME6,ME6,ME6, MT,MT,MT,MT,MT,MT,MT,MT,MT,MT, MD7, MCS7, MA6, MD6, MD6, MD6, MD6, MD6, MT, MT, MT, MT, MT, MT,
	MT, MT, MD6, ME6, MA6, MCS7, MCS7, MCS7, MCS7, MCS7, MT, MT, MT, MT, MT, MT, MT, MT };
uint8_t Aoharu2[] = {MCS7, MCS7, MCS7, MD7, MD7, MD7, MA7, MA7, MCS7, MCS7, MCS7, MD7, MD7, MD7, MA6, MA6};
uint8_t Aoharu3[] = {MCS7, MCS7, MCS7, MD7, MD7, MD7, MA7, MA7, ME7, ME7, ME7, MD7, MD7, MD7, MA6, MA6 };
uint8_t Aoharu4[] = {MCS7, MCS7, MCS7, MD7, MD7, MD7, MA7, MA7, MCS7, MCS7, MCS7, MA7, MA7, MA7, ME7, ME7};
uint8_t Aoharu5[] = { MD7, MD7, MD7, MA6, MA6, MA6, MA7, MA7, MD7, MD7, MD7, MD7, MT, MT, MT, MT, 
	MD7, MD7, MD7, MA6, MA6, MA6, MA7, MA7, MD7, MD7, MD7, MD7, MT, MT, MT, MT, 
	MD7, MD7, MD7, MA6, MA6, MA6, MA7, MA7, ME7, ME7, ME7, ME7, MT, MT, MT, MT, 
	ME7, ME7, ME7, MFS7, MFS7, MFS7, MD7, MD7, MA6, MA6, MA6, MA6, MT, MT, MT, MT};

volatile uint32_t count = 0;
volatile uint32_t mdycount=0;
volatile uint32_t subcount = 0;
uint16_t mdy;
uint8_t volume, toneseq=0;


void SysTick_Handler( void ) __attribute__( ( interrupt ) );
void SysTick_Handler( void ) 
{
	// move the compare further ahead in time.
	// as a warning, if more than this length of time
	// passes before triggering, you may miss your
	// interrupt.
	SysTick->CMP += ( 48 * 2 );

	/* clear IRQ */
	SysTick->SR = 0;

	//2us마다 발동되는 인터럽트
	if ( count )
		subcount++;
	else
		subcount = 0;
	if ( count && subcount >= 500 )
	{
		count--;
		subcount = 0;
	}
	if ( mdycount ) mdycount--;
}
void systick_init( void )
{
	/* disable default SysTick behavior */
	SysTick->CTLR = 0;

	/* enable the SysTick IRQ */
	NVIC_EnableIRQ( SysTicK_IRQn );

	/* Set the tick interval to 1ms for normal op */
	SysTick->CMP = ( 48*2 ) - 1;

	/* Start at zero */
	SysTick->CNT = 0;

	/* Enable SysTick counter, IRQ, HCLK/1 */
	SysTick->CTLR = SYSTICK_CTLR_STE | SYSTICK_CTLR_STIE | SYSTICK_CTLR_STCLK;
}

/******************************************************************************************
 * initialize TIM2 for PWM
 ******************************************************************************************/
void t2pwm_init( void )
{
	// Enable GPIOC, GPIOD, TIM2, and AFIO *very important!*
	RCC->APB2PCENR |= RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC;
	RCC->APB1PCENR |= RCC_APB1Periph_TIM2;

	// PD4 is T2CH1, 10MHz Output alt func, push-pull (also works in oepn drain OD_AF)
	//GPIOD->CFGLR &= ~(0xf<<(4*4));
	//GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*4);

	// PD3 is T2CH2, 10MHz Output alt func, push-pull (also works in oepn drain OD_AF)
	//GPIOD->CFGLR &= ~(0xf<<(4*3));
	//GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*3);

	// PC1 is T2CH1_, 10MHz Output alt func, push-pull (also works in oepn drain OD_AF)
	GPIOC->CFGLR &= ~(0xf<<(4*1));
	GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*1);

	// PC7 is T2CH2_, 10MHz Output alt func, push-pull (also works in oepn drain OD_AF)
	GPIOC->CFGLR &= ~(0xf<<(4*7));
	GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*7);
	
	// Reset TIM2 to init all regs
	RCC->APB1PRSTR |= RCC_APB1Periph_TIM2;
	RCC->APB1PRSTR &= ~RCC_APB1Periph_TIM2;
	
	// SMCFGR: default clk input is CK_INT
	// set TIM2 clock prescaler divider 
	TIM2->PSC = 0x0000;
	// set PWM total cycle width
	TIM2->ATRLR = 255;
	
	// for channel 1 and 2, let CCxS stay 00 (output), set OCxM to 110 (PWM I)
	// enabling preload causes the new pulse width in compare capture register only to come into effect when UG bit in SWEVGR is set (= initiate update) (auto-clears)
	TIM2->CHCTLR1 |= TIM_OC1M_2 | TIM_OC1M_1 | TIM_OC1PE;
	TIM2->CHCTLR1 |= TIM_OC2M_2 | TIM_OC2M_1 | TIM_OC2PE;

	// CTLR1: default is up, events generated, edge align
	// enable auto-reload of preload
	TIM2->CTLR1 |= TIM_ARPE;

	// Enable CH1 output, positive pol
	TIM2->CCER |= TIM_CC1E | TIM_CC1P;
	// Enable CH2 output, positive pol
	TIM2->CCER |= TIM_CC2E | TIM_CC2P;

	// initialize counter
	TIM2->SWEVGR |= TIM_UG;

	// Enable TIM2
	TIM2->CTLR1 |= TIM_CEN;

	//USE AFIO 11 MODE
	AFIO->PCFR1 |= AFIO_PCFR1_TIM2_REMAP_FULLREMAP;
}

/*****************************************************************************************
 * set timer channel PW
 *****************************************************************************************/
void t2pwm_setpw(uint8_t chl, uint16_t width)
{
	switch(chl&3)
	{
		case 0: TIM2->CH1CVR = width; break;
		case 1: TIM2->CH2CVR = width; break;
		case 2: TIM2->CH3CVR = width; break;
		case 3: TIM2->CH4CVR = width; break;
	}
	TIM2->SWEVGR |= TIM_UG; // load new value in compare capture register
}

/*****************************************************************************************
 * Remap T1CH1/T1CH2 from PD4/PD3 to PC1/PC7
 *
 * Can remap on-the fly; no need to re-initialize timers, reset GPIO mode/config, etc.
 *
 * Leaves the previous pins configured as 'alternate function' mode - i.e. disconnected
 * from the GPIO peripheral, and floating.
 *
 *****************************************************************************************/

//음계 세팅(T/2[us] = f[hz]*1000000/2 , 크기(PWM, 0~255), 재생시간[ms])
void settone( uint16_t mmdy, uint8_t mvolume, uint32_t mcount )
{
	if (mmdy==0)
	{
		mdycount = mdy = volume = toneseq = 0;
		count = mcount;
		return;
	}
	mdycount = mmdy;
	mdy = mmdy/2;
	volume = mvolume;
	toneseq = 1;
	count = mcount;
}

//음 재생(main의 while(1)에서 무한히 반복되며, 원활한 재생을 위해 main의 while(1) 내에서 추가적인 반복문, Delay함수 등으로 지연되지 않아야 함)
void tone()
{
	if ( !toneseq || !mdy ) return;
	if ( mdycount == 0 && count )
	{
		t2pwm_setpw( 0, 0 );
		mdycount = mdy * 2;
		toneseq = 1;
	}
	else if ( mdycount == 0 && count == 0 )
	{
		t2pwm_setpw( 0, 0 );
		toneseq = 0;
	}
	else if ( mdycount >= mdy && toneseq == 1 )
	{
		t2pwm_setpw( 0, 0 );
		toneseq = 2;
	}	
	else if ( mdycount < mdy && toneseq == 2 )
	{
		t2pwm_setpw( 0, volume );
		toneseq = 1;
	}

}

int main()
{

	uint32_t i;
	uint16_t mseq=0, msel=0;
	SystemInit();
	Delay_Ms( 100 );

	t2pwm_init(); //PWM(볼륨 셋업)
	systick_init(); //Tick 인터럽트 셋업
	GPIO_port_enable( GPIO_port_C ); //C0핀 OUTPUT(스피커)
	GPIO_pinMode( GPIOv_from_PORT_PIN( GPIO_port_C, 0 ), GPIO_pinMode_O_pushPull, GPIO_Speed_50MHz );
	GPIO_port_enable( GPIO_port_D ); //D6핀 OUTPUT(LED)
	GPIO_pinMode( GPIOv_from_PORT_PIN( GPIO_port_D, 6 ), GPIO_pinMode_O_pushPull, GPIO_Speed_50MHz );
	
	//부팅완료시 LED 켜짐
	GPIO_digitalWrite( GPIOv_from_PORT_PIN( GPIO_port_D, 6 ), high );


	while(1)
	{
		tone();
		
		switch (msel)
		{
			case 0:
			case 1:
			case 2:
				if ( mdycount == 0 && !count )
				{
					settone( Melody[Aoharu1[mseq]], DEF_VOL, 200 );
					mseq++;
					if ( mseq == sizeof( Aoharu1 ) / sizeof( Aoharu1[0] ) )
					{
						mseq = 0;
						msel++;
					}
				}
				break;
			case 3:
				if ( mdycount == 0 && !count )
				{
					settone( Melody[Aoharu1[mseq]], DEF_VOL, 200 );
					mseq++;
					if ( mseq == 48 )
					{
						mseq = 0;
						msel++;
					}
				}
				break;
			case 4:
				if ( mdycount == 0 && !count )
				{
					settone( Melody[Aoharu1[mseq]], DEF_VOL, 200 );
					mseq++;
					if ( mseq == 8 )
					{
						mseq = 0;
						msel++;
					}
				}
				break;
			case 5:
				if ( mdycount == 0 && !count )
				{
					settone( Melody[MT], 0, 2400 );
					mseq = 0;
					msel++;
				}
				break;
			case 6:
				if ( mdycount == 0 && !count )
				{
					settone( Melody[Aoharu2[mseq]], DEF_VOL, 200 );
					mseq++;
					if ( mseq == sizeof( Aoharu2 ) / sizeof( Aoharu2[0] ) )
					{
						mseq = 0;
						msel++;
					}
				}
				break;
			case 7:
				if ( mdycount == 0 && !count )
				{
					settone( Melody[Aoharu3[mseq]], DEF_VOL, 200 );
					mseq++;
					if ( mseq == sizeof( Aoharu3 ) / sizeof( Aoharu3[0] ) )
					{
						mseq = 0;
						msel++;
					}
				}
				break;
			case 8:
				if ( mdycount == 0 && !count )
				{
					settone( Melody[Aoharu2[mseq]], DEF_VOL, 200 );
					mseq++;
					if ( mseq == sizeof( Aoharu2 ) / sizeof( Aoharu2[0] ) )
					{
						mseq = 0;
						msel++;
					}
				}
				break;
			case 9:
				if ( mdycount == 0 && !count )
				{
					settone( Melody[Aoharu4[mseq]], DEF_VOL, 200 );
					mseq++;
					if ( mseq == sizeof( Aoharu4 ) / sizeof( Aoharu4[0] ) )
					{
						mseq = 0;
						msel++;
					}
				}
				break;
			case 10:
				if ( mdycount == 0 && !count )
				{
					settone( Melody[Aoharu2[mseq]], DEF_VOL, 200 );
					mseq++;
					if ( mseq == sizeof( Aoharu2 ) / sizeof( Aoharu2[0] ) )
					{
						mseq = 0;
						msel++;
					}
				}
				break;
			case 11:
				if ( mdycount == 0 && !count )
				{
					settone( Melody[Aoharu3[mseq]], DEF_VOL, 200 );
					mseq++;
					if ( mseq == sizeof( Aoharu3 ) / sizeof( Aoharu3[0] ) )
					{
						mseq = 0;
						msel++;
					}
				}
				break;
			case 12:
				if ( mdycount == 0 && !count )
				{
					settone( Melody[Aoharu2[mseq]], DEF_VOL, 200 );
					mseq++;
					if ( mseq == sizeof( Aoharu2 ) / sizeof( Aoharu2[0] ) )
					{
						mseq = 0;
						msel++;
					}
				}
				break;
			case 13:
				if ( mdycount == 0 && !count )
				{
					settone( Melody[Aoharu2[mseq]], DEF_VOL, 200 );
					mseq++;
					if ( mseq == 8 )
					{
						mseq = 0;
						msel++;
					}
				}
				break;
			case 14:
			case 15:
			case 16:
			case 17:
				if ( mdycount == 0 && !count )
				{
					settone( Melody[Aoharu5[mseq]], DEF_VOL, 200 );
					mseq++;
					if ( mseq == sizeof( Aoharu5 ) / sizeof( Aoharu5[0] ) )
					{
						mseq = 0;
						msel++;
					}
				}
				break;
			case 18:
				if ( mdycount == 0 && !count )
				{
					settone( Melody[Aoharu5[mseq]], DEF_VOL, 200 );
					mseq++;
					if ( mseq == 9 )
					{
						mseq = 0;
						msel++;
					}
				}
				break;
			case 19:
			case 20:
				if ( mdycount == 0 && !count )
				{
					settone( Melody[Aoharu1[mseq]], DEF_VOL, 200 );
					mseq++;
					if ( mseq == sizeof( Aoharu1 ) / sizeof( Aoharu1[0] ) )
					{
						mseq = 0;
						msel++;
					}
				}
				break;
			case 21:
				if ( mdycount == 0 && !count )
				{
					settone( Melody[MT], 0, 2400 );
					mseq = 0;
					msel = 0;
					
				}
				break;

		}



		
		
	}
}
