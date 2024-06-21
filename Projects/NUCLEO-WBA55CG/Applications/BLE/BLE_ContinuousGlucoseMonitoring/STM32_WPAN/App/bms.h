/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service3.h
  * @author  MCD Application Team
  * @brief   Header for service3.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BMS_H
#define BMS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  BMS_BMCP,
  BMS_BMF,
  /* USER CODE BEGIN Service3_CharOpcode_t */

  /* USER CODE END Service3_CharOpcode_t */
  BMS_CHAROPCODE_LAST
} BMS_CharOpcode_t;

typedef enum
{
  BMS_BMCP_WRITE_EVT,
  BMS_BMF_READ_EVT,
  /* USER CODE BEGIN Service3_OpcodeEvt_t */

  /* USER CODE END Service3_OpcodeEvt_t */
  BMS_BOOT_REQUEST_EVT
} BMS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service3_Data_t */

  /* USER CODE END Service3_Data_t */
} BMS_Data_t;

typedef struct
{
  BMS_OpcodeEvt_t       EvtOpcode;
  BMS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service3_NotificationEvt_t */

  /* USER CODE END Service3_NotificationEvt_t */
} BMS_NotificationEvt_t;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void BMS_Init(void);
void BMS_Notification(BMS_NotificationEvt_t *p_Notification);
tBleStatus BMS_UpdateValue(BMS_CharOpcode_t CharOpcode, BMS_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*BMS_H */
