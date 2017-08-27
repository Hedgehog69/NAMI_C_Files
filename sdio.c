#include <sdio.h>


uint32_t RCA = 0;

void vInitGPIO(void)
{  
  RCC->AHB1ENR|=RCC_AHB1ENR_GPIOCEN;
  RCC->AHB1ENR|=RCC_AHB1ENR_GPIODEN;


  GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_8|GPIO_OTYPER_OT_12);
  GPIOC->MODER  |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER12_1;
  GPIOC->PUPDR  |= ~(GPIO_PUPDR_PUPDR8|GPIO_PUPDR_PUPDR12);
  GPIOC->OSPEEDR|= (GPIO_OSPEEDER_OSPEEDR8|GPIO_OSPEEDER_OSPEEDR12);
  GPIOC->AFR[1] |= ((0x0C<<16) | 0x0C );

  GPIOD->OTYPER &= ~GPIO_OTYPER_OT_2;
  GPIOD->MODER  &= ~GPIO_MODER_MODER2;
  GPIOD->MODER  |= GPIO_MODER_MODER2_1;
  GPIOD->PUPDR  |= ~GPIO_PUPDR_PUPDR2;
  GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2;
  GPIOD->AFR[0] |= ((0x0C)<<8);  
}

void vInitSDIO(void)
{
  RCC->APB2ENR|=RCC_APB2ENR_SDIOEN;
  RCC->AHB1ENR|=RCC_AHB1ENR_DMA2EN;

  vInitGPIO();

  delay(5000);
  SDIO->CLKCR &=(~SDIO_CLKCR_WIDBUS);
  delay(5000);
  SDIO->CLKCR = (uint32_t)(SDIO_CLKCR_CLKEN|SDIO_CLKCR_CLKDIV|SDIO_CLKCR_HWFC_EN);
  SDIO->POWER|=0x03;
  delay(5000);
}

uint32_t uiSendCommand(uint8_t command, uint32_t argument, uint16_t wait_responce, uint8_t init)
{
  SDIO->CMD = 0;
  SDIO->ARG = 0;

  SDIO->ARG = argument;
  switch(wait_responce)
  {
    case(Resp_No):
    {
      SDIO->CMD |= (SDIO_CMD_CPSMEN|command);
      break;
    }
    case(Resp_Short):
    {
      SDIO->CMD |= (SDIO_CMD_WAITRESP_0|SDIO_CMD_CPSMEN|command);
      break;
    }
    case(Resp_Long):
    {
      wait_responce = SDIO_CMD_WAITRESP_0|SDIO_CMD_WAITRESP_1;
      SDIO->CMD |= (wait_responce|SDIO_CMD_CPSMEN|command);
      break;		
    }
    default:
      break;

  }
  if(init==SET)
    delay(20000);
  else
    delay(2000);

  return SDIO->RESP1;
}

void vInitSDMMC_Disk(void)
{
  uint32_t responce = 0;
  uiSendCommand(MMC_RESET,0x0,Resp_No,SET);
  uiSendCommand(MMC_SEND_IF_COND,0x000001AA,Resp_Short,SET);	
  do
  {	
    uiSendCommand(MMC_APP_CMD,0x0,Resp_Short,SET);
    responce=uiSendCommand(MMC_ACMD_SD_SEND_OP_COND,(uint32_t)0x80100000 | (uint32_t)(0x40000000 | 0x51000000),Resp_Short,SET);	
    SDIO->ICR=0xFFFFFFFF;
  }
  while((responce>>31)==0);

  uiSendCommand(MMC_ALL_SEND_CID,0x0,Resp_Long,SET);
  uint32_t rca = uiSendCommand(MMC_SEND_RELATIVE_ADDR,0x0,Resp_Short,SET);
  RCA = (rca>>16);

  uiSendCommand(SELECT_CARD,RCA<<16,Resp_Short,SET);//1-bit wide bus only!!!!
  
  SDIO->CLKCR |=(SDIO_CLKCR_HWFC_EN);
  
  SDIO->CLKCR &=(~SDIO_CLKCR_CLKDIV);		
}

FlagStatus xGetFlagStatus(uint32_t SDIO_FLAG)
{
  if ((SDIO->STA & SDIO_FLAG) != (uint32_t)RESET)
  {
    return SET;
  }
  else
  {
    return RESET;
  }
}

void vStartDMARX(uint8_t *BufferDST, uint32_t BufferSize);

void vRead_MMC_Disk(uint8_t sector_data[],uint32_t sector,uint16_t count)
{
    SDIO->DTIMER = 0x005B8D80;
    SDIO->DLEN = 0x200*count;

    uiSendCommand(READ_MULTIPLE_BLOCK,sector,Resp_Short,RESET);

    SDIO->DCTRL |= ((MMC_DATA_BLOCK_SIZE_512)|(SDIO_DCTRL_DTEN|SDIO_DCTRL_DTDIR)|SDIO_DCTRL_RWSTART);

    vStartDMARX(sector_data, 0x200 * count);

    SDIO->DCTRL &= ~SDIO_DCTRL_RWSTART;
    SDIO->DCTRL |= (SDIO_DCTRL_RWSTOP|SDIO_DCTRL_DMAEN);

    while(xGetFlagStatus(SDIO_STA_DATAEND|SDIO_STA_DCRCFAIL|SDIO_STA_DTIMEOUT|SDIO_STA_RXOVERR)!=SET);

    uiSendCommand(STOP_TRANSMISSION,0,Resp_Short,RESET);

    SDIO->DCTRL=0x0;
    SDIO->ICR=0xFFFFFFFF;  
}

void vStartDMARX(uint8_t *BufferDST, uint32_t BufferSize)
{
	
    DMA2->HIFCR=0xFFFFFFFF;
    DMA2->LIFCR=0xFFFFFFFF;

    DMA2_Stream3->CR &= 0x0;
    DMA2_Stream3->FCR &= 0x0;

    DMA2_Stream3->CR |=	DMA_SxCR_CHSEL_2;
    DMA2_Stream3->CR |=	DMA_SxCR_MBURST_0;
    DMA2_Stream3->CR |=	DMA_SxCR_PBURST_0;
    DMA2_Stream3->CR &= ~DMA_SxCR_DIR;
    DMA2_Stream3->CR &=	~DMA_SxCR_PINC;
    DMA2_Stream3->CR |=	DMA_SxCR_MINC;
    DMA2_Stream3->CR |=	DMA_SxCR_PL;
    DMA2_Stream3->CR |=	DMA_SxCR_MSIZE_1;
    DMA2_Stream3->CR |=	DMA_SxCR_PSIZE_1;
    DMA2_Stream3->CR &=	~DMA_SxCR_CIRC;

    DMA2_Stream3->FCR |= DMA_SxFCR_DMDIS;
    DMA2_Stream3->FCR |= DMA_SxFCR_FTH;

    DMA2_Stream3->NDTR = BufferSize;
    DMA2_Stream3->PAR = SDIO_FIFO_ADDRESS;
    DMA2_Stream3->M0AR = (uint32_t)BufferDST;
    DMA2_Stream3->CR |= (DMA_SxCR_EN|DMA_SxCR_PFCTRL);
}


