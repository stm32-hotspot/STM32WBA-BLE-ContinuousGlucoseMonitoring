/**
  ******************************************************************************
  * @file    cgms_racp.c
  * @author  MCD Application Team
  * @brief   CGMS RACP
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


/* Includes ------------------------------------------------------------------*/
#include "common_blesvc.h"
#include "cgms_app.h"
#include "cgms_racp.h"
#include "cgms_db.h"

/* Private typedef -----------------------------------------------------------*/
#define BLE_CFG_CGMS_RACP_OP_CODE_REPORT_STORED_RECORDS_LESS_EQUAL             1
#define BLE_CFG_CGMS_RACP_OP_CODE_REPORT_STORED_RECORDS_WITHIN_RANGE           1
#define BLE_CFG_CGMS_RACP_OP_CODE_REPORT_STORED_RECORDS_FIRST                  1
#define BLE_CFG_CGMS_RACP_OP_CODE_REPORT_STORED_RECORDS_LAST                   1

#define BLE_CFG_CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS                        1   
#define BLE_CFG_CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS_LESS_EQUAL             1
#define BLE_CFG_CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS_WITHIN_RANGE           1
#define BLE_CFG_CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS_FIRST                  1
#define BLE_CFG_CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS_LAST                   1

#define BLE_CFG_CGMS_RACP_OP_CODE_REPORT_NUMBER_STORED_RECORDS_LESS_EQUAL      1   
#define BLE_CFG_CGMS_RACP_OP_CODE_REPORT_NUMBER_STORED_RECORDS_WITHIN_RANGE    1
#define BLE_CFG_CGMS_RACP_OP_CODE_REPORT_NUMBER_STORED_RECORDS_FIRST           1
#define BLE_CFG_CGMS_RACP_OP_CODE_REPORT_NUMBER_STORED_RECORDS_LAST            1
    
/* typedef used for storing the specific filter type to be used */
typedef __PACKED_STRUCT
{
  uint8_t OpCode;
  uint8_t Operator;
  uint8_t Operand[CGMS_RACP_OP_CODE_REQUEST_OPERAND_MAX_LENGTH];
} CGMS_RACP_Procedure_t;

typedef __PACKED_STRUCT
{
  uint16_t NumberOfStoredRecords;
} CGMS_RACP_NumberOfStoredRecordsResponseOperand;

typedef __PACKED_STRUCT
{
  uint8_t RequestOpCode;
  uint8_t ResponseCodeValue;
} CGMS_RACP_ResponseCodeResponseOperand;

typedef __PACKED_STRUCT
{
  uint8_t OpCode;
  const uint8_t Operator; /* always 0 */
  uint8_t Operand[CGMS_RACP_OP_CODE_RESPONSE_OPERAND_MAX_LENGTH];
} CGMS_RACP_Response_t;

typedef struct
{
  CGMS_RACP_Procedure_t Procedure;
  CGMS_RACP_Response_t Response;
  CGMS_RACP_State_t State;
} CGMS_RACP_Context_t;

/* Private variables ---------------------------------------------------------*/
CGMS_RACP_Record_t record;
FlagStatus recordSent = SET;

/**
* START of Section BLE_DRIVER_CONTEXT
*/
PLACE_IN_SECTION("BLE_DRIVER_CONTEXT") static CGMS_RACP_Context_t CGMS_RACP_Context;

/* Private functions ---------------------------------------------------------*/
extern tBleStatus cgms_update_racp_characteristic(uint8_t * pData, uint8_t dataLength);
extern uint8_t a_CGMS_UpdateCharData[247];

/* RACP response functions */
tBleStatus cgms_racp_send_number_of_stored_records_response(uint16_t numberOfRecords);

/* RACP filter functions */
uint8_t cgms_racp_timeoffset_is_greater_or_equal(const CGMS_RACP_Record_t * record, const CGMS_RACP_Filter_t * filter);
uint8_t cgms_racp_timeoffset_is_less_or_equal(const CGMS_RACP_Record_t * record, const CGMS_RACP_Filter_t * filter);
uint8_t cgms_racp_timeoffset_is_within_range(const CGMS_RACP_Record_t * record, const CGMS_RACP_Filter_t * filter);
tBleStatus cgms_racp_updateCGMMeasurement(CGMS_MeasurementValue_t *pMeasurement);

/**
* @brief Check if the record matches greater or equal criteria
* @param record : Record to be checked
* @param filter : Filtering criteria against which the record it tested
* @retval TRUE if matches, FALSE (!TRUE) otherwise
*/
uint8_t cgms_racp_timeoffset_is_greater_or_equal(const CGMS_RACP_Record_t * record, const CGMS_RACP_Filter_t * filter)
{
  if (record->measurement.TimeOffset >= ((CGMS_RACP_TimeOffsetFilter_t *)filter)->Minimum)
  {
    return TRUE;
  }
  
  return FALSE;
} /* end of cgms_racp_timeoffset_is_greater_or_equal() */

/**
* @brief Check if the record matches less or equal criteria
* @param record : Record to be checked
* @param filter : Filtering criteria against which the record it tested
* @retval TRUE if matches, FALSE (!TRUE) otherwise
*/
uint8_t cgms_racp_timeoffset_is_less_or_equal(const CGMS_RACP_Record_t * record, const CGMS_RACP_Filter_t * filter)
{
  if (record->measurement.TimeOffset <= ((CGMS_RACP_TimeOffsetFilter_t *)filter)->Maximum)
  {
    return TRUE;
  }
  
  return FALSE;
} /* end of cgms_racp_timeoffset_is_less_or_equal() */

/**
* @brief Check if the record matches within range criteria
* @param record : Record to be checked
* @param filter : Filtering criteria against which the record it tested
* @retval TRUE if matches, FALSE (!TRUE) otherwise
*/
uint8_t cgms_racp_timeoffset_is_within_range(const CGMS_RACP_Record_t * record, const CGMS_RACP_Filter_t * filter)
{
  if ((cgms_racp_timeoffset_is_greater_or_equal(record, filter) == TRUE)
      && (cgms_racp_timeoffset_is_less_or_equal(record, filter) == TRUE))
  {
    return TRUE;
  }
  
  return FALSE;
} /* end of cgms_racp_timeoffset_is_within_range() */

/**
* @brief Send a RACP response as consequence of a RACP request
* @param responseCode: RACP response code or number of stored records
* @retval None
*/
tBleStatus cgms_racp_send_response_code(uint8_t responseCode)
{
  uint8_t responseData[CGMS_RACP_RESPONSE_LENGTH];
  tBleStatus retval = BLE_STATUS_FAILED;
  
  CGMS_RACP_Context.Response.OpCode = CGMS_RACP_OP_CODE_RESPONSE_CODE;
  ((CGMS_RACP_ResponseCodeResponseOperand *)CGMS_RACP_Context.Response.Operand)->RequestOpCode = CGMS_RACP_Context.Procedure.OpCode;
  ((CGMS_RACP_ResponseCodeResponseOperand *)CGMS_RACP_Context.Response.Operand)->ResponseCodeValue = responseCode;
  
  responseData[0] = CGMS_RACP_Context.Response.OpCode;
  responseData[1] = CGMS_RACP_OPERATOR_NULL;
  responseData[2] = CGMS_RACP_Context.Procedure.OpCode;
  responseData[3] = responseCode;
  
  /* RACP response is indicated */
  retval = cgms_update_racp_characteristic(responseData, CGMS_RACP_RESPONSE_LENGTH);
  
  if(retval == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("RACP Response, OpCode: 0x%02X > Request OpCode: 0x%02X, Response Code: 0x%02X\r\n", responseData[0], responseData[2], responseData[3]);
    LOG_INFO_APP("RACP STATE = RACP_PROCEDURE_FINISHED\r\n");
    
    /* unset flag for RACP operation already in progress */
    CGMS_RACP_Context.State = CGMS_RACP_STATE_IDLE;
    // STM_TODO : Add App event callback
  }
  else 
  {
    LOG_INFO_APP("FAILED to send RACP response 0x%02X\r\n", retval);
    // STM_TODO : Implement error management, error management to be further extended, no propagation currently
  }
  
  return retval;
} /* end of cgms_racp_send_response_code() */

/**
* @brief Send requested number of stored records matching the given criteria
* @param numberOfRecords: number of stored records
* @retval None
*/
tBleStatus cgms_racp_send_number_of_stored_records_response(uint16_t numberOfRecords)
{
  uint8_t responseData[CGMS_RACP_RESPONSE_LENGTH];
  tBleStatus retval = BLE_STATUS_FAILED;
  
  CGMS_RACP_Context.Response.OpCode = CGMS_RACP_OP_CODE_NUMBER_STORED_RECORDS_RESPONSE;
  ((CGMS_RACP_NumberOfStoredRecordsResponseOperand *)CGMS_RACP_Context.Response.Operand)->NumberOfStoredRecords = numberOfRecords;
  
  responseData[0] = CGMS_RACP_Context.Response.OpCode;
  responseData[1] = CGMS_RACP_OPERATOR_NULL;
  responseData[2] = (uint8_t)(0x00FF & numberOfRecords);
  responseData[3] = (uint8_t)(numberOfRecords >> 8);
  
  /* RACP response is indicated */
  retval = cgms_update_racp_characteristic(responseData, CGMS_RACP_RESPONSE_LENGTH);
  
  if(retval == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("RACP Response, OpCode: 0x%02X > Number of records: %d\r\n", responseData[0], (uint16_t)responseData[2]);
    LOG_INFO_APP("RACP STATE = RACP_PROCEDURE_FINISHED\r\n");
    
    /* unset flag for RACP operation already in progress */
    CGMS_RACP_Context.State = CGMS_RACP_STATE_IDLE;
    // STM_TODO : Add App event callback
  }
  else 
  {
    LOG_INFO_APP("FAILED to send RACP response 0x%02X\r\n", retval);
    // STM_TODO : Implement error management, error management to be further extended, no propagation currently
  }
  
  return retval;
} /* end of cgms_racp_send_number_of_stored_records_response() */

tBleStatus cgms_racp_updateCGMMeasurement(CGMS_MeasurementValue_t *pMeasurement)
{
  CGMS_Data_t msg_conf;
  uint8_t cgmmeasurement_char_length = 0;
  tBleStatus retval = BLE_STATUS_FAILED;
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  uint16_t CRCValue;
#endif
  
  a_CGMS_UpdateCharData[cgmmeasurement_char_length] = (uint8_t)(pMeasurement->Size);
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  a_CGMS_UpdateCharData[cgmmeasurement_char_length] += 2;
#endif  
  cgmmeasurement_char_length++;
  a_CGMS_UpdateCharData[cgmmeasurement_char_length] = (uint8_t)(pMeasurement->Flags);
  cgmmeasurement_char_length++;
  a_CGMS_UpdateCharData[cgmmeasurement_char_length] = (uint8_t)(pMeasurement->GlucoseConcentration & 0x00FF);
  cgmmeasurement_char_length++;
  a_CGMS_UpdateCharData[cgmmeasurement_char_length] = (uint8_t)(pMeasurement->GlucoseConcentration >> 8);
  cgmmeasurement_char_length++;
  a_CGMS_UpdateCharData[cgmmeasurement_char_length] = (uint8_t)(pMeasurement->TimeOffset & 0x00FF);
  cgmmeasurement_char_length++;
  a_CGMS_UpdateCharData[cgmmeasurement_char_length] = (uint8_t)(pMeasurement->TimeOffset >> 8);
  cgmmeasurement_char_length++;
  
  if (pMeasurement->SensorStatusAnnunciation.Warning != 0)
  {
    a_CGMS_UpdateCharData[cgmmeasurement_char_length] = (uint8_t)(pMeasurement->SensorStatusAnnunciation.Warning);
    cgmmeasurement_char_length++;
  }
  
  if (pMeasurement->SensorStatusAnnunciation.CalTemp != 0)
  {
    a_CGMS_UpdateCharData[cgmmeasurement_char_length] = (uint8_t)(pMeasurement->SensorStatusAnnunciation.CalTemp);
    cgmmeasurement_char_length++;
  }
  
  if (pMeasurement->SensorStatusAnnunciation.Status != 0)
  {
    a_CGMS_UpdateCharData[cgmmeasurement_char_length] = (uint8_t)(pMeasurement->SensorStatusAnnunciation.Status);
    cgmmeasurement_char_length++;
  }
  
#if (BLE_CFG_CGMS_CGM_FEATURE_CGM_QUALITY == 1)
  a_CGMS_UpdateCharData[cgmmeasurement_char_length] = (uint8_t)(pMeasurement->CgmQuality & 0x00FF);
  cgmmeasurement_char_length++;
  a_CGMS_UpdateCharData[cgmmeasurement_char_length] = (uint8_t)(pMeasurement->CgmQuality >> 8);
  cgmmeasurement_char_length++;
#endif
#if (BLE_CFG_CGMS_CGM_FEATURE_CGM_TREND_INFORMATION == 1)
  a_CGMS_UpdateCharData[cgmmeasurement_char_length] = (uint8_t)(pMeasurement->CgmTrendInformation & 0x00FF);
  cgmmeasurement_char_length++;
  a_CGMS_UpdateCharData[cgmmeasurement_char_length] = (uint8_t)(pMeasurement->CgmTrendInformation >> 8);
  cgmmeasurement_char_length++;
#endif
  
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  CRCValue = CGMS_APP_ComputeCRC(a_CGMS_UpdateCharData, cgmmeasurement_char_length);
  a_CGMS_UpdateCharData[cgmmeasurement_char_length] = CRCValue;
  cgmmeasurement_char_length++;
  a_CGMS_UpdateCharData[cgmmeasurement_char_length] = CRCValue >> 8;
  cgmmeasurement_char_length++;
#endif
  
  msg_conf.Length = cgmmeasurement_char_length;
  msg_conf.p_Payload = a_CGMS_UpdateCharData;
  retval = CGMS_UpdateValue(CGMS_CGM, &msg_conf);
  
  return retval;
} /* end of CGMS_UpdateCGMMeasurementCharacteristic() */

/* Public functions ----------------------------------------------------------*/

/**
* @brief Initialize the RACP Context state and configuration
* @param None
* @retval None
*/
void CGMS_RACP_Init(void)
{
  CGMS_RACP_Context.State = CGMS_RACP_STATE_IDLE;
  memset(&CGMS_RACP_Context.Procedure, 0x00, sizeof(CGMS_RACP_Procedure_t));
  memset(&CGMS_RACP_Context.Response, 0x00, sizeof(CGMS_RACP_Response_t));
} /* end of CGMS_RACP_Init() */

/**
* @brief RACP Long Procedure processing function 
* @param None
* @retval TRUE if OK and still pending, FALSE if no Long Procedure or not pending anymore
*/
uint8_t CGMS_RACP_ProcessReportRecordsProcedure(void)
{
  uint8_t retval = FALSE;
  tBleStatus ble_status;
  CGMS_RACP_App_Event_t RACPAppEvent;
  
  if (CGMS_RACP_Context.State != CGMS_RACP_STATE_IDLE)
  {
    if (CGMS_RACP_Context.State != CGMS_RACP_STATE_PROCEDURE_ABORT_RECEIVED)
    {
      switch(CGMS_RACP_Context.Procedure.OpCode)
      {
        case CGMS_RACP_OP_CODE_REPORT_STORED_RECORDS:
          {
            if (recordSent == SET)
            {
              if (CGMS_DB_GetNextSelectedRecord(&record) == TRUE)
              {
                LOG_INFO_APP("CGMS RACP next selected record found in DB\r\n");
                ble_status = cgms_racp_updateCGMMeasurement(&(record.measurement));
                // STM_TODO - would be worth to change the behavior, so if BLE operation fails, it can retry to send out the same record again, the best might be that the CGMS_UpdateCGMMeasurementCharacteristic would be called from the APP layer only
                if (ble_status == BLE_STATUS_SUCCESS)
                {
                  recordSent = SET;
                  LOG_INFO_APP("CGMS RACP measurement record sent successfully, TimeOffset = %d\r\n", record.measurement.TimeOffset);
                  RACPAppEvent.EventCode = CGMS_RACP_APP_REPORT_RECORDS_PROCEDURE_NOTIFY_NEXT_RECORD_EVENT;
                  CGMS_RACP_APP_EventHandler(&RACPAppEvent);
                  retval = TRUE;
                }
                else 
                {
                  /* Last recording sending failed */
                  recordSent = RESET;
                  // STM_TODO : Implement error management, error management to be further extended, no propagation currently
                  retval = FALSE;
                }
              }
              else 
              {
                /* No further record selected */
                LOG_INFO_APP("CGMS RACP no more selected record found in DB\r\n");
                RACPAppEvent.EventCode = CGMS_RACP_APP_REPORT_RECORDS_PROCEDURE_FINISHED_EVENT;
                CGMS_RACP_APP_EventHandler(&RACPAppEvent);
                cgms_racp_send_response_code(CGMS_RACP_RESPONSE_SUCCESS);
                retval = FALSE;
              }
            }
            else 
            {
              ble_status = cgms_racp_updateCGMMeasurement(&(record.measurement));
              // STM_TODO - would be worth to change the behavior, so if BLE operation fails, it can retry to send out the same record again, the best might be that the CGMS_UpdateCGMMeasurementCharacteristic would be called from the APP layer only
              if (ble_status == BLE_STATUS_SUCCESS)
              {
                recordSent = SET;
                LOG_INFO_APP("CGMS RACP first measurement record sent successfully, TimeOffset = %d\r\n", record.measurement.TimeOffset);
                retval = TRUE;
              }
              else 
              {
                /* Last recording sending failed */
                LOG_INFO_APP("CGMS RACP last measurement record sent failed\r\n");
                recordSent = RESET;
                // STM_TODO : Implement error management, error management to be further extended, no propagation currently
                retval = FALSE;
              }
            }
          }
          break;
          
        default:
          {
            retval = FALSE;
          }
          break;
      }
    }
    else 
    {
      LOG_INFO_APP("CGMS_RACP_STATE_PROCEDURE_ABORT_RECEIVED\r\n");
      /* No further record to be sent */
      RACPAppEvent.EventCode = CGMS_RACP_APP_REPORT_RECORDS_PROCEDURE_FINISHED_EVENT;
      CGMS_RACP_APP_EventHandler(&RACPAppEvent);
      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_SUCCESS);
    }
  }
  
  return retval;
} /* end of CGMS_RACP_ProcessReportRecordsProcedure() */

/**
* @brief RACP request handler 
* @param requestData: pointer to received RACP request data
* @param requestDataLength: received RACP request length
* @retval None
*/
void CGMS_RACP_RequestHandler(uint8_t * pRequestData, uint8_t requestDataLength)
{
  uint16_t recordsNum = 0;
  CGMS_RACP_App_Event_t RACPAppEvent;
  
  LOG_INFO_APP("RACP Request, request data length: %d\r\n", requestDataLength);
  
  if ((CGMS_RACP_Context.State != CGMS_RACP_STATE_IDLE) && ((pRequestData[CGMS_RACP_OP_CODE_POSITION]) != CGMS_RACP_OP_CODE_ABORT_OPERATION))
  {
    // STM_TODO : Implement error management, error management to be further extended, no propagation currently
  }
  else 
  {
    if (requestDataLength < CGMS_RACP_FILTER_TYPE_NO_OPERAND_LENGTH)
    {
      /* RACP procedure data must be always at least 2 bytes long */
      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_OPCODE_NOT_SUPPORTED);
    }
    else 
    {
      CGMS_RACP_Context.Procedure.OpCode = pRequestData[CGMS_RACP_OP_CODE_POSITION];
      CGMS_RACP_Context.Procedure.Operator = pRequestData[CGMS_RACP_OPERATOR_POSITION];
      LOG_INFO_APP("RACP Procedure OpCode: 0x%02X, Operator: 0x%02X\r\n", CGMS_RACP_Context.Procedure.OpCode, CGMS_RACP_Context.Procedure.OpCode);
      memset(&CGMS_RACP_Context.Procedure.Operand, 0x00, CGMS_RACP_OP_CODE_RESPONSE_OPERAND_MAX_LENGTH);
      
      /* Set in progress flag on reception of a new RACP request */
      CGMS_RACP_Context.State = CGMS_RACP_STATE_PROCEDURE_IN_PROGRESS;
      LOG_INFO_APP("RACP STATE = RACP_PROCEDURE_IN_PROGRESS\r\n");
      
      /* Check and Process the OpCode */
      switch(CGMS_RACP_Context.Procedure.OpCode)
      {
        case CGMS_RACP_OP_CODE_REPORT_STORED_RECORDS:
          {
            LOG_INFO_APP("CGMS_RACP_REPORT_STORED_RECORDS_OP_CODE\r\n");
            switch(CGMS_RACP_Context.Procedure.Operator)
            {
              case CGMS_RACP_OPERATOR_ALL_RECORDS:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_ALL_RECORDS\r\n");
                  if (requestDataLength != CGMS_RACP_FILTER_TYPE_NO_OPERAND_LENGTH)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                  }
                  else 
                  {
                    recordsNum = CGMS_DB_SelectRecords(NULL, NULL);
                    if (recordsNum > 0)
                    {
                      RACPAppEvent.EventCode = CGMS_RACP_APP_REPORT_RECORDS_PROCEDURE_STARTED_EVENT;
                      CGMS_RACP_APP_EventHandler(&RACPAppEvent);
                    }
                    else 
                    {
                      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_NO_RECORDS);
                    }
                  }
                }
                break;
                
              case CGMS_RACP_OPERATOR_GREATER_EQUAL:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_GREATER_EQUAL\r\n");
                  LOG_INFO_APP("requestDataLength %d CGMS_RACP_FILTER_TYPE_LESS_GREATER_TIME_OFFSET_LENGTH %d\r\n",
                               requestDataLength, CGMS_RACP_FILTER_TYPE_LESS_GREATER_TIME_OFFSET_LENGTH);
                  LOG_INFO_APP("pRequestData[CGMS_RACP_OPERAND_POSITION] %d CGMS_RACP_FILTER_TYPE_TIME_OFFSET %d\r\n",
                               pRequestData[CGMS_RACP_OPERAND_POSITION], CGMS_RACP_FILTER_TYPE_TIME_OFFSET);
                  if (requestDataLength != CGMS_RACP_FILTER_TYPE_LESS_GREATER_TIME_OFFSET_LENGTH)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                  }
                  else if (pRequestData[CGMS_RACP_OPERAND_POSITION] != CGMS_RACP_FILTER_TYPE_TIME_OFFSET)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_OPERAND_NOT_SUPPORTED);
                  }
                  else 
                  {
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->FilterType = pRequestData[CGMS_RACP_OPERAND_POSITION];
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Minimum = 
                      (uint16_t)(pRequestData[CGMS_RACP_OPERAND_POSITION+1] | (pRequestData[CGMS_RACP_OPERAND_POSITION+2] << 8));
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Maximum = 0xFFFF;
                    recordsNum = CGMS_DB_SelectRecords(&cgms_racp_timeoffset_is_greater_or_equal, (const CGMS_RACP_Filter_t *)(&(CGMS_RACP_Context.Procedure.Operand)));
                    if (recordsNum > 0)
                    {     
                      RACPAppEvent.EventCode = CGMS_RACP_APP_REPORT_RECORDS_PROCEDURE_STARTED_EVENT;
                      CGMS_RACP_APP_EventHandler(&RACPAppEvent);
                    }
                    else 
                    {
                      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_NO_RECORDS);
                    }
                  }
                }
                break;
                
#if (BLE_CFG_CGMS_RACP_OP_CODE_REPORT_STORED_RECORDS_LESS_EQUAL == 1)
              case CGMS_RACP_OPERATOR_LESS_EQUAL:
                {
                  LOG_INFO_APP("BLE_CFG_CGMS_RACP_OP_CODE_REPORT_STORED_RECORDS_LESS_EQUAL\r\n");
                  if (requestDataLength != CGMS_RACP_FILTER_TYPE_LESS_GREATER_TIME_OFFSET_LENGTH)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                  }
                  else if (pRequestData[CGMS_RACP_OPERAND_POSITION] != CGMS_RACP_FILTER_TYPE_TIME_OFFSET)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_OPERAND_NOT_SUPPORTED);
                  }
                  else 
                  {
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->FilterType = pRequestData[CGMS_RACP_OPERAND_POSITION];
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Minimum = 0x0000;
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Maximum = 
                      (uint16_t)(pRequestData[CGMS_RACP_OPERAND_POSITION+1] | (pRequestData[CGMS_RACP_OPERAND_POSITION+2] << 8));
                    recordsNum = CGMS_DB_SelectRecords(&cgms_racp_timeoffset_is_less_or_equal, (const CGMS_RACP_Filter_t *)(&(CGMS_RACP_Context.Procedure.Operand)));
                    if (recordsNum > 0)
                    {
                      RACPAppEvent.EventCode = CGMS_RACP_APP_REPORT_RECORDS_PROCEDURE_STARTED_EVENT;
                      CGMS_RACP_APP_EventHandler(&RACPAppEvent);
                    }
                    else 
                    {
                      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_NO_RECORDS);
                    }
                  }
                }
                break;
#endif /* BLE_CFG_CGMS_RACP_OP_CODE_REPORT_STORED_RECORDS_LESS_EQUAL */
                
#if (BLE_CFG_CGMS_RACP_OP_CODE_REPORT_STORED_RECORDS_WITHIN_RANGE == 1)
              case CGMS_RACP_OPERATOR_WITHIN_RANGE:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_WITHIN_RANGE\r\n");
                  if (requestDataLength != CGMS_RACP_FILTER_TYPE_WITHIN_RANGE_TIME_OFFSET_LENGTH)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                  }
                  else if (pRequestData[CGMS_RACP_OPERAND_POSITION] != CGMS_RACP_FILTER_TYPE_TIME_OFFSET)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_OPERAND_NOT_SUPPORTED);
                  }
                  else 
                  {
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->FilterType = pRequestData[CGMS_RACP_OPERAND_POSITION];
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Minimum = 
                      (uint16_t)(pRequestData[CGMS_RACP_OPERAND_POSITION+1] | (pRequestData[CGMS_RACP_OPERAND_POSITION+2] << 8));
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Maximum = 
                      (uint16_t)(pRequestData[CGMS_RACP_OPERAND_POSITION+3] | (pRequestData[CGMS_RACP_OPERAND_POSITION+4] << 8));
                    if (((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Minimum > ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Maximum)
                    {
                      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                    }
                    else 
                    {
                      recordsNum = CGMS_DB_SelectRecords(&cgms_racp_timeoffset_is_within_range, (const CGMS_RACP_Filter_t *)(&(CGMS_RACP_Context.Procedure.Operand)));
                      if (recordsNum > 0)
                      {
                        RACPAppEvent.EventCode = CGMS_RACP_APP_REPORT_RECORDS_PROCEDURE_STARTED_EVENT;
                        CGMS_RACP_APP_EventHandler(&RACPAppEvent);
                      }
                      else 
                      {
                        cgms_racp_send_response_code(CGMS_RACP_RESPONSE_NO_RECORDS);
                      }
                    }
                  }
                }
                break;
#endif /* BLE_CFG_CGMS_RACP_OP_CODE_REPORT_STORED_RECORDS_WITHIN_RANGE */
                
#if (BLE_CFG_CGMS_RACP_OP_CODE_REPORT_STORED_RECORDS_FIRST == 1)
              case CGMS_RACP_OPERATOR_FIRST_RECORD:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_FIRST_RECORD\r\n");
                  if (requestDataLength != CGMS_RACP_FILTER_TYPE_NO_OPERAND_LENGTH)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                  }
                  else 
                  {
                    recordsNum = CGMS_DB_SelectRecordByIndex(0x0000);
                    if (recordsNum > 0)
                    {
                      RACPAppEvent.EventCode = CGMS_RACP_APP_REPORT_RECORDS_PROCEDURE_STARTED_EVENT;
                      CGMS_RACP_APP_EventHandler(&RACPAppEvent);
                    }
                    else 
                    {
                      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_NO_RECORDS);
                    }
                  }
                }
                break;
#endif /* BLE_CFG_CGMS_RACP_OP_CODE_REPORT_STORED_RECORDS_FIRST */
                
#if (BLE_CFG_CGMS_RACP_OP_CODE_REPORT_STORED_RECORDS_LAST == 1)
              case CGMS_RACP_OPERATOR_LAST_RECORD:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_LAST_RECORD\r\n");
                  if (requestDataLength != CGMS_RACP_FILTER_TYPE_NO_OPERAND_LENGTH)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                  }
                  else 
                  {
                    recordsNum = CGMS_DB_SelectRecordByIndex(0xFFFF);
                    if (recordsNum > 0)
                    {
                      RACPAppEvent.EventCode = CGMS_RACP_APP_REPORT_RECORDS_PROCEDURE_STARTED_EVENT;
                      CGMS_RACP_APP_EventHandler(&RACPAppEvent);
                    }
                    else 
                    {
                      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_NO_RECORDS);
                    }
                  }
                }
                break;
#endif /* BLE_CFG_CGMS_RACP_OP_CODE_REPORT_STORED_RECORDS_LAST */
                
              case CGMS_RACP_OPERATOR_NULL:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_NULL\r\n");
                  cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERATOR);
                }
                break;
                
              default:
                {
                  cgms_racp_send_response_code(CGMS_RACP_RESPONSE_OPERATOR_NOT_SUPPORTED);
                }
                break;
            }
          }
          break;
        
#if (BLE_CFG_CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS == 1)
        case CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS:
          {
            LOG_INFO_APP("CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS\r\n"); 
            switch(CGMS_RACP_Context.Procedure.Operator)
            {
              case CGMS_RACP_OPERATOR_ALL_RECORDS:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_ALL_RECORDS\r\n"); 
                  if (requestDataLength != CGMS_RACP_FILTER_TYPE_NO_OPERAND_LENGTH)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                  }
                  else 
                  {
                    recordsNum = CGMS_DB_DeleteRecords(NULL, NULL);
                    if (recordsNum > 0)
                    {
                      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_SUCCESS);
                    }
                    else 
                    {
                      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_NO_RECORDS);
                    }
                  }
                }
                break;
                
#if (BLE_CFG_CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS_GREATER_EQUAL == 1)
              case CGMS_RACP_OPERATOR_GREATER_EQUAL:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_GREATER_EQUAL\r\n"); 
                  if (requestDataLength != CGMS_RACP_FILTER_TYPE_LESS_GREATER_TIME_OFFSET_LENGTH)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                  }
                  else if (pRequestData[CGMS_RACP_OPERAND_POSITION] != CGMS_RACP_FILTER_TYPE_TIME_OFFSET)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_OPERAND_NOT_SUPPORTED);
                  }
                  else 
                  {
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->FilterType = pRequestData[CGMS_RACP_OPERAND_POSITION];
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Minimum = 
                      (uint16_t)(pRequestData[CGMS_RACP_OPERAND_POSITION+1] | (pRequestData[CGMS_RACP_OPERAND_POSITION+2] << 8));
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Maximum = 0xFFFF;
                    recordsNum = CGMS_DB_DeleteRecords(&cgms_racp_timeoffset_is_greater_or_equal, (const CGMS_RACP_Filter_t *)(&(CGMS_RACP_Context.Procedure.Operand)));
                    if (recordsNum > 0)
                    {
                      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_SUCCESS);
                    }
                    else 
                    {
                      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_NO_RECORDS);
                    }
                  }
                }
                break;
#endif /* BLE_CFG_CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS_GREATER_EQUAL */
                
#if (BLE_CFG_CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS_LESS_EQUAL == 1)
              case CGMS_RACP_OPERATOR_LESS_EQUAL:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_LESS_EQUAL\r\n"); 
                  if (requestDataLength != CGMS_RACP_FILTER_TYPE_LESS_GREATER_TIME_OFFSET_LENGTH)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                  }
                  else if (pRequestData[CGMS_RACP_OPERAND_POSITION] != CGMS_RACP_FILTER_TYPE_TIME_OFFSET)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_OPERAND_NOT_SUPPORTED);
                  }
                  else 
                  {
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->FilterType = pRequestData[CGMS_RACP_OPERAND_POSITION];
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Minimum = 0x0000;
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Maximum = 
                      (uint16_t)(pRequestData[CGMS_RACP_OPERAND_POSITION+1] | (pRequestData[CGMS_RACP_OPERAND_POSITION+2] << 8));
                    recordsNum = CGMS_DB_DeleteRecords(&cgms_racp_timeoffset_is_less_or_equal, (const CGMS_RACP_Filter_t *)(&(CGMS_RACP_Context.Procedure.Operand)));
                    if (recordsNum > 0)
                    {
                      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_SUCCESS);
                    }
                    else 
                    {
                      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_NO_RECORDS);
                    }
                  }
                }
                break;
#endif /* BLE_CFG_CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS_LESS_EQUAL */
                
#if (BLE_CFG_CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS_WITHIN_RANGE == 1)
              case CGMS_RACP_OPERATOR_WITHIN_RANGE:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_WITHIN_RANGE\r\n"); 
                  if (requestDataLength != CGMS_RACP_FILTER_TYPE_WITHIN_RANGE_TIME_OFFSET_LENGTH)
                  {
                    LOG_INFO_APP("CGMS_RACP_RESPONSE_INVALID_OPERAND\r\n"); 
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                  }
                  else if (pRequestData[CGMS_RACP_OPERAND_POSITION] != CGMS_RACP_FILTER_TYPE_TIME_OFFSET)
                  {
                    LOG_INFO_APP("CGMS_RACP_RESPONSE_OPERAND_NOT_SUPPORTED\r\n"); 
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_OPERAND_NOT_SUPPORTED);
                  }
                  else 
                  {
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->FilterType = pRequestData[CGMS_RACP_OPERAND_POSITION];
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Minimum = 
                      (uint16_t)(pRequestData[CGMS_RACP_OPERAND_POSITION+1] | (pRequestData[CGMS_RACP_OPERAND_POSITION+2] << 8));
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Maximum = 
                      (uint16_t)(pRequestData[CGMS_RACP_OPERAND_POSITION+3] | (pRequestData[CGMS_RACP_OPERAND_POSITION+4] << 8));
                    if (((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Minimum > ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Maximum)
                    {
                      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                      LOG_INFO_APP("CGMS_RACP_RESPONSE_INVALID_OPERAND\r\n"); 
                    }
                    else 
                    {
                      recordsNum = CGMS_DB_DeleteRecords(&cgms_racp_timeoffset_is_within_range, (const CGMS_RACP_Filter_t *)(&(CGMS_RACP_Context.Procedure.Operand)));
                      if (recordsNum > 0)
                      {
                        cgms_racp_send_response_code(CGMS_RACP_RESPONSE_SUCCESS);
                        LOG_INFO_APP("CGMS_RACP_RESPONSE_SUCCESS\r\n"); 
                      }
                      else 
                      {
                        cgms_racp_send_response_code(CGMS_RACP_RESPONSE_NO_RECORDS);
                        LOG_INFO_APP("CGMS_RACP_RESPONSE_NO_RECORDS\r\n"); 
                      }
                    }
                  }
                }
                break;
#endif /* BLE_CFG_CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS_WITHIN_RANGE */
                
#if (BLE_CFG_CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS_FIRST == 1)
              case CGMS_RACP_OPERATOR_FIRST_RECORD:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_FIRST_RECORD\r\n"); 
                  if (requestDataLength != CGMS_RACP_FILTER_TYPE_NO_OPERAND_LENGTH)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                  }
                  else 
                  {
                    recordsNum = CGMS_DB_DeleteRecordByIndex(0x0000);
                    if (recordsNum > 0)
                    {
                      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_SUCCESS);
                    }
                    else 
                    {
                      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_NO_RECORDS);
                    }
                  }
                }
                break;
#endif /* BLE_CFG_CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS_FIRST */
                
#if (BLE_CFG_CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS_LAST == 1)
              case CGMS_RACP_OPERATOR_LAST_RECORD:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_LAST_RECORD\r\n"); 
                  if (requestDataLength != CGMS_RACP_FILTER_TYPE_NO_OPERAND_LENGTH)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                  }
                  else 
                  {
                    recordsNum = CGMS_DB_DeleteRecordByIndex(0xFFFF);
                    if (recordsNum > 0)
                    {
                      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_SUCCESS);
                    }
                    else 
                    {
                      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_NO_RECORDS);
                    }
                  }
                }
                break;
#endif /* BLE_CFG_CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS_LAST */
                
              case CGMS_RACP_OPERATOR_NULL:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_NULL\r\n"); 
                  cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERATOR);
                }
                break;
                
              default:
                {
                  cgms_racp_send_response_code(CGMS_RACP_RESPONSE_OPERATOR_NOT_SUPPORTED);
                }
                break;
            }
          }
          break;
#endif /* BLE_CFG_CGMS_RACP_OP_CODE_DELETE_STORED_RECORDS */
        
        case CGMS_RACP_OP_CODE_REPORT_NUMBER_STORED_RECORDS:
          {
            LOG_INFO_APP("CGMS_RACP_OP_CODE_REPORT_NUMBER_STORED_RECORDS\r\n"); 
            switch(CGMS_RACP_Context.Procedure.Operator)
            {
              case CGMS_RACP_OPERATOR_ALL_RECORDS:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_ALL_RECORDS\r\n"); 
                  if(requestDataLength != CGMS_RACP_FILTER_TYPE_NO_OPERAND_LENGTH)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                  }
                  else 
                  {
                    recordsNum = CGMS_DB_GetRecordsCount(NULL, NULL);
                    cgms_racp_send_number_of_stored_records_response(recordsNum);
                  }
                }
                break;
                
              case CGMS_RACP_OPERATOR_GREATER_EQUAL:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_GREATER_EQUAL\r\n"); 
                  if(requestDataLength != CGMS_RACP_FILTER_TYPE_LESS_GREATER_TIME_OFFSET_LENGTH)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                  }
                  else if (pRequestData[CGMS_RACP_OPERAND_POSITION] != CGMS_RACP_FILTER_TYPE_TIME_OFFSET)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_OPERAND_NOT_SUPPORTED);
                  }
                  else 
                  {
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->FilterType = pRequestData[CGMS_RACP_OPERAND_POSITION];
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Minimum = 
                      (uint16_t)(pRequestData[CGMS_RACP_OPERAND_POSITION+1] | (pRequestData[CGMS_RACP_OPERAND_POSITION+2] << 8));
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Maximum = 0xFFFF;
                    recordsNum = CGMS_DB_GetRecordsCount(&cgms_racp_timeoffset_is_greater_or_equal, (const CGMS_RACP_Filter_t *)(&(CGMS_RACP_Context.Procedure.Operand)));
                    cgms_racp_send_number_of_stored_records_response(recordsNum);
                  }
                }
                break;
                
#if (BLE_CFG_CGMS_RACP_OP_CODE_REPORT_NUMBER_STORED_RECORDS_LESS_EQUAL == 1)
              case CGMS_RACP_OPERATOR_LESS_EQUAL:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_LESS_EQUAL\r\n"); 
                  if(requestDataLength != CGMS_RACP_FILTER_TYPE_LESS_GREATER_TIME_OFFSET_LENGTH)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                  }
                  else if (pRequestData[CGMS_RACP_OPERAND_POSITION] != CGMS_RACP_FILTER_TYPE_TIME_OFFSET)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_OPERAND_NOT_SUPPORTED);
                  }
                  else 
                  {
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->FilterType = pRequestData[CGMS_RACP_OPERAND_POSITION];
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Minimum = 0x0000;
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Maximum = 
                      (uint16_t)(pRequestData[CGMS_RACP_OPERAND_POSITION+1] | (pRequestData[CGMS_RACP_OPERAND_POSITION+2] << 8));
                    recordsNum = CGMS_DB_GetRecordsCount(&cgms_racp_timeoffset_is_less_or_equal, (const CGMS_RACP_Filter_t *)(&(CGMS_RACP_Context.Procedure.Operand)));
                    cgms_racp_send_number_of_stored_records_response(recordsNum);
                  }
                }
                break;
#endif /* BLE_CFG_CGMS_RACP_OP_CODE_REPORT_NUMBER_STORED_RECORDS_LESS_EQUAL */
                
#if (BLE_CFG_CGMS_RACP_OP_CODE_REPORT_NUMBER_STORED_RECORDS_WITHIN_RANGE == 1)
              case CGMS_RACP_OPERATOR_WITHIN_RANGE:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_WITHIN_RANGE\r\n"); 
                  if(requestDataLength != CGMS_RACP_FILTER_TYPE_WITHIN_RANGE_TIME_OFFSET_LENGTH)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                  }
                  else if (pRequestData[CGMS_RACP_OPERAND_POSITION] != CGMS_RACP_FILTER_TYPE_TIME_OFFSET)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_OPERAND_NOT_SUPPORTED);
                  }
                  else 
                  {
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->FilterType = pRequestData[CGMS_RACP_OPERAND_POSITION];
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Minimum = 
                      (uint16_t)(pRequestData[CGMS_RACP_OPERAND_POSITION+1] | (pRequestData[CGMS_RACP_OPERAND_POSITION+2] << 8));
                    ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Maximum = 
                      (uint16_t)(pRequestData[CGMS_RACP_OPERAND_POSITION+3] | (pRequestData[CGMS_RACP_OPERAND_POSITION+4] << 8));
                    if (((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Minimum > ((CGMS_RACP_TimeOffsetFilter_t *)(&CGMS_RACP_Context.Procedure.Operand))->Maximum)
                    {
                      cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                    }
                    else 
                    {
                      recordsNum = CGMS_DB_GetRecordsCount(&cgms_racp_timeoffset_is_within_range, (const CGMS_RACP_Filter_t *)(&(CGMS_RACP_Context.Procedure.Operand)));
                      cgms_racp_send_number_of_stored_records_response(recordsNum);
                    }
                  }
                }
                break;
#endif /* BLE_CFG_CGMS_RACP_OP_CODE_REPORT_NUMBER_STORED_RECORDS_WITHIN_RANGE */
                
#if (BLE_CFG_CGMS_RACP_OP_CODE_REPORT_NUMBER_STORED_RECORDS_FIRST == 1)
              case CGMS_RACP_OPERATOR_FIRST_RECORD:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_ALL_RECORDS\r\n"); 
                  if(requestDataLength != CGMS_RACP_FILTER_TYPE_NO_OPERAND_LENGTH)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                  }
                  else 
                  {
                    recordsNum = CGMS_DB_GetRecordsCount(NULL, NULL);
                    if (recordsNum != 0)
                    {
                      recordsNum = 1;
                    }
                    cgms_racp_send_number_of_stored_records_response(recordsNum);
                  }
                }
                break;
#endif /* BLE_CFG_CGMS_RACP_OP_CODE_REPORT_NUMBER_STORED_RECORDS_FIRST */
                
#if (BLE_CFG_CGMS_RACP_OP_CODE_REPORT_NUMBER_STORED_RECORDS_LAST == 1)
              case CGMS_RACP_OPERATOR_LAST_RECORD:
                {
                  LOG_INFO_APP("CGMS_RACP_OPERATOR_ALL_RECORDS\r\n"); 
                  if(requestDataLength != CGMS_RACP_FILTER_TYPE_NO_OPERAND_LENGTH)
                  {
                    cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERAND);
                  }
                  else 
                  {
                    recordsNum = CGMS_DB_GetRecordsCount(NULL, NULL);
                    if (recordsNum != 0)
                    {
                      recordsNum = 1;
                    }
                    cgms_racp_send_number_of_stored_records_response(recordsNum);
                  }
                }
                break;
#endif /* BLE_CFG_CGMS_RACP_OP_CODE_REPORT_NUMBER_STORED_RECORDS_LAST */
                
              case CGMS_RACP_OPERATOR_NULL:
                {
                  cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERATOR);
                }
                break;
                
              default:
                {
                  cgms_racp_send_response_code(CGMS_RACP_RESPONSE_OPERATOR_NOT_SUPPORTED);
                }
                break;
            }
          }
          break;
        
        case CGMS_RACP_OP_CODE_ABORT_OPERATION:
          {
            LOG_INFO_APP("CGMS_RACP_OP_CODE_ABORT_OPERATION\r\n"); 
            if((requestDataLength != CGMS_RACP_FILTER_TYPE_NO_OPERAND_LENGTH) ||
               (CGMS_RACP_Context.Procedure.Operator != CGMS_RACP_OPERATOR_NULL))
            {
              /* Operator must be 0x00 and NO operand for Abort operation procedure */
              cgms_racp_send_response_code(CGMS_RACP_RESPONSE_INVALID_OPERATOR);
            }
            else 
            {
              CGMS_RACP_Context.State = CGMS_RACP_STATE_PROCEDURE_ABORT_RECEIVED;
            }
          }
          break;
          
        default:
          {
            cgms_racp_send_response_code(CGMS_RACP_RESPONSE_OPCODE_NOT_SUPPORTED);
          }
          break;
      }
    }
  }
} /* end of CGMS_RACP_RequestHandler() */

/**
* @brief RACP new write request permit check
* @param [in] requestData: pointer to received RACP request data
* @param [in] requestDataLength: received RACP request length
* @retval 0x00 when no error, error code otherwise
*/
uint8_t CGMS_RACP_CheckRequestValid(uint8_t * pRequestData, uint8_t requestDataLength)
{
  uint8_t retval = 0x00;
  
  LOG_INFO_APP("RACP Request, request data length: %d\r\n", requestDataLength);
  
  if ((CGMS_RACP_Context.State == CGMS_RACP_STATE_PROCEDURE_IN_PROGRESS) && (pRequestData[CGMS_RACP_OP_CODE_POSITION] != CGMS_RACP_OP_CODE_ABORT_OPERATION))
  {
    retval = CGMS_ATT_ERROR_CODE_PROCEDURE_ALREADY_IN_PROGRESS;
  }
  else if (CGMS_APP_GetRACPCharacteristicIndicationEnabled() == FALSE)
  {
    retval = CGMS_ATT_ERROR_CODE_CLIENT_CHAR_CONF_DESC_IMPROPERLY_CONFIGURED;
  }
  
  return retval;
} /* end of CGMS_RACP_CheckRequestValid() */

/**
* @brief Process the acknowledge of sent indication
* @param None
* @retval None
*/
void CGMS_RACP_AcknowledgeHandler(void)
{
  // STM_TODO : Implement indication acknowledge reception handler, in fact all the operations should be finished after the indication acknowledge is received from GATT client, if not received within 30sec, procedure failed
} /* end of CGMS_RACP_AcknowledgeHandler() */

/**
* @brief Get the flag holding whether any RACP operation is already in progress or not
* @param None
* @retval None
*/
CGMS_RACP_State_t CGMS_RACP_GetState(void)
{
  return CGMS_RACP_Context.State;
} /* end of CGMS_RACP_GetOperationInProgress() */

