  /**
  ******************************************************************************
  * @file    cgms_socp.c
  * @author  MCD Application Team
  * @brief   CGMS SOCP
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
#include "cgms_socp.h"
#include "cgms_app.h"

/******************************************************************************
* Macro Declarations
******************************************************************************/
/* Private typedef -----------------------------------------------------------*/
    

typedef struct
{
  CGMS_SOCP_Procedure_t Procedure;
  CGMS_SOCP_Response_t Response;
  CGMS_SOCP_State_t State;
} CGMS_SOCP_Context_t;

/* Private variables ---------------------------------------------------------*/
/**
 * START of Section BLE_DRIVER_CONTEXT
 */
PLACE_IN_SECTION("BLE_DRIVER_CONTEXT") static CGMS_SOCP_Context_t CGMS_SOCP_Context;

/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static tBleStatus cgms_socp_send_response_code(uint8_t responseCode);
#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_GLUCOSE_CALIBRATION_VALUE == 1)
static tBleStatus cgms_socp_send_calibration_data_record_response(CGMS_SOCP_CalibrationDataRecord_t * pCalibrationDataRecord);
#endif
#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_CGM_COMMUNICATION_INTERVAL == 1)
static tBleStatus cgms_socp_send_communication_interval_response(uint8_t communicationIntervalInMinutes);
#endif
#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_HIGH_ALERT_LEVEL == 1)
static tBleStatus cgms_socp_send_high_alert_level_response(uint16_t highAlertLevel);
#endif
#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_LOW_ALERT_LEVEL == 1)
static tBleStatus cgms_socp_send_low_alert_level_response(uint16_t lowAlertLevel);
#endif
#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_HYPO_ALERT_LEVEL == 1)
static tBleStatus cgms_socp_send_hypo_alert_level_response(uint16_t hypoAlertLevel);
#endif
#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_HYPER_ALERT_LEVEL == 1)
static tBleStatus cgms_socp_send_hyper_alert_level_response(uint16_t hyperAlertLevel);
#endif

/* Private functions ---------------------------------------------------------*/
extern tBleStatus cgms_update_socp_characteristic(uint8_t opCode, uint8_t * pOperand, uint8_t operandLength);

/**
  * @brief SOCP send response code to the previous request
  * @param [in] responseCode : response code
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus cgms_socp_send_response_code(uint8_t responseCode)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  
  CGMS_SOCP_Context.Response.OpCode = CGMS_SOCP_OP_CODE_RESPONSE_CODE;
  ((CGMS_SOCP_GeneralResponse_t *)CGMS_SOCP_Context.Procedure.Operand)->RequestOpCode = CGMS_SOCP_Context.Procedure.OpCode;
  ((CGMS_SOCP_GeneralResponse_t *)CGMS_SOCP_Context.Procedure.Operand)->ResponseCode = responseCode;
  
  /* SOCP response is indicated */
  retval = cgms_update_socp_characteristic(CGMS_SOCP_Context.Response.OpCode, CGMS_SOCP_Context.Procedure.Operand, CGMS_SOCP_OP_CODE_RESPONSE_OPERAND_LENGTH);
  
  if(retval == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("SOCP response sent successfully opcode: 0x%X, responseCode: 0x%X\r\n", CGMS_SOCP_Context.Procedure.OpCode, responseCode);
    
    /* unset flag for SOCP operation already in progress */
    CGMS_SOCP_Context.State = CGMS_SOCP_STATE_IDLE;
  }
  else 
  {
    LOG_INFO_APP("FAILED to send SOCP response %02X\r\n", retval);
    // STM_TODO : Implement error management, error management to be further extended, no propagation currently
  }
  
  return retval;
} /* end of cgms_socp_send_response_code() */

#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_GLUCOSE_CALIBRATION_VALUE == 1)
/**
  * @brief SOCP send calibration data record response to the previous request
  * @param [in] communicationIntervalInMinutes : communication interval in minutes
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus cgms_socp_send_calibration_data_record_response(CGMS_SOCP_CalibrationDataRecord_t * pCalibrationDataRecord)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  
  CGMS_SOCP_Context.Response.OpCode = CGMS_SOCP_OP_CODE_CALIBRATION_VALUE_RESPONSE;
  ((CGMS_SOCP_CalibrationDataRecord_t *)CGMS_SOCP_Context.Procedure.Operand)->CalibrationDataRecordNumber = pCalibrationDataRecord->CalibrationDataRecordNumber;
  ((CGMS_SOCP_CalibrationDataRecord_t *)CGMS_SOCP_Context.Procedure.Operand)->CalibrationStatus = pCalibrationDataRecord->CalibrationStatus;
  ((CGMS_SOCP_CalibrationDataRecord_t *)CGMS_SOCP_Context.Procedure.Operand)->CalibrationTime = pCalibrationDataRecord->CalibrationTime;
  ((CGMS_SOCP_CalibrationDataRecord_t *)CGMS_SOCP_Context.Procedure.Operand)->CalibrationTypeSampleLocation = pCalibrationDataRecord->CalibrationTypeSampleLocation;
  ((CGMS_SOCP_CalibrationDataRecord_t *)CGMS_SOCP_Context.Procedure.Operand)->GlucoseConcentrationOfCalibration = pCalibrationDataRecord->GlucoseConcentrationOfCalibration;
  ((CGMS_SOCP_CalibrationDataRecord_t *)CGMS_SOCP_Context.Procedure.Operand)->NextCalibrationTime = pCalibrationDataRecord->NextCalibrationTime;
  
  /* SOCP response is indicated */
  retval = cgms_update_socp_characteristic(CGMS_SOCP_Context.Response.OpCode, CGMS_SOCP_Context.Procedure.Operand, CGMS_SOCP_OP_CODE_OPERAND_MAX_LENGTH);
  
  if(retval == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("SOCP response sent successfully opcode: 0x%X, calibration data record: { 0x%04X, 0x%04X, 0x%02X, 0x%04d, 0x%04d, 0x%02d }\r\n",
                CGMS_SOCP_OP_CODE_CALIBRATION_VALUE_RESPONSE,
                pCalibrationDataRecord->CalibrationDataRecordNumber,
                pCalibrationDataRecord->CalibrationStatus,
                pCalibrationDataRecord->CalibrationTime,
                pCalibrationDataRecord->CalibrationTypeSampleLocation,
                pCalibrationDataRecord->GlucoseConcentrationOfCalibration,
                pCalibrationDataRecord->NextCalibrationTime);
    
    /* unset flag for SOCP operation already in progress */
    CGMS_SOCP_Context.State = CGMS_SOCP_STATE_IDLE;
  }
  else 
  {
    LOG_INFO_APP("FAILED to send SOCP response %02X\r\n", retval);
    // STM_TODO : Implement error management, error management to be further extended, no propagation currently
  }
  
  return retval;
} /* end of cgms_socp_send_calibration_data_record_response() */
#endif

#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_CGM_COMMUNICATION_INTERVAL == 1)
/**
  * @brief SOCP send communication interval response to the previous request
  * @param [in] communicationIntervalInMinutes : communication interval in minutes
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus cgms_socp_send_communication_interval_response(uint8_t communicationIntervalInMinutes)
{
  tBleStatus retval = BLE_STATUS_FAILED;
    
  CGMS_SOCP_Context.Response.OpCode = CGMS_SOCP_OP_CODE_COMMUNICATION_INTERVAL_RESPONSE;
  *((CGMS_SOCP_CommunicationInterval_t *)CGMS_SOCP_Context.Procedure.Operand) = communicationIntervalInMinutes;
  
  /* SOCP response is indicated */
  retval = cgms_update_socp_characteristic(CGMS_SOCP_Context.Response.OpCode, 
                                           CGMS_SOCP_Context.Procedure.Operand, 
                                           CGMS_SOCP_OP_CODE_CGM_COMMUNICATION_INTERVAL_OPERAND_LENGTH);
  
  if(retval == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("SOCP response sent successfully opcode: 0x%X, communication interval in minutes: %d\r\n", 
                 CGMS_SOCP_OP_CODE_COMMUNICATION_INTERVAL_RESPONSE, 
                 communicationIntervalInMinutes);
    
    /* unset flag for SOCP operation already in progress */
    CGMS_SOCP_Context.State = CGMS_SOCP_STATE_IDLE;
  }
  else 
  {
    LOG_INFO_APP("FAILED to send SOCP response %02X\r\n", retval);
    // STM_TODO : Implement error management, error management to be further extended, no propagation currently
  }
  
  return retval;
} /* end of cgms_socp_send_communication_interval_response() */
#endif

#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_HIGH_ALERT_LEVEL == 1)
/**
  * @brief SOCP send high alert level response to the previous request
  * @param [in] high alert level : communication interval in minutes
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus cgms_socp_send_high_alert_level_response(uint16_t highAlertLevel)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  
  CGMS_SOCP_Context.Response.OpCode = CGMS_SOCP_OP_CODE_PATIENT_HIGH_ALERT_LEVEL_RESPONSE;
  *((uint16_t *)CGMS_SOCP_Context.Procedure.Operand) = highAlertLevel;
  
  /* SOCP response is indicated */
  retval = cgms_update_socp_characteristic(CGMS_SOCP_Context.Response.OpCode, 
                                           CGMS_SOCP_Context.Procedure.Operand, 
                                           CGMS_SOCP_OP_CODE_GET_ALERT_LEVEL_REQUEST_OPERAND_LENGTH);
  
  if(retval == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("SOCP response sent successfully opcode: 0x%X, high alert level: %d\r\n",
                 CGMS_SOCP_OP_CODE_PATIENT_HIGH_ALERT_LEVEL_RESPONSE,
                 highAlertLevel);
    
    /* unset flag for SOCP operation already in progress */
    CGMS_SOCP_Context.State = CGMS_SOCP_STATE_IDLE;
  }
  else 
  {
    LOG_INFO_APP("FAILED to send SOCP response %02X\r\n", retval);
    // STM_TODO : Implement error management, error management to be further extended, no propagation currently
  }
  
  return retval;
} /* end of cgms_socp_send_high_alert_level_response() */
#endif

#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_LOW_ALERT_LEVEL == 1)
/**
  * @brief SOCP send low alert level response to the previous request
  * @param [in] low alert level : communication interval in minutes
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus cgms_socp_send_low_alert_level_response(uint16_t lowAlertLevel)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  
  CGMS_SOCP_Context.Response.OpCode = CGMS_SOCP_OP_CODE_PATIENT_LOW_ALERT_LEVEL_RESPONSE;
  *((uint16_t *)CGMS_SOCP_Context.Procedure.Operand) = lowAlertLevel;
  
  /* SOCP response is indicated */
  retval = cgms_update_socp_characteristic(CGMS_SOCP_Context.Response.OpCode, CGMS_SOCP_Context.Procedure.Operand, CGMS_SOCP_OP_CODE_GET_ALERT_LEVEL_REQUEST_OPERAND_LENGTH);
  
  if(retval == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("SOCP response sent successfully opcode: 0x%X, low alert level: %d\r\n",
                 CGMS_SOCP_OP_CODE_PATIENT_LOW_ALERT_LEVEL_RESPONSE,
                 lowAlertLevel);
    
    /* unset flag for SOCP operation already in progress */
    CGMS_SOCP_Context.State = CGMS_SOCP_STATE_IDLE;
  }
  else 
  {
    LOG_INFO_APP("FAILED to send SOCP response %02X\r\n", retval);
    // STM_TODO : Implement error management, error management to be further extended, no propagation currently
  }
  
  return retval;
} /* end of cgms_socp_send_low_alert_level_response() */
#endif

#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_HYPO_ALERT_LEVEL == 1)
/**
  * @brief SOCP send hypo alert level response to the previous request
  * @param [in] hypo alert level : level
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus cgms_socp_send_hypo_alert_level_response(uint16_t hypoAlertLevel)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  
  CGMS_SOCP_Context.Response.OpCode = CGMS_SOCP_OP_CODE_PATIENT_HYPO_ALERT_LEVEL_RESPONSE;
  *((uint16_t *)CGMS_SOCP_Context.Procedure.Operand) = hypoAlertLevel;
  
  /* SOCP response is indicated */
  retval = cgms_update_socp_characteristic(CGMS_SOCP_Context.Response.OpCode, CGMS_SOCP_Context.Procedure.Operand, CGMS_SOCP_OP_CODE_GET_ALERT_LEVEL_REQUEST_OPERAND_LENGTH);
  
  if(retval == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("SOCP response sent successfully opcode: 0x%X, hypo alert level: %d\r\n",
                 CGMS_SOCP_OP_CODE_PATIENT_HYPO_ALERT_LEVEL_RESPONSE,
                 hypoAlertLevel);
    
    /* unset flag for SOCP operation already in progress */
    CGMS_SOCP_Context.State = CGMS_SOCP_STATE_IDLE;
  }
  else 
  {
    LOG_INFO_APP("FAILED to send SOCP response %02X\r\n", retval);
    // STM_TODO : Implement error management, error management to be further extended, no propagation currently
  }
  
  return retval;
} /* end of cgms_socp_send_hypo_alert_level_response() */
#endif

#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_HYPER_ALERT_LEVEL == 1)
/**
  * @brief SOCP send hyper alert level response to the previous request
  * @param [in] hyper alert level : level
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus cgms_socp_send_hyper_alert_level_response(uint16_t hyperAlertLevel)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  
  CGMS_SOCP_Context.Response.OpCode = CGMS_SOCP_OP_CODE_PATIENT_HYPER_ALERT_LEVEL_RESPONSE;
  *((uint16_t *)CGMS_SOCP_Context.Procedure.Operand) = hyperAlertLevel;
  
  /* SOCP response is indicated */
  retval = cgms_update_socp_characteristic(CGMS_SOCP_Context.Response.OpCode, CGMS_SOCP_Context.Procedure.Operand, CGMS_SOCP_OP_CODE_GET_ALERT_LEVEL_REQUEST_OPERAND_LENGTH);
  
  if(retval == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("SOCP response sent successfully opcode: 0x%X, hyper alert level: %d\r\n",
                 CGMS_SOCP_OP_CODE_PATIENT_HYPER_ALERT_LEVEL_RESPONSE,
                 hyperAlertLevel);
    
    /* unset flag for SOCP operation already in progress */
    CGMS_SOCP_Context.State = CGMS_SOCP_STATE_IDLE;
  }
  else 
  {
    LOG_INFO_APP("FAILED to send SOCP response %02X\r\n", retval);
    // STM_TODO : Implement error management, error management to be further extended, no propagation currently
  }
  
  return retval;
} /* end of cgms_socp_send_hyper_alert_level_response() */
#endif

#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_RATE_OF_DECREASE_ALERT_LEVEL == 1)
/**
  * @brief SOCP send rate of decrease alert level response to the previous request
  * @param [in] rate of decrease alert level : level
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus cgms_socp_send_rate_of_decrease_alert_level_response(uint16_t decreaseRateAlertLevel)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  
  CGMS_SOCP_Context.Response.OpCode = CGMS_SOCP_OP_CODE_RATE_OF_DECREASE_ALERT_LEVEL_RESPONSE;
  *((uint16_t *)CGMS_SOCP_Context.Procedure.Operand) = decreaseRateAlertLevel;
  
  /* SOCP response is indicated */
  retval = cgms_update_socp_characteristic(CGMS_SOCP_Context.Response.OpCode, CGMS_SOCP_Context.Procedure.Operand, CGMS_SOCP_OP_CODE_GET_RATE_ALERT_LEVEL_REQUEST_OPERAND_LENGTH);
  
  if(retval == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("SOCP response sent successfully opcode: 0x%X, rate of decrease alert level: %d\r\n",
                 CGMS_SOCP_OP_CODE_RATE_OF_DECREASE_ALERT_LEVEL_RESPONSE,
                 decreaseRateAlertLevel);
    
    /* unset flag for SOCP operation already in progress */
    CGMS_SOCP_Context.State = CGMS_SOCP_STATE_IDLE;
  }
  else 
  {
    LOG_INFO_APP("FAILED to send SOCP response %02X\r\n", retval);
    // STM_TODO : Implement error management, error management to be further extended, no propagation currently
  }
  
  return retval;
} /* end of cgms_socp_send_rate_of_decrease_alert_level_response() */
#endif

#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_RATE_OF_INCREASE_ALERT_LEVEL == 1)
/**
  * @brief SOCP send rate of increase alert level response to the previous request
  * @param [in] rate of increase alert level : level
  * @retval BLE_STATUS_SUCCESS if ok, error code otherwise
  */
static tBleStatus cgms_socp_send_rate_of_increase_alert_level_response(uint16_t increaseRateAlertLevel)
{
  tBleStatus retval = BLE_STATUS_FAILED;
  
  CGMS_SOCP_Context.Response.OpCode = CGMS_SOCP_OP_CODE_RATE_OF_INCREASE_ALERT_LEVEL_RESPONSE;
  *((uint16_t *)CGMS_SOCP_Context.Procedure.Operand) = increaseRateAlertLevel;
  
  /* SOCP response is indicated */
  retval = cgms_update_socp_characteristic(CGMS_SOCP_Context.Response.OpCode, 
                                           CGMS_SOCP_Context.Procedure.Operand, 
                                           CGMS_SOCP_OP_CODE_GET_RATE_ALERT_LEVEL_REQUEST_OPERAND_LENGTH);
  
  if(retval == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("SOCP response sent successfully opcode: 0x%X, rate of increase alert level: %d\r\n",
                 CGMS_SOCP_OP_CODE_RATE_OF_INCREASE_ALERT_LEVEL_RESPONSE,
                 increaseRateAlertLevel);
    
    /* unset flag for SOCP operation already in progress */
    CGMS_SOCP_Context.State = CGMS_SOCP_STATE_IDLE;
  }
  else 
  {
    LOG_INFO_APP("FAILED to send SOCP response %02X\r\n", retval);
    // STM_TODO : Implement error management, error management to be further extended, no propagation currently
  }
  
  return retval;
} /* end of cgms_socp_send_rate_of_increase_alert_level_response() */
#endif

/* Public functions ----------------------------------------------------------*/

/**
  * @brief SOCP context initialization
  * @param None
  * @retval None
  */
void CGMS_SOCP_Init(void)
{
  memset(&(CGMS_SOCP_Context.Procedure), 0x00, sizeof(CGMS_SOCP_Procedure_t));
  memset(&(CGMS_SOCP_Context.Response), 0x00, sizeof(CGMS_SOCP_Procedure_t));
  
  CGMS_SOCP_Context.State = CGMS_SOCP_STATE_IDLE;
} /* end CGMS_SOCP_Init() */

/**
  * @brief SOCP request handler 
  * @param [in] pRequestData : pointer to received SOCP request data
  * @param [in] requestDataLength : received SOCP request data length
  * @retval None
  */
void CGMS_SOCP_RequestHandler(uint8_t * pRequestData, uint8_t requestDataLength)
{
  LOG_INFO_APP("SOCP Request, request data length: %d\r\n", requestDataLength);
  
  if (CGMS_SOCP_Context.State != CGMS_SOCP_STATE_IDLE)
  {
    // STM_TODO : The specification doesn't say anything about receiving new request, so what to do in such a case? We could make an event callback to App layer to decide and send Procedure not completed if App refuses
  }
  else 
  {
    /* set in progress flag on reception of a new SOCP event */
    CGMS_SOCP_Context.State = CGMS_SOCP_STATE_PROCEDURE_IN_PROGRESS;
    
    if (requestDataLength < (CGMS_SOCP_REQUEST_MIN_LENGTH + CGMS_SOCP_CRC_LENGTH))
    {
      /* SOCP procedure data must be always at least 1 byte long - we should never receive empty event */
      // STM_TODO : Implement error management, error management to be further extended, no propagation currently
      cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_OPCODE_NOT_SUPPORTED);
    }
    else 
    {
      /* Get the requested procedure OpCode */
      CGMS_SOCP_Context.Procedure.OpCode = pRequestData[CGMS_SOCP_OP_CODE_POSITION];
      LOG_INFO_APP("SOCP Procedure OpCode: 0x%02X\r\n", CGMS_SOCP_Context.Procedure.OpCode);
      
      /* Check and Process the OpCode */
      switch(CGMS_SOCP_Context.Procedure.OpCode)
      {
#if (BLE_CFG_CGMS_SOCP_OP_CODE_SET_CGM_COMMUNICATION_INTERVAL == 1)
        case CGMS_SOCP_OP_CODE_SET_CGM_COMMUNICATION_INTERVAL:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_SET_CGM_COMMUNICATION_INTERVAL\r\n"); 
            if(requestDataLength != (CGMS_SOCP_OP_CODE_LENGTH + CGMS_SOCP_OP_CODE_CGM_COMMUNICATION_INTERVAL_OPERAND_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else 
            {
              CGMS_SOCP_App_Event_t AppEvent;
              
              //((CGMS_SOCP_CommunicationInterval_t *)CGMS_SOCP_Context.Procedure.Operand)->CommunicationIntervalInMinutes = pRequestData[CGMS_SOCP_OPERAND_POSITION];
              LOG_INFO_APP("SOCP Set communication interval in minutes: %d\r\n", (pRequestData[CGMS_SOCP_OPERAND_POSITION]));
              AppEvent.EventCode = CGMS_SOCP_SET_CGM_COMMUNICATION_INTERVAL_EVENT;
              AppEvent.EventData.p_Payload = &(pRequestData[CGMS_SOCP_OPERAND_POSITION]);
              AppEvent.EventData.Length = sizeof(CGMS_SOCP_CommunicationInterval_t);
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) == TRUE)
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_SUCCESS\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_SUCCESS);
              }
              else 
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_SET_CGM_COMMUNICATION_INTERVAL */
          
#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_CGM_COMMUNICATION_INTERVAL == 1)
        case CGMS_SOCP_OP_CODE_GET_CGM_COMMUNICATION_INTERVAL:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_GET_CGM_COMMUNICATION_INTERVAL\r\n"); 
            if(requestDataLength != (CGMS_SOCP_REQUEST_MIN_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else
            {
              CGMS_SOCP_App_Event_t AppEvent;
              CGMS_SOCP_CommunicationInterval_t communicationInterval;  
                
              memset(&communicationInterval, 0x00, sizeof(CGMS_SOCP_CommunicationInterval_t));
              AppEvent.EventCode = CGMS_SOCP_GET_CGM_COMMUNICATION_INTERVAL_EVENT;
              AppEvent.EventData.p_Payload = (uint8_t *)(&communicationInterval);
              AppEvent.EventData.Length = sizeof(CGMS_SOCP_CommunicationInterval_t);
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) != FALSE)
              {
                cgms_socp_send_communication_interval_response(communicationInterval);;
              }
              else 
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_GET_CGM_COMMUNICATION_INTERVAL */
          
#if (BLE_CFG_CGMS_SOCP_OP_CODE_SET_GLUCOSE_CALIBRATION_VALUE == 1)
        case CGMS_SOCP_OP_CODE_SET_GLUCOSE_CALIBRATION_VALUE:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_SET_GLUCOSE_CALIBRATION_VALUE\r\n"); 
            if(requestDataLength != (CGMS_SOCP_OP_CODE_LENGTH + CGMS_SOCP_OP_CODE_SET_CALIBRATION_RECORD_REQUEST_OPERAND_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else
            {
              CGMS_SOCP_App_Event_t AppEvent;
              
              LOG_INFO_APP("SOCP Set calibration record: { 0x%04X, 0x%04X, 0x%02X, 0x%04d, 0x%04d, 0x%02d }\r\n",
                          ((CGMS_SOCP_CalibrationDataRecord_t *)&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))->GlucoseConcentrationOfCalibration,
                          ((CGMS_SOCP_CalibrationDataRecord_t *)&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))->CalibrationTime,
                          ((CGMS_SOCP_CalibrationDataRecord_t *)&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))->CalibrationTypeSampleLocation,
                          ((CGMS_SOCP_CalibrationDataRecord_t *)&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))->NextCalibrationTime,
                          ((CGMS_SOCP_CalibrationDataRecord_t *)&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))->CalibrationDataRecordNumber,
                          ((CGMS_SOCP_CalibrationDataRecord_t *)&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))->CalibrationStatus);

              AppEvent.EventCode = CGMS_SOCP_SET_GLUCOSE_CALIBRATION_VALUE_EVENT;
              AppEvent.EventData.p_Payload = &(pRequestData[CGMS_SOCP_OPERAND_POSITION]);
              AppEvent.EventData.Length = sizeof(CGMS_SOCP_CalibrationDataRecord_t);
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) != FALSE)
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_SUCCESS\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_SUCCESS);
              }
              else
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_SET_GLUCOSE_CALIBRATION_VALUE */
          
#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_GLUCOSE_CALIBRATION_VALUE == 1)
        case CGMS_SOCP_OP_CODE_GET_GLUCOSE_CALIBRATION_VALUE:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_GET_GLUCOSE_CALIBRATION_VALUE\r\n");
            if(requestDataLength != (CGMS_SOCP_OP_CODE_LENGTH + CGMS_SOCP_OP_CODE_GET_CALIBRATION_RECORD_REQUEST_OPERAND_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else 
            {
              CGMS_SOCP_App_Event_t AppEvent;
              CGMS_SOCP_CalibrationDataRecord_t calibrationDataRecord;
              
              LOG_INFO_APP("SOCP Get calibration record: { 0x%04X }\r\n",
                         *((CGMS_SOCP_CalibrationDataRecordNumber_t *)(&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))));
              memset(&calibrationDataRecord, 0x00, sizeof(CGMS_SOCP_CalibrationDataRecord_t));
              AppEvent.EventCode = CGMS_SOCP_GET_GLUCOSE_CALIBRATION_VALUE_EVENT;
              calibrationDataRecord.CalibrationDataRecordNumber = *((CGMS_SOCP_CalibrationDataRecordNumber_t *)(&(pRequestData[CGMS_SOCP_OPERAND_POSITION])));
              AppEvent.EventData.p_Payload = (uint8_t *)(&calibrationDataRecord);
              AppEvent.EventData.Length = sizeof(CGMS_SOCP_CalibrationDataRecord_t);
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) != FALSE)
              {
                cgms_socp_send_calibration_data_record_response(&calibrationDataRecord);
              }
              else 
              {
                // STM_TODO : Is it necessary to implement CGMS_SOCP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED too?
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_GET_GLUCOSE_CALIBRATION_VALUE */
          
#if (BLE_CFG_CGMS_SOCP_OP_CODE_START_SESSION == 1)
        case CGMS_SOCP_OP_CODE_START_SESSION:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_START_SESSION\r\n"); 
            if(requestDataLength != (CGMS_SOCP_REQUEST_MIN_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else 
            {
              CGMS_SOCP_App_Event_t AppEvent;
              
              AppEvent.EventCode = CGMS_SOCP_START_SESSION_EVENT;
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) == TRUE)
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_SUCCESS\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_SUCCESS);
              }
              else 
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_START_SESSION */
          
#if (BLE_CFG_CGMS_SOCP_OP_CODE_STOP_SESSION == 1)
        case CGMS_SOCP_OP_CODE_STOP_SESSION:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_STOP_SESSION\r\n"); 
            if(requestDataLength != (CGMS_SOCP_REQUEST_MIN_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else 
            {
              CGMS_SOCP_App_Event_t AppEvent;
              
              AppEvent.EventCode = CGMS_SOCP_STOP_SESSION_EVENT;
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) == TRUE)
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_SUCCESS\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_SUCCESS);
              }
              else 
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_STOP_SESSION */
          
#if (BLE_CFG_CGMS_SOCP_OP_CODE_RESET_DEVICE_SPECIFIC_ALERT_LEVEL == 1)
        case CGMS_SOCP_OP_CODE_RESET_DEVICE_SPECIFIC_ALERT_LEVEL:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_RESET_DEVICE_SPECIFIC_ALERT_LEVEL\r\n"); 
            if(requestDataLength != (CGMS_SOCP_REQUEST_MIN_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else 
            {
              CGMS_SOCP_App_Event_t AppEvent;
              
              AppEvent.EventCode = CGMS_SOCP_RESET_DEVICE_SPECIFIC_ALERT_LEVEL_EVENT;
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) == TRUE)
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_SUCCESS\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_SUCCESS);
              }
              else 
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_RESET_DEVICE_SPECIFIC_ALERT_LEVEL */
          
#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_HIGH_ALERT_LEVEL == 1)
        case CGMS_SOCP_OP_CODE_GET_PATIENT_HIGH_ALERT_LEVEL:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_GET_PATIENT_HIGH_ALERT_LEVEL\r\n");
            if(requestDataLength != (CGMS_SOCP_REQUEST_MIN_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else 
            {
              CGMS_SOCP_App_Event_t AppEvent;
              uint16_t highAlertLevel;
              
              LOG_INFO_APP("SOCP Get high alert level: { 0x%04X }\r\n",
                         *((uint16_t *)(&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))));
              AppEvent.EventCode = CGMS_SOCP_GET_PATIENT_HIGH_ALERT_LEVEL;
              AppEvent.EventData.p_Payload = (uint8_t *)(&highAlertLevel);
              AppEvent.EventData.Length = sizeof(uint16_t);
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) != FALSE)
              {
                cgms_socp_send_high_alert_level_response(highAlertLevel);
              }
              else 
              {
                // STM_TODO : Is it necessary to implement CGMS_SOCP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED too?
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_GET_GLUCOSE_CALIBRATION_VALUE */

#if (BLE_CFG_CGMS_SOCP_OP_CODE_SET_PATIENT_HIGH_ALERT_LEVEL == 1)
        case CGMS_SOCP_OP_CODE_SET_PATIENT_HIGH_ALERT_LEVEL:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_SET_PATIENT_HIGH_ALERT_LEVEL\r\n");
            if(requestDataLength != (CGMS_SOCP_OP_CODE_LENGTH + CGMS_SOCP_OP_CODE_SET_ALERT_LEVEL_REQUEST_OPERAND_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else 
            {
              CGMS_SOCP_App_Event_t AppEvent;
              
              LOG_INFO_APP("SOCP Set high alert level: { 0x%04X }\r\n",
                         *((uint16_t *)(&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))));
              AppEvent.EventCode = CGMS_SOCP_SET_PATIENT_HIGH_ALERT_LEVEL;
              AppEvent.EventData.p_Payload = &(pRequestData[CGMS_SOCP_OPERAND_POSITION]);
              AppEvent.EventData.Length = sizeof(uint16_t);
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) != FALSE)
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_SUCCESS\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_SUCCESS);
              }
              else 
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_GET_GLUCOSE_CALIBRATION_VALUE */
          
#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_LOW_ALERT_LEVEL == 1)
        case CGMS_SOCP_OP_CODE_GET_PATIENT_LOW_ALERT_LEVEL:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_GET_PATIENT_LOW_ALERT_LEVEL\r\n");
            if(requestDataLength != (CGMS_SOCP_REQUEST_MIN_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else 
            {
              CGMS_SOCP_App_Event_t AppEvent;
              uint16_t lowAlertLevel;
              
              LOG_INFO_APP("SOCP Get low alert level: { 0x%04X }\r\n",
                         *((uint16_t *)(&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))));
              AppEvent.EventCode = CGMS_SOCP_GET_PATIENT_LOW_ALERT_LEVEL;
              AppEvent.EventData.p_Payload = (uint8_t *)(&lowAlertLevel);
              AppEvent.EventData.Length = sizeof(uint16_t);
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) != FALSE)
              {
                cgms_socp_send_low_alert_level_response(lowAlertLevel);
              }
              else 
              {
                // STM_TODO : Is it necessary to implement CGMS_SOCP_RESPONSE_CODE_PROCEDURE_NOT_COMPLETED too?
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_LOW_ALERT_LEVEL */

#if (BLE_CFG_CGMS_SOCP_OP_CODE_SET_PATIENT_LOW_ALERT_LEVEL == 1)
        case CGMS_SOCP_OP_CODE_SET_PATIENT_LOW_ALERT_LEVEL:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_SET_PATIENT_LOW_ALERT_LEVEL\r\n");
            if(requestDataLength != (CGMS_SOCP_OP_CODE_LENGTH + CGMS_SOCP_OP_CODE_SET_ALERT_LEVEL_REQUEST_OPERAND_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else 
            {
              CGMS_SOCP_App_Event_t AppEvent;
              
              LOG_INFO_APP("SOCP Set low alert level: { 0x%04X }\r\n",
                         *((uint16_t *)(&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))));
              AppEvent.EventCode = CGMS_SOCP_SET_PATIENT_LOW_ALERT_LEVEL;
              AppEvent.EventData.p_Payload = &(pRequestData[CGMS_SOCP_OPERAND_POSITION]);
              AppEvent.EventData.Length = sizeof(uint16_t);
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) != FALSE)
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_SUCCESS\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_SUCCESS);
              }
              else 
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_GET_GLUCOSE_CALIBRATION_VALUE */
          
#if (BLE_CFG_CGMS_SOCP_OP_CODE_SET_PATIENT_HYPO_ALERT_LEVEL == 1)
        case CGMS_SOCP_OP_CODE_SET_PATIENT_HYPO_ALERT_LEVEL:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_SET_PATIENT_HYPO_ALERT_LEVEL\r\n");
            if(requestDataLength != (CGMS_SOCP_OP_CODE_LENGTH + CGMS_SOCP_OP_CODE_SET_ALERT_LEVEL_REQUEST_OPERAND_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else 
            {
              CGMS_SOCP_App_Event_t AppEvent;
              
              LOG_INFO_APP("SOCP Set hypo alert level: { 0x%04X }\r\n",
                          *((uint16_t *)(&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))));
              AppEvent.EventCode = CGMS_SOCP_SET_PATIENT_HYPO_ALERT_LEVEL;
              AppEvent.EventData.p_Payload = &(pRequestData[CGMS_SOCP_OPERAND_POSITION]);
              AppEvent.EventData.Length = sizeof(uint16_t);
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) != FALSE)
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_SUCCESS\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_SUCCESS);
              }
              else 
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_SET_PATIENT_HIPO_ALERT_LEVEL */
          
#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_HYPO_ALERT_LEVEL == 1)
        case CGMS_SOCP_OP_CODE_GET_PATIENT_HYPO_ALERT_LEVEL:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_GET_PATIENT_HYPO_ALERT_LEVEL\r\n");
            if(requestDataLength != (CGMS_SOCP_REQUEST_MIN_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else 
            {
              CGMS_SOCP_App_Event_t AppEvent;
              uint16_t hypoAlertLevel;
              
              LOG_INFO_APP("SOCP Get hypo alert level: { 0x%04X }\r\n",
                         *((uint16_t *)(&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))));
              AppEvent.EventCode = CGMS_SOCP_GET_PATIENT_HYPO_ALERT_LEVEL;
              AppEvent.EventData.p_Payload = (uint8_t *)(&hypoAlertLevel);
              AppEvent.EventData.Length = sizeof(uint16_t);
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) != FALSE)
              {
                cgms_socp_send_hypo_alert_level_response(hypoAlertLevel);
              }
              else 
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_HYPO_ALERT_LEVEL */

#if (BLE_CFG_CGMS_SOCP_OP_CODE_SET_PATIENT_HYPER_ALERT_LEVEL == 1)
        case CGMS_SOCP_OP_CODE_SET_PATIENT_HYPER_ALERT_LEVEL:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_SET_PATIENT_HYPER_ALERT_LEVEL\r\n");
            if(requestDataLength != (CGMS_SOCP_OP_CODE_LENGTH + CGMS_SOCP_OP_CODE_SET_ALERT_LEVEL_REQUEST_OPERAND_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else 
            {
              CGMS_SOCP_App_Event_t AppEvent;
              
              LOG_INFO_APP("SOCP Set hyper alert level: { 0x%04X }\r\n",
                         *((uint16_t *)(&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))));
              AppEvent.EventCode = CGMS_SOCP_SET_PATIENT_HYPER_ALERT_LEVEL;
              AppEvent.EventData.p_Payload = &(pRequestData[CGMS_SOCP_OPERAND_POSITION]);
              AppEvent.EventData.Length = sizeof(uint16_t);
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) != FALSE)
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_SUCCESS\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_SUCCESS);
              }
              else 
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_SET_PATIENT_HYPER_ALERT_LEVEL */
          
#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_HYPER_ALERT_LEVEL == 1)
        case CGMS_SOCP_OP_CODE_GET_PATIENT_HYPER_ALERT_LEVEL:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_GET_PATIENT_HYPER_ALERT_LEVEL\r\n");
            if(requestDataLength != (CGMS_SOCP_REQUEST_MIN_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else 
            {
              CGMS_SOCP_App_Event_t AppEvent;
              uint16_t hyperAlertLevel;
              
              LOG_INFO_APP("SOCP Get hyper alert level: { 0x%04X }\r\n",
                         *((uint16_t *)(&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))));
              AppEvent.EventCode = CGMS_SOCP_GET_PATIENT_HYPER_ALERT_LEVEL;
              AppEvent.EventData.p_Payload = (uint8_t *)(&hyperAlertLevel);
              AppEvent.EventData.Length = sizeof(uint16_t);
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) != FALSE)
              {
                cgms_socp_send_hyper_alert_level_response(hyperAlertLevel);
              }
              else 
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_GET_PATIENT_HYPO_ALERT_LEVEL */

#if (BLE_CFG_CGMS_SOCP_OP_CODE_SET_RATE_OF_DECREASE_ALERT_LEVEL == 1)
        case CGMS_SOCP_OP_CODE_SET_RATE_OF_DECREASE_ALERT_LEVEL:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_SET_RATE_OF_DECREASE_ALERT_LEVEL\r\n");
            if(requestDataLength != (CGMS_SOCP_OP_CODE_LENGTH + CGMS_SOCP_OP_CODE_SET_RATE_ALERT_LEVEL_REQUEST_OPERAND_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else 
            {
              CGMS_SOCP_App_Event_t AppEvent;
              
              LOG_INFO_APP("SOCP Set rate of decrease alert level: { 0x%04X }\r\n",
                         *((uint16_t *)(&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))));
              AppEvent.EventCode = CGMS_SOCP_SET_RATE_OF_DECREASE_ALERT_LEVEL;
              AppEvent.EventData.p_Payload = &(pRequestData[CGMS_SOCP_OPERAND_POSITION]);
              AppEvent.EventData.Length = sizeof(uint16_t);
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) != FALSE)
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_SUCCESS\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_SUCCESS);
              }
              else 
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_SET_RATE_OF_DECREASE_ALERT_LEVEL */
          
#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_RATE_OF_DECREASE_ALERT_LEVEL == 1)
        case CGMS_SOCP_OP_CODE_GET_RATE_OF_DECREASE_ALERT_LEVEL:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_GET_RATE_OF_DECREASE_ALERT_LEVEL\r\n");
            if(requestDataLength != (CGMS_SOCP_REQUEST_MIN_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else 
            {
              CGMS_SOCP_App_Event_t AppEvent;
              uint16_t decreaseRateAlertLevel;
              
              LOG_INFO_APP("SOCP Get rate of decrease alert level: { 0x%04X }\r\n",
                         *((uint16_t *)(&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))));
              AppEvent.EventCode = CGMS_SOCP_GET_RATE_OF_DECREASE_ALERT_LEVEL;
              AppEvent.EventData.p_Payload = (uint8_t *)(&decreaseRateAlertLevel);
              AppEvent.EventData.Length = sizeof(uint16_t);
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) != FALSE)
              {
                cgms_socp_send_rate_of_decrease_alert_level_response(decreaseRateAlertLevel);
              }
              else 
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_GET_RATE_OF_DECREASE_ALERT_LEVEL */

#if (BLE_CFG_CGMS_SOCP_OP_CODE_SET_RATE_OF_INCREASE_ALERT_LEVEL == 1)
        case CGMS_SOCP_OP_CODE_SET_RATE_OF_INCREASE_ALERT_LEVEL:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_SET_RATE_OF_INCREASE_ALERT_LEVEL\r\n");
            if(requestDataLength != (CGMS_SOCP_OP_CODE_LENGTH + CGMS_SOCP_OP_CODE_SET_RATE_ALERT_LEVEL_REQUEST_OPERAND_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else 
            {
              CGMS_SOCP_App_Event_t AppEvent;
              
              LOG_INFO_APP("SOCP Set rate of increase alert level: { 0x%04X }\r\n",
                         *((uint16_t *)(&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))));
              AppEvent.EventCode = CGMS_SOCP_SET_RATE_OF_INCREASE_ALERT_LEVEL;
              AppEvent.EventData.p_Payload = &(pRequestData[CGMS_SOCP_OPERAND_POSITION]);
              AppEvent.EventData.Length = sizeof(uint16_t);
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) != FALSE)
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_SUCCESS\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_SUCCESS);
              }
              else 
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_SET_RATE_OF_INCREASE_ALERT_LEVEL */
          
#if (BLE_CFG_CGMS_SOCP_OP_CODE_GET_RATE_OF_INCREASE_ALERT_LEVEL == 1)
        case CGMS_SOCP_OP_CODE_GET_RATE_OF_INCREASE_ALERT_LEVEL:
          {
            LOG_INFO_APP("CGMS_SOCP_OP_CODE_GET_RATE_OF_INCREASE_ALERT_LEVEL\r\n");
            if(requestDataLength != (CGMS_SOCP_REQUEST_MIN_LENGTH + CGMS_SOCP_CRC_LENGTH))
            {
              // STM_TODO : Implement error management, error management to be further extended, no propagation currently
              LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND\r\n"); 
              cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_INVALID_OPERAND);
            }
            else 
            {
              CGMS_SOCP_App_Event_t AppEvent;
              uint16_t increaseRateAlertLevel;
              
              LOG_INFO_APP("SOCP Get rate of increase alert level: { 0x%04X }\r\n",
                         *((uint16_t *)(&(pRequestData[CGMS_SOCP_OPERAND_POSITION]))));
              AppEvent.EventCode = CGMS_SOCP_GET_RATE_OF_INCREASE_ALERT_LEVEL;
              AppEvent.EventData.p_Payload = (uint8_t *)(&increaseRateAlertLevel);
              AppEvent.EventData.Length = sizeof(uint16_t);
              if (CGMS_SOCP_APP_EventHandler(&AppEvent) != FALSE)
              {
                cgms_socp_send_rate_of_increase_alert_level_response(increaseRateAlertLevel);
              }
              else 
              {
                LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE\r\n"); 
                cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_PARAMETER_OUT_OF_RANGE);
              }
            }
          }
          break;
#endif /* BLE_CFG_CGMS_SOCP_OP_CODE_GET_RATE_OF_INCREASE_ALERT_LEVEL */

        default:
          // STM_TODO : Implement error management, error management to be further extended, no propagation currently
          LOG_INFO_APP("CGMS_SOCP_RESPONSE_CODE_OPCODE_NOT_SUPPORTED\r\n"); 
          cgms_socp_send_response_code(CGMS_SOCP_RESPONSE_CODE_OPCODE_NOT_SUPPORTED);
          break;
      }
    }
  }
} /* end CGMS_SOCP_RequestHandler() */


/**
* @brief SOCP new write request permit check
* @param [in] pRequestData: Pointer to the request data byte array
* @param [in] pRequestDataLength: Length of the request data byte array
* @retval 0x00 when no error, error code otherwise
*/
uint8_t CGMS_SOCP_CheckRequestValid(uint8_t * pRequestData, uint8_t requestDataLength)
{
  uint8_t retval = 0x00;
#if  (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  uint16_t E2E_CRC;
  
  E2E_CRC = (pRequestData[requestDataLength - 1] << 8) | pRequestData[requestDataLength - 2];
  LOG_INFO_APP("requestDataLength: 0x%x\r\n", requestDataLength); 
  LOG_INFO_APP("Received E2E_CRC: 0x%x\r\n", E2E_CRC); 
  LOG_INFO_APP("Calculated E2E_CRC: 0x%x\r\n", CGMS_APP_ComputeCRC(pRequestData, requestDataLength - 2)); 
#endif
  
  if (CGMS_APP_GetCGMSOCPCharacteristicIndicationEnabled() == FALSE)
  {
    retval = CGMS_ATT_ERROR_CODE_CLIENT_CHAR_CONF_DESC_IMPROPERLY_CONFIGURED;
  }
#if  (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  else if (CGMS_APP_ComputeCRC(pRequestData, requestDataLength - 2) != E2E_CRC)
  {
    LOG_INFO_APP("Invalide E2E_CRC\r\n"); 
    retval = CGMS_ATT_ERROR_CODE_INVALID_CRC;
  }
#endif
  
  return retval;
} /* end of CGMS_SOCP_CheckRequestValid() */

/**
  * @brief Process the acknowledge of sent indication
  * @param None
  * @retval None
  */
void CGMS_SOCP_AcknowledgeHandler(void)
{
  // STM_TODO : Implement indication acknowledge reception handler, in fact all the operations should be finished after the indication acknowledge is received from GATT client, if not received within 30sec, procedure failed
} /* end CGMS_SOCP_AcknowledgeHandler() */


