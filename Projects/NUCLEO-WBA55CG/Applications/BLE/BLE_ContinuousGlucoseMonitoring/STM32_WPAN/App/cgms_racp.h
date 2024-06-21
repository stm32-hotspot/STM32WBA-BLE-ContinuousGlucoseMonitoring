
/**
  ******************************************************************************
  * @file    cgms_racp.h
  * @author  MCD Application Team
  * @brief   Header for cgms_racp.c module
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
#ifndef __CGMS_RACP_H
#define __CGMS_RACP_H

#ifdef __cplusplus
extern "C" 
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "cgms.h"
  
/* Exported defines ----------------------------------------------------------*/
/******** Glucose record access control point (RACP) characteristic *****/

/* Record Access Control Point: Op Code Values */
/* RACP requests */
/* Mandatory */
#define CGMS_RACP_OP_CODE_REPORT_STORED_RECORDS          (0x01)
#define CGMS_RACP_OP_CODE_ABORT_OPERATION                (0x03)
#define CGMS_RACP_OP_CODE_REPORT_NUMBER_STORED_RECORDS   (0x04)
#define CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS          (0x02)
  
/* RACP responses */
#define CGMS_RACP_OP_CODE_NUMBER_STORED_RECORDS_RESPONSE (0x05)
#define CGMS_RACP_OP_CODE_RESPONSE_CODE                  (0x06)
 
/* Record Access Control Point: Operator Values */
/* Mandatory */
#define CGMS_RACP_OPERATOR_NULL          (0x00)
#define CGMS_RACP_OPERATOR_ALL_RECORDS   (0x01)  
#define CGMS_RACP_OPERATOR_GREATER_EQUAL (0x03)

/* Optional */
#define CGMS_RACP_OPERATOR_LESS_EQUAL    (0x02)
#define CGMS_RACP_OPERATOR_WITHIN_RANGE  (0x04)
#define CGMS_RACP_OPERATOR_FIRST_RECORD  (0x05)
#define CGMS_RACP_OPERATOR_LAST_RECORD   (0x06)

/* Record Access Control Point: filter types values */
#define CGMS_RACP_FILTER_TYPE_NONE             (0x00)
#define CGMS_RACP_FILTER_TYPE_TIME_OFFSET      (0x01)

/* Record Access Control Point: filter types valid length */
#define CGMS_RACP_FILTER_TYPE_NO_OPERAND_LENGTH                    (2)
#define CGMS_RACP_FILTER_TYPE_LESS_GREATER_TIME_OFFSET_LENGTH      (5)
#define CGMS_RACP_FILTER_TYPE_WITHIN_RANGE_TIME_OFFSET_LENGTH      (7)

/* Record Access Control Point: fields position */
#define CGMS_RACP_OP_CODE_POSITION       (0)
#define CGMS_RACP_OPERATOR_POSITION      (1)
#define CGMS_RACP_OPERAND_POSITION       (2)

/* Record Access Control Point: responses values */
#define CGMS_RACP_RESPONSE_SUCCESS                                     (0x01)
#define CGMS_RACP_RESPONSE_OPCODE_NOT_SUPPORTED                        (0x02)
#define CGMS_RACP_RESPONSE_INVALID_OPERATOR                            (0x03)
#define CGMS_RACP_RESPONSE_OPERATOR_NOT_SUPPORTED                      (0x04)
#define CGMS_RACP_RESPONSE_INVALID_OPERAND                             (0x05)
#define CGMS_RACP_RESPONSE_NO_RECORDS                                  (0x06) 
#define CGMS_RACP_RESPONSE_ABORT_FAILED                                (0x07) 
#define CGMS_RACP_RESPONSE_PROCEDURE_NOT_COMPLETED                     (0x08)
#define CGMS_RACP_RESPONSE_OPERAND_NOT_SUPPORTED                       (0x09) /* TBR:CGMS_RACP_RESPONSE_FILTER_TYPE_NOT_SUPPORTED */

#define CGMS_RACP_COMMAND_MAX_LEN                (17) /* TBC (Within Range with Time)*/
#define CGMS_RACP_RESPONSE_LENGTH                (4)  /* RACP response length */

#define CGMS_RACP_OP_CODE_REQUEST_OPERAND_MAX_LENGTH                        (5)
#define CGMS_RACP_OP_CODE_RESPONSE_OPERAND_MAX_LENGTH                       (2)
  
/* Exported types ------------------------------------------------------------*/

typedef __PACKED_STRUCT
{
  uint8_t FilterType;
  uint16_t Minimum;
  uint16_t Maximum;
} CGMS_RACP_TimeOffsetFilter_t;
  
typedef __PACKED_STRUCT
{
  uint8_t FilterType;
  uint8_t FilterParam[sizeof(CGMS_RACP_TimeOffsetFilter_t)];
} CGMS_RACP_Filter_t;

typedef struct {
  CGMS_MeasurementValue_t measurement;
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  uint16_t CRCValue;
#endif
} CGMS_RACP_Record_t;

typedef enum
{
  CGMS_RACP_STATE_IDLE,
  CGMS_RACP_STATE_PROCEDURE_IN_PROGRESS,
  CGMS_RACP_STATE_PROCEDURE_ABORT_RECEIVED
} CGMS_RACP_State_t;

typedef enum
{
  CGMS_RACP_APP_REPORT_RECORDS_PROCEDURE_STARTED_EVENT,
  CGMS_RACP_APP_REPORT_RECORDS_PROCEDURE_FINISHED_EVENT,
  CGMS_RACP_APP_REPORT_RECORDS_PROCEDURE_NOTIFY_NEXT_RECORD_EVENT
} CGMS_RACP_App_EventCode_t;

typedef struct
{
  CGMS_RACP_App_EventCode_t      EventCode;
  CGMS_Data_t                    EventData;
  uint16_t                       ConnectionHandle;
  uint8_t                        ServiceInstance;
} CGMS_RACP_App_Event_t;

/* Exported constants ------------------------------------------------------*/
/* External variables ------------------------------------------------------*/
/* Exported macros ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------*/

void CGMS_RACP_Init(void);
void CGMS_RACP_RequestHandler(uint8_t * pRequestData, uint8_t requestDataLength);
uint8_t CGMS_RACP_CheckRequestValid(uint8_t * pRequestData, uint8_t requestDataLength);
void CGMS_RACP_AcknowledgeHandler(void);
uint8_t CGMS_RACP_ProcessReportRecordsProcedure(void);
uint8_t CGMS_RACP_SendNextRecord(void);
CGMS_RACP_State_t CGMS_RACP_GetState(void);
uint8_t CGMS_RACP_APP_EventHandler(CGMS_RACP_App_Event_t * pNotification);
tBleStatus cgms_racp_send_response_code(uint8_t responseCode);

#endif /* __CGMS_RACP_H */
