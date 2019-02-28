#include "stm32f0xx.h"        // Device header

#define T191_Addr					0x78
#define LCD_ON()					GPIOA->BSRR |=1<<1
#define LCD_RES()					GPIOA->BRR  |=1<<1

#define CS_LO()						GPIOA->BRR 	|=1<<4
#define CS_HI()						GPIOA->BSRR |=1<<4
#define MISO							1<<6				//Pa6

#define RX_TX_MODE()			(GPIOA->IDR & 1<<2) //Pa2 - 0 - Rx mode/ 1 - Tx mode

//---------------------CC1101 - Defines----------------------------
#define 	WRITE_SINGLE     				0x00
#define 	WRITE_BURST     				0x40		
#define 	READ_SINGLE     				0x80		
#define 	READ_BURST      				0xC0

#define 	BYTES_IN_RXFIFO     		0x7F 		
#define 	CRC_OK              		0x80 		

#define		TX_FIFO_SINGL 					0x3F													// Single byte access to TX FIFO 
#define		TX_FIFO_BURST						0x7F 													// Burst access to TX FIFO 
#define		RX_FIFO_SINGL						0xBF 													// Single byte access to RX FIFO 
#define		RX_FIFO_BURST	 					0xFF 													// Burst access to RX FIFO 

// ------------------------CC1100  CONTROL--------------------------------
#define IOCFG2       	0x00        	// GDO2 output pin configuration 
#define IOCFG1       	0x01        	// GDO1 output pin configuration 
#define IOCFG0       	0x02        	// GDO0 output pin configuration 
#define FIFOTHR     	0x03       		// RX FIFO and TX FIFO thresholds
#define SYNC1        	0x04        	// Sync word, high INT8U						
#define SYNC0        	0x05        	// Sync word, low INT8U						
#define PKTLEN       	0x06        	// Packet length								
#define PKTCTRL1     	0x07       		// Packet automation control						
#define PKTCTRL0    	0x08       		// Packet automation control						
#define ADDR         	0x09        	// Device address								
#define CHANNR       	0x0A        	// Channel number								
#define FSCTRL1      	0x0B        	// Frequency synthesizer control						
#define FSCTRL0      	0x0C        	// Frequency synthesizer control					
#define FREQ2        	0x0D        	// Frequency control word, high INT8U				
#define FREQ1        	0x0E        	// Frequency control word, middle INT8U				
#define FREQ0        	0x0F        	// Frequency control word, low INT8U				
#define MDMCFG4      	0x10          // Modem configuration								
#define MDMCFG3      	0x11        	// Modem configuration							
#define MDMCFG2      	0x12        	// Modem configuration							
#define MDMCFG1      	0x13        	// Modem configuration							
#define MDMCFG0      	0x14        	// Modem configuration							
#define DEVIATN      	0x15        	// Modem deviation setting						
#define MCSM2        	0x16        	// Main Radio Control State Machine configuration	
#define MCSM1        	0x17        	// Main Radio Control State Machine configuration	
#define MCSM0        	0x18        	// Main Radio Control State Machine configuration	
#define FOCCFG       	0x19        	// Frequency Offset Compensation configuration		
#define BSCFG        	0x1A        	// Bit Synchronization configuration				
#define AGCCTRL2     	0x1B        	// AGC control								
#define AGCCTRL1     	0x1C        	// AGC control								
#define AGCCTRL0     	0x1D        	// AGC control								
#define WOREVT1      	0x1E        	// High INT8U Event 0 timeout					
#define WOREVT0      	0x1F        	// Low INT8U Event 0 timeout					
#define WORCTRL      	0x20        	// Wake On Radio control						
#define FREND1       	0x21        	// Front end RX configuration					
#define FREND0       	0x22       		// Front end TX configuration					
#define FSCAL3       	0x23        	// Frequency synthesizer calibration				
#define FSCAL2       	0x24        	// Frequency synthesizer calibration				
#define FSCAL1       	0x25        	// Frequency synthesizer calibration				
#define FSCAL0       	0x26        	// Frequency synthesizer calibration				
#define RCCTRL1      	0x27        	// RC oscillator configuration					
#define RCCTRL0      	0x28        	// RC oscillator configuration					
#define FSTEST       	0x29        	// Frequency synthesizer calibration control		
#define PTEST        	0x2A        	// Production test								
#define AGCTEST      	0x2B        	// AGC test									
#define TEST2        	0x2C        	// Various test settings							
#define TEST1        	0x2D        	// Various test settings							
#define TEST0        	0x2E        	// Various test settings							

// ---------------STROBE commands--WriteOnly-----------------------------------
#define SRES         0x30					// Reset chip.										
#define SFSTXON      0x31					// 	Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).
#define SXOFF        0x32					// 	Turn off crystal oscillator.                                               
#define SCAL         0x33					// 	Calibrate frequency synthesizer and turn it off		(enables quick start).								
#define SRX          0x34					// 	Enable RX. Perform calibration first if coming from IDLE and MCSM0.FS_AUTOCAL=1.							
#define STX          0x35					// 	In IDLE state: Enable TX. Perform calibration first if MCSM0.FS_AUTOCAL=1. If in RX state and CCA is enabled:	Only go to TX if channel is clear.
#define SIDLE        0x36					//	Exit RX / TX, turn off frequency synthesizer and exit Wake-On-Radio mode if applicable.
#define SAFC         0x37        	//	Perform AFC adjustment of the frequency synthesizer	
#define SWOR         0x38        	//	Start automatic RX polling sequence (Wake-on-Radio)	
#define SPWD         0x39        	//	Enter power down mode when CSn goes high.			
#define SFRX         0x3A        	//	Flush the RX FIFO buffer.							
#define SFTX         0x3B        	//	Flush the TX FIFO buffer.							
#define SWORRST      0x3C        	//	Reset real time clock.								
#define SNOP         0x3D        	//	No operation. May be used to pad strobe commands to two	INT8Us for simpler software.							
//----------------------STATUS REGSITER - ReadOnly ------------------------------
#define PARTNUM      0xF0 				//0x30 
#define VERSION      0xF1					//0x31
#define FREQEST      0xF2					//0x32
#define LQI          0xF3					//0x33
#define RSSI         0xF4					//0x34
#define MARCSTATE    0xF5					//0x35
#define WORTIME1     0xF6					//0x36
#define WORTIME0     0xF7					//0x37
#define PKTSTATUS    0xF8					//0x38
#define VCO_VC_DAC   0xF9					//0x39

#define TXBYTES      0xFA					//0x3A
#define RXBYTES      0xFB					//0x3B
#define RCCCTRLSTAT1 0xFC
#define RCCCTRLSTAT0 0xFD

#define PATABLE      0xFE
#define RX_TX_FIFO   0xFF

#define RSSIOFFSET 		74  // for CC1100 on Freq = 868/ 1.2 kBoud

#define SPI1_DR_8bit 			*((__IO uint8_t *)&SPI1->DR)		// Limit for spi bus 8 bit

uint8_t i,flag=0,RX_BUF[32],Rx_Flag=0,rx_bytes;
uint8_t rssi_lev,stat,marc,lqi,sec_tic=0,act_count=0,crc_bit;

uint8_t TX_BUF[32] = {'H','y','g','r','o','=',' ','f','f',' ',' ',' ',' ',' '};

uint16_t TimingDelay,led_count,tsc_timer,ms1000;
uint32_t scan_tsc;
void TimingDelayDec(void) 																													{
 if (TimingDelay			!=0x00) TimingDelay--;
 if (!led_count) {led_count=500; GPIOB->ODR ^=1;}
 if (!ms1000) {ms1000=1000;sec_tic=1;}
 if (!tsc_timer) {tsc_timer=50;TSC->CR |=TSC_CR_START;}
 led_count--; tsc_timer--;ms1000--;
 }

void TIM17_IRQHandler(void)																													{
		if (TIM17->SR & TIM_SR_UIF) {
					TimingDelayDec();
  				TIM17->SR &=(~TIM_SR_UIF);
		}
}	
void TSC_IRQHandler(void)																														{// TSC - Interrupt

	if (TSC->ISR & TSC_ISR_EOAF)
	{
		TSC->ICR |= TSC_ICR_EOAIC;  //Reset end of acquisition 
		scan_tsc = TSC->IOGXCR[4];
	}
	
	if (TSC->ISR & TSC_ISR_MCEF)
	{
		TSC->ISR |= TSC_ICR_MCEIC;  //Reset Max count flag
		TSC->ICR |= TSC_ICR_EOAIC;  //Reset end of acquisition 
	}
}
void delay_ms (uint16_t DelTime) 																										{
    TimingDelay=DelTime;
  while(TimingDelay!= 0x00);
}


//-----------------------------CC1101----------------------------------------
static const uint8_t CC1101_config[] = 																							{//867.999 / GFSK / 2.4kBoud / Dev=5.2kHz
IOCFG0,      0x06,
FIFOTHR,     0x47,
PKTCTRL0,    0x05,
FSCTRL1,     0x06,
FREQ2,       0x21,
FREQ1,       0x62,
FREQ0,       0x76,
MDMCFG4,     0xF6,
MDMCFG3,     0x83,
MDMCFG2,     0x03,
DEVIATN,     0x15,
MCSM0,       0x18,
FOCCFG,      0x16,
WORCTRL,     0xFB,
FSCAL3,      0xE9,
FSCAL2,      0x2A,
FSCAL1,      0x00,
FSCAL0,      0x1F,
TEST2,       0x81,
TEST1,       0x35,
TEST0,       0x09
};

uint8_t CC1101_Strobe(uint8_t strobe) 																							{// Strobe command (Write only)
 uint8_t temp;
	CS_LO();
while (GPIOA->IDR & MISO){};							//waiting until CC1101 ready
while (!(SPI1->SR & SPI_SR_TXE)){}; SPI1_DR_8bit = strobe;
while ((SPI1->SR & SPI_SR_BSY)){};	
while (!(SPI1->SR & SPI_SR_RXNE)){}; temp = SPI1->DR;
//while (SPI1->SR & SPI_SR_BSY){};
CS_HI();
return temp;
}
uint8_t CC1101_WriteSingle(uint8_t command,uint8_t value) 													{//WriteSingle

	uint8_t temp;
CS_LO();
while (GPIOA->IDR & MISO){};							//waiting until CC1101 ready

while (!(SPI1->SR & SPI_SR_TXE)){};  SPI1_DR_8bit = command;
while (SPI1->SR & SPI_SR_BSY){};
while (!(SPI1->SR & SPI_SR_RXNE)){}; temp=SPI1_DR_8bit;
	
while (!(SPI1->SR & SPI_SR_TXE)){};	 SPI1_DR_8bit = value;
while (SPI1->SR & SPI_SR_BSY){};
while (!(SPI1->SR & SPI_SR_RXNE)){}; temp=SPI1_DR_8bit;	

CS_HI();
return temp;	
}

uint8_t CC1101_ReadSingle(uint8_t command) 																					{//ReadSingle

	uint8_t temp;
CS_LO();
while (GPIOA->IDR & MISO){};							//waiting until CC1101 ready
	
while (!(SPI1->SR & SPI_SR_TXE)){}; SPI1_DR_8bit = (command | READ_SINGLE);
while (SPI1->SR & SPI_SR_BSY){};
while (!(SPI1->SR & SPI_SR_RXNE)){}; command = SPI1_DR_8bit;
while (!(SPI1->SR & SPI_SR_TXE)){};	 SPI1_DR_8bit = 0x00;
while (SPI1->SR & SPI_SR_BSY){};
while (!(SPI1->SR & SPI_SR_RXNE)){};	temp = SPI1_DR_8bit;

CS_HI();	
return temp;	
}
void CC1101_WriteBurst( uint8_t addr, uint8_t *buff, uint8_t size )									{//WriteBurst

	uint8_t j_,temp;
CS_LO();
while (GPIOA->IDR & MISO){};							//waiting until CC1101 ready
while (!(SPI1->SR & SPI_SR_TXE)){};	SPI1_DR_8bit = (addr | WRITE_BURST);
while (SPI1->SR & SPI_SR_BSY){};		
while (!(SPI1->SR & SPI_SR_RXNE)){}; temp=SPI1_DR_8bit;	
for( j_ = 0; j_ < size; j_ ++ )
 {
  while (!(SPI1->SR & SPI_SR_TXE)){};	SPI1_DR_8bit = buff[j_]; 
	while (SPI1->SR & SPI_SR_BSY){};
	while (!(SPI1->SR & SPI_SR_RXNE)){};temp=SPI1_DR_8bit;	
 }
CS_HI();
}
void CC1101_ReadBurst( uint8_t cmd, uint8_t *buff, uint8_t size )										{//ReadBurst
 
	uint8_t j_,temp;
CS_LO();
while (GPIOA->IDR & MISO){};							//waiting until CC1101 ready
while (!(SPI1->SR & SPI_SR_TXE)){};	SPI1_DR_8bit = (cmd | READ_BURST);
while (SPI1->SR & SPI_SR_BSY){};
while (!(SPI1->SR & SPI_SR_RXNE)){}; temp=SPI1_DR_8bit;	
for( j_ = 0; j_ < size; j_ ++ )
 {
  while (!(SPI1->SR & SPI_SR_TXE)){};	SPI1_DR_8bit = 0x00; 
	while (SPI1->SR & SPI_SR_BSY){};
	while (!(SPI1->SR & SPI_SR_RXNE)){}; buff[j_] = SPI1_DR_8bit;		
 }
 CS_HI();
}
void CC1101_Init(void)																															{//CC1101_Init
uint8_t qnt,i_temp=0;
qnt=sizeof (CC1101_config);
CC1101_Strobe(SRES);	
	
while (i_temp < qnt)
{
	CC1101_WriteSingle(CC1101_config[i_temp],CC1101_config[i_temp+1]);
	i_temp+=2;	
}
CC1101_Strobe(SIDLE);
CC1101_Strobe(SFRX);
CC1101_Strobe(SFTX);
}

uint8_t RSSIDBM (uint8_t rssi_dec)																									{//RSSI
uint8_t rssi_dbm;
	if (rssi_dec >=128) {rssi_dbm=RSSIOFFSET +(256-rssi_dec)/2; }
	else 								{rssi_dbm=RSSIOFFSET -(rssi_dec/2);     }
return rssi_dbm;
}
//------------------------------------T191 LCD ------------------------------
const unsigned char T191_font[]= 																										{// Font 8*5 
	0x00, 0x00, 0x00, 0x00, 0x00 ,   // sp  32
     0x00, 0x00, 0x2f, 0x00, 0x00 ,   // !   33
     0x00, 0x07, 0x00, 0x07, 0x00 ,   // "   34
     0x14, 0x7f, 0x14, 0x7f, 0x14 ,   // #   35
     0x24, 0x2a, 0x7f, 0x2a, 0x12 ,   // $   36
     0xc4, 0xc8, 0x10, 0x26, 0x46 ,   // %   37
     0x36, 0x49, 0x55, 0x22, 0x50 ,   // &   38
     0x00, 0x05, 0x03, 0x00, 0x00 ,   // '   39
     0x00, 0x1c, 0x22, 0x41, 0x00 ,   // (   40
     0x00, 0x41, 0x22, 0x1c, 0x00 ,   // )   41
     0x14, 0x08, 0x3E, 0x08, 0x14 ,   // *   42
     0x08, 0x08, 0x3E, 0x08, 0x08 ,   // +   43
     0x00, 0x00, 0x50, 0x30, 0x00 ,   // ,   44
     0x10, 0x10, 0x10, 0x10, 0x10 ,   // -   45
     0x00, 0x60, 0x60, 0x00, 0x00 ,   // .   46
     0x20, 0x10, 0x08, 0x04, 0x02 ,   // /   47
     0x3E, 0x51, 0x49, 0x45, 0x3E ,   // 0   48
     0x00, 0x42, 0x7F, 0x40, 0x00 ,   // 1   49
     0x42, 0x61, 0x51, 0x49, 0x46 ,   // 2   50
     0x21, 0x41, 0x45, 0x4B, 0x31 ,   // 3   51
     0x18, 0x14, 0x12, 0x7F, 0x10 ,   // 4   52
     0x27, 0x45, 0x45, 0x45, 0x39 ,   // 5   53
     0x3C, 0x4A, 0x49, 0x49, 0x30 ,   // 6   54
     0x01, 0x71, 0x09, 0x05, 0x03 ,   // 7   55
     0x36, 0x49, 0x49, 0x49, 0x36 ,   // 8   56
     0x06, 0x49, 0x49, 0x29, 0x1E ,   // 9   57
     0x00, 0x36, 0x36, 0x00, 0x00 ,   // :   58
     0x00, 0x56, 0x36, 0x00, 0x00 ,   // ;   59
     0x08, 0x14, 0x22, 0x41, 0x00 ,   // <   60
     0x14, 0x14, 0x14, 0x14, 0x14 ,   // =   61
     0x00, 0x41, 0x22, 0x14, 0x08 ,   // >   62
     0x02, 0x01, 0x51, 0x09, 0x06 ,   // ?   63
     0x32, 0x49, 0x59, 0x51, 0x3E ,   // @   64
     0x7E, 0x11, 0x11, 0x11, 0x7E ,   // A   65
     0x7F, 0x49, 0x49, 0x49, 0x36 ,   // B   66
     0x3E, 0x41, 0x41, 0x41, 0x22 ,   // C   67
     0x7F, 0x41, 0x41, 0x22, 0x1C ,   // D   68
     0x7F, 0x49, 0x49, 0x49, 0x41 ,   // E   69
     0x7F, 0x09, 0x09, 0x09, 0x01 ,   // F   70
     0x3E, 0x41, 0x49, 0x49, 0x7A ,   // G   71
     0x7F, 0x08, 0x08, 0x08, 0x7F ,   // H   72
     0x00, 0x41, 0x7F, 0x41, 0x00 ,   // I   73
     0x20, 0x40, 0x41, 0x3F, 0x01 ,   // J   74
     0x7F, 0x08, 0x14, 0x22, 0x41 ,   // K   75
     0x7F, 0x40, 0x40, 0x40, 0x40 ,   // L   76
     0x7F, 0x02, 0x0C, 0x02, 0x7F ,   // M   77
     0x7F, 0x04, 0x08, 0x10, 0x7F ,   // N   78
     0x3E, 0x41, 0x41, 0x41, 0x3E ,   // O   79
     0x7F, 0x09, 0x09, 0x09, 0x06 ,   // P   80
     0x3E, 0x41, 0x51, 0x21, 0x5E ,   // Q   81
     0x7F, 0x09, 0x19, 0x29, 0x46 ,   // R   82
     0x46, 0x49, 0x49, 0x49, 0x31 ,   // S   83
     0x01, 0x01, 0x7F, 0x01, 0x01 ,   // T   84
     0x3F, 0x40, 0x40, 0x40, 0x3F ,   // U   85
     0x1F, 0x20, 0x40, 0x20, 0x1F ,   // V   86
     0x3F, 0x40, 0x38, 0x40, 0x3F ,   // W   87
     0x63, 0x14, 0x08, 0x14, 0x63 ,   // X   88
     0x07, 0x08, 0x70, 0x08, 0x07 ,   // Y   89
     0x61, 0x51, 0x49, 0x45, 0x43 ,   // Z   90
     0x00, 0x7F, 0x41, 0x41, 0x00 ,   // [   91
     0x55, 0x2A, 0x55, 0x2A, 0x55 ,   // 55  92
     0x00, 0x41, 0x41, 0x7F, 0x00 ,   // ]   93
     0x04, 0x02, 0x01, 0x02, 0x04 ,   // ^   94
     0x40, 0x40, 0x40, 0x40, 0x40 ,   // _   95
     0x00, 0x01, 0x02, 0x04, 0x00 ,   // '   96
     0x20, 0x54, 0x54, 0x54, 0x78 ,   // a   97
     0x7F, 0x48, 0x44, 0x44, 0x38 ,   // b   98
     0x38, 0x44, 0x44, 0x44, 0x20 ,   // c   99
     0x38, 0x44, 0x44, 0x48, 0x7F ,   // d   100
     0x38, 0x54, 0x54, 0x54, 0x18 ,   // e   101
     0x08, 0x7E, 0x09, 0x01, 0x02 ,   // f   102
     0x0C, 0x52, 0x52, 0x52, 0x3E ,   // g   103
     0x7F, 0x08, 0x04, 0x04, 0x78 ,   // h   104
     0x00, 0x44, 0x7D, 0x40, 0x00 ,   // i   105
     0x20, 0x40, 0x44, 0x3D, 0x00 ,   // j   106
     0x7F, 0x10, 0x28, 0x44, 0x00 ,   // k   107
     0x00, 0x41, 0x7F, 0x40, 0x00 ,   // l   108
     0x7C, 0x04, 0x18, 0x04, 0x78 ,   // m   109
     0x7C, 0x08, 0x04, 0x04, 0x78 ,   // n   110
     0x38, 0x44, 0x44, 0x44, 0x38 ,   // o   111
     0x7C, 0x14, 0x14, 0x14, 0x08 ,   // p   112
     0x08, 0x14, 0x14, 0x18, 0x7C ,   // q   113
     0x7C, 0x08, 0x04, 0x04, 0x08 ,   // r   114
     0x48, 0x54, 0x54, 0x54, 0x20 ,   // s   115
     0x04, 0x3F, 0x44, 0x40, 0x20 ,   // t   116
     0x3C, 0x40, 0x40, 0x20, 0x7C ,   // u   117
     0x1C, 0x20, 0x40, 0x20, 0x1C ,   // v   118
     0x3C, 0x40, 0x30, 0x40, 0x3C ,   // w   119
     0x44, 0x28, 0x10, 0x28, 0x44 ,   // x   120
     0x0C, 0x50, 0x50, 0x50, 0x3C ,   // y   121
     0x44, 0x64, 0x54, 0x4C, 0x44 ,   // z   122
    //
     0x00, 0x08, 0x36, 0x41, 0x00 ,   //7B- {
     0x00, 0x00, 0x7f, 0x00, 0x00 ,   //7C- |
     0x00, 0x41, 0x36, 0x08, 0x00 ,   //7D- }
     0x04, 0x02, 0x04, 0x08, 0x04 ,   //7E- ~
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //7F- 
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //80- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //81- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //82- ‚
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //83- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //84- „
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //85- …
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //86- †
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //87- ‡
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //88- €
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //89- ‰
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //8A- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //8B- ‹
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //8C- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //8D- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //8E- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //8F- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //90- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //91- ‘
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //92- ’
     0x00, 0x00, 0x00, 0x00, 0x00 ,   // 93- “
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //94- ”
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //95- •
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //96- –
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //97- —
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //98- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //99- ™ 
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //9A- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //9B- ›
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //9C- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //9D- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //9E- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //9F- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //A0-  
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //A1- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //A2- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //A3- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //A4- ¤
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //A5- ?
     0x00, 0x00, 0x36, 0x00, 0x00 ,   //A6- ¦
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //A7- §
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //A8- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //A9- ©
     0x3E, 0x49, 0x49, 0x49, 0x22 ,   //AA- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //AB- «
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //AC- ¬
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //AD- ­
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //AE- ®
     0x44, 0x45, 0x7C, 0x45, 0x44 ,   //AF- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //B0- °
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //B1- ±
     0x00, 0x41, 0x7F, 0x41, 0x00 ,   //B2- ?
     0x00, 0x44, 0x7D, 0x40, 0x00 ,   //B3- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //B4- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //B5- µ
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //B6- ¶
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //B7- ·
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //B8- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //B9- ?
     0x38, 0x54, 0x44, 0x28, 0x00 ,   //BA- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //BB- »
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //BC- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //BD- ?
     0x00, 0x00, 0x00, 0x00, 0x00 ,   //BE- ?
     0x4A, 0x48, 0x7A, 0x40, 0x40 ,   //BF- ?                             
    //???????
 0x7E, 0x11, 0x11, 0x11, 0x7E , // A        /*192*/
 0x7F, 0x49, 0x49, 0x49, 0x31 , // ?
 0x7F, 0x49, 0x49, 0x49, 0x36 , // B
 0x7F, 0x01, 0x01, 0x01, 0x03 , // ?
 0x60, 0x3E, 0x21, 0x21, 0x7F , // ?
 0x7F, 0x49, 0x49, 0x49, 0x41 , // E
 0x63, 0x14, 0x7F, 0x14, 0x63 , // ?
 0x22, 0x49, 0x49, 0x49, 0x36 , // ?
 0x7F, 0x10, 0x08, 0x04, 0x7F , // ?
 0x7F, 0x10, 0x09, 0x04, 0x7F , // ?
 0x7F, 0x08, 0x14, 0x22, 0x41 , // K
 0x7C, 0x02, 0x01, 0x01, 0x7F , // ?
 0x7F, 0x02, 0x0C, 0x02, 0x7F , // M
 0x7F, 0x08, 0x08, 0x08, 0x7F , // H
 0x3E, 0x41, 0x41, 0x41, 0x3E , // O
 0x7F, 0x01, 0x01, 0x01, 0x7F , // ?
 0x7F, 0x09, 0x09, 0x09, 0x06 , // P
 0x3E, 0x41, 0x41, 0x41, 0x22 , // C
 0x01, 0x01, 0x7F, 0x01, 0x01 , // T
 0x07, 0x48, 0x48, 0x48, 0x3F , // ?
 0x1C, 0x22, 0x7F, 0x22, 0x1C , // ?
 0x63, 0x14, 0x08, 0x14, 0x63 , // X
 0x3F, 0x20, 0x20, 0x3F, 0x60 , // ?
 0x07, 0x08, 0x08, 0x08, 0x7F , // ?
 0x7F, 0x40, 0x7F, 0x40, 0x7F , // ?
 0x3F, 0x20, 0x3F, 0x20, 0x7F , // ?
 0x01, 0x7F, 0x48, 0x48, 0x30 , // ?
 0x7F, 0x48, 0x48, 0x30, 0x7F , // ?
 0x7F, 0x48, 0x48, 0x48, 0x30 , // ?
 0x22, 0x49, 0x49, 0x49, 0x3E , // ?
 0x7F, 0x08, 0x3E, 0x41, 0x3E , // ?
 0x46, 0x29, 0x19, 0x09, 0x7F , // ?
 0x20, 0x54, 0x54, 0x54, 0x78 , // a
 0x78, 0x54, 0x54, 0x54, 0x20 , // b
 0x7C, 0x54, 0x54, 0x54, 0x28 , // ?
 0x7C, 0x04, 0x04, 0x04, 0x00 , // ?
 0x60, 0x38, 0x24, 0x38, 0x60 , // ?
 0x38, 0x54, 0x54, 0x54, 0x18 , // e
 0x6C, 0x10, 0x7C, 0x10, 0x6C , // ?
 0x28, 0x44, 0x54, 0x54, 0x28 , // ?
 0x3C, 0x40, 0x40, 0x20, 0x7C , // ?
 0x3C, 0x40, 0x42, 0x20, 0x7C , // ?
 0x7C, 0x10, 0x10, 0x28, 0x44 , // ?
 0x60, 0x10, 0x08, 0x04, 0x7C , // ?
 0x7C, 0x08, 0x10, 0x08, 0x7C , // ?
 0x7C, 0x10, 0x10, 0x10, 0x7C , // ?
 0x38, 0x44, 0x44, 0x44, 0x38 , // o
 0x7C, 0x04, 0x04, 0x04, 0x7C , // ?
 0x7C, 0x14, 0x14, 0x14, 0x08 , // p
 0x38, 0x44, 0x44, 0x44, 0x20 , // c
 0x04, 0x04, 0x7C, 0x04, 0x04 , // ?
 0x0C, 0x50, 0x50, 0x50, 0x3C , // y
 0x18, 0x24, 0x7C, 0x24, 0x18 , // ?
 0x44, 0x28, 0x10, 0x28, 0x44 , // x
 0x3C, 0x20, 0x20, 0x3C, 0x60 , // ?
 0x0C, 0x10, 0x10, 0x10, 0x7C , // ?
 0x7C, 0x40, 0x7C, 0x40, 0x7C , // ?
 0x3C, 0x20, 0x3C, 0x20, 0x7C , // ?
 0x04, 0x7C, 0x48, 0x48, 0x30 , // ?
 0x7C, 0x48, 0x30, 0x00, 0x7C , // ?
 0x7C, 0x48, 0x48, 0x48, 0x30 , // ?
 0x28, 0x44, 0x54, 0x54, 0x38 , // ?
 0x7C, 0x10, 0x38, 0x44, 0x38 , // ?
 0x58, 0x24, 0x24, 0x24, 0x7C  //  ?
 };


void I2C_write(unsigned char byte_send)																			    		{//WRITE
	 while(!(I2C1->ISR & I2C_ISR_TXE)){}; 
	 I2C1->TXDR = byte_send;
	}
void I2C_Start(void)    																														{//Start
	 while(I2C1->ISR & I2C_ISR_BUSY){}; //I2C_ISR_BUSY
	 I2C1->CR2 |= (T191_Addr << I2C_CR2_SADD_Pos) | (0xff << I2C_CR2_NBYTES_Pos) | I2C_CR2_START;
	 while(!(I2C1->ISR & I2C_ISR_TXE)){}; // TX buf Empty
	}
void I2C_Stop(void) 																																{//Stop
	 while(!(I2C1->ISR & I2C_ISR_TXE)){}; // TX buf Empty
	 I2C1->CR2 |= I2C_CR2_STOP;
	 while(!(I2C1->ISR & I2C_ISR_STOPF)){};
	 I2C1->ICR |= I2C_ICR_STOPCF;		
 }
void LCD_Init(void) 																																{// INIT LCD T191
    
     LCD_RES();delay_ms(10);
  	 LCD_ON();delay_ms(10);

     I2C_Start();
     I2C_write(0x00);I2C_write(0x31);
     I2C_write(0x14);I2C_write(0x06); 																			// Initial Settings
     I2C_write(0x30);I2C_write(0x11);
     I2C_write(0x05);I2C_write(0x31);
     I2C_write(0x9A); 																											// Contrast
     I2C_write(0x0C); 																											//0x0C -Normal mode, 00 - Mirror mode
     I2C_write(0x30);I2C_write(0x0C); 																			// ?????????? ?????
     I2C_write(0x40);I2C_write(0x80); 																			// GoTo x=0, y=0
     I2C_Stop();
    }
void LCD_Gotoxy ( uint8_t x, uint8_t y )														    						{//78.00.30.80|x.40|y
       I2C_Start();
       I2C_write(0x00);
       I2C_write(0x30);
       I2C_write(0x80 | 1+x*6);  																//(0...15 Colls
       I2C_write(0x40 | y);  																		//(0...7 - Rows)
       I2C_Stop();
    }

void LCD_Write_data (unsigned char dat_byte)																	      {//78.40.XX
       I2C_Start();
       I2C_write(0x40);
       I2C_write(dat_byte);
       I2C_Stop();
      }

void LCD_mode(uint8_t mode)																													{//78.00.30.0C - Normal; 78.00.30.0D - Inverse 
      
       I2C_Start();
       if(mode > 0)
       {I2C_write(0x00);I2C_write(0x30);I2C_write(0x0C);}
       else
       {I2C_write(0x00);I2C_write(0x30);I2C_write(0x0D);}
       I2C_Stop();
      }
void LCD_Char(char flash_o, uint8_t mode)															        			{// Print Symbol
      	 unsigned char i,ch; 
         I2C_Start();
         I2C_write(0x40);
          for(i=0; i<5; i++)
      	  {
      	    ch = T191_font[(flash_o - 32)*5+i];
      	     if(mode) { I2C_write(~ch); }
      			 else  	 { I2C_write(ch);}
      		 if(i == 4)
      			  {
      			     if(mode) { I2C_write(0xff); }
      				  else   {	 I2C_write(0x00); }
      			  }
      	   }
         I2C_Stop();
          }
void LCD_PrintStr(char *s, uint8_t mode)																	    			{// Print String
          while (*s)
        {
          LCD_Char(*s, mode);
          s++;
        }
      }
void LCD_Clear(void)      																													{// Clear LCD
    	 unsigned char i;
    	for(i = 0; i < 10; i++) LCD_PrintStr("                ",0);
      }
       
void LCD_ClearStr(uint8_t y,uint8_t qn)      																									{// Clear String
 uint8_t temp_i; 	 
	for (temp_i=0;temp_i<qn;temp_i++)
	 {
		 LCD_Gotoxy(0,y+temp_i);    	
		 LCD_PrintStr("                ",0);
   }
}
void LCD_PrintDec(long value,uint8_t mode) 																					{// Print Dec
	
	char i=1,d=0;
	unsigned char text[10];
	do 
  { 
    if (value >=10)  {
				d = value % 10; 																				// ??????? ?? ???????
				text[i] = d + '0'; 																			// ????? ASCII ?????? ? ???????? ? ?????
				value /= 10; 																						// "?????????? ????? ??????" -- ??????? ?? 10
			}
		else 
			{	text[i] = value + '0';
				value=0;
			}
 		i++;
  }
	while(value); 
	i--;			
	do
	{	LCD_Char(text[i], mode);
		i--;
	}
	while(i);
}
			
void LCD_PrintHex(long value,uint8_t mode) 																					{// Print Hex
	
	char i=1,d=0;
	unsigned char text[10];
	do 
  { 
    if (value >=0x10)  {
				d = value % 0x10; 																				
				if(d<0x0A) text[i] = d + '0'; 																			
				else 			 text[i] = d + 0x37;
				value /= 0x10; 																						
			}
		else 
			{	
				if(value < 0x0A)	text[i] = value + '0';			//0..9
				else 							text[i] = value + 0x37;			//A...F
				value=0;
			}
 		i++;
  }
	while(value); 
	i--;			
	do
	{	LCD_Char(text[i], mode);
		i--;
	}
	while(i);
}

void LCD_PrintBin(uint8_t value,uint8_t mode) 																			{// Print Bin
	
	uint8_t i=1,d=0;
	uint8_t text[8];
	do 
  { 
    if (value >=2)  {
				d = value % 2; 																				
				text[i] = d + '0'; 																			
				value /= 0x2; 																						
			}
		else 
			{	
				text[i] = value + '0';			//0..9
				value=0;
			}
 		i++;
  }
	while(i<9); 
	i--;			
	do
	{	LCD_Char(text[i], mode);
		i--;
	}
	while(i);
}
//---------------------------------End of LCD INIT----------------------------

void EXTI0_1_IRQHandler(void)																												{
  if(EXTI->PR & EXTI_PR_PIF0)
	{
		EXTI->PR |= EXTI_PR_PIF0;
		Rx_Flag=1;
	}
}


	void initial (void)																																{
//---------------TIM17------------------
  RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;    																			//HSI 8 MHz - 1 msek
  TIM17->PSC = 8000-1;
  TIM17->ARR = 1;
  TIM17->CR1 |= TIM_CR1_ARPE | TIM_CR1_DIR | TIM_CR1_CEN; 											// 
	TIM17->DIER |=TIM_DIER_UIE;
	NVIC_EnableIRQ (TIM17_IRQn);
	NVIC_SetPriority(TIM17_IRQn,0x05);	

//-------------------GPIOB-Blinking Led		
	RCC->AHBENR  |= RCC_AHBENR_GPIOBEN; 																					//
	GPIOB->MODER |= GPIO_MODER_MODER0_0;																					//Pb0-Out 
//-------------------GPIOA-CONFIG RX MODE----------
	RCC->AHBENR  |= RCC_AHBENR_GPIOAEN; 																					//
	GPIOA->MODER &=(~GPIO_MODER_MODER2);																					//LCD_Res_pin	
	GPIOA->PUPDR |=GPIO_PUPDR_PUPDR2_0;																						//Pull-Up
//------------I2C1---------------------	
	RCC->AHBENR 		|=RCC_AHBENR_GPIOFEN;
	GPIOF->MODER 		|=GPIO_MODER_MODER0_1 		| GPIO_MODER_MODER1_1; 							//Alt -mode /Pf0 - SDA, Pf1- SCL
	GPIOF->OSPEEDR 	|=GPIO_OSPEEDER_OSPEEDR0 	| GPIO_OSPEEDER_OSPEEDR1;						//50 MHz max speed
	GPIOF->AFR[0] 	|=(1<<GPIO_AFRL_AFRL0_Pos) |(1<<GPIO_AFRL_AFRL1_Pos);  				//I2C - Alternative

	GPIOA->MODER |= GPIO_MODER_MODER1_0;																					//LCD_Res_pin

	RCC->APB1ENR |=RCC_APB1ENR_I2C1EN;
	I2C1->TIMINGR |=(0x5<< I2C_TIMINGR_PRESC_Pos);						// If HSI = 8MHz,  Presc =0x05 - 380kHz Freq
	I2C1->CR2 &=(~I2C_CR2_HEAD10R) & (~I2C_CR2_ADD10);
	I2C1->CR1 |=I2C_CR1_PE;
	
//------------------------SPI-----------------------------------
	RCC->AHBENR 		|=RCC_AHBENR_GPIOAEN;
	GPIOA->OSPEEDR 	|=GPIO_OSPEEDER_OSPEEDR4 | GPIO_OSPEEDER_OSPEEDR5 | GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7;
	GPIOA->MODER 		|=GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1; 	//Pa4 - out,Pa5..7 - Alt_mode 
	GPIOA->AFR[0] 	|=(0<<GPIO_AFRL_AFRL7_Pos) |(0<<GPIO_AFRL_AFRL6_Pos) | (0<<GPIO_AFRL_AFRL5_Pos);  // SPI - Alternative
	GPIOA->MODER 		|=GPIO_MODER_MODER4_0;
	
	RCC->APB2ENR |=RCC_APB2ENR_SPI1EN;
	SPI1->CR1 |=SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR | (2<<SPI_CR1_BR_Pos);  // if HSI8 - SpiSpeed (BR=2) - 1MHz
	SPI1->CR2 |=SPI_CR2_FRXTH;
	SPI1->CR1 |=SPI_CR1_SPE;
	CS_HI();
	
//----------------------EXTI-----------------------------------
	RCC->APB2ENR |=RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[1] |= SYSCFG_EXTICR1_EXTI0_PA;
	EXTI->FTSR |= EXTI_FTSR_TR0; 
	
  NVIC_SetPriority(EXTI0_1_IRQn, 2); 
  NVIC_EnableIRQ(EXTI0_1_IRQn); 
	//EXTI->IMR |= EXTI_IMR_MR0;				//Cannot activate immediately, because need switch GD0 to Rx\Tx buf Interrupt mode
			
//------------------------TSC---------------------------------
	GPIOB->MODER  |= GPIO_MODER_MODER3_1 |GPIO_MODER_MODER4_1;										// Alt -mode 
	GPIOB->AFR[0]	|=(3<<GPIO_AFRL_AFRL3_Pos) |(3<<GPIO_AFRL_AFRL4_Pos);						// TSC
	
	RCC->AHBENR |=RCC_AHBENR_TSCEN;
	TSC->CR |= (0b10<<TSC_CR_CTPH_Pos) | (1<<TSC_CR_CTPL_Pos) | (0b11<<TSC_CR_PGPSC_Pos) | (0b110<<TSC_CR_MCV_Pos) | TSC_CR_TSCE;
	TSC->IER |=TSC_IER_MCEIE | TSC_IER_EOAIE;
	TSC->IOHCR &=(~TSC_IOHCR_G5_IO1) & (~TSC_IOHCR_G5_IO2);  //Hysteresis - off
	TSC->IOSCR |=TSC_IOSCR_G5_IO1;															// Condensor
	TSC->IOCCR |=TSC_IOSCR_G5_IO2; 															// Sensor
	TSC->IOGCSR |=TSC_IOGCSR_G5E;

	NVIC_EnableIRQ (TSC_IRQn);
	NVIC_SetPriority(TSC_IRQn,0x03);
	

__enable_irq ();	

// -------------Not used Pins - Pull Up---------------------------------------
GPIOA->PUPDR |=GPIO_PUPDR_PUPDR3_0 | GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR10_0;
GPIOB->PUPDR |=GPIO_PUPDR_PUPDR1_0 | GPIO_PUPDR_PUPDR5_0 | GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR7_0;
	
} 

int main(void)
{
initial();
CC1101_Init();	
EXTI->IMR |= EXTI_IMR_MR0;
	
if(!RX_TX_MODE()) {
			LCD_Init();
			LCD_Clear();
			LCD_Gotoxy (4,0);LCD_PrintStr(" CC1101 ",1);
			LCD_Gotoxy (0,1);LCD_PrintStr("Ver. ",0); 	LCD_PrintDec(CC1101_ReadSingle(VERSION),0);	
											 LCD_PrintStr(" Part. ",0); LCD_PrintDec(CC1101_ReadSingle(PARTNUM),0);
	
			CC1101_Strobe(SRX);
}
if(RX_TX_MODE()) {
	//CC1101_WriteSingle(PATABLE,0xC0);
}
	

//-----------------------------initial data----------------------------------

while (1)  /* Main loop */
{
	if(Rx_Flag)
{	
	rx_bytes=CC1101_ReadSingle(RXBYTES)&0x7F;
	stat=CC1101_ReadSingle(PKTSTATUS);
	CC1101_ReadBurst(RX_FIFO_BURST, RX_BUF,10);
 	sec_tic=1;
	act_count=0;
	CC1101_Strobe(SFRX);
	CC1101_Strobe(SRX);
}
else
{
	if (sec_tic) {act_count++;}

}



if (!RX_TX_MODE() && sec_tic){																	//RX Mode
	rssi_lev= RSSIDBM(CC1101_ReadSingle(RSSI));
	LCD_Gotoxy (0,3);	LCD_PrintStr("CRC= ",0); 		if (stat &0x80) {LCD_PrintStr(" Ok  ",0);}
																																else {LCD_PrintStr(" Bad ",0);}
	LCD_Gotoxy (0,4);	LCD_PrintStr("RSSI=-",0);		LCD_PrintDec(rssi_lev,0);LCD_PrintStr("  ",0);
		if (Rx_Flag) {
		LCD_Gotoxy (0,6);	LCD_PrintStr(">",0); LCD_PrintStr(RX_BUF,0);	
		LCD_Gotoxy (0,7);	LCD_PrintStr("Hyg= ",0);LCD_PrintDec((uint16_t)(RX_BUF[7]<<8|RX_BUF[8]),0);
		}
	sec_tic=0;Rx_Flag=0;
	
	if (act_count>1) {
		LCD_ClearStr(6,2);
		act_count=0;
		rx_bytes=0;
	}
}
if (RX_TX_MODE()) //&& sec_tic)	 																// TX - mode
{
delay_ms(1000);

TX_BUF[8]= (0xff & scan_tsc);	
TX_BUF[7]=(0xff & (scan_tsc>>8));

	
CC1101_Strobe(SIDLE);	
CC1101_WriteSingle(PATABLE,0xC0);
CC1101_WriteBurst(TX_FIFO_BURST, TX_BUF, 10);
CC1101_Strobe(STX);
sec_tic=0;
	

}	


} // end - main loop 
} // end - Main  
	
#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ while (1)  {  } }
#endif
