#pragma once

#include <stm32f4xx.h>
#include <main.h>

#define SDIO_FIFO_ADDRESS                ((uint32_t)0x40012C80)
#define MMC_DATA_BLOCK_SIZE_512              0x9<<4
enum Wait_Resp
{
  Resp_No,
  Resp_Short,	
  Resp_Long,	
};


#define MMC_RESET 0
#define MMC_SEND_IF_COND 8
#define MMC_APP_CMD 55
#define MMC_ACMD_SD_SEND_OP_COND 41
#define MMC_ALL_SEND_CID 2
#define MMC_SEND_RELATIVE_ADDR 3
#define SELECT_CARD 7
#define READ_MULTIPLE_BLOCK 18
#define STOP_TRANSMISSION 12
#define SEND_STATUS 13

#define MMC_READY 0x00000900