#include "stm32f4xx.h"

typedef struct appCheckInfo {
  u32 AppSize;
  u16 Crc;
  u16 AppInfoCrc;
} TAppCheckInfo;


#pragma location = 0x08008200
__root const TAppCheckInfo AppCheckInfo = {
  .AppSize = 0xAAAAAAAA,
  .Crc = 0xBBBB,
  .AppInfoCrc = 0xCCCC
};
