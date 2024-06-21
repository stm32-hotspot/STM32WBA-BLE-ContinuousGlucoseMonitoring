
/**
  ******************************************************************************
  * @file    cgms_db.h
  * @author  MCD Application Team
  * @brief   Header for cgms_db.c module
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
#ifndef __CGMS_DB_H
#define __CGMS_DB_H

#ifdef __cplusplus
extern "C" 
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "cgms.h"
#include "cgms_racp.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void CGMS_DB_Init(void);
void CGMS_DB_AddRecord(const CGMS_RACP_Record_t * pSource);
uint8_t CGMS_DB_GetNextSelectedRecord(CGMS_RACP_Record_t * pTarget);
uint16_t CGMS_DB_GetRecordsCount( uint8_t (*Arbiter)(const CGMS_RACP_Record_t *, const CGMS_RACP_Filter_t *), const CGMS_RACP_Filter_t * filter);
uint16_t CGMS_DB_SelectRecords( uint8_t (*Arbiter)(const CGMS_RACP_Record_t *, const CGMS_RACP_Filter_t *), const CGMS_RACP_Filter_t * filter);
uint16_t CGMS_DB_DeleteRecords( uint8_t (*Arbiter)(const CGMS_RACP_Record_t *, const CGMS_RACP_Filter_t *), const CGMS_RACP_Filter_t * filter);
uint8_t CGMS_DB_GetRecordByIndex(uint16_t index, CGMS_RACP_Record_t * pTarget);
uint8_t CGMS_DB_SelectRecordByIndex(uint16_t index);
uint8_t CGMS_DB_DeleteRecordByIndex( uint16_t index );
uint16_t CGMS_DB_ResetRecordsSelection( void );
  
#endif /* __CGMS_DB_H */
