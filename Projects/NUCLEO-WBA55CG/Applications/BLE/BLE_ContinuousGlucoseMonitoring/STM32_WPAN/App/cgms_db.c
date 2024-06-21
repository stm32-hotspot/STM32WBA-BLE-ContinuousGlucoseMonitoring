/**
  ******************************************************************************
  * @file    cgms_db.c
  * @author  MCD Application Team
  * @brief   CGMS Records Database
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
#include "cgms_db.h"

/* Private typedef -----------------------------------------------------------*/

typedef struct CGMS_DB_Item
{
  CGMS_RACP_Record_t Record;
  FlagStatus IsSelected;
  struct CGMS_DB_Item * NextItem;
} CGMS_DB_Item_t;

typedef struct
{
  CGMS_DB_Item_t * Head;
} CGMS_DB_Context_t;

/* Private variables ---------------------------------------------------------*/
/* Private functions definition ----------------------------------------------*/
static uint8_t cgms_db_delete_last_item(CGMS_DB_Item_t ** head);
static uint8_t cgms_db_delete_first_item(CGMS_DB_Item_t ** head);

/* START of Section BLE_DRIVER_CONTEXT
 */
PLACE_IN_SECTION("BLE_DRIVER_CONTEXT") static CGMS_DB_Context_t CGMS_DB_Context = {0};

/* Private Functions ---------------------------------------------------------*/

/**
* @brief  Delete last item of the CGMS Database
* @param  head : pointer to pointer of the first DB item
* @retval TRUE if item exists and deleted, FALSE (!TRUE) otherwise
*/
static uint8_t cgms_db_delete_last_item(CGMS_DB_Item_t ** head)
{
  CGMS_DB_Item_t * nextTail = NULL;
  
  if (*head != NULL)
  {
    if ((*head)->NextItem != NULL)
    {
      nextTail = (*head);
      while (nextTail->NextItem->NextItem != NULL)
      {
        nextTail = nextTail->NextItem;
      }
      
      free(nextTail->NextItem);
      nextTail->NextItem = NULL;
    }
    else 
    {
      free(CGMS_DB_Context.Head);
    }
    return TRUE;
  }
   
  return FALSE;
} /* end of cgms_db_delete_last_item() */

/**
* @brief  Delete first item of the CGMS Database
* @param  head : pointer to pointer of the first DB item
* @retval TRUE if item exists and deleted, FALSE (!TRUE) otherwise
*/
static uint8_t cgms_db_delete_first_item(CGMS_DB_Item_t ** head)
{
  CGMS_DB_Item_t * nextHead = NULL;
  
  if (*head != NULL)
  {
    nextHead = (*head)->NextItem;
    free(*head);
    *head = nextHead;
    
    return TRUE;
  }
   
  return FALSE;
} /* end of cgms_db_delete_first_item() */

/* Public Functions ----------------------------------------------------------*/

/**
* @brief  Initialize the CGMS database
* @param  UUID: UUID of the characteristic
* @param  pPayload: Payload of the characteristic
* @retval None
*/
void CGMS_DB_Init(void)
{
  CGMS_DB_Context.Head = NULL;
} /* end of CGMS_DB_Init() */

/**
* @brief  Get a specified record from the CGMS database
* @param  pTarget: Pointer to the memory to copy the requested record data
* @retval TRUE if any selected record found, FALSE (!TRUE) otherwise
*/
uint8_t CGMS_DB_GetNextSelectedRecord(CGMS_RACP_Record_t * pTarget)
{
  CGMS_DB_Item_t * currentItem = CGMS_DB_Context.Head;
  
  while (currentItem != NULL)
  {
    if (currentItem->IsSelected == SET)
    {
      memcpy(pTarget, &(currentItem->Record), sizeof(CGMS_RACP_Record_t));
      currentItem->IsSelected = RESET;
      return TRUE;
    }
    currentItem = currentItem->NextItem;
  }
  
  return FALSE;
} /* end of CGMS_DB_GetRecord() */

/**
* @brief  Deselect (mark) all items in the database
* @param  None
* @retval Count of the items found selected
*/
uint16_t CGMS_DB_ResetRecordsSelection( void )
{
  uint16_t deselectedItemsCount = 0;
  CGMS_DB_Item_t * currentItem = CGMS_DB_Context.Head;
  
  while (currentItem != NULL)
  {
    if (currentItem->IsSelected == SET)
    {
      currentItem->IsSelected = RESET;
      deselectedItemsCount++;
    }
    currentItem = currentItem->NextItem;
  }
  
  return deselectedItemsCount;
} /* end of CGMS_DB_ResetRecordsSelection() */

/**
* @brief  Select (mark) all items in the database matching the filtering criteria
* @param  Arbiter: Pointer to function implementing the requested select logic
* @retval Count of the items selected
*/
uint16_t CGMS_DB_SelectRecords( uint8_t (*FilterFunc)(const CGMS_RACP_Record_t *, const CGMS_RACP_Filter_t *), const CGMS_RACP_Filter_t * filter)
{
  uint16_t selectedItemsCount = 0;
  CGMS_DB_Item_t * currentItem = CGMS_DB_Context.Head;
    
  while (currentItem != NULL)
  {
    if (FilterFunc == NULL)
    {
      currentItem->IsSelected = SET;
      selectedItemsCount++;
    }
    else 
    {
      if (FilterFunc(&(currentItem->Record), filter) == TRUE)
      {
        currentItem->IsSelected = SET;
        selectedItemsCount++;
      }
      else 
      {
        currentItem->IsSelected = RESET;
      }
    }
    currentItem = currentItem->NextItem;
  }
  
  return selectedItemsCount;
} /* end of CGMS_DB_SelectRecords() */

/**
* @brief  Delete all items in the database matching the filtering criteria
* @param  Arbiter: Pointer to function implementing the requested delete logic
* @retval Count of the items selected
*/
uint16_t CGMS_DB_DeleteRecords( uint8_t (*FilterFunc)(const CGMS_RACP_Record_t *, const CGMS_RACP_Filter_t *), const CGMS_RACP_Filter_t * filter)
{
  uint16_t deletedItemsCount = 0;
  uint16_t index = 0;
  CGMS_DB_Item_t * currentItem = CGMS_DB_Context.Head;
  
  if (FilterFunc == NULL)
  {
    while (CGMS_DB_Context.Head != NULL)
    {
      if (cgms_db_delete_first_item(&(CGMS_DB_Context.Head)) != FALSE)
      {
        deletedItemsCount++;
      }
    }
  }
  else 
  {
    while (currentItem != NULL)
    {
      if (FilterFunc(&(currentItem->Record), filter) == TRUE)
      {
        if (CGMS_DB_DeleteRecordByIndex(index) != FALSE)
        {
          currentItem = CGMS_DB_Context.Head;
          index = 0;
          deletedItemsCount++;
        }
      }
      else 
      {
        /* We go next, item doesn't match the criteria */
        index++;
        currentItem = currentItem->NextItem;
      }
    }
  }
    
  return deletedItemsCount;
} /* end of CGMS_DB_DeleteRecords */

/**
* @brief  Delete the last record stored in the CGMS database
* @param  index: 0xFFFF (remove last), 0x0000 (remove first), other (remove exact)
* @retval FALSE if no record, TRUE (!FALSE) otherwise
*/
uint8_t CGMS_DB_DeleteRecordByIndex( uint16_t index )
{
  CGMS_DB_Item_t * currentItem;
  CGMS_DB_Item_t * temp = NULL;
  uint16_t n;
  
  if (CGMS_DB_Context.Head != NULL)
  {
    if (index == 0x0000)
    {
      return cgms_db_delete_first_item(&(CGMS_DB_Context.Head));
    }
    else if (index == 0xFFFF)
    {
      return cgms_db_delete_last_item(&(CGMS_DB_Context.Head));
    }
    else 
    {
      currentItem = CGMS_DB_Context.Head;
      for (n = 0; n < (index-1); n++)
      {
        if (currentItem->NextItem == NULL)
        {
          return FALSE;
        }
        currentItem = currentItem->NextItem;
      }
      
      temp = currentItem->NextItem;
      currentItem->NextItem = temp->NextItem;
      free(temp);
      return TRUE;
    }
  }
   
  return FALSE;
} /* end of CGMS_DB_DeleteRecordByIndex() */

/**
* @brief  Add a new record to the CGMS database
* @param  pNewRecords: Pointer to the new record to be added
* @retval None
*/
void CGMS_DB_AddRecord(const CGMS_RACP_Record_t * pNewRecord)
{
  CGMS_DB_Item_t * currentItem = CGMS_DB_Context.Head;
  
  if (currentItem == NULL)
  {
    CGMS_DB_Context.Head = malloc(sizeof(CGMS_DB_Item_t));
    currentItem = CGMS_DB_Context.Head;
  }
  else 
  {
    while (currentItem->NextItem != NULL)
    {
      currentItem = currentItem->NextItem;
    }
     
    currentItem->NextItem = malloc(sizeof(CGMS_DB_Item_t));
    currentItem = currentItem->NextItem;
  }
  
  memcpy(&(currentItem->Record), pNewRecord, sizeof(CGMS_RACP_Record_t));
  currentItem->IsSelected = RESET;
  currentItem->NextItem = NULL;
} /* end of CGMS_DB_AddRecord() */

/**
* @brief  Get the count of records stored in the CGMS database
* @param  Arbiter: Pointer to function implementing the requested compare logic
* @retval Number of records
*/
uint16_t CGMS_DB_GetRecordsCount( uint8_t (*FilterFunc)(const CGMS_RACP_Record_t *, const CGMS_RACP_Filter_t *), const CGMS_RACP_Filter_t * filter)
{
  uint16_t foundItemsCount = 0;
  CGMS_DB_Item_t * currentItem = CGMS_DB_Context.Head;
    
  if (currentItem == NULL)
  {
    return 0;
  }
  
  while (currentItem != NULL)
  {
    if (FilterFunc == NULL)
    {
      foundItemsCount++;
    }
    else 
    {
      if (FilterFunc(&(currentItem->Record), filter) == TRUE)
      {
        foundItemsCount++;
      }
      else 
      {
        /* Do nothing - item doesn't match the criteria */
      }
    }
    currentItem = currentItem->NextItem;
  }
  
  return foundItemsCount;
} /* end of CGMS_DB_GetRecordsNum() */

/**
* @brief  Get the last record stored in the CGMS database
* @param  index: 0xFFFF (get last), 0x0000 (get first), other (get exact)
* @param  pTarget: Pointer to the memory to copy the requested record data
* @retval FALSE if no record, TRUE (!FALSE) otherwise
*/
uint8_t CGMS_DB_GetRecordByIndex(uint16_t index, CGMS_RACP_Record_t * pTarget)
{
  uint16_t n;
  CGMS_DB_Item_t * currentItem;
  
  if (CGMS_DB_Context.Head != NULL)
  {
    currentItem = CGMS_DB_Context.Head;
    
    if (index == 0x0000)
    {
      memcpy(pTarget, &(currentItem->Record), sizeof(CGMS_RACP_Record_t));
      return TRUE;
    }
    else if (index == 0xFFFF)
    {
      while(currentItem->NextItem != NULL)
      {
        currentItem = currentItem->NextItem;
      }
      memcpy(pTarget, &(currentItem->Record), sizeof(CGMS_RACP_Record_t));
      return TRUE;
    }
    else 
    {
      for (n = 0; n < index; n++)
      {
        if (currentItem->NextItem != NULL)
        {
          return FALSE;
        }
        currentItem = currentItem->NextItem;
      }
      memcpy(pTarget, &(currentItem->Record), sizeof(CGMS_RACP_Record_t));
      return TRUE;
    }
  }
   
  return FALSE;
} /* end of CGMS_DB_GetRecordByIndex() */

/**
* @brief  Select record stored in the CGMS database by its index
* @param  index: 0xFFFF (get last), 0x0000 (get first), other (get exact)
* @param  pTarget: Pointer to the memory to copy the requested record data
* @retval FALSE if no record, TRUE (!FALSE) otherwise
*/
uint8_t CGMS_DB_SelectRecordByIndex(uint16_t index)
{
  uint16_t n;
  CGMS_DB_Item_t * currentItem;
  
  if (CGMS_DB_Context.Head != NULL)
  {
    currentItem = CGMS_DB_Context.Head;
    
    if (index == 0x0000)
    {
      currentItem->IsSelected = SET;
      return TRUE;
    }
    else if (index == 0xFFFF)
    {
      while(currentItem->NextItem != NULL)
      {
        currentItem = currentItem->NextItem;
      }
      currentItem->IsSelected = SET;
      return TRUE;
    }
    else 
    {
      for (n = 0; n < index; n++)
      {
        if (currentItem->NextItem != NULL)
        {
          return FALSE;
        }
        currentItem = currentItem->NextItem;
      }
      currentItem->IsSelected = SET;
      return TRUE;
    }
  }
   
  return FALSE;
} /* end of CGMS_DB_SelectRecordByIndex() */

