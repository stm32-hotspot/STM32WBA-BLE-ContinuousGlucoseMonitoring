/**
  ******************************************************************************
  * @file    cgms_sst.c
  * @author  MCD Application Team
  * @brief   CGMS Session Start Time
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
#include "cgms_sst.h"
#include "cgms_app.h"
/* Private typedef -----------------------------------------------------------*/

typedef __PACKED_STRUCT
{
  uint16_t  Year;
  uint8_t   Month;
  uint8_t   Day;
  uint8_t   Hours;
  uint8_t   Minutes;
  uint8_t   Seconds;
} CGMS_SST_DateTime_t;

typedef __PACKED_STRUCT
{
  CGMS_SST_DateTime_t DateTime;
  int8_t TimeZone;
  uint8_t DSTOffset;
} CGMS_SST_SessionStartTime_t;

typedef struct
{
  CGMS_SST_SessionStartTime_t SessionStartTime;
  CGMS_SST_State_t State;
} CGMS_SST_Context_t;

/* Private variables ---------------------------------------------------------*/
/**
 * START of Section BLE_DRIVER_CONTEXT
 */
PLACE_IN_SECTION("BLE_DRIVER_CONTEXT") static CGMS_SST_Context_t CGMS_SST_Context;

/* Private functions ---------------------------------------------------------*/
uint8_t cgms_sst_check_datetime_valid(const CGMS_SST_DateTime_t * pDateTime);
uint8_t cgms_sst_check_timezone_valid(const int8_t * pTimeZone);
uint8_t cgms_sst_check_dstoffset_valid(const uint8_t * pDstOffset);

/**
  * @brief Check if the Date and Time are valid
  * @param pDateTime : structure holding the date and time as defined in the CGMS specification
  * @retval TRUE if OK, FALSE (!TRUE) otherwise
  */
uint8_t cgms_sst_check_datetime_valid(const CGMS_SST_DateTime_t * pDateTime)
{
  if ((pDateTime->Year < CGMS_SST_DATETIME_YEAR_MINIMUM) || (pDateTime->Year > CGMS_SST_DATETIME_YEAR_MAXIMUM))
  {
    return FALSE;
  }
  else if (pDateTime->Month > 12)
  {
    return FALSE;
  }
  else if (pDateTime->Day > 31)
  {
    return FALSE;
  }
  else if (pDateTime->Hours > 23)
  {
    return FALSE;
  }
  else if (pDateTime->Minutes > 59)
  {
    return FALSE;
  }
  else if (pDateTime->Seconds > 59)
  {
    return FALSE;
  }
  
  return TRUE;
} /* end of cgms_sst_check_datetime_valid() */

/**
  * @brief Check if the Timezone is valid
  * @param pTimeZone : timezone as defined in the CGMS specification
  * @retval TRUE if OK, FALSE (!TRUE) otherwise
  */
uint8_t cgms_sst_check_timezone_valid(const int8_t * pTimeZone)
{
  if (((*pTimeZone) < CGMS_SST_TIMEZONE_MINIMUM) || ((*pTimeZone) > CGMS_SST_TIMEZONE_MAXIMUM))
  {
    return FALSE;
  }
  
  return TRUE;
} /* end of cgms_sst_check_timezone_valid() */

/**
  * @brief Check if the Daylight Saving Time offset is valid
  * @param pDstOffset : Daylight Saving Time offset value as defined in the CGMS specification
  * @retval TRUE if OK, FALSE (!TRUE) otherwise
  */
uint8_t cgms_sst_check_dstoffset_valid(const uint8_t * pDstOffset)
{
  switch(*pDstOffset)
  {
  case CGMS_SST_DSTOFFSET_STANDARD_TIME:
    break;
  case CGMS_SST_DSTOFFSET_HALF_AN_HOUR_DAYLIGHT_TIME:
    break;
  case CGMS_SST_DSTOFFSET_DAYLIGHT_TIME:
    break;
  case CGMS_SST_DSTOFFSET_DOUBLE_DAYLIGHT_TIME:
    break;
  case CGMS_SST_DSTOFFSET_DST_NOT_KNOWN:
    break;
  default:
    return FALSE;
  }
  
  return TRUE;
} /* end of cgms_sst_check_dstoffset_valid() */

/* Public functions ----------------------------------------------------------*/

/**
  * @brief SST context initialization
  * @param None
  * @retval None
  */
void CGMS_SST_Init(void)
{
  CGMS_SST_Context.State = CGMS_SST_STATE_SST_NOT_INITIALIZED;
  memset(&(CGMS_SST_Context.SessionStartTime), 0x00, sizeof(CGMS_SST_SessionStartTime_t));
} /* end of CGMS_SST_Init() */

/**
  * @brief SST request handler 
  * @param [in] pRequestData : pointer to received RACP request data
  * @param [in] requestDataLength : received RACP request data length
  * @retval None
  */
void CGMS_SST_RequestHandler(uint8_t * pRequestData, uint8_t requestDataLength)
{
  // STM_TODO : We should take the new time and propagate to application layer
} /* end of CGMS_SST_RequestHandler() */

/**
  * @brief SST new write request permit check
  * @param [in] pRequestData: Pointer to the request data byte array
  * @param [in] pRequestDataLength: Length of the request data byte array
  * @retval 0x00 when no error, error code otherwise
  */
uint8_t CGMS_SST_CheckRequestValid(uint8_t * pRequestData, uint8_t requestDataLength)
{
  CGMS_SST_SessionStartTime_t * NewSessionStartTime;
#if  (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  uint16_t E2E_CRC;
#endif
  
  NewSessionStartTime = ((CGMS_SST_SessionStartTime_t *)pRequestData);

  if (cgms_sst_check_datetime_valid((const CGMS_SST_DateTime_t *)(&(NewSessionStartTime->DateTime))) != TRUE)
  {
    return CGMS_ATT_ERROR_CODE_OUT_OF_RANGE;
  }
  else if (cgms_sst_check_timezone_valid((const int8_t *)(&(NewSessionStartTime->TimeZone))) != TRUE)
  {
    return CGMS_ATT_ERROR_CODE_OUT_OF_RANGE;
  }
  else if (cgms_sst_check_dstoffset_valid((const uint8_t *)(&(NewSessionStartTime->DSTOffset))) != TRUE)
  {
    return CGMS_ATT_ERROR_CODE_OUT_OF_RANGE;
  }
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)  
  else if(requestDataLength < (sizeof(CGMS_SST_DateTime_t) + 2 + 2))
  {
    LOG_INFO_APP("Missing E2E_CRC\r\n"); 
    return CGMS_ATT_ERROR_CODE_MISSING_CRC;
  }
#endif
  
#if  (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  E2E_CRC = (pRequestData[requestDataLength - 1] << 8) | pRequestData[requestDataLength - 2];
  LOG_INFO_APP("requestDataLength: 0x%x\r\n", requestDataLength); 
  LOG_INFO_APP("Received E2E_CRC: 0x%x\r\n", E2E_CRC); 
  LOG_INFO_APP("Calculated E2E_CRC: 0x%x\r\n", CGMS_APP_ComputeCRC(pRequestData, requestDataLength - 2)); 

  if (CGMS_APP_ComputeCRC(pRequestData, requestDataLength - 2) != E2E_CRC)
  {
    LOG_INFO_APP("Invalide E2E_CRC\r\n"); 
    return CGMS_ATT_ERROR_CODE_INVALID_CRC;
  }
#endif
    
  return 0x00;
} /* end of CGMS_SST_CheckRequestValid() */

