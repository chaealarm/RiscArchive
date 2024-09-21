#include "ch32v003fun.h"
#include <stdio.h>
#include <string.h>
#include "rv003usb.h"
#include "ch32v003_GPIO_branchless.h"

/* some bit definitions for systick regs */ //Systick 설정을 위한 define
#define SYSTICK_SR_CNTIF ( 1 << 0 )
#define SYSTICK_CTLR_STE ( 1 << 0 )
#define SYSTICK_CTLR_STIE ( 1 << 1 )
#define SYSTICK_CTLR_STCLK ( 1 << 2 )
#define SYSTICK_CTLR_STRE ( 1 << 3 )
#define SYSTICK_CTLR_SWIE ( 1 << 31 )

//각각 음계변수에 해당하는 음계를 문자로 define
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

//멜로디, C3부터 B7까지, 마지막 60번은 묵음
uint16_t Melody[61] = { 3822, 3608, 3405, 3214, 3034, 2864, 2703, 2551, 2408, 2273, 2145, 2025, 1911, 1804, 1703, 1607,
    1517, 1432, 1351, 1276, 1204, 1136, 1073, 1012, 956, 902, 851, 804, 758, 716, 676, 638, 602, 568, 536, 506, 478,
    451, 426, 402, 379, 358, 338, 319, 301, 284, 268, 253, 239, 225, 213, 201, 190, 179, 169, 159, 150, 142, 134, 127, 0 };

//Aoharu 반복음계 변수
uint8_t Aoharu1[] = { MD7, MCS7, MA6, MFS6,MFS6,MFS6,MFS6,MFS6, MT, MT, MT,MT, MT,MT,MT,MT,MD7, MCS7, MA6, ME6,ME6,ME6, MT,MT,MT,MT,MT,MT,MT,MT,MT,MT, MD7, MCS7, MA6, MD6, MD6, MD6, MD6, MD6, MT, MT, MT, MT, MT, MT,
    MT, MT, MD6, ME6, MA6, MCS7, MCS7, MCS7, MCS7, MCS7, MT, MT, MT, MT, MT, MT, MT, MT };
uint8_t Aoharu2[] = { MCS7, MCS7, MCS7, MD7, MD7, MD7, MA7, MA7, MCS7, MCS7, MCS7, MD7, MD7, MD7, MA6, MA6 };
uint8_t Aoharu3[] = { MCS7, MCS7, MCS7, MD7, MD7, MD7, MA7, MA7, ME7, ME7, ME7, MD7, MD7, MD7, MA6, MA6 };
uint8_t Aoharu4[] = { MCS7, MCS7, MCS7, MD7, MD7, MD7, MA7, MA7, MCS7, MCS7, MCS7, MA7, MA7, MA7, ME7, ME7 };
uint8_t Aoharu5[] = { MD7, MD7, MD7, MA6, MA6, MA6, MA7, MA7, MD7, MD7, MD7, MD7, MT, MT, MT, MT,
    MD7, MD7, MD7, MA6, MA6, MA6, MA7, MA7, MD7, MD7, MD7, MD7, MT, MT, MT, MT,
    MD7, MD7, MD7, MA6, MA6, MA6, MA7, MA7, ME7, ME7, ME7, ME7, MT, MT, MT, MT,
    ME7, ME7, ME7, MFS7, MFS7, MFS7, MD7, MD7, MA6, MA6, MA6, MA6, MT, MT, MT, MT };

//Endless Carnival 반복음계 변수
uint8_t Ecarnival1[16] = { MG6, MD7, MC7, MD7, MAS6, MC7, MA6, MAS6, MG6, MA6, MF6, MG6, MD6, MF6, MC6, MD6 };
uint8_t Ecarnival2[6] = { MG6, MA6, MAS6, MC7, MAS6, MAS6 };
uint8_t Ecarnival3[6] = { MG6, MA6, MAS6, MC7, MD7, MD7 };
uint8_t Ecarnival4[6] = { MF7, MF7, MF6, MF6 };
uint8_t Ecarnival5[] = { MD7, MC7, MAS6, MF6, MG6, MDS6, MG6, MD7, MC7, MD7, MAS6, MC7, MA6, MAS6, MA6, MF6, MC6, MD6, MF6, MA6, MC7, MAS6, MA6, MAS6, MC7, MD7, MF7, MA7, MAS7, MAS7, MA7, MF7, MC7, MAS6, MA6, MF6, MG6, MD7, MC7, MD7, MAS6, MC7, MA6, MAS6, MG6, MA6, MF6, MG6, MA6, MAS6, MD7, MF7, MC7, MAS6, MA6, MAS6, MA6, MF6, MC6, MD6, MF6, MD6, MC6, MD6, MC6, MAS5, MA5, MF5 };
uint8_t Ecarnival6[] = { MDS6, MDS6, MDS6, MDS6, MDS6, MDS6, MT, MD6, MG6, MG6, MG6, MA6, MA6, MA6, MA6, MAS6, MC7, MC7, MC7, MD7, MD7, MD7, MT, MT, MT, MT, MT, MT, MC7, MC7, MAS6, MC7, MA6, MA6, MA6, MA6, MA6, MA6, MT, MT, MT, MT };

volatile uint32_t count = 0; //1ms 카운트, 음계를 해당 시간만큼 재생 카운트
volatile uint32_t mdycount = 0; //mdy(멜로디 음계) T(1/f, 주기) 카운트
volatile uint32_t subcount = 0; //4us마다 카운트하여 250회(1ms) 됐을때 count를 1 증가
uint16_t mdy; //tone()내에서 사용하는 멜로디, T/4
uint8_t volume, toneseq = 0; //tone()내에서 사용하는 볼륨, tone()내에서 재생을 위한 순서(시퀀스)
uint8_t DEF_VOL = 222; //멜로디 재생시 볼륨 설정
uint8_t istoneon = 0, is0Ao1EC = 0; //멜로디 재생/정지, Aoharu/Endless Carnival 선택

uint16_t mseq = 0, msel = 0; //반복음계 변수의 몇번째인지 순서, 멜로디 맵 재생순서

uint8_t last_key0=0, last_key1=0, last_key2 = 0; //각각 main while문 또는 인터럽트가 끝나기 직전의 키 상태를 저장 
uint32_t keytime0=0; //0번키 2초간 눌렸을때용 카운트

uint16_t kchat00=0, kchat01=0; //각각 0번키 상승, 하강 채터링
uint64_t ledcount=0; //멜로디 재생시 LED(녹색) 점멸용 카운트

uint8_t tsajoystick[8] = { 0x00 }; //USB HID 전송대기열

void tone();
void settone(uint16_t mmdy, uint8_t mvolume, uint32_t mcount);
void Aoharu_seq();
void ECarnival_seq();

void SysTick_Handler(void) __attribute__((interrupt));
void SysTick_Handler(void)
{
    // move the compare further ahead in time.
    // as a warning, if more than this length of time
    // passes before triggering, you may miss your
    // interrupt.
    // 4us마다 인터럽트 발동
    // 주의 : 인터럽트 내 실행은 4us내로 끝나야 하며, 못 끝날경우 그 뒤의 실행은 무시됨
    SysTick->CMP += (48 * 4);

    //clear IRQ
    SysTick->SR = 0;

    subcount = count ? subcount + 1 : 0;
    if (count && subcount >= 250)
    {
        count--;
        subcount = 0;
    }
    mdycount = mdycount ? mdycount - 1 : 0;
    if (!GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_A, 1)))
    {
        keytime0 = keytime0 < 250000 ? keytime0 + 1 : keytime0;
    }
    else
    {
        keytime0 = 0;
    } 

    if(ledcount==50000 && istoneon ){
         GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_C, 7), high);
    }
    else if(ledcount==100000 && istoneon ){
         GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_C, 7), low);
         ledcount=0;
    }
    else if(!istoneon) GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_C, 7), low);
    ledcount=ledcount<100000?ledcount+1:ledcount;
    
    if(istoneon) tone();

}

//Systick Inturrupt(멜로디 재생/시간계산용) init 세팅
void systick_init(void)
{
    // disable default SysTick behavior 
    SysTick->CTLR = 0;

    // enable the SysTick IRQ 
    NVIC_EnableIRQ(SysTicK_IRQn);

    // 4us마다 인터럽트 발동
    SysTick->CMP = (48 * 4) - 1;

    // Start at zero 
    SysTick->CNT = 0;

    // Enable SysTick counter, IRQ, HCLK/1 
    SysTick->CTLR = SYSTICK_CTLR_STE | SYSTICK_CTLR_STIE | SYSTICK_CTLR_STCLK;
}

//T2 PWM(볼륨조절용) init 세팅
void t2pwm_init(void)
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
    GPIOC->CFGLR &= ~(0xf << (4 * 1));
    GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF) << (4 * 1);

    // PC7 is T2CH2_, 10MHz Output alt func, push-pull (also works in oepn drain OD_AF)
    //GPIOC->CFGLR &= ~(0xf << (4 * 7));
    //GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF) << (4 * 7);

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

//T2 PWM Width 지정(볼륨조절용)
void t2pwm_setpw(uint8_t chl, uint16_t width)
{
    switch (chl & 3)
    {
    case 0: TIM2->CH1CVR = width; break;
    case 1: TIM2->CH2CVR = width; break;
    case 2: TIM2->CH3CVR = width; break;
    case 3: TIM2->CH4CVR = width; break;
    }
    TIM2->SWEVGR |= TIM_UG; // load new value in compare capture register
}

//음계 세팅(T/4[us] = f[hz]*1000000/4 , 크기(PWM, 0~255), 재생시간[ms])
void settone(uint16_t mmdy, uint8_t mvolume, uint32_t mcount)
{
    if (mmdy == 0)
    {
        mdycount = mdy = volume = toneseq = 0;
        count = mcount;
        return;
    }
    mdycount = mmdy;
    mdy = mmdy / 4;
    volume = mvolume;
    toneseq = 1;
    count = mcount;
}

//음 재생(Systick 인터럽트에서 재생하는 동안 무한히 반복되며, 원활한 재생을 위해 추가적인 반복문, Delay함수 등으로 지연되지 않아야 함)
void tone()
{
    if (!toneseq || !mdy) return;
    if (mdycount == 0 && count)
    {
        t2pwm_setpw(0, 0);
        mdycount = mdy * 2;
        toneseq = 1;
    }
    else if (mdycount == 0 && count == 0)
    {
        t2pwm_setpw(0, 0);
        toneseq = 0;
    }
    else if (mdycount >= mdy && toneseq == 1)
    {
        t2pwm_setpw(0, 0);
        toneseq = 2;
    }
    else if (mdycount < mdy && toneseq == 2)
    {
        t2pwm_setpw(0, volume);
        toneseq = 1;
    }

}

// Aoharu 멜로디 map
void Aoharu_seq()
{
    switch (msel)
    {
    case 0:
    case 1:
    case 2:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Aoharu1[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == sizeof(Aoharu1) / sizeof(Aoharu1[0]))
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 3:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Aoharu1[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == 48)
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 4:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Aoharu1[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == 8)
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 5:
        if (mdycount == 0 && !count)
        {
            settone(Melody[MT], 0, 2400);
            mseq = 0;
            msel++;
        }
        break;
    case 6:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Aoharu2[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == sizeof(Aoharu2) / sizeof(Aoharu2[0]))
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 7:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Aoharu3[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == sizeof(Aoharu3) / sizeof(Aoharu3[0]))
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 8:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Aoharu2[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == sizeof(Aoharu2) / sizeof(Aoharu2[0]))
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 9:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Aoharu4[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == sizeof(Aoharu4) / sizeof(Aoharu4[0]))
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 10:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Aoharu2[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == sizeof(Aoharu2) / sizeof(Aoharu2[0]))
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 11:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Aoharu3[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == sizeof(Aoharu3) / sizeof(Aoharu3[0]))
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 12:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Aoharu2[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == sizeof(Aoharu2) / sizeof(Aoharu2[0]))
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 13:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Aoharu2[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == 8)
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
        if (mdycount == 0 && !count)
        {
            settone(Melody[Aoharu5[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == sizeof(Aoharu5) / sizeof(Aoharu5[0]))
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 18:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Aoharu5[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == 9)
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 19:
    case 20:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Aoharu1[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == sizeof(Aoharu1) / sizeof(Aoharu1[0]))
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 21:
        if (mdycount == 0 && !count)
        {
            settone(Melody[MT], 0, 2400);
            mseq = 0;
            msel = 0;

        }
        break;

    }
}

//Endless Carnival 멜로디 map
void ECarnival_seq()
{
    switch (msel)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Ecarnival1[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == 16)
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Ecarnival2[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == 6)
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
    case 18:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Ecarnival3[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == 6)
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 19:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Ecarnival4[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == 4)
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Ecarnival2[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == 6)
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Ecarnival3[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == 6)
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 30:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Ecarnival5[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == sizeof(Ecarnival5) / sizeof(Ecarnival5[0]))
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 31:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Ecarnival6[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == sizeof(Ecarnival6) / sizeof(Ecarnival6[0]))
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 32:
    case 33:
    case 34:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Ecarnival3[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == sizeof(Ecarnival3) / sizeof(Ecarnival3[0]))
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 35:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Ecarnival5[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == 4)
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
        if (mdycount == 0 && !count)
        {
            settone(Melody[Ecarnival2[mseq]], DEF_VOL, 200);
            mseq++;
            if (mseq == 6)
            {
                mseq = 0;
                msel++;
            }
        }
        break;
    case 41:
        if (mdycount == 0 && !count)
        {
            settone(Melody[MA6], DEF_VOL, 200);
            msel++;
        }
        break;
    case 42:
        if (mdycount == 0 && !count)
        {
            settone(Melody[MA6], DEF_VOL, 1600);
            msel++;
        }
        break;
    case 43:
        if (mdycount == 0 && !count)
        {
            settone(Melody[MG6], DEF_VOL, 1600);
            msel++;
        }
        break;
    case 44:
        if (mdycount == 0 && !count)
        {
            settone(Melody[MG7], DEF_VOL, 1600);
            msel = 0;
            mseq=0;
        }
        break;

    }
}

int main()
{
	SystemInit();
	usb_setup();
	GPIO_port_enable(GPIO_port_A);
	GPIO_port_enable(GPIO_port_C);
	GPIO_port_enable(GPIO_port_D);
    GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_C, 1), GPIO_pinMode_O_pushPull, GPIO_Speed_50MHz); //C1핀 OUTPUT(스피커)
    GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 2), GPIO_pinMode_O_pushPull, GPIO_Speed_50MHz); //C7핀 OUTPUT(LED_WHITE)
    GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 6), GPIO_pinMode_O_pushPull, GPIO_Speed_50MHz); //D6핀 OUTPUT(LED_BLUE)
    GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_C, 7), GPIO_pinMode_O_pushPull, GPIO_Speed_50MHz); //C7핀 OUTPUT(LED_GREEN)
    GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_A, 1), GPIO_pinMode_I_pullUp, GPIO_Speed_In); //A1핀 INPUT(버튼0), 내장풀업 사용
    GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_C, 2), GPIO_pinMode_I_pullUp, GPIO_Speed_In); //c2핀 INPUT(버튼1), 내장풀업 사용
    GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_C, 3), GPIO_pinMode_I_pullUp, GPIO_Speed_In); //c3핀 INPUT(버튼2), 내장풀업 사용
    
    Delay_Ms(10);

    t2pwm_init(); //PWM(볼륨 셋업)
    //systick_init(); //Tick 인터럽트 셋업

    Delay_Ms(100);
    
	while(1)
	{

        //tone 종류 선택(1=Endless Carnival, 0=Aoharu)
        if (is0Ao1EC)
        {
            if (istoneon) ECarnival_seq();
            GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_D, 6), high);
            GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_D, 2), low);
        }
        else
        {
            if (istoneon) Aoharu_seq();
            GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_D, 6), low);
            GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_D, 2), high);
        }

        //0번키 누르고 뗌(멜로디 재생/정지 전환 및 Systick/USB HID 인터럽트 전환)
        if (GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_A, 1)) && keytime0 < 250000 && kchat00==1 && kchat01==50)
        {
            istoneon = !istoneon;
            if(istoneon)
            {
                NVIC_DisableIRQ( EXTI7_0_IRQn );
                GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 5), GPIO_pinMode_I_floating, GPIO_Speed_In); 
                GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 4), GPIO_pinMode_I_floating, GPIO_Speed_In); 
                GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 3), GPIO_pinMode_I_floating, GPIO_Speed_In); 
                systick_init();
            }
            else
            {
                NVIC_DisableIRQ(SysTicK_IRQn);
                usb_setup();
            }
            kchat00=0;
            kchat01=0;
        }
        //0번키 2초 이상 누름, 멜로디 재생중일때는 볼륨조절, 정지 중일때는 멜로디곡 전환(및 아래 코드에서 Systick 인터럽트로 전환된것을 USB HID 인터럽트로 재전환)
        if (!GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_A, 1)) && keytime0 == 250000)
        {
            if (istoneon)
            {
                switch (DEF_VOL)
                {
                case 222: DEF_VOL = 250; break;
                case 250: DEF_VOL = 150; break;
                case 150: DEF_VOL = 180; break;
                case 180: DEF_VOL = 200; break;
                case 200: DEF_VOL = 222; break;
                }
            }
            else
            {
                is0Ao1EC = !is0Ao1EC;
                msel=toneseq=mdy=mdycount=count=mseq=0;
                Delay_Ms(500);
                NVIC_DisableIRQ(SysTicK_IRQn);
                usb_setup();
            }
            keytime0++;
        }

        //1번이나 2번키 눌렸을때 USB HID 인터럽트로 전환
        if((!GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_C, 2)) || !GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_C, 3))) && istoneon)
        {
            NVIC_DisableIRQ(SysTicK_IRQn);
            usb_setup();
            istoneon=0;
        }
        //0번 키 상승채터링 방지
		if(!GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_A, 1))) kchat00=300;
        if(kchat00>1 && GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_A, 1))) kchat00--;
        //0번 키 하강채터링 방지
        if(!GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_A, 1))) kchat01=kchat01<50?kchat01+1:kchat01;
        else if(kchat01<50) kchat01=0;
        if(keytime0 >= 250000) kchat01=0;

        //푸쉬버튼 스위치 0번 2초 이상 누르고 뗌(상승트리거), 상승채터링 및 키 눌림 지속시간 초기화
        if (GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_A, 1)) && keytime0 == 250001 && kchat00==1)
        {
            keytime0=0;
            kchat00=0;
        }
        //푸쉬버튼 스위치 0번이 멜로디 정지 상태에서 하강트리거일 때 Systick 인터럽트로 전환
        if (!GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_A, 1)) && !last_key0)
        {
            if(!istoneon)
            {
                NVIC_DisableIRQ( EXTI7_0_IRQn );
                GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 5), GPIO_pinMode_I_floating, GPIO_Speed_In); 
                GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 4), GPIO_pinMode_I_floating, GPIO_Speed_In); 
                GPIO_pinMode(GPIOv_from_PORT_PIN(GPIO_port_D, 3), GPIO_pinMode_I_floating, GPIO_Speed_In); 
                systick_init();
            }
        }
        //이전에 눌린 키 저장(0번)
        last_key0 = !GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_A, 1));
	}
}

//USB HID 인터럽트
void usb_handle_user_in_request( struct usb_endpoint * e, uint8_t * scratchpad, int endp, uint32_t sendtok, struct rv003usb_internal * ist )
{
	if( endp == 2 )
	{
		// Keyboard (8 bytes)

        usb_send_data(tsajoystick, 8, 0, sendtok); //USB HID 데이터 전송

        //1번키 하강트리거, 키보드 CTRL+V
        if (!GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_C, 2)) && !last_key1)
        {
            tsajoystick[0] = 0x01; //키보드 LEFT CTRL
            tsajoystick[4] = 4 + 'v' - 'a'; //키보드 v
            GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_D, 7), high); //녹색 LED 점등
        }
        //1번키 하강트리거, 키보드 CTRL+C
        else if (!GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_C, 3)) && !last_key2)
        {
            tsajoystick[0] = 0x01; //키보드 LEFT CTRL
            tsajoystick[4] = 4 + 'c' - 'a'; //키보드 c
            GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_D, 7), high); //녹색 LED 점등
        }
        else
        {
            if (GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_C, 2)) && GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_C, 3))) GPIO_digitalWrite(GPIOv_from_PORT_PIN(GPIO_port_D, 7), low); //녹색 LED 소등
            tsajoystick[4] = 0;
            tsajoystick[0] = 0;
        }

        //이전에 눌린 키 저장(1번, 2번)
        last_key1 = !GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_C, 2));
        last_key2 = !GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_C, 3));

        /*  //USB 클럭으로 버튼 누르는 시간 계산(멜로디 정지상태에서 길게눌렀을때 USB 연결해제 안하고 하도록 구현은 가능하나, USB Device로 작동 안하고 전원만 받는경우 동작불가) 
        if (!GPIO_digitalRead(GPIOv_from_PORT_PIN(GPIO_port_A, 1)))
        {
            keytime0 = keytime0 < 250000 ? keytime0 + 2000 : keytime0;
        }
        else
        {
            keytime0 = 0;
        } */

	}
	else
	{
		// endp가 그 외 제어용일 경우
		usb_send_empty( sendtok );
	}
}