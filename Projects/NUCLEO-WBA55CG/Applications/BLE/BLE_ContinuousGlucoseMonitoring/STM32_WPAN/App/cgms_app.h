/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1_app.h
  * @author  MCD Application Team
  * @brief   Header for service1_app.c
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
#ifndef CGMS_APP_H
#define CGMS_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  CGMS_CONN_HANDLE_EVT,
  CGMS_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service1_OpcodeNotificationEvt_t */

  /* USER CODE END Service1_OpcodeNotificationEvt_t */

  CGMS_LAST_EVT,
} CGMS_APP_OpcodeNotificationEvt_t;

typedef struct
{
  CGMS_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN CGMS_APP_ConnHandleNotEvt_t */

  /* USER CODE END CGMS_APP_ConnHandleNotEvt_t */
} CGMS_APP_ConnHandleNotEvt_t;
/* USER CODE BEGIN ET */
#define DUMMY_RECORDS_COUNT                                                   20
#define BLE_CFG_CGMS_CALIBRATION_DATA_RECORD_TEST_NUM                        100
#define BLE_CFG_CGMS_CGM_FEATURE_CALIBRATION                                   1
#define BLE_CFG_CGMS_CGM_FEATURE_PATIENT_LOW_HIGH_ALERTS                       1
#define BLE_CFG_CGMS_CGM_FEATURE_HYPO_ALERTS                                   1
#define BLE_CFG_CGMS_CGM_FEATURE_HYPER_ALERTS                                  1
#define BLE_CFG_CGMS_CGM_FEATURE_RATE_OF_INCREASE_DECREASE_ALERTS              1
#define BLE_CFG_CGMS_CGM_FEATURE_DEVICE_SPECIFIC_ALERT                         1
#define BLE_CFG_CGMS_CGM_FEATURE_SENSOR_MALFUNCTION_DETECTION                  1
#define BLE_CFG_CGMS_CGM_FEATURE_SENSOR_TEMPERATURE_HIGH_LOW_DETECTION         1
#define BLE_CFG_CGMS_CGM_FEATURE_SENSOR_RESULT_HIGH_LOW_DETECTION              1
#define BLE_CFG_CGMS_CGM_FEATURE_LOW_BATTERY_DETECTION                         1
#define BLE_CFG_CGMS_CGM_FEATURE_SENSOR_TYPE_ERROR_DETECTION                   1
#define BLE_CFG_CGMS_CGM_FEATURE_GENERAL_DEVICE_FAULT                          1
#define BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC                                       1
#define BLE_CFG_CGMS_CGM_FEATURE_MULTIPLE_BOND                                 1
#define BLE_CFG_CGMS_CGM_FEATURE_MULTIPLE_SESSIONS                             1
#define BLE_CFG_CGMS_CGM_FEATURE_CGM_TREND_INFORMATION                         1
#define BLE_CFG_CGMS_CGM_FEATURE_CGM_QUALITY                                   1
#define BLE_CFG_CGMS_CGM_SAMPLE_LOCATION                                       CGMS_CGM_SAMPLE_LOCATION_FINGER
#define BLE_CFG_CGMS_CGM_TYPE                                                  CGMS_CGM_TYPE_CAPILLARY_WHOLE_BLOOD
   
#define BLE_CFG_CGMS_SHORTEST_CGM_COMM_INTERVAL_SUPPORTED_BY_DEVICE_IN_MINS   10  /**< 1min */
#define BLE_CFG_CGMS_DEFAULT_CGM_COMM_INTERVAL_VALUE                         (1)

#define BLE_CFG_CGMS_BATTERY_MEASUREMENT_INTERVAL_IN_SECS                     10

#define BLE_CFG_CGMS_DB_RECORD_MAX_NUM                                       500

#define BLE_CFG_CGMS_DB_RECORD_TEST_NUM                                       40/*10*/
#define BLE_CFG_CGMS_CALIBRATION_DATA_RECORD_TEST_NUM                        100
    
//#define BLE_CFG_CGMS_SOCP_OP_CODE_START_SESSION                                1
//#define BLE_CFG_CGMS_SOCP_OP_CODE_STOP_SESSION                                 1
//#define BLE_CFG_CGMS_SOCP_OP_CODE_GET_GLUCOSE_CALIBRATION_VALUE                1
//#define BLE_CFG_CGMS_SOCP_OP_CODE_SET_GLUCOSE_CALIBRATION_VALUE                1
//#define BLE_CFG_CGMS_SOCP_OP_CODE_GET_CGM_COMMUNICATION_INTERVAL               1
//#define BLE_CFG_CGMS_SOCP_OP_CODE_SET_CGM_COMMUNICATION_INTERVAL               1
//#define BLE_CFG_CGMS_SOCP_OP_CODE_RESET_DEVICE_SPECIFIC_ALERT_LEVEL            1
    
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
#define CGMS_CRC_LENGTH                                                  (sizeof(uint16_t))
#else
#define CGMS_CRC_LENGTH                                                  (0)
#endif
    
/**
 * Enabled or Disable CGMS debug or PTS testing
 */
#define BLE_CFG_CGMS_DEBUG                                                     1
#define BLE_CFG_CGMS_DEBUG_RESET_DB_ON_DISCONNECT                              0
#define BLE_CFG_CGMS_DEBUG_STOP_SESSION_ON_DISCONNECT                          0

#define CGMS_APP_MEASUREMENT_INTERVAL                                  (10*1000)
#define CGMS_APP_BATTERY_MEASUREMENT_INTERVAL (BLE_CFG_CGMS_BATTERY_MEASUREMENT_INTERVAL_IN_SECS*1000)
#define CGMS_APP_RACP_INTERVAL                                            (1000)

#define CGMS_APP_MEASUREMENT_FLAGS    ((BLE_CFG_CGMS_CGM_FEATURE_CGM_TREND_INFORMATION*CGMS_FLAGS_CGM_TREND_INFORMATION_PRESENT) |\
                                       (BLE_CFG_CGMS_CGM_FEATURE_CGM_QUALITY*CGMS_FLAGS_CGM_QUALITY_PRESENT)                     |\
                                       (CGMS_FLAGS_WARNING_OCTET_PRESENT)                                                        |\
                                       (CGMS_FLAGS_CAL_TEMP_OCTET_PRESENT)                                                       |\
                                       (CGMS_FLAGS_MEASUREMENT_STATUS_OCTET_PRESENT))
#define BLE_CFG_CGMS_PTS                                                     (0)
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
void CGMS_APP_Init(void);
void CGMS_APP_EvtRx(CGMS_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EFP */
uint8_t CGMS_APP_GetCGMSOCPCharacteristicIndicationEnabled(void);
uint8_t CGMS_APP_GetRACPCharacteristicIndicationEnabled(void);
void CGMS_APP_UpdateFeature(void);
uint32_t CGMS_APP_ComputeCRC(uint8_t * pData, uint8_t dataLength);
/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*CGMS_APP_H */
