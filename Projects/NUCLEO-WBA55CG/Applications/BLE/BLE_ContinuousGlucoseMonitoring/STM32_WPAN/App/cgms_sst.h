
/**
  ******************************************************************************
  * @file    cgms_sst.h
  * @author  MCD Application Team
  * @brief   Header for cgms_sst.c module
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
#ifndef __CGMS_SST_H
#define __CGMS_SST_H

#ifdef __cplusplus
extern "C" 
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "cgms.h"
  
/* Exported defines ----------------------------------------------------------*/
#define CGMS_SST_DATETIME_YEAR_MINIMUM                (1582)
#define CGMS_SST_DATETIME_YEAR_MAXIMUM                (9999)
#define CGMS_SST_DATETIME_MONTH_NOT_KNOWN             (0)
#define CGMS_SST_DATETIME_DAY_NOT_KNOWN               (0)
#define CGMS_SST_TIMEZONE_MINIMUM                     (-48)
#define CGMS_SST_TIMEZONE_MAXIMUM                     (56)
#define CGMS_SST_DSTOFFSET_STANDARD_TIME              (0)
#define CGMS_SST_DSTOFFSET_HALF_AN_HOUR_DAYLIGHT_TIME (2)
#define CGMS_SST_DSTOFFSET_DAYLIGHT_TIME              (4)
#define CGMS_SST_DSTOFFSET_DOUBLE_DAYLIGHT_TIME       (8)
#define CGMS_SST_DSTOFFSET_DST_NOT_KNOWN              (255)
  
/* Exported types ------------------------------------------------------------*/
//     
//typedef PACKED_STRUCT {
//  uint16_t Maximum;
//  FlagStatus MaximumApplied;
//  uint16_t Minimum;
//  FlagStatus MinimumApplied;
//} CGMS_RACP_TimeOffsetFilter_t;
//  
//typedef PACKED_STRUCT {
//  uint16_t FilterParam[sizeof(CGMS_RACP_TimeOffsetFilter_t)];
//} CGMS_RACP_RecordFilterParams_t;
//
//typedef struct {
//   CGMS_MeasurementValue_t measurement;
//} CGMS_RACP_Record_t;

typedef enum
{
  CGMS_SST_STATE_SST_NOT_INITIALIZED,
  CGMS_SST_STATE_SST_INITIALIZED
} CGMS_SST_State_t;

/* Exported constants ------------------------------------------------------*/
/* External variables ------------------------------------------------------*/
/* Exported macros ---------------------------------------------------------*/
/* Exported functions ------------------------------------------------------*/

void CGMS_SST_Init(void);
     
/**
  * @brief SST request handler 
  * @param [in] pRequestData : pointer to received RACP request data
  * @param [in] requestDataLength : received RACP request data length
  * @retval none
  */
void CGMS_SST_RequestHandler(uint8_t * pRequestData, uint8_t requestDataLength);
uint8_t CGMS_SST_CheckRequestValid(uint8_t * pRequestData, uint8_t requestDataLength);

#endif /* __CGMS_SST_H */

