/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service3_app.h
  * @author  MCD Application Team
  * @brief   Header for service3_app.c
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
#ifndef BMS_APP_H
#define BMS_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  BMS_CONN_HANDLE_EVT,
  BMS_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service3_OpcodeNotificationEvt_t */

  /* USER CODE END Service3_OpcodeNotificationEvt_t */

  BMS_LAST_EVT,
} BMS_APP_OpcodeNotificationEvt_t;

typedef struct
{
  BMS_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN BMS_APP_ConnHandleNotEvt_t */

  /* USER CODE END BMS_APP_ConnHandleNotEvt_t */
} BMS_APP_ConnHandleNotEvt_t;
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
void BMS_APP_Init(void);
void BMS_APP_EvtRx(BMS_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*BMS_APP_H */
