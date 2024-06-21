/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1_app.c
  * @author  MCD Application Team
  * @brief   service1_app application definition.
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
#include "cgms_app.h"
#include "cgms.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "crc_ctrl.h"
#include "crc_ctrl_conf.h"
#include "cgms_db.h"
#include "cgms_racp.h"
#include "cgms_socp.h"
#include "stm32_timer.h"
#if ((CFG_LED_SUPPORTED == 1) || (CFG_BUTTON_SUPPORTED == 1))
#include "stm32wbaxx_nucleo.h"
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  Cgm_NOTIFICATION_OFF,
  Cgm_NOTIFICATION_ON,
  Cgf_INDICATION_OFF,
  Cgf_INDICATION_ON,
  Racp_INDICATION_OFF,
  Racp_INDICATION_ON,
  Cfocp_INDICATION_OFF,
  Cfocp_INDICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  CGMS_APP_SENDINFORMATION_LAST
} CGMS_APP_SendInformation_t;

typedef struct
{
  CGMS_APP_SendInformation_t     Cgm_Notification_Status;
  CGMS_APP_SendInformation_t     Cgf_Indication_Status;
  CGMS_APP_SendInformation_t     Racp_Indication_Status;
  CGMS_APP_SendInformation_t     Cfocp_Indication_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */
  CGMS_FeatureValue_t CGMFeature;
  CGMS_StatusValue_t CGMStatus;
  uint16_t GlucoseConcentration;
  uint16_t CurrentTimeOffset;
  uint8_t CGMCommunicationIntervalInMins;
  UTIL_TIMER_Object_t TimerGlucoseMeasurement_Id;
  UTIL_TIMER_Object_t TimerBatteryMeasurement_Id;
  CGMS_SOCP_CalibrationDataRecord_t CalibrationDataRecord[BLE_CFG_CGMS_CALIBRATION_DATA_RECORD_TEST_NUM];
  uint16_t LastCalibrationDataRecordNumber;
  uint16_t CessionRunTime;
  uint16_t HighAlertLevel;
  uint16_t LowAlertLevel;
  uint16_t HypoAlertLevel;
  uint16_t HyperAlertLevel;
  uint16_t IncreaseRateAlertLevel;
  uint16_t DecreaseRateAlertLevel;
  UTIL_TIMER_Object_t TimerRACPProcess_Id;
  
  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} CGMS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CESSION_RUN_TIME                                                      24
#define HIGH_ALERT_LEVEL                                                     100
#define LOW_ALERT_LEVEL                                                       70
#define HYPO_ALERT_LEVEL                                                      70
#define HYPER_ALERT_LEVEL                                                    110 
#define MAX_ALERT_LEVEL                                                   0x07FE
#define INCREASE_RATE_ALERT_LEVEL                                             10
#define DECREASE_RATE_ALERT_LEVEL                                             10
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */
extern CRC_HandleTypeDef hcrc;
/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static CGMS_APP_Context_t CGMS_APP_Context;

uint8_t a_CGMS_UpdateCharData[247];

/* USER CODE BEGIN PV */
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
/**
 * @brief CRC Handle configuration for CGM use
 */
CRCCTRL_Handle_t CGM_Handle;
#endif

uint32_t dummyRecordsPosition = 0;

const CGMS_RACP_Record_t DummyRecords[DUMMY_RECORDS_COUNT] =
{
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(80,0),  0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(90,0),  0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(100,0), 0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(110,0), 0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(120,0), 0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(130,0), 0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(140,0), 0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(150,0), 0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(150,0), 0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(145,0), 0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(140,0), 0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(135,0), 0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(130,0), 0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(125,0), 0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(120,0), 0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(115,0), 0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(110,0), 0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(105,0), 0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(100,0), 0, {1, 1, 1}, 0, 0} },
  { {13, CGMS_APP_MEASUREMENT_FLAGS, SFLOAT_TO_UINT16(90,0),  0, {1, 1, 1}, 0, 0} }		
};

CGMS_SOCP_CalibrationDataRecord_t NoCalibrationDataRecord =
{
  SFLOAT_TO_UINT16(2047,0),
  0, 0, 0, 0, 0
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void CGMS_Cgm_SendNotification(void);
static void CGMS_Cgf_SendIndication(void);
static void CGMS_Racp_SendIndication(void);
static void CGMS_Cfocp_SendIndication(void);

/* USER CODE BEGIN PFP */
static void cgmsapp_timer_handler_measure_glucose( void *arg );
static void cgmsapp_timer_handler_measure_battery(void *arg);
static void cgmsapp_timer_handler_racp_process( void *arg );
static void cgmsapp_task_glucose_measurement( void );
static void cgmsapp_task_battery_measurement( void );
static void cgmsapp_start_session( void );
static void cgmsapp_stop_session( void );
void CGMS_APP_PTS_TestDBInit(void);
static void cgmsapp_task_racp_process_report_record(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void CGMS_Notification(CGMS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case CGMS_CGM_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char1_NOTIFY_ENABLED_EVT */
      {
#if (BLE_CFG_CGMS_SOCP_OP_CODE_START_SESSION == 0)
        cgmsapp_start_session();
#endif
        LOG_INFO_APP("CGMS_CGM_NOTIFY_ENABLED_EVT\r\n");
        CGMS_APP_Context.Cgm_Notification_Status = Cgm_NOTIFICATION_ON;
      }
      /* USER CODE END Service1Char1_NOTIFY_ENABLED_EVT */
      break;

    case CGMS_CGM_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char1_NOTIFY_DISABLED_EVT */
      {
#if (BLE_CFG_CGMS_SOCP_OP_CODE_START_SESSION == 0)
        cgmsapp_stop_session();
#endif
        LOG_INFO_APP("CGMS_CGM_NOTIFY_DISABLED_EVT\r\n");
        CGMS_APP_Context.Cgm_Notification_Status = Cgm_NOTIFICATION_OFF;
      }
      /* USER CODE END Service1Char1_NOTIFY_DISABLED_EVT */
      break;

    case CGMS_CGF_READ_EVT:
      /* USER CODE BEGIN Service1Char2_READ_EVT */

      /* USER CODE END Service1Char2_READ_EVT */
      break;

    case CGMS_CGF_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char2_INDICATE_ENABLED_EVT */
      {
        CGMS_APP_Context.Cgf_Indication_Status = Cgf_INDICATION_ON;
        LOG_INFO_APP("CGMS_CGF_INDICATE_ENABLED_EVT\r\n");
      }
      /* USER CODE END Service1Char2_INDICATE_ENABLED_EVT */
      break;

    case CGMS_CGF_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char2_INDICATE_DISABLED_EVT */
      {
        CGMS_APP_Context.Cgf_Indication_Status = Cgf_INDICATION_OFF;
        LOG_INFO_APP("CGMS_CGF_INDICATE_DISABLED_EVT\r\n");
      }
      /* USER CODE END Service1Char2_INDICATE_DISABLED_EVT */
      break;

    case CGMS_CGS_READ_EVT:
      /* USER CODE BEGIN Service1Char3_READ_EVT */

      /* USER CODE END Service1Char3_READ_EVT */
      break;

    case CGMS_CSST_READ_EVT:
      /* USER CODE BEGIN Service1Char4_READ_EVT */

      /* USER CODE END Service1Char4_READ_EVT */
      break;

    case CGMS_CSST_WRITE_EVT:
      /* USER CODE BEGIN Service1Char4_WRITE_EVT */

      /* USER CODE END Service1Char4_WRITE_EVT */
      break;

    case CGMS_CSRT_READ_EVT:
      /* USER CODE BEGIN Service1Char5_READ_EVT */

      /* USER CODE END Service1Char5_READ_EVT */
      break;

    case CGMS_RACP_WRITE_EVT:
      /* USER CODE BEGIN Service1Char6_WRITE_EVT */

      /* USER CODE END Service1Char6_WRITE_EVT */
      break;

    case CGMS_RACP_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char6_INDICATE_ENABLED_EVT */
      {
        CGMS_APP_Context.Racp_Indication_Status = Racp_INDICATION_ON;
        LOG_INFO_APP("CGMS_RACP_INDICATE_ENABLED_EVT\r\n");
      }
      /* USER CODE END Service1Char6_INDICATE_ENABLED_EVT */
      break;

    case CGMS_RACP_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char6_INDICATE_DISABLED_EVT */
      {
        CGMS_APP_Context.Racp_Indication_Status = Racp_INDICATION_OFF;
        LOG_INFO_APP("CGMS_RACP_INDICATE_DISABLED_EVT\r\n");
      }
      /* USER CODE END Service1Char6_INDICATE_DISABLED_EVT */
      break;

    case CGMS_CFOCP_WRITE_EVT:
      /* USER CODE BEGIN Service1Char7_WRITE_EVT */

      /* USER CODE END Service1Char7_WRITE_EVT */
      break;

    case CGMS_CFOCP_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char7_INDICATE_ENABLED_EVT */
      {
        CGMS_APP_Context.Cfocp_Indication_Status = Cfocp_INDICATION_ON;
        LOG_INFO_APP("CGMS_CFOCP_INDICATE_ENABLED_EVT\r\n");
      }
      /* USER CODE END Service1Char7_INDICATE_ENABLED_EVT */
      break;

    case CGMS_CFOCP_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char7_INDICATE_DISABLED_EVT */
      {
        CGMS_APP_Context.Cfocp_Indication_Status = Cfocp_INDICATION_OFF;
        LOG_INFO_APP("CGMS_CFOCP_INDICATE_DISABLED_EVT\r\n");
      }
      /* USER CODE END Service1Char7_INDICATE_DISABLED_EVT */
      break;

    default:
      /* USER CODE BEGIN Service1_Notification_default */

      /* USER CODE END Service1_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service1_Notification_2 */

  /* USER CODE END Service1_Notification_2 */
  return;
}

void CGMS_APP_EvtRx(CGMS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case CGMS_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service1_APP_CONN_HANDLE_EVT */
      break;

    case CGMS_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_DISCON_HANDLE_EVT */
      CGMS_RACP_Init();
      CGMS_SOCP_Init();
      /* USER CODE END Service1_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service1_APP_EvtRx_default */

      /* USER CODE END Service1_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service1_APP_EvtRx_2 */

  /* USER CODE END Service1_APP_EvtRx_2 */

  return;
}

void CGMS_APP_Init(void)
{
  UNUSED(CGMS_APP_Context);
  CGMS_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  uint16_t n;
  CGMS_Data_t msg_conf;
  uint8_t cgm_char_length;
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  uint16_t CRCValue;
  CRCCTRL_Cmd_Status_t result;
#endif
  
  /* Initialize the CGMS records database */
  CGMS_DB_Init();
  CGMS_APP_Context.CurrentTimeOffset = 0;
  
  CGMS_RACP_Init();
  CGMS_SOCP_Init();
  
  /* Register tasks for Glucose Measurement, Battery Level update, report Record processing */
  UTIL_SEQ_RegTask( 1<<CFG_TASK_GLUCOSE_MEASUREMENT_ID, UTIL_SEQ_RFU, cgmsapp_task_glucose_measurement );
  UTIL_SEQ_RegTask( 1<<CFG_TASK_BATTERY_MEASUREMENT_ID, UTIL_SEQ_RFU, cgmsapp_task_battery_measurement );
  UTIL_SEQ_RegTask( 1<<CFG_TASK_RACP_PROCESS_REPORT_RECORD_ID, UTIL_SEQ_RFU, cgmsapp_task_racp_process_report_record );
    
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  CGM_Handle.Uid = 0x00;
  CGM_Handle.PreviousComputedValue = 0x00;
  CGM_Handle.State = HANDLE_NOT_REG;
  CGM_Handle.Configuration.DefaultPolynomialUse = hcrc.Init.DefaultPolynomialUse;
  CGM_Handle.Configuration.DefaultInitValueUse = hcrc.Init.DefaultInitValueUse;
  CGM_Handle.Configuration.GeneratingPolynomial = hcrc.Init.GeneratingPolynomial;
  CGM_Handle.Configuration.CRCLength = hcrc.Init.CRCLength;
  CGM_Handle.Configuration.InputDataInversionMode = hcrc.Init.InputDataInversionMode;
  CGM_Handle.Configuration.OutputDataInversionMode = hcrc.Init.OutputDataInversionMode;
  CGM_Handle.Configuration.InputDataFormat = hcrc.InputDataFormat;

  /* Register CRC Handle */
  result = CRCCTRL_RegisterHandle(&CGM_Handle);
  if (result != CRCCTRL_OK)
  {
    Error_Handler();
  }
#endif

  /* Initalization of Calibration Data records */
  for (n = 1; n <= BLE_CFG_CGMS_CALIBRATION_DATA_RECORD_TEST_NUM; n++)
  {
    memset(&(CGMS_APP_Context.CalibrationDataRecord[n-1]), 0x00, sizeof(CGMS_SOCP_CalibrationDataRecord_t));
    CGMS_APP_Context.CalibrationDataRecord[n-1].CalibrationDataRecordNumber = n;
  }
  CGMS_APP_Context.LastCalibrationDataRecordNumber = 10;
  
  /* Initialization of Feature char */
  CGMS_APP_Context.CGMFeature.Feature = 0;
  CGMS_APP_Context.CGMFeature.Feature |= CGMS_CGM_FEATURE_CALIBRATION                           |
                                         CGMS_CGM_FEATURE_PATIENT_LOW_HIGH_ALERTS               |
                                         CGMS_CGM_FEATURE_HYPO_ALERTS                           |
                                         CGMS_CGM_FEATURE_HYPER_ALERTS                          |
                                         CGMS_CGM_FEATURE_RATE_OF_INCREASE_DECREASE_ALERTS      |
                                         CGMS_CGM_FEATURE_DEVICE_SPECIFIC_ALERT                 |
                                         CGMS_CGM_FEATURE_SENSOR_MALFUNCTION_DETECTION          |
                                         CGMS_CGM_FEATURE_SENSOR_TEMPERATURE_HIGH_LOW_DETECTION |
                                         CGMS_CGM_FEATURE_SENSOR_RESULT_HIGH_LOW_DETECTION      |
                                         CGMS_CGM_FEATURE_LOW_BATTERY_DETECTION                 |
                                         CGMS_CGM_FEATURE_SENSOR_TYPE_ERROR_DETECTION           |
                                         CGMS_CGM_FEATURE_GENERAL_DEVICE_FAULT                  |
                                         CGMS_CGM_FEATURE_E2E_CRC                               |
                                         CGMS_CGM_FEATURE_MULTIPLE_BOND                         |
                                         CGMS_CGM_FEATURE_MULTIPLE_SESSIONS                     |
                                         CGMS_CGM_FEATURE_CGM_TREND_INFORMATION                 |
                                         CGMS_CGM_FEATURE_CGM_QUALITY;
  CGMS_APP_Context.CGMFeature.SampleLocation = BLE_CFG_CGMS_CGM_SAMPLE_LOCATION;
  CGMS_APP_Context.CGMFeature.Type = BLE_CFG_CGMS_CGM_TYPE;
  if(((CGMS_APP_Context.CGMFeature.Feature) & CGMS_CGM_FEATURE_E2E_CRC) != (CGMS_CGM_FEATURE_E2E_CRC))
  {
    CRCValue = 0xFFFF;
  }
  
  cgm_char_length = 0;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(CGMS_APP_Context.CGMFeature.Feature & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((CGMS_APP_Context.CGMFeature.Feature >> 8) & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((CGMS_APP_Context.CGMFeature.Feature >> 16) & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((CGMS_APP_Context.CGMFeature.Type) | ((CGMS_APP_Context.CGMFeature.SampleLocation) << 4));
  cgm_char_length++;
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  CRCValue = CGMS_APP_ComputeCRC(a_CGMS_UpdateCharData, cgm_char_length);
#endif
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(CRCValue & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((CRCValue >> 8) & 0xFF);
  cgm_char_length++;
  msg_conf.Length = cgm_char_length;
  msg_conf.p_Payload = a_CGMS_UpdateCharData;
  CGMS_UpdateValue(CGMS_CGF, &msg_conf);
  
  CGMS_APP_Context.GlucoseConcentration = 0xD040;
  
  /* Initialization of Status char */
#if (BLE_CFG_CGMS_PTS == 1)
  CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Warning = CGMS_WARNING_OCTET_DEVICE_BATTERY_LOW    |
                                                                CGMS_WARNING_OCTET_DEVICE_SPECIFIC_ALERT;
#else
  CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Warning = CGMS_WARNING_OCTET_SESSION_STOPPED       |
                                                                CGMS_WARNING_OCTET_DEVICE_BATTERY_LOW    |
                                                                CGMS_WARNING_OCTET_DEVICE_SPECIFIC_ALERT;
#endif  
  cgm_char_length = 0;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(CGMS_APP_Context.CGMStatus.TimeOffset & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((CGMS_APP_Context.CGMStatus.TimeOffset >> 8) & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Warning;
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.CalTemp;
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Status;
  cgm_char_length++;
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  CRCValue = CGMS_APP_ComputeCRC(a_CGMS_UpdateCharData, cgm_char_length);

  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(CRCValue & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((CRCValue >> 8) & 0xFF);
  cgm_char_length++;
#endif
  msg_conf.Length = cgm_char_length;
  msg_conf.p_Payload = a_CGMS_UpdateCharData;
  CGMS_UpdateValue(CGMS_CGS, &msg_conf);
  
  /* Initialization of Cession Start Time char */
  cgm_char_length = 0;
  a_CGMS_UpdateCharData[cgm_char_length] = 0xE7;
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = 0x07;  /* Current Year */
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = 0x04;  /* Current Month */
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = 0x01;     /* Current Day */
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = 0x00;     /* Current Hour */
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = 0x00;     /* Current Minute */
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = 0x00;     /* Current Second */
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = 0x00;     /* Current Time Zone */
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = 0x00;     /* Current DST Offset */
  cgm_char_length++;
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  CRCValue = CGMS_APP_ComputeCRC(a_CGMS_UpdateCharData, cgm_char_length);

  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(CRCValue & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((CRCValue >> 8) & 0xFF);
  cgm_char_length++;
#endif
  msg_conf.Length = cgm_char_length;
  msg_conf.p_Payload = a_CGMS_UpdateCharData;
  CGMS_UpdateValue(CGMS_CSST, &msg_conf);

  /* Initialization of Cession Run Time char */
  CGMS_APP_Context.CessionRunTime = CESSION_RUN_TIME;

  cgm_char_length = 0;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(CGMS_APP_Context.CessionRunTime & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((CGMS_APP_Context.CessionRunTime >> 8) & 0xFF);
  cgm_char_length++;
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  CRCValue = CGMS_APP_ComputeCRC(a_CGMS_UpdateCharData, cgm_char_length);

  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(CRCValue & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((CRCValue >> 8) & 0xFF);
  cgm_char_length++;
#endif
  msg_conf.Length = cgm_char_length;
  msg_conf.p_Payload = a_CGMS_UpdateCharData;
  CGMS_UpdateValue(CGMS_CSRT, &msg_conf);
  
  CGMS_APP_Context.CurrentTimeOffset = 0;
  CGMS_APP_Context.CGMCommunicationIntervalInMins = BLE_CFG_CGMS_DEFAULT_CGM_COMM_INTERVAL_VALUE;
  
  /* Generate some dummy data - just for testing purposes */
#if (BLE_CFG_CGMS_DEBUG == 1)
  CGMS_APP_PTS_TestDBInit();
#endif  
  
  /* Create timer for Continuous Glucose Measurement */
  UTIL_TIMER_Create(&(CGMS_APP_Context.TimerGlucoseMeasurement_Id),
                    CGMS_APP_MEASUREMENT_INTERVAL,
                    UTIL_TIMER_PERIODIC,
                    &cgmsapp_timer_handler_measure_glucose, 0);
  
  /* Create timer for Battery Measurement */
  UTIL_TIMER_Create(&(CGMS_APP_Context.TimerBatteryMeasurement_Id),
                    CGMS_APP_BATTERY_MEASUREMENT_INTERVAL,
                    UTIL_TIMER_PERIODIC,
                    &cgmsapp_timer_handler_measure_battery, 0);
  
  /* Create timer for RACP process */
  UTIL_TIMER_Create(&(CGMS_APP_Context.TimerRACPProcess_Id),
                    CGMS_APP_RACP_INTERVAL,
                    UTIL_TIMER_ONESHOT,
                    &cgmsapp_timer_handler_racp_process, 0);
  
  /* Start timer for Battery Measurement */
  UTIL_TIMER_StartWithPeriod( &(CGMS_APP_Context.TimerBatteryMeasurement_Id), CGMS_APP_BATTERY_MEASUREMENT_INTERVAL);
  
  CGMS_APP_Context.HighAlertLevel = (uint16_t)HIGH_ALERT_LEVEL;
  CGMS_APP_Context.LowAlertLevel = (uint16_t)LOW_ALERT_LEVEL;
  CGMS_APP_Context.HypoAlertLevel = (uint16_t)HYPO_ALERT_LEVEL;
  CGMS_APP_Context.HyperAlertLevel = (uint16_t)HYPER_ALERT_LEVEL;
  CGMS_APP_Context.IncreaseRateAlertLevel = (uint16_t)INCREASE_RATE_ALERT_LEVEL;
  CGMS_APP_Context.DecreaseRateAlertLevel = (uint16_t)DECREASE_RATE_ALERT_LEVEL;
  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
void CGMS_APP_PTS_TestDBInit(void)
{
  uint16_t index;
  CGMS_RACP_Record_t record;

  CGMS_APP_Context.CurrentTimeOffset = 0;

  for (index = 0; index < BLE_CFG_CGMS_DB_RECORD_TEST_NUM; index++)
  {
    if (CGMS_APP_Context.CGMCommunicationIntervalInMins == 0xFF)
    {
      CGMS_APP_Context.CurrentTimeOffset += BLE_CFG_CGMS_SHORTEST_CGM_COMM_INTERVAL_SUPPORTED_BY_DEVICE_IN_MINS;
    }
    else 
    {
      CGMS_APP_Context.CurrentTimeOffset += CGMS_APP_Context.CGMCommunicationIntervalInMins;
    }
    
    record.measurement.Size = DummyRecords[dummyRecordsPosition].measurement.Size;
    record.measurement.Flags = DummyRecords[dummyRecordsPosition].measurement.Flags;
    record.measurement.GlucoseConcentration =DummyRecords[dummyRecordsPosition].measurement.GlucoseConcentration;
    record.measurement.TimeOffset = CGMS_APP_Context.CurrentTimeOffset;
    
    record.measurement.SensorStatusAnnunciation.CalTemp = DummyRecords[dummyRecordsPosition].measurement.SensorStatusAnnunciation.CalTemp;
    record.measurement.SensorStatusAnnunciation.Status = DummyRecords[dummyRecordsPosition].measurement.SensorStatusAnnunciation.Status;
    record.measurement.SensorStatusAnnunciation.Warning = DummyRecords[dummyRecordsPosition].measurement.SensorStatusAnnunciation.Warning;
    
#if (BLE_CFG_CGMS_CGM_FEATURE_CGM_QUALITY == 1)
    record.measurement.CgmQuality =  DummyRecords[dummyRecordsPosition].measurement.CgmQuality;
#endif
#if (BLE_CFG_CGMS_CGM_FEATURE_CGM_TREND_INFORMATION == 1)
    record.measurement.CgmTrendInformation = DummyRecords[dummyRecordsPosition].measurement.CgmTrendInformation;
#endif
    CGMS_DB_AddRecord(&record);
    
    
    if (dummyRecordsPosition < (DUMMY_RECORDS_COUNT-1))
    {
      dummyRecordsPosition++;
    }
    else 
    {
      dummyRecordsPosition = 0;
    }
  }
}

uint8_t CGMS_SOCP_APP_EventHandler(CGMS_SOCP_App_Event_t *pSOCPAppEvent)
{
  uint16_t temp;
  CGMS_Data_t msg_conf;
  uint8_t cgm_char_length;
  
  switch(pSOCPAppEvent->EventCode)
  {      
    case CGMS_SOCP_START_SESSION_EVENT:
      {
#if (BLE_CFG_CGMS_PTS == 1)
        if (((CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Warning) & CGMS_WARNING_OCTET_SESSION_STOPPED) == CGMS_WARNING_OCTET_SESSION_STOPPED)
        {
          LOG_INFO_APP("CGMS_WARNING_OCTET_SESSION_STOPPED set\r\n"); 
          return FALSE;
        }
#endif
        cgmsapp_start_session();
      }
      break;
      
    case CGMS_SOCP_STOP_SESSION_EVENT:
      {
        cgmsapp_stop_session();
      }
      break;
      
    case CGMS_SOCP_RESET_DEVICE_SPECIFIC_ALERT_LEVEL_EVENT:
      { 
        uint8_t temp;
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
        uint16_t CRCValue;
#endif
        
        cgm_char_length = 0;
        a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(CGMS_APP_Context.CGMStatus.TimeOffset & 0xFF);
        cgm_char_length++;
        a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(CGMS_APP_Context.CGMStatus.TimeOffset >> 8);
        cgm_char_length++;
        temp = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Warning;
        temp &= (~CGMS_WARNING_OCTET_DEVICE_SPECIFIC_ALERT);
        CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Warning =  temp;
        a_CGMS_UpdateCharData[cgm_char_length] = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Warning;
        cgm_char_length++;
        a_CGMS_UpdateCharData[cgm_char_length] = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.CalTemp;
        cgm_char_length++;
        a_CGMS_UpdateCharData[cgm_char_length] = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Status;
        cgm_char_length++;
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
        CRCValue = CGMS_APP_ComputeCRC(a_CGMS_UpdateCharData, cgm_char_length);

        a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(CRCValue & 0xFF);
        cgm_char_length++;
        a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((CRCValue >> 8) & 0xFF);
        cgm_char_length++;
#endif
        msg_conf.Length = cgm_char_length;
        msg_conf.p_Payload = a_CGMS_UpdateCharData;
        CGMS_UpdateValue(CGMS_CGS, &msg_conf);
      }
      break;
      
    case CGMS_SOCP_SET_CGM_COMMUNICATION_INTERVAL_EVENT:
      {
        CGMS_APP_Context.CGMCommunicationIntervalInMins = *((CGMS_SOCP_CommunicationInterval_t *)(pSOCPAppEvent->EventData.p_Payload));
        if(CGMS_APP_Context.CGMCommunicationIntervalInMins == 0)
        {
          cgmsapp_stop_session();
        }
        else
        {
          cgmsapp_start_session();
        }
      }
      break;
      
    case CGMS_SOCP_GET_CGM_COMMUNICATION_INTERVAL_EVENT:
      {
        *((CGMS_SOCP_CommunicationInterval_t *)(pSOCPAppEvent->EventData.p_Payload)) = CGMS_APP_Context.CGMCommunicationIntervalInMins;
      }
      break;
      
    case CGMS_SOCP_SET_GLUCOSE_CALIBRATION_VALUE_EVENT:
      {
        if (CGMS_APP_Context.LastCalibrationDataRecordNumber <= BLE_CFG_CGMS_CALIBRATION_DATA_RECORD_TEST_NUM)
        {
          memcpy(&(CGMS_APP_Context.CalibrationDataRecord[CGMS_APP_Context.LastCalibrationDataRecordNumber]), ((CGMS_SOCP_CalibrationDataRecord_t *)(pSOCPAppEvent->EventData.p_Payload)), sizeof(CGMS_SOCP_CalibrationDataRecord_t));
          CGMS_APP_Context.CalibrationDataRecord[CGMS_APP_Context.LastCalibrationDataRecordNumber].CalibrationDataRecordNumber = CGMS_APP_Context.LastCalibrationDataRecordNumber + 1;
          CGMS_APP_Context.LastCalibrationDataRecordNumber++;
        }
        else 
        {
          return FALSE;
        }
      }
      break;
      
    case CGMS_SOCP_GET_GLUCOSE_CALIBRATION_VALUE_EVENT:
      {
        temp = ((CGMS_SOCP_CalibrationDataRecord_t *)(pSOCPAppEvent->EventData.p_Payload))->CalibrationDataRecordNumber;
        if (temp == 0xFFFF) // STM_TODO : Get rid of the magic value (0xFFFF defined by spec as the last record)
        {
          memcpy(((CGMS_SOCP_CalibrationDataRecord_t *)(pSOCPAppEvent->EventData.p_Payload)), &(CGMS_APP_Context.CalibrationDataRecord[CGMS_APP_Context.LastCalibrationDataRecordNumber-1]), sizeof(CGMS_SOCP_CalibrationDataRecord_t));
        }
        else if ((temp <= CGMS_APP_Context.LastCalibrationDataRecordNumber) && (temp > 0))
        {
          memcpy(((CGMS_SOCP_CalibrationDataRecord_t *)(pSOCPAppEvent->EventData.p_Payload)), &(CGMS_APP_Context.CalibrationDataRecord[temp-1]), sizeof(CGMS_SOCP_CalibrationDataRecord_t));
        }
        else if (temp == 0x0000) // STM_TODO : Get rid of the magic value (0x0000 defined by spec as no record available)
        {
          memcpy(((CGMS_SOCP_CalibrationDataRecord_t *)(pSOCPAppEvent->EventData.p_Payload)), &(NoCalibrationDataRecord), sizeof(CGMS_SOCP_CalibrationDataRecord_t));
        }
        else 
        {
          return FALSE;
        }
      }
      break;
      
    case CGMS_SOCP_SET_PATIENT_HIGH_ALERT_LEVEL:
      {
        if(*((uint16_t *)(pSOCPAppEvent->EventData.p_Payload)) < MAX_ALERT_LEVEL)
        {
          CGMS_APP_Context.HighAlertLevel = *((uint16_t *)(pSOCPAppEvent->EventData.p_Payload));
        }
        else
        {
          return FALSE;
        }
      }
      break;
      
    case CGMS_SOCP_GET_PATIENT_HIGH_ALERT_LEVEL:
      {
        *((uint16_t *)(pSOCPAppEvent->EventData.p_Payload)) = CGMS_APP_Context.HighAlertLevel;
      }
      break;
      
    case CGMS_SOCP_SET_PATIENT_LOW_ALERT_LEVEL:
      {
        if(*((uint16_t *)(pSOCPAppEvent->EventData.p_Payload)) < MAX_ALERT_LEVEL)
        {
          CGMS_APP_Context.LowAlertLevel = *((uint16_t *)(pSOCPAppEvent->EventData.p_Payload));
        }
        else
        {
          return FALSE;
        }
      }
      break;
      
    case CGMS_SOCP_GET_PATIENT_LOW_ALERT_LEVEL:
      {
        *((uint16_t *)(pSOCPAppEvent->EventData.p_Payload)) = CGMS_APP_Context.LowAlertLevel;
      }
      break;
      
    case CGMS_SOCP_SET_PATIENT_HYPO_ALERT_LEVEL:
      {
        if(*((uint16_t *)(pSOCPAppEvent->EventData.p_Payload)) < MAX_ALERT_LEVEL)
        {
          CGMS_APP_Context.HypoAlertLevel = *((uint16_t *)(pSOCPAppEvent->EventData.p_Payload));
        }
        else
        {
          return FALSE;
        }
      }
      break;
      
    case CGMS_SOCP_GET_PATIENT_HYPO_ALERT_LEVEL:
      {
        *((uint16_t *)(pSOCPAppEvent->EventData.p_Payload)) = CGMS_APP_Context.HypoAlertLevel;
      }
      break;
      
    case CGMS_SOCP_SET_PATIENT_HYPER_ALERT_LEVEL:
      {
        if(*((uint16_t *)(pSOCPAppEvent->EventData.p_Payload)) < MAX_ALERT_LEVEL)
        {
          CGMS_APP_Context.HyperAlertLevel = *((uint16_t *)(pSOCPAppEvent->EventData.p_Payload));
        }
        else
        {
          return FALSE;
        }
      }
      break;
      
    case CGMS_SOCP_GET_PATIENT_HYPER_ALERT_LEVEL:
      {
        *((uint16_t *)(pSOCPAppEvent->EventData.p_Payload)) = CGMS_APP_Context.HyperAlertLevel;
      }
      break;
      
    case CGMS_SOCP_SET_RATE_OF_DECREASE_ALERT_LEVEL:
      {
        if(*((uint16_t *)(pSOCPAppEvent->EventData.p_Payload)) < MAX_ALERT_LEVEL)
        {
          CGMS_APP_Context.DecreaseRateAlertLevel = *((uint16_t *)(pSOCPAppEvent->EventData.p_Payload));
        }
        else
        {
          return FALSE;
        }
      }
      break;
      
    case CGMS_SOCP_GET_RATE_OF_DECREASE_ALERT_LEVEL:
      {
        *((uint16_t *)(pSOCPAppEvent->EventData.p_Payload)) = CGMS_APP_Context.DecreaseRateAlertLevel;
      }
      break;
      
    case CGMS_SOCP_SET_RATE_OF_INCREASE_ALERT_LEVEL:
      {
        if(*((uint16_t *)(pSOCPAppEvent->EventData.p_Payload)) < MAX_ALERT_LEVEL)
        {
          CGMS_APP_Context.IncreaseRateAlertLevel = *((uint16_t *)(pSOCPAppEvent->EventData.p_Payload));
        }
        else
        {
          return FALSE;
        }
      }
      break;
      
    case CGMS_SOCP_GET_RATE_OF_INCREASE_ALERT_LEVEL:
      {
        *((uint16_t *)(pSOCPAppEvent->EventData.p_Payload)) = CGMS_APP_Context.IncreaseRateAlertLevel;
      }
      break;
      
    default:
      break;
  }
  
  return TRUE;
}

uint8_t CGMS_RACP_APP_EventHandler(CGMS_RACP_App_Event_t *pRACPAppEvent)
{
  switch(pRACPAppEvent->EventCode)
  {      
    case CGMS_RACP_APP_REPORT_RECORDS_PROCEDURE_STARTED_EVENT:
      {
        LOG_INFO_APP("CGMS_RACP_APP_REPORT_RECORDS_PROCEDURE_STARTED_EVENT\r\n"); 

        UTIL_TIMER_Start(&(CGMS_APP_Context.TimerRACPProcess_Id));
      }
      break;
      
    case CGMS_RACP_APP_REPORT_RECORDS_PROCEDURE_FINISHED_EVENT:
      {
        LOG_INFO_APP("CGMS_RACP_APP_REPORT_RECORDS_PROCEDURE_FINISHED_EVENT\r\n");
      }
      break;
      
    case CGMS_RACP_APP_REPORT_RECORDS_PROCEDURE_NOTIFY_NEXT_RECORD_EVENT:
      {
        LOG_INFO_APP("CGMS_RACP_APP_REPORT_RECORDS_PROCEDURE_NOTIFY_NEXT_RECORD_EVENT\r\n"); 

        UTIL_TIMER_Start(&(CGMS_APP_Context.TimerRACPProcess_Id));
      }
      break;
      
    default:
      break;
  }
  
  return TRUE;
}

/**
  * @brief Get the flag holding whether CGM SOCP characteristic indication is enabled or not
  * @param None
  * @retval None
  */
uint8_t CGMS_APP_GetCGMSOCPCharacteristicIndicationEnabled(void)
{
  return ((CGMS_APP_Context.Cfocp_Indication_Status == Cfocp_INDICATION_ON) ? TRUE: FALSE);
} /* end of CGMS_APP_GetCGMSOCPCharacteristicIndicationEnabled() */

/**
  * @brief Get the flag holding whether any RACP characteristic indication is enabled or not
  * @param None
  * @retval None
  */
uint8_t CGMS_APP_GetRACPCharacteristicIndicationEnabled(void)
{
  return ((CGMS_APP_Context.Racp_Indication_Status == Racp_INDICATION_ON) ? TRUE: FALSE);
} /* end of CGMS_APP_GetRACPCharacteristicIndicationEnabled() */

/**
  * @brief Update Feature characteristic
  * @param None
  * @retval None
  */
void CGMS_APP_UpdateFeature(void)
{
  CGMS_Data_t msg_conf;
  uint8_t cgm_char_length = 0;
  tBleStatus ret;
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  uint16_t CRCValue;
#endif
  
  /* Configure available Features, Sample location and CGM System Type */
  CGMS_APP_Context.CGMFeature.Feature = 0;
  CGMS_APP_Context.CGMFeature.Feature |= CGMS_CGM_FEATURE_CALIBRATION                           |
                                         CGMS_CGM_FEATURE_PATIENT_LOW_HIGH_ALERTS               |
                                         CGMS_CGM_FEATURE_HYPO_ALERTS                           |
                                         CGMS_CGM_FEATURE_HYPER_ALERTS                          |
                                         CGMS_CGM_FEATURE_RATE_OF_INCREASE_DECREASE_ALERTS      |
                                         CGMS_CGM_FEATURE_DEVICE_SPECIFIC_ALERT                 |
                                         CGMS_CGM_FEATURE_SENSOR_MALFUNCTION_DETECTION          |
                                         CGMS_CGM_FEATURE_SENSOR_TEMPERATURE_HIGH_LOW_DETECTION |
                                         CGMS_CGM_FEATURE_SENSOR_RESULT_HIGH_LOW_DETECTION      |
                                         CGMS_CGM_FEATURE_LOW_BATTERY_DETECTION                 |
                                         CGMS_CGM_FEATURE_SENSOR_TYPE_ERROR_DETECTION           |
                                         CGMS_CGM_FEATURE_GENERAL_DEVICE_FAULT                  |
                                         CGMS_CGM_FEATURE_E2E_CRC                               |
                                         CGMS_CGM_FEATURE_MULTIPLE_BOND                         |
                                         CGMS_CGM_FEATURE_MULTIPLE_SESSIONS                     |
                                         CGMS_CGM_FEATURE_CGM_TREND_INFORMATION                 |
                                         CGMS_CGM_FEATURE_CGM_QUALITY;
  CGMS_APP_Context.CGMFeature.SampleLocation = BLE_CFG_CGMS_CGM_SAMPLE_LOCATION;
  CGMS_APP_Context.CGMFeature.Type = BLE_CFG_CGMS_CGM_TYPE;
  if(((CGMS_APP_Context.CGMFeature.Feature) & CGMS_CGM_FEATURE_E2E_CRC) != (CGMS_CGM_FEATURE_E2E_CRC))
    CRCValue = 0xFFFF;

  cgm_char_length = 0;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(CGMS_APP_Context.CGMFeature.Feature & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((CGMS_APP_Context.CGMFeature.Feature >> 8) & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((CGMS_APP_Context.CGMFeature.Feature >> 16) & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((CGMS_APP_Context.CGMFeature.Type) | ((CGMS_APP_Context.CGMFeature.SampleLocation) << 4));
  cgm_char_length++;
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  CRCValue = CGMS_APP_ComputeCRC(a_CGMS_UpdateCharData, cgm_char_length);
#endif
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(CRCValue & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((CRCValue >> 8) & 0xFF);
  cgm_char_length++;
  msg_conf.Length = cgm_char_length;
  msg_conf.p_Payload = a_CGMS_UpdateCharData;
  ret = CGMS_UpdateValue(CGMS_CGF, &msg_conf);
  if(ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("CGMS_CGF update fails\n");
  }

} /* end of CGMS_APP_GetRACPCharacteristicIndicationEnabled() */
/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void CGMS_Cgm_SendNotification(void) /* Property Notification */
{
  CGMS_APP_SendInformation_t notification_on_off = Cgm_NOTIFICATION_OFF;
  CGMS_Data_t cgms_notification_data;

  cgms_notification_data.p_Payload = (uint8_t*)a_CGMS_UpdateCharData;
  cgms_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char1_NS_1*/

  /* USER CODE END Service1Char1_NS_1*/

  if (notification_on_off != Cgm_NOTIFICATION_OFF)
  {
    CGMS_UpdateValue(CGMS_CGM, &cgms_notification_data);
  }

  /* USER CODE BEGIN Service1Char1_NS_Last*/

  /* USER CODE END Service1Char1_NS_Last*/

  return;
}

__USED void CGMS_Cgf_SendIndication(void) /* Property Indication */
{
  CGMS_APP_SendInformation_t indication_on_off = Cgf_INDICATION_OFF;
  CGMS_Data_t cgms_indication_data;

  cgms_indication_data.p_Payload = (uint8_t*)a_CGMS_UpdateCharData;
  cgms_indication_data.Length = 0;

  /* USER CODE BEGIN Service1Char2_IS_1*/

  /* USER CODE END Service1Char2_IS_1*/

  if (indication_on_off != Cgf_INDICATION_OFF)
  {
    CGMS_UpdateValue(CGMS_CGF, &cgms_indication_data);
  }

  /* USER CODE BEGIN Service1Char2_IS_Last*/

  /* USER CODE END Service1Char2_IS_Last*/

  return;
}

__USED void CGMS_Racp_SendIndication(void) /* Property Indication */
{
  CGMS_APP_SendInformation_t indication_on_off = Racp_INDICATION_OFF;
  CGMS_Data_t cgms_indication_data;

  cgms_indication_data.p_Payload = (uint8_t*)a_CGMS_UpdateCharData;
  cgms_indication_data.Length = 0;

  /* USER CODE BEGIN Service1Char6_IS_1*/

  /* USER CODE END Service1Char6_IS_1*/

  if (indication_on_off != Racp_INDICATION_OFF)
  {
    CGMS_UpdateValue(CGMS_RACP, &cgms_indication_data);
  }

  /* USER CODE BEGIN Service1Char6_IS_Last*/

  /* USER CODE END Service1Char6_IS_Last*/

  return;
}

__USED void CGMS_Cfocp_SendIndication(void) /* Property Indication */
{
  CGMS_APP_SendInformation_t indication_on_off = Cfocp_INDICATION_OFF;
  CGMS_Data_t cgms_indication_data;

  cgms_indication_data.p_Payload = (uint8_t*)a_CGMS_UpdateCharData;
  cgms_indication_data.Length = 0;

  /* USER CODE BEGIN Service1Char7_IS_1*/

  /* USER CODE END Service1Char7_IS_1*/

  if (indication_on_off != Cfocp_INDICATION_OFF)
  {
    CGMS_UpdateValue(CGMS_CFOCP, &cgms_indication_data);
  }

  /* USER CODE BEGIN Service1Char7_IS_Last*/

  /* USER CODE END Service1Char7_IS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/
static void cgmsapp_task_glucose_measurement(void)
{
  CGMS_RACP_Record_t record;
  uint8_t cgm_char_length;
  
#if (CFG_LED_SUPPORTED == 1)
    BSP_LED_On(LED_RED);
#endif
  
  if (CGMS_APP_Context.CGMCommunicationIntervalInMins == 0xFF)
  {
    CGMS_APP_Context.CurrentTimeOffset += BLE_CFG_CGMS_SHORTEST_CGM_COMM_INTERVAL_SUPPORTED_BY_DEVICE_IN_MINS;
  }
  else 
  {
    CGMS_APP_Context.CurrentTimeOffset += CGMS_APP_Context.CGMCommunicationIntervalInMins;
  }
  
  cgm_char_length = 0;
  record.measurement.Size = DummyRecords[dummyRecordsPosition].measurement.Size;
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  record.measurement.Size = (record.measurement.Size) + 2;
#endif
  a_CGMS_UpdateCharData[cgm_char_length] = record.measurement.Size;
  cgm_char_length++;

  record.measurement.Flags = DummyRecords[dummyRecordsPosition].measurement.Flags;
  if (CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Warning != 0)
  {
    record.measurement.Flags |= CGMS_FLAGS_WARNING_OCTET_PRESENT;
  }
  
  if (CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.CalTemp != 0)
  {
    record.measurement.Flags |= CGMS_FLAGS_CAL_TEMP_OCTET_PRESENT;
  }
  
  if (CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Status != 0)
  {
    record.measurement.Flags |= CGMS_FLAGS_MEASUREMENT_STATUS_OCTET_PRESENT;
  }
  
  a_CGMS_UpdateCharData[cgm_char_length] = record.measurement.Flags;
  cgm_char_length++;
  
  record.measurement.GlucoseConcentration = DummyRecords[dummyRecordsPosition].measurement.GlucoseConcentration;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(record.measurement.GlucoseConcentration & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(record.measurement.GlucoseConcentration >> 8);
  cgm_char_length++;
  
  record.measurement.TimeOffset = CGMS_APP_Context.CurrentTimeOffset;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(record.measurement.TimeOffset & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(record.measurement.TimeOffset >> 8);
  cgm_char_length++;
  
  record.measurement.SensorStatusAnnunciation.Warning = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Warning;
  if(record.measurement.SensorStatusAnnunciation.Warning == 0)
    record.measurement.SensorStatusAnnunciation.Warning = 1;
  a_CGMS_UpdateCharData[cgm_char_length] = record.measurement.SensorStatusAnnunciation.Warning;
  cgm_char_length++;
  record.measurement.SensorStatusAnnunciation.CalTemp = DummyRecords[dummyRecordsPosition].measurement.SensorStatusAnnunciation.CalTemp;
  record.measurement.SensorStatusAnnunciation.CalTemp = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.CalTemp;
  if(record.measurement.SensorStatusAnnunciation.CalTemp == 0)
    record.measurement.SensorStatusAnnunciation.CalTemp = 1;
  a_CGMS_UpdateCharData[cgm_char_length] = record.measurement.SensorStatusAnnunciation.CalTemp;
  cgm_char_length++;
  record.measurement.SensorStatusAnnunciation.Status = DummyRecords[dummyRecordsPosition].measurement.SensorStatusAnnunciation.Status;
  record.measurement.SensorStatusAnnunciation.Status = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Status;
  if(record.measurement.SensorStatusAnnunciation.Status == 0)
    record.measurement.SensorStatusAnnunciation.Status = 1;
  a_CGMS_UpdateCharData[cgm_char_length] = record.measurement.SensorStatusAnnunciation.Status;
  cgm_char_length++;
  
#if (BLE_CFG_CGMS_CGM_FEATURE_CGM_QUALITY == 1)
  record.measurement.CgmQuality =  DummyRecords[dummyRecordsPosition].measurement.CgmQuality;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(record.measurement.CgmQuality & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((record.measurement.CgmQuality >> 8) & 0xFF);
  cgm_char_length++;
#endif
  
#if (BLE_CFG_CGMS_CGM_FEATURE_CGM_TREND_INFORMATION == 1)
  record.measurement.CgmTrendInformation = DummyRecords[dummyRecordsPosition].measurement.CgmTrendInformation;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(record.measurement.CgmTrendInformation & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((record.measurement.CgmTrendInformation >> 8) & 0xFF);
  cgm_char_length++;
#endif
  
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  record.CRCValue = CGMS_APP_ComputeCRC(a_CGMS_UpdateCharData, cgm_char_length);
  
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(record.CRCValue & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((record.CRCValue >> 8) & 0xFF);
  cgm_char_length++;
#endif
  
  CGMS_DB_AddRecord(&record);
  
  if (CGMS_RACP_GetState() != CGMS_RACP_STATE_PROCEDURE_IN_PROGRESS)
  {
    // STM_TODO : Implement a FIFO, since in case the RACP operation is in progress, the new measurement characteristic value should be added to some FIFO (buffer) and notified after the RACP operation is finished
    CGMS_Data_t msg_conf;
    
    msg_conf.Length = cgm_char_length;
    msg_conf.p_Payload = a_CGMS_UpdateCharData;
    CGMS_UpdateValue(CGMS_CGM, &msg_conf);
  }
  
  if (dummyRecordsPosition < (DUMMY_RECORDS_COUNT-1))
  {
    dummyRecordsPosition++;
  }
  else 
  {
    dummyRecordsPosition = 0;
  }
  
#if (CFG_LED_SUPPORTED == 1)
  BSP_LED_Off(LED_RED);
#endif
  
  return;
}

static void cgmsapp_start_session( void )
{    
  CGMS_Data_t msg_conf;
  uint8_t cgm_char_length;
  uint8_t temp;
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  uint16_t CRCValue;
#endif
  
  LOG_INFO_APP("cgmsapp_start_session\r\n");
  
  UTIL_TIMER_Stop(&(CGMS_APP_Context.TimerGlucoseMeasurement_Id));
  if (CGMS_APP_Context.CGMCommunicationIntervalInMins == 0)
  {
    /* Do nothing - Timer already stopped */
  }
  else 
  {
#if (BLE_CFG_CGMS_DEBUG != 1)
    /* Re-initialize the CGMS records database */
    CGMS_DB_Init();
    CGMS_APP_Context.CurrentTimeOffset = 0;
#endif
    
    if (CGMS_APP_Context.CGMCommunicationIntervalInMins == 0xFF)
    {
#if (BLE_CFG_CGMS_DEBUG == 1)
      /* 10sec rate */
      UTIL_TIMER_StartWithPeriod( &(CGMS_APP_Context.TimerGlucoseMeasurement_Id), CGMS_APP_MEASUREMENT_INTERVAL);
#else
      UTIL_TIMER_StartWithPeriod( &(CGMS_APP_Context.TimerGlucoseMeasurement_Id), 
                                 BLE_CFG_CGMS_SHORTEST_CGM_COMM_INTERVAL_SUPPORTED_BY_DEVICE_IN_MINS*60*1000);
#endif
    }
    else 
    {
      UTIL_TIMER_StartWithPeriod( &(CGMS_APP_Context.TimerGlucoseMeasurement_Id), 
                                 CGMS_APP_Context.CGMCommunicationIntervalInMins*60*1000);
    }
    
    cgm_char_length = 0;
    a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(CGMS_APP_Context.CGMStatus.TimeOffset & 0xFF);
    cgm_char_length++;
    a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(CGMS_APP_Context.CGMStatus.TimeOffset >> 8);
    cgm_char_length++;
    temp = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Warning;
    temp &= ~(CGMS_WARNING_OCTET_SESSION_STOPPED);
    CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Warning = temp;
    a_CGMS_UpdateCharData[cgm_char_length] = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Warning;
    cgm_char_length++;
    a_CGMS_UpdateCharData[cgm_char_length] = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.CalTemp;
    cgm_char_length++;
    a_CGMS_UpdateCharData[cgm_char_length] = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Status;
    cgm_char_length++;
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
    CRCValue = CGMS_APP_ComputeCRC(a_CGMS_UpdateCharData, cgm_char_length);

    a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(CRCValue & 0xFF);
    cgm_char_length++;
    a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((CRCValue >> 8) & 0xFF);
    cgm_char_length++;
#endif

    msg_conf.Length = cgm_char_length;
    msg_conf.p_Payload = a_CGMS_UpdateCharData;
    CGMS_UpdateValue(CGMS_CGS, &msg_conf);

#if (CFG_LED_SUPPORTED == 1)
    BSP_LED_On(LED_BLUE);
#endif
  }
}

static void cgmsapp_stop_session( void )
{
  CGMS_Data_t msg_conf;
  uint8_t cgm_char_length;
  uint8_t temp;
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  uint16_t CRCValue;
#endif

  LOG_INFO_APP("cgmsapp_stop_session\r\n");

  UTIL_TIMER_Stop(&(CGMS_APP_Context.TimerGlucoseMeasurement_Id));
  cgm_char_length = 0;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(CGMS_APP_Context.CGMStatus.TimeOffset & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(CGMS_APP_Context.CGMStatus.TimeOffset >> 8);
  cgm_char_length++;

  temp = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Warning;
  temp |= CGMS_WARNING_OCTET_SESSION_STOPPED;
  CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Warning = temp;
  a_CGMS_UpdateCharData[cgm_char_length] = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Warning;
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.CalTemp;
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Status;
  cgm_char_length++;
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  CRCValue = CGMS_APP_ComputeCRC(a_CGMS_UpdateCharData, cgm_char_length);

  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)(CRCValue & 0xFF);
  cgm_char_length++;
  a_CGMS_UpdateCharData[cgm_char_length] = (uint8_t)((CRCValue >> 8) & 0xFF);
  cgm_char_length++;
#endif

  msg_conf.Length = cgm_char_length;
  msg_conf.p_Payload = a_CGMS_UpdateCharData;
  CGMS_UpdateValue(CGMS_CGS, &msg_conf);
  
#if (CFG_LED_SUPPORTED == 1)
  BSP_LED_Off(LED_BLUE);
#endif
}

static void cgmsapp_task_battery_measurement(void)
{
  /* Place your code here */
  uint8_t temp;
  
  temp = CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Warning;
  temp |= CGMS_WARNING_OCTET_DEVICE_BATTERY_LOW;
  CGMS_APP_Context.CGMStatus.SensorStatusAnnunciation.Warning = temp;
}

static void cgmsapp_timer_handler_measure_battery(void *arg)
{
  /**
  * The code shall be executed in the background as aci command may be sent
  * The background is the only place where the application can make sure a new aci command
  * is not sent if there is a pending one
  */
  UTIL_SEQ_SetTask( 1<<CFG_TASK_BATTERY_MEASUREMENT_ID, CFG_SEQ_PRIO_0);
}

static void cgmsapp_timer_handler_measure_glucose(void *arg)
{
  /**
  * The code shall be executed in the background as aci command may be sent
  * The background is the only place where the application can make sure a new aci command
  * is not sent if there is a pending one
  */
  UTIL_SEQ_SetTask( 1<<CFG_TASK_GLUCOSE_MEASUREMENT_ID, CFG_SEQ_PRIO_0);
  
  return;
}

static void cgmsapp_timer_handler_racp_process(void *arg)
{
  /**
  * The code shall be executed in the background as aci command may be sent
  * The background is the only place where the application can make sure a new aci command
  * is not sent if there is a pending one
  */
  UTIL_SEQ_SetTask( 1<<CFG_TASK_RACP_PROCESS_REPORT_RECORD_ID, CFG_SEQ_PRIO_0);
}

uint32_t CGMS_APP_ComputeCRC(uint8_t * pData, uint8_t dataLength)
{
  CRCCTRL_Cmd_Status_t result;
  uint32_t crcValue;
  // STM_TODO : Return value of both HAL functions should be further tested
  // STM_TODO : Add HW semaphore for CRC
  
  result = CRCCTRL_Calculate (&CGM_Handle,
                              (uint32_t *)pData,
                              dataLength,
                              &crcValue);
  
  if (result != CRCCTRL_OK)
  {
    Error_Handler();
  }
  return crcValue;
}

static void cgmsapp_task_racp_process_report_record(void)
{
  CGMS_RACP_ProcessReportRecordsProcedure();
}

/* USER CODE END FD_LOCAL_FUNCTIONS*/
