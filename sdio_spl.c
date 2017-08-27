#include <stm32f4xx_sdio.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_tim.h>
#include <stm32f4xx_dma.h>
#include <led.h>
#include <main.h>


void SD_LowLevel_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize)
{
  DMA_InitTypeDef SDDMA_InitStructure;

  DMA_ClearFlag(SD_SDIO_DMA_STREAM, SD_SDIO_DMA_FLAG_FEIF | SD_SDIO_DMA_FLAG_DMEIF | SD_SDIO_DMA_FLAG_TEIF | SD_SDIO_DMA_FLAG_HTIF | SD_SDIO_DMA_FLAG_TCIF);

  /* DMA2 Stream3  or Stream6 disable */
  DMA_Cmd(SD_SDIO_DMA_STREAM, DISABLE);

  /* DMA2 Stream3 or Stream6 Config */
  DMA_DeInit(SD_SDIO_DMA_STREAM);

  SDDMA_InitStructure.DMA_Channel = SD_SDIO_DMA_CHANNEL;
  SDDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS;
  SDDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)BufferDST;
  SDDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  SDDMA_InitStructure.DMA_BufferSize = 0;
  SDDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  SDDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  SDDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  SDDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  SDDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  SDDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  SDDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  SDDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  SDDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
  SDDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;
  DMA_Init(SD_SDIO_DMA_STREAM, &SDDMA_InitStructure);
  DMA_ITConfig(SD_SDIO_DMA_STREAM, DMA_IT_TC, ENABLE);
  DMA_FlowControllerConfig(SD_SDIO_DMA_STREAM, DMA_FlowCtrl_Peripheral);

  /* DMA2 Stream3 or Stream6 enable */
  DMA_Cmd(SD_SDIO_DMA_STREAM, ENABLE);
}


void SD_LowLevel_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize)
{
  DMA_InitTypeDef SDDMA_InitStructure;

  DMA_ClearFlag(SD_SDIO_DMA_STREAM, SD_SDIO_DMA_FLAG_FEIF | SD_SDIO_DMA_FLAG_DMEIF | SD_SDIO_DMA_FLAG_TEIF | SD_SDIO_DMA_FLAG_HTIF | SD_SDIO_DMA_FLAG_TCIF);

  /* DMA2 Stream3  or Stream6 disable */
  DMA_Cmd(SD_SDIO_DMA_STREAM, DISABLE);

  /* DMA2 Stream3  or Stream6 Config */
  DMA_DeInit(SD_SDIO_DMA_STREAM);

  SDDMA_InitStructure.DMA_Channel = SD_SDIO_DMA_CHANNEL;
  SDDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS;
  SDDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)BufferSRC;
  SDDMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  SDDMA_InitStructure.DMA_BufferSize = 0;
  SDDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  SDDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  SDDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  SDDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  SDDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  SDDMA_InitStructure.DMA_Priority = DMA_Priority_High;
  SDDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  SDDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  SDDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
  SDDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;
  DMA_Init(SD_SDIO_DMA_STREAM, &SDDMA_InitStructure);
  DMA_ITConfig(SD_SDIO_DMA_STREAM, DMA_IT_TC, ENABLE);
  DMA_FlowControllerConfig(SD_SDIO_DMA_STREAM, DMA_FlowCtrl_Peripheral);

  /* DMA2 Stream3  or Stream6 enable */
  DMA_Cmd(SD_SDIO_DMA_STREAM, ENABLE);
    
}

uint32_t RCA=0;

void delay(uint64_t del)
{
	for(int i=del;i>0;i--)
		asm("nop");
}

void init_SDIO()
{
  RCC_AHB1PeriphClockCmd(  RCC_AHB1Periph_GPIOC,  ENABLE  );
  RCC_AHB1PeriphClockCmd(  RCC_AHB1Periph_GPIOD,  ENABLE  );

  GPIO_InitTypeDef gpio;
  gpio.GPIO_Mode   =  GPIO_Mode_AF;
  gpio.GPIO_OType  =  GPIO_OType_PP;
  gpio.GPIO_Pin    =  GPIO_Pin_8|GPIO_Pin_12|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
  gpio.GPIO_PuPd   =  GPIO_PuPd_NOPULL;
  gpio.GPIO_Speed  =  GPIO_Speed_50MHz;
  GPIO_Init(  GPIOC,  &gpio  );

  GPIO_InitTypeDef gpioD;
  gpioD.GPIO_Mode   =  GPIO_Mode_AF;
  gpioD.GPIO_OType  =  GPIO_OType_PP;
  gpioD.GPIO_Pin    =  GPIO_Pin_2;
  gpioD.GPIO_PuPd   =  GPIO_PuPd_NOPULL;
  gpioD.GPIO_Speed  =  GPIO_Speed_50MHz;
  GPIO_Init(  GPIOD,  &gpioD  );

  GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SDIO);

  GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_SDIO);

  RCC_APB2PeriphClockCmd(  RCC_APB2Periph_SDIO,  ENABLE  );

  SDIO_InitTypeDef sdio;
  sdio.SDIO_BusWide=SDIO_BusWide_1b;
  sdio.SDIO_ClockBypass=SDIO_ClockBypass_Disable;
  sdio.SDIO_ClockDiv=0xFF;
  sdio.SDIO_ClockEdge=SDIO_ClockEdge_Rising;
  sdio.SDIO_ClockPowerSave=SDIO_ClockPowerSave_Disable;
  sdio.SDIO_HardwareFlowControl=SDIO_HardwareFlowControl_Disable;
  SDIO_Init(&sdio);

  SDIO_SetPowerState(SDIO_PowerState_ON);
  SDIO_ClockCmd(ENABLE);
  RCC_AHB1PeriphClockCmd(SD_SDIO_DMA_CLK, ENABLE);
}

void DPSM_Init(uint8_t mode,uint8_t sector)
{

	if(mode==0)
	{
	  SDIO_DataInitTypeDef data;
	  data.SDIO_DataBlockSize=SDIO_DataBlockSize_512b;
	  data.SDIO_DataLength=512*sector;
	  data.SDIO_DataTimeOut=0x005B8D80;
	  data.SDIO_DPSM=SDIO_DPSM_Enable;
	  data.SDIO_TransferDir=SDIO_TransferDir_ToCard;
	  data.SDIO_TransferMode=SDIO_TransferMode_Block;
	  SDIO_DataConfig(&data);
	}
	else
	{
	  SDIO_DataInitTypeDef data;
	  data.SDIO_DataBlockSize=SDIO_DataBlockSize_512b;
	  data.SDIO_DataLength=512*sector;
	  data.SDIO_DataTimeOut=0x005B8D80;
	  data.SDIO_DPSM=SDIO_DPSM_Enable;
	  data.SDIO_TransferDir=SDIO_TransferDir_ToSDIO;
	  data.SDIO_TransferMode=SDIO_TransferMode_Block;
	  SDIO_DataConfig(&data);
	}

}

void DPSM_DeInit()
{
	  SDIO->DCTRL=0x0;
}


void send_command(uint8_t index, uint32_t argument, uint32_t CPSM, uint32_t Resp, uint32_t Wait, uint32_t init)
{

  SDIO_CmdInitTypeDef cmd;
  cmd.SDIO_CmdIndex=index;
  cmd.SDIO_Argument=argument;
  cmd.SDIO_CPSM=CPSM;
  cmd.SDIO_Response=Resp;
  cmd.SDIO_Wait=Wait;
  SDIO_SendCommand(&cmd);
  if(init==1)
    delay(200000);
  else
    delay(1000);
}

void Fast_Bus(uint8_t init)
{
  send_command(55,RCA<<16,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,init);
  send_command(6,0x00000002,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,init);

  SDIO_InitTypeDef sdio;
  sdio.SDIO_BusWide=SDIO_BusWide_4b;
  sdio.SDIO_ClockBypass=SDIO_ClockBypass_Disable;
  sdio.SDIO_ClockDiv=0x00;
  sdio.SDIO_ClockEdge=SDIO_ClockEdge_Rising;
  sdio.SDIO_ClockPowerSave=SDIO_ClockPowerSave_Disable;
  sdio.SDIO_HardwareFlowControl=SDIO_HardwareFlowControl_Disable;
  SDIO_Init(&sdio);

  do
  {
  SDIO->ICR=0xFFFFFFFF;
  send_command(13,RCA<<16,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,0);
  }
  while(SDIO_GetResponse(SDIO_RESP1)!=0x00000900);
}

void Fast_Bus_1B()
{  
  send_command(55,RCA<<16,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,0);
  send_command(6,0x0,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,0);

  SDIO_InitTypeDef sdio;
  sdio.SDIO_BusWide=SDIO_BusWide_1b;
  sdio.SDIO_ClockBypass=SDIO_ClockBypass_Disable;
  sdio.SDIO_ClockDiv=0x00;
  sdio.SDIO_ClockEdge=SDIO_ClockEdge_Rising;
  sdio.SDIO_ClockPowerSave=SDIO_ClockPowerSave_Disable;
  sdio.SDIO_HardwareFlowControl=SDIO_HardwareFlowControl_Disable;
  SDIO_Init(&sdio);

  do
  {
  SDIO->ICR=0xFFFFFFFF;
  send_command(13,RCA<<16,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,0);
  }
  while(SDIO_GetResponse(SDIO_RESP1)!=0x00000900);
}

uint8_t MMC_disk_initialize()
{
	do
	{
	  SDIO->ICR=0xFFFFFFFF;
	  send_command(0x0,0x0,SDIO_CPSM_Enable,SDIO_Response_No,SDIO_Wait_No,1);
	  send_command(8,0x000001AA,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,1);
	}
	while(SDIO_GetFlagStatus(SDIO_FLAG_CMDREND)!=SET);
	uint32_t c=0;
	do
	{
		send_command(55,0x0,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,1);
		send_command(41,(uint32_t) 0x80100000 | (uint32_t) 0x40000000,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,1);
		c=SDIO_GetResponse(SDIO_RESP1);
		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
	}
	while((c>>31)==0);


	send_command(2,0x0,SDIO_CPSM_Enable,SDIO_Response_Long,SDIO_Wait_No,1);
	send_command(3,0x0,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,1);
	uint32_t rca=SDIO_GetResponse(SDIO_RESP1);
	RCA=rca>>16;
    send_command(7,RCA<<16,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,1);
    Fast_Bus(1);
	return 0;
}



uint8_t MMC_disk_read(uint8_t sector_data[],uint32_t sector,uint16_t count)
{
  Fast_Bus_1B();
 
  SDIO->DCTRL=0x0;
  SDIO->ICR=0xFFFFFFFF;
  
  send_command(18,sector,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,0);  
 
  DPSM_Init(1,count);
 
  SDIO_DMACmd(ENABLE);
  SD_LowLevel_DMA_RxConfig((uint32_t *)sector_data, (count * 512));
  while(DMA_GetFlagStatus(DMA2_Stream3,DMA_FLAG_TCIF3)!=SET);
  send_command(12,0x0,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,0);
  
  DPSM_DeInit();
  if(SDIO_GetFlagStatus(SDIO_FLAG_DATAEND)!=SET)
	Toggle_LED(Stop_Led);

  SDIO->ICR=0xFFFFFFFF;
   
  if(SDIO_GetResponse(SDIO_RESP1)!=0x00000900)
  {
	DPSM_DeInit();
	do
	{
	  SDIO->ICR=0xFFFFFFFF;	 
	  send_command(13,RCA<<16,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,0);	 
	}
	while((SDIO_GetResponse(SDIO_RESP1)!=0x00000900));  	
  }

  Fast_Bus(0);
  return 0;

}

uint8_t MMC_disk_write(uint8_t const sector_data[],uint32_t sector,uint16_t count)
{
  SDIO->DCTRL=0x0;

  send_command(55,RCA<<16,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,0); 
  send_command(23,count,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,0);

  send_command(25,sector,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,0);
  DPSM_Init(0,count);

  SDIO_DMACmd(ENABLE);
  SD_LowLevel_DMA_TxConfig((uint32_t *)sector_data, (count * 512));  
  while ((SDIO_GetFlagStatus(SDIO_FLAG_DATAEND)|SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT)|SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR)|SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL))!=SET);
  send_command(12,0x0,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,0);
  
  DPSM_DeInit();
  if(SDIO_GetFlagStatus(SDIO_FLAG_DATAEND)!=SET)
	Toggle_LED(Stop_Led);
 
  SDIO->ICR=0xFFFFFFFF;  	 
  
  if(SDIO_GetResponse(SDIO_RESP1)!=0x00000900)
  {	
	do
	{
	  SDIO->ICR=0xFFFFFFFF;	  	  
	  send_command(13,RCA<<16,SDIO_CPSM_Enable,SDIO_Response_Short,SDIO_Wait_No,0);	 
	  
	}
	while((SDIO_GetResponse(SDIO_RESP1)!=0x00000900));  	
  }
 
  return 0;
}
