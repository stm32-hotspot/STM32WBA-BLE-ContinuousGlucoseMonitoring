/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service3_app.c
  * @author  MCD Application Team
  * @brief   service3_app application definition.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_common.h"
#include "app_ble.h"
#include "ll_sys_if.h"
#include "dbg_trace.h"
#include "ble.h"
#include "bms_app.h"
#include "bms.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  /* USER CODE BEGIN Service3_APP_SendInformation_t */

  /* USER CODE END Service3_APP_SendInformation_t */
  BMS_APP_SENDINFORMATION_LAST
} BMS_APP_SendInformation_t;

typedef struct
{
  /* USER CODE BEGIN Service3_APP_Context_t */
  uint32_t BMFeatureChar;
  /* USER CODE END Service3_APP_Context_t */
  uint16_t              ConnectionHandle;
} BMS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DELETE_BOND_BR_EDR_LE                                               1<<0
#define DELETE_BOND_BR_EDR_LE_AUTHORIZATION_CODE                            1<<1
#define DELETE_BOND_BR_EDR                                                  1<<2
#define DELETE_BOND_BR_EDR_AUTHORIZATION_CODE                               1<<3
#define DELETE_BOND_LE                                                      1<<4
#define DELETE_BOND_LE_AUTHORIZATION_CODE                                   1<<5
#define DELETE_ALL_BONDS_BR_EDR_LE                                          1<<6
#define DELETE_ALL_BONDS_BR_EDR_LE_AUTHORIZATION_CODE                       1<<7
#define DELETE_ALL_BONDS_BR_EDR                                             1<<8
#define DELETE_ALL_BONDS_BR_EDR_AUTHORIZATION_CODE                          1<<9
#define DELETE_ALL_BONDS_LE                                                1<<10
#define DELETE_ALL_BONDS_LE_AUTHORIZATION_CODE                             1<<11
#define DELETE_ALL_EXCEPT_REQUEST_BONDS_BR_EDR_LE                          1<<12
#define DELETE_ALL_EXCEPT_REQUEST_BONDS_BR_EDR_LE_AUTHORIZATION_CODE       1<<13
#define DELETE_ALL_EXCEPT_REQUEST_BONDS_BR_EDR                             1<<14
#define DELETE_ALL_EXCEPT_REQUEST_BONDS_BR_EDR_AUTHORIZATION_CODE          1<<15
#define DELETE_ALL_EXCEPT_REQUEST_BONDS_LE                                 1<<16
#define DELETE_ALL_EXCEPT_REQUEST_BONDS_LE_AUTHORIZATION_CODE              1<<17
#define IDENTIFY                                                           1<<18
#define EXTENSION                                                          1<<23
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static BMS_APP_Context_t BMS_APP_Context;

uint8_t a_BMS_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void BMS_Notification(BMS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service3_Notification_1 */

  /* USER CODE END Service3_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service3_Notification_Service3_EvtOpcode */

    /* USER CODE END Service3_Notification_Service3_EvtOpcode */

    case BMS_BMCP_WRITE_EVT:
      /* USER CODE BEGIN Service3Char1_WRITE_EVT */

      /* USER CODE END Service3Char1_WRITE_EVT */
      break;

    case BMS_BMF_READ_EVT:
      /* USER CODE BEGIN Service3Char2_READ_EVT */

      /* USER CODE END Service3Char2_READ_EVT */
      break;

    default:
      /* USER CODE BEGIN Service3_Notification_default */

      /* USER CODE END Service3_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service3_Notification_2 */

  /* USER CODE END Service3_Notification_2 */
  return;
}

void BMS_APP_EvtRx(BMS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service3_APP_EvtRx_1 */

  /* USER CODE END Service3_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service3_APP_EvtRx_Service3_EvtOpcode */

    /* USER CODE END Service3_APP_EvtRx_Service3_EvtOpcode */
    case BMS_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service3_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service3_APP_CONN_HANDLE_EVT */
      break;

    case BMS_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service3_APP_DISCON_HANDLE_EVT */

      /* USER CODE END Service3_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service3_APP_EvtRx_default */

      /* USER CODE END Service3_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service3_APP_EvtRx_2 */

  /* USER CODE END Service3_APP_EvtRx_2 */

  return;
}

void BMS_APP_Init(void)
{
  UNUSED(BMS_APP_Context);
  BMS_Init();

  /* USER CODE BEGIN Service3_APP_Init */
  /**
   * Set BM Feature
   */
  BMS_Data_t bms_data;
  uint8_t bms_char_length;
  
  BMS_APP_Context.BMFeatureChar = 0;
  BMS_APP_Context.BMFeatureChar |= DELETE_ALL_BONDS_LE |
                                   DELETE_BOND_LE;

  bms_char_length = 0;
  a_BMS_UpdateCharData[bms_char_length] = (BMS_APP_Context.BMFeatureChar) & 0xFF;
  bms_char_length++;
  a_BMS_UpdateCharData[bms_char_length] = ((BMS_APP_Context.BMFeatureChar) >> 8) & 0xFF;
  bms_char_length++;
  a_BMS_UpdateCharData[bms_char_length] = ((BMS_APP_Context.BMFeatureChar) >> 16) & 0xFF;
  bms_char_length++;
  bms_data.Length = bms_char_length;
  bms_data.p_Payload = a_BMS_UpdateCharData;
  BMS_UpdateValue(BMS_BMF, &bms_data);
  /* USER CODE END Service3_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/
