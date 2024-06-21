/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1.h
  * @author  MCD Application Team
  * @brief   Header for service1.c
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
#ifndef CGMS_H
#define CGMS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */
/* Set, unset record status flag values */
#define SET_FLAG                                                          (TRUE)
#define UNSET_FLAG                                                       (FALSE)
  
/* UUID for Continuous Glucose Monitoring Service */
#define CONTINUOUS_GLUCOSE_MONITORING_SERVICE_UUID                     (0x181F)
#define CGM_MEASUREMENT_CHAR_UUID                                      (0x2AA7)
#define CGM_FEATURE_CHAR_UUID                                          (0x2AA8)
#define CGM_STATUS_CHAR_UUID                                           (0x2AA9)
#define CGM_SESSION_START_TIME_CHAR_UUID                               (0x2AAA)
#define CGM_SESSION_RUN_TIME_CHAR_UUID                                 (0x2AAB)
#define CGM_SPECIFIC_OPS_CONTROL_POINT_CHAR_UUID                       (0x2AAC)
#define RECORD_ACCESS_CONTROL_POINT_CHAR_UUID                          GLUCOSE_RACP_CHAR_UUID

#define CGMS_FLAGS_CGM_TREND_INFORMATION_PRESENT                 (0x01)
#define CGMS_FLAGS_CGM_QUALITY_PRESENT                           (0x02)
#define CGMS_FLAGS_WARNING_OCTET_PRESENT                         (0x20)
#define CGMS_FLAGS_CAL_TEMP_OCTET_PRESENT                        (0x40)
#define CGMS_FLAGS_MEASUREMENT_STATUS_OCTET_PRESENT              (0x80)
  
#define CGMS_WARNING_OCTET_SESSION_STOPPED                       (0x01)
#define CGMS_WARNING_OCTET_DEVICE_BATTERY_LOW                    (0x02)
#define CGMS_WARNING_OCTET_SENSOR_TYPE_INCORRECT_FOR_DEVICE      (0x04)
#define CGMS_WARNING_OCTET_SENSOR_MALFUNCTION                    (0x08)
#define CGMS_WARNING_OCTET_DEVICE_SPECIFIC_ALERT                 (0x10)
#define CGMS_WARNING_OCTET_GENERAL_DEVICE_FAULT                  (0x20)
  
#define CGMS_CAL_TEMP_OCTET_TIME_SYNCHRONIZATION_REQUIRED        (0x01)
#define CGMS_CAL_TEMP_OCTET_CALIBRATION_NOT_ALLOWED              (0x02)
#define CGMS_CAL_TEMP_OCTET_CALIBRATION_RECOMMENDED              (0x04)
#define CGMS_CAL_TEMP_OCTET_CALIBRATION_REQUIRED                 (0x08)
#define CGMS_CAL_TEMP_OCTET_SENSOR_TEMPERATURE_TOO_HIGH          (0x10)
#define CGMS_CAL_TEMP_OCTET_SENSOR_TEMPERATURE_TOO_LOW           (0x20)
  
#define CGMS_MEASUREMENT_STATUS_OCTET_SENSOR_RESULT_LOWER_THAN_PATIENT_LOW_LEVEL (0x01)
#define CGMS_MEASUREMENT_STATUS_OCTET_SENSOR_RESULT_HIGHER_THAN_PATIENT_HIGH_LEVEL (0x02)
#define CGMS_MEASUREMENT_STATUS_OCTET_SENSOR_RESULT_LOWER_THAN_HYPO_LEVEL (0x04)
#define CGMS_MEASUREMENT_STATUS_OCTET_SENSOR_RESULT_HIGHER_THAN_HYPER_LEVEL (0x08)
#define CGMS_MEASUREMENT_STATUS_OCTET_SENSOR_RATE_OF_DECREASE_EXCEEDED (0x10)
#define CGMS_MEASUREMENT_STATUS_OCTET_SENSOR_RATE_OF_INCREASE_EXCEEDED (0x20)
#define CGMS_MEASUREMENT_STATUS_OCTET_SENSOR_RESULT_LOWER_THAN_DEVICE_CAN_PROCESS (0x40)
#define CGMS_MEASUREMENT_STATUS_OCTET_SENSOR_RESULT_HIGHER_THAN_DEVICE_CAN_PROCESS (0x80)
  
#define CGMS_CGM_FEATURE_CALIBRATION                                  (0x000001)
#define CGMS_CGM_FEATURE_PATIENT_LOW_HIGH_ALERTS                      (0x000002)
#define CGMS_CGM_FEATURE_HYPO_ALERTS                                  (0x000004)
#define CGMS_CGM_FEATURE_HYPER_ALERTS                                 (0x000008)
#define CGMS_CGM_FEATURE_RATE_OF_INCREASE_DECREASE_ALERTS             (0x000010)
#define CGMS_CGM_FEATURE_DEVICE_SPECIFIC_ALERT                        (0x000020)
#define CGMS_CGM_FEATURE_SENSOR_MALFUNCTION_DETECTION                 (0x000040)
#define CGMS_CGM_FEATURE_SENSOR_TEMPERATURE_HIGH_LOW_DETECTION        (0x000080)
  
#define CGMS_CGM_FEATURE_SENSOR_RESULT_HIGH_LOW_DETECTION             (0x000100)
#define CGMS_CGM_FEATURE_LOW_BATTERY_DETECTION                        (0x000200)
#define CGMS_CGM_FEATURE_SENSOR_TYPE_ERROR_DETECTION                  (0x000400)
#define CGMS_CGM_FEATURE_GENERAL_DEVICE_FAULT                         (0x000800)
#define CGMS_CGM_FEATURE_E2E_CRC                                      (0x001000)
#define CGMS_CGM_FEATURE_MULTIPLE_BOND                                (0x002000)
#define CGMS_CGM_FEATURE_MULTIPLE_SESSIONS                            (0x004000)
#define CGMS_CGM_FEATURE_CGM_TREND_INFORMATION                        (0x008000)
  
#define CGMS_CGM_FEATURE_CGM_QUALITY                                  (0x010000)
     
/* New values defined on Core spec supplement (CSS v4), common profile and service error code
   It is used on PTS 5.3.0 */
#define CGMS_ATT_ERROR_CODE_PROCEDURE_ALREADY_IN_PROGRESS               (0xFE)
  
/* New values defined on Core spec supplement (CSS v4), common profile and service error code
   It is used on PTS 5.3.0 */
#define CGMS_ATT_ERROR_CODE_CLIENT_CHAR_CONF_DESC_IMPROPERLY_CONFIGURED (0xFD)
#define CGMS_ATT_ERROR_CODE_MISSING_CRC                                 (0x80)
#define CGMS_ATT_ERROR_CODE_INVALID_CRC                                 (0x81)
#define CGMS_ATT_ERROR_CODE_OUT_OF_RANGE                                (0xFF)
  
#define CGMS_CRC_POLYNOMIAL                                        (0x00001021U)
/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  CGMS_CGM,
  CGMS_CGF,
  CGMS_CGS,
  CGMS_CSST,
  CGMS_CSRT,
  CGMS_RACP,
  CGMS_CFOCP,
  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */
  CGMS_CHAROPCODE_LAST
} CGMS_CharOpcode_t;

typedef enum
{
  CGMS_CGM_NOTIFY_ENABLED_EVT,
  CGMS_CGM_NOTIFY_DISABLED_EVT,
  CGMS_CGF_READ_EVT,
  CGMS_CGF_INDICATE_ENABLED_EVT,
  CGMS_CGF_INDICATE_DISABLED_EVT,
  CGMS_CGS_READ_EVT,
  CGMS_CSST_READ_EVT,
  CGMS_CSST_WRITE_EVT,
  CGMS_CSRT_READ_EVT,
  CGMS_RACP_WRITE_EVT,
  CGMS_RACP_INDICATE_ENABLED_EVT,
  CGMS_RACP_INDICATE_DISABLED_EVT,
  CGMS_CFOCP_WRITE_EVT,
  CGMS_CFOCP_INDICATE_ENABLED_EVT,
  CGMS_CFOCP_INDICATE_DISABLED_EVT,
  /* USER CODE BEGIN Service1_OpcodeEvt_t */
  CGMS_CGM_SESSION_START_TIME_UPDATED,
  /* USER CODE END Service1_OpcodeEvt_t */
  CGMS_BOOT_REQUEST_EVT
} CGMS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */
} CGMS_Data_t;

typedef struct
{
  CGMS_OpcodeEvt_t       EvtOpcode;
  CGMS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */
} CGMS_NotificationEvt_t;

/* USER CODE BEGIN ET */
typedef uint16_t SFLOAT;

/* Glucose measurement characteristic: Base Time structure */
typedef struct _tBasetime
{
   uint16_t year;
   uint8_t month;
   uint8_t day;
   uint8_t hours;
   uint8_t minutes;
   uint8_t seconds;
} tBasetime;
  
typedef enum
{
  CGMS_CGM_TYPE_CAPILLARY_WHOLE_BLOOD = 1,
  CGMS_CGM_TYPE_CAPILLARY_PLASMA = 2,
  CGMS_CGM_TYPE_VENOUS_WHOLE_BLOOD = 3,
  CGMS_CGM_TYPE_VENOUS_PLASMA = 4,
  CGMS_CGM_TYPE_ARTERIAL_WHOLE_BLOOD = 5,
  CGMS_CGM_TYPE_ARTERIAL_PLASMA = 6,
  CGMS_CGM_TYPE_UNDETERMINED_WHOLE_BLOOD = 7,
  CGMS_CGM_TYPE_UNDETERMINED_PLASMA = 8,
  CGMS_CGM_TYPE_INTERSTIFIAL_FLUID = 9,
  CGMS_CGM_TYPE_CONTROL_SOLUTION = 10
} CGMS_CgmType_t;
  
typedef enum
{
  CGMS_CGM_SAMPLE_LOCATION_FINGER = 1,
  CGMS_CGM_SAMPLE_LOCATION_ALTERNATE_SITE_TEST = 2,
  CGMS_CGM_SAMPLE_LOCATION_EARLOBE = 3,
  CGMS_CGM_SAMPLE_LOCATION_CONTROL_SOLUTION = 4,
  CGMS_CGM_SAMPLE_LOCATION_SUBCUTANEOUS_TISSUE = 5,
  CGMS_CGM_SAMPLE_LOCATION_VALUE_NOT_AVAILABLE = 15
} CGMS_CgmSampleLocation_t;

typedef struct
{
  uint8_t Warning;
  uint8_t CalTemp;
  uint8_t Status;
} CGMS_SensorStatusAnnunciationValue_t;

typedef struct
{
  uint8_t Size;
  uint8_t Flags;
  SFLOAT GlucoseConcentration;
  uint16_t TimeOffset;
  CGMS_SensorStatusAnnunciationValue_t SensorStatusAnnunciation;
#if (BLE_CFG_CGMS_CGM_FEATURE_CGM_TREND_INFORMATION == 1)
  SFLOAT CgmTrendInformation;
#endif
#if (BLE_CFG_CGMS_CGM_FEATURE_CGM_QUALITY == 1)
  SFLOAT CgmQuality;
#endif
} CGMS_MeasurementValue_t;

typedef struct 
{
  uint32_t Feature;
  uint8_t  Type;
  uint8_t  SampleLocation;
} CGMS_FeatureValue_t;

typedef struct 
{
  uint16_t TimeOffset;
  CGMS_SensorStatusAnnunciationValue_t SensorStatusAnnunciation;
} CGMS_StatusValue_t;

typedef struct
{
  CGMS_OpcodeEvt_t               EventCode;
  CGMS_Data_t                    EventData;
  uint16_t                       ConnectionHandle;
  uint8_t                        ServiceInstance;
} CGMS_App_Event_t;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define SFLOAT_TO_UINT16(m, e)   (((uint16_t)(m) & 0x0FFFU) | (uint16_t)((int16_t)(e) << 12))     
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void CGMS_Init(void);
void CGMS_Notification(CGMS_NotificationEvt_t *p_Notification);
tBleStatus CGMS_UpdateValue(CGMS_CharOpcode_t CharOpcode, CGMS_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*CGMS_H */
