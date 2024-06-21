
/**
  ******************************************************************************
  * @file    sgms_socp.h
  * @author  MCD Application Team
  * @brief   Header for cgms_socp.c module
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
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CGMS_SOCP_H
#define __CGMS_SOCP_H

#ifdef __cplusplus
extern "C" 
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "cgms.h"
#include "cgms_app.h" 

/* Exported defines-----------------------------------------------------------*/
#define BLE_CFG_CGMS_SOCP_OP_CODE_START_SESSION                                1
#define BLE_CFG_CGMS_SOCP_OP_CODE_STOP_SESSION                                 1
#define BLE_CFG_CGMS_SOCP_OP_CODE_GET_GLUCOSE_CALIBRATION_VALUE                1
#define BLE_CFG_CGMS_SOCP_OP_CODE_SET_GLUCOSE_CALIBRATION_VALUE                1
#define BLE_CFG_CGMS_SOCP_OP_CODE_GET_CGM_COMMUNICATION_INTERVAL               1
#define BLE_CFG_CGMS_SOCP_OP_CODE_SET_CGM_COMMUNICATION_INTERVAL               1
#define BLE_CFG_CGMS_SOCP_OP_CODE_SET_PATIENT_HIGH_ALERT_LEVEL                 1
#define BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_HIGH_ALERT_LEVEL                 1
#define BLE_CFG_CGMS_SOCP_OP_CODE_SET_PATIENT_LOW_ALERT_LEVEL                  1
#define BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_LOW_ALERT_LEVEL                  1
#define BLE_CFG_CGMS_SOCP_OP_CODE_SET_PATIENT_HYPO_ALERT_LEVEL                 1
#define BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_HYPO_ALERT_LEVEL                 1
#define BLE_CFG_CGMS_SOCP_OP_CODE_SET_PATIENT_HYPER_ALERT_LEVEL                1
#define BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_HYPER_ALERT_LEVEL                1
#define BLE_CFG_CGMS_SOCP_OP_CODE_SET_RATE_OF_DECREASE_ALERT_LEVEL             1
#define BLE_CFG_CGMS_SOCP_OP_CODE_GET_RATE_OF_DECREASE_ALERT_LEVEL             1
#define BLE_CFG_CGMS_SOCP_OP_CODE_SET_RATE_OF_INCREASE_ALERT_LEVEL             1
#define BLE_CFG_CGMS_SOCP_OP_CODE_GET_RATE_OF_INCREASE_ALERT_LEVEL             1
#define BLE_CFG_CGMS_SOCP_OP_CODE_GET_RATE_OF_INCREASE_ALERT_LEVEL             1
#define BLE_CFG_CGMS_SOCP_OP_CODE_RESET_DEVICE_SPECIFIC_ALERT_LEVEL            1
  
/******** CGM Specific Ops Control Point (SOCP) characteristic *****/

/* CGM Specific Ops Control Point: Op Code Values */
/* SOCP requests */
/* Mandatory */
#define CGMS_SOCP_OP_CODE_SET_CGM_COMMUNICATION_INTERVAL         (0x01)
#define CGMS_SOCP_OP_CODE_GET_CGM_COMMUNICATION_INTERVAL         (0x02)
  
/* Optional */
#define CGMS_SOCP_OP_CODE_SET_GLUCOSE_CALIBRATION_VALUE          (0x04) 
#define CGMS_SOCP_OP_CODE_GET_GLUCOSE_CALIBRATION_VALUE          (0x05) 
#define CGMS_SOCP_OP_CODE_SET_PATIENT_HIGH_ALERT_LEVEL           (0x07)
#define CGMS_SOCP_OP_CODE_GET_PATIENT_HIGH_ALERT_LEVEL           (0x08)
#define CGMS_SOCP_OP_CODE_SET_PATIENT_LOW_ALERT_LEVEL            (0x0A)
#define CGMS_SOCP_OP_CODE_GET_PATIENT_LOW_ALERT_LEVEL            (0x0B)
#define CGMS_SOCP_OP_CODE_SET_PATIENT_HYPO_ALERT_LEVEL           (0x0D)
#define CGMS_SOCP_OP_CODE_GET_PATIENT_HYPO_ALERT_LEVEL           (0x0E)
#define CGMS_SOCP_OP_CODE_SET_PATIENT_HYPER_ALERT_LEVEL          (0x10)
#define CGMS_SOCP_OP_CODE_GET_PATIENT_HYPER_ALERT_LEVEL          (0x11)
#define CGMS_SOCP_OP_CODE_SET_RATE_OF_DECREASE_ALERT_LEVEL       (0x13)
#define CGMS_SOCP_OP_CODE_GET_RATE_OF_DECREASE_ALERT_LEVEL       (0x14)
#define CGMS_SOCP_OP_CODE_SET_RATE_OF_INCREASE_ALERT_LEVEL       (0x16)
#define CGMS_SOCP_OP_CODE_GET_RATE_OF_INCREASE_ALERT_LEVEL       (0x17)
#define CGMS_SOCP_OP_CODE_RESET_DEVICE_SPECIFIC_ALERT_LEVEL      (0x19) 
#define CGMS_SOCP_OP_CODE_START_SESSION                          (0x1A)
#define CGMS_SOCP_OP_CODE_STOP_SESSION                           (0x1B)

/* SOCP responses */
/* Mandatory */
#define CGMS_SOCP_OP_CODE_COMMUNICATION_INTERVAL_RESPONSE        (0x03)
#define CGMS_SOCP_OP_CODE_RESPONSE_CODE                          (0x1C)
  
/* Optional */
#define CGMS_SOCP_OP_CODE_CALIBRATION_VALUE_RESPONSE             (0x06)
#define CGMS_SOCP_OP_CODE_PATIENT_HIGH_ALERT_LEVEL_RESPONSE      (0x09)
#define CGMS_SOCP_OP_CODE_PATIENT_LOW_ALERT_LEVEL_RESPONSE       (0x0C)
#define CGMS_SOCP_OP_CODE_PATIENT_HYPO_ALERT_LEVEL_RESPONSE      (0x0F)
#define CGMS_SOCP_OP_CODE_PATIENT_HYPER_ALERT_LEVEL_RESPONSE     (0x12)
#define CGMS_SOCP_OP_CODE_RATE_OF_DECREASE_ALERT_LEVEL_RESPONSE  (0x15)
#define CGMS_SOCP_OP_CODE_RATE_OF_INCREASE_ALERT_LEVEL_RESPONSE  (0x18)

/* CGM Specific Ops Control Point: types valid length */
#define CGMS_SOCP_OP_CODE_LENGTH                                           (1)
#define CGMS_SOCP_OP_CODE_CGM_COMMUNICATION_INTERVAL_OPERAND_LENGTH        (1)
#define CGMS_SOCP_OP_CODE_RESPONSE_OPERAND_LENGTH                          (2)
#define CGMS_SOCP_OP_CODE_SET_CALIBRATION_RECORD_REQUEST_OPERAND_LENGTH    (sizeof(CGMS_SOCP_CalibrationDataRecord_t))
#define CGMS_SOCP_OP_CODE_GET_CALIBRATION_RECORD_REQUEST_OPERAND_LENGTH    (sizeof(uint16_t))
#define CGMS_SOCP_OP_CODE_GET_CALIBRATION_RECORD_RESPONSE_OPERAND_LENGTH   (CGMS_SOCP_OP_CODE_SET_CALIBRATION_RECORD_REQUEST_OPERAND_LENGTH)
#define CGMS_SOCP_OP_CODE_SET_ALERT_LEVEL_REQUEST_OPERAND_LENGTH           (sizeof(uint16_t))
#define CGMS_SOCP_OP_CODE_GET_ALERT_LEVEL_REQUEST_OPERAND_LENGTH           (sizeof(uint16_t))
#define CGMS_SOCP_OP_CODE_GET_ALERT_LEVEL_RESPONSE_OPERAND_LENGTH          (CGMS_SOCP_OP_CODE_SET_ALERT_LEVEL_REQUEST_OPERAND_LENGTH)
#define CGMS_SOCP_OP_CODE_SET_RATE_ALERT_LEVEL_REQUEST_OPERAND_LENGTH      (sizeof(uint16_t))
#define CGMS_SOCP_OP_CODE_GET_RATE_ALERT_LEVEL_REQUEST_OPERAND_LENGTH      (sizeof(uint16_t))
#define CGMS_SOCP_OP_CODE_GET_RATE_ALERT_LEVEL_RESPONSE_OPERAND_LENGTH     (CGMS_SOCP_OP_CODE_SET_RATE_ALERT_LEVEL_REQUEST_OPERAND_LENGTH)
#define CGMS_SOCP_OP_CODE_OPERAND_MAX_LENGTH                               (CGMS_SOCP_OP_CODE_SET_CALIBRATION_RECORD_REQUEST_OPERAND_LENGTH)
#define CGMS_SOCP_REQUEST_MIN_LENGTH                                       (CGMS_SOCP_OP_CODE_LENGTH)
#define CGMS_SOCP_RESPONSE_MAX_LENGTH                                      (CGMS_SOCP_OP_CODE_LENGTH + CGMS_SOCP_OP_CODE_OPERAND_MAX_LENGTH)
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
#define CGMS_SOCP_CRC_LENGTH                                              (sizeof(uint16_t))
#else
#define CGMS_SOCP_CRC_LENGTH                                              (0)
#endif
  
/* CGM Specific Ops Control Point: fields position */
#define CGMS_SOCP_OP_CODE_POSITION     (0)
#define CGMS_SOCP_OPERAND_POSITION     (CGMS_SOCP_OP_CODE_LENGTH)

/* CGM Specific Ops Control Point: responses values */
#define CGMS_SOCP_RESPONSE_CODE_SUCCESS                                  (0x01)
#define CGMS_SOCP_RESPONSE_CODE_OPCODE_NOT_SUPPORTED                     (0x02)
#define CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND                          (0x03)
#define CGMS_SOCP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED                  (0x04)
#define CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE                   (0x05)

/* Exported types ------------------------------------------------------------*/
  
typedef enum
{
  CGMS_SOCP_STATE_IDLE,
  CGMS_SOCP_STATE_PROCEDURE_IN_PROGRESS,
  CGMS_SOCP_STATE_PROCEDURE_FINISHED,
  CGMS_SOCP_STATE_PROCEDURE_RESPONSE_PENDING,
  CGMS_SOCP_STATE_WAITING_FOR_RESPONSE_ACKNOWLEDGE
} CGMS_SOCP_State_t;

typedef __PACKED_STRUCT
{
  SFLOAT GlucoseConcentrationOfCalibration;
  uint16_t CalibrationTime;
  uint8_t CalibrationTypeSampleLocation;
  uint16_t NextCalibrationTime;
  uint16_t CalibrationDataRecordNumber;
  uint8_t CalibrationStatus;
} CGMS_SOCP_CalibrationDataRecord_t;

typedef uint16_t CGMS_SOCP_CalibrationDataRecordNumber_t;
typedef uint8_t CGMS_SOCP_CommunicationInterval_t;

typedef __PACKED_STRUCT
{
  uint8_t RequestOpCode;
  uint8_t ResponseCode;
} CGMS_SOCP_GeneralResponse_t;

typedef __PACKED_STRUCT
{
  uint8_t OpCode;
  uint8_t Operand[CGMS_SOCP_OP_CODE_OPERAND_MAX_LENGTH];
} CGMS_SOCP_Procedure_t;

typedef __PACKED_STRUCT
{
  uint8_t OpCode;
  uint8_t Operand[CGMS_SOCP_OP_CODE_OPERAND_MAX_LENGTH];
} CGMS_SOCP_Response_t;

typedef enum
{
  CGMS_SOCP_SET_CGM_COMMUNICATION_INTERVAL_EVENT,
  CGMS_SOCP_GET_CGM_COMMUNICATION_INTERVAL_EVENT,
  CGMS_SOCP_SET_GLUCOSE_CALIBRATION_VALUE_EVENT,
  CGMS_SOCP_GET_GLUCOSE_CALIBRATION_VALUE_EVENT,
  CGMS_SOCP_START_SESSION_EVENT,
  CGMS_SOCP_STOP_SESSION_EVENT,
  CGMS_SOCP_RESET_DEVICE_SPECIFIC_ALERT_LEVEL_EVENT,
  CGMS_SOCP_GET_PATIENT_HIGH_ALERT_LEVEL,
  CGMS_SOCP_SET_PATIENT_HIGH_ALERT_LEVEL,
  CGMS_SOCP_GET_PATIENT_LOW_ALERT_LEVEL,
  CGMS_SOCP_SET_PATIENT_LOW_ALERT_LEVEL,
  CGMS_SOCP_GET_PATIENT_HYPO_ALERT_LEVEL,
  CGMS_SOCP_SET_PATIENT_HYPO_ALERT_LEVEL,
  CGMS_SOCP_GET_PATIENT_HYPER_ALERT_LEVEL,
  CGMS_SOCP_SET_PATIENT_HYPER_ALERT_LEVEL,
  CGMS_SOCP_GET_RATE_OF_DECREASE_ALERT_LEVEL,
  CGMS_SOCP_SET_RATE_OF_DECREASE_ALERT_LEVEL,
  CGMS_SOCP_GET_RATE_OF_INCREASE_ALERT_LEVEL,
  CGMS_SOCP_SET_RATE_OF_INCREASE_ALERT_LEVEL
} CGMS_SOCP_App_EventCode_t;

typedef struct
{
  CGMS_SOCP_App_EventCode_t      EventCode;
  CGMS_Data_t                    EventData;
  uint16_t                       ConnectionHandle;
  uint8_t                        ServiceInstance;
} CGMS_SOCP_App_Event_t;

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void CGMS_SOCP_Init(void);
void CGMS_SOCP_RequestHandler(uint8_t * pRequestData, uint8_t requestDataLength);
void CGMS_SOCP_AcknowledgeHandler(void);
uint8_t CGMS_SOCP_CheckRequestValid(uint8_t * pRequestData, uint8_t requestDataLength);
uint8_t CGMS_SOCP_APP_EventHandler(CGMS_SOCP_App_Event_t * pNotification);

#endif /* __CGMS_SOCP_H */
