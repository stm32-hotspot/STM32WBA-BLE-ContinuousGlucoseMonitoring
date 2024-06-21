/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1.c
  * @author  MCD Application Team
  * @brief   service1 definition.
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
#include "common_blesvc.h"
#include "cgms.h"

/* USER CODE BEGIN Includes */
#include "cgms_app.h"
#include "cgms_db.h"
#include "cgms_racp.h"
#include "cgms_socp.h"
#include "cgms_sst.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  CgmsSvcHdle;                  /**< Cgms Service Handle */
  uint16_t  CgmCharHdle;                  /**< CGM Characteristic Handle */
  uint16_t  CgfCharHdle;                  /**< CGF Characteristic Handle */
  uint16_t  CgsCharHdle;                  /**< CGS Characteristic Handle */
  uint16_t  CsstCharHdle;                  /**< CSST Characteristic Handle */
  uint16_t  CsrtCharHdle;                  /**< CSRT Characteristic Handle */
  uint16_t  RacpCharHdle;                  /**< RACP Characteristic Handle */
  uint16_t  CfocpCharHdle;                  /**< CFOCP Characteristic Handle */
/* USER CODE BEGIN Context */
  FlagStatus    CGMSOCPCharacteristicIndicationEnabled;
  FlagStatus    RACPCharacteristicIndicationEnabled;
/* USER CODE END Context */
}CGMS_Context_t;

/* Private defines -----------------------------------------------------------*/
#define UUID_128_SUPPORTED  1

#if (UUID_128_SUPPORTED == 1)
#define BM_UUID_LENGTH  UUID_TYPE_128
#else
#define BM_UUID_LENGTH  UUID_TYPE_16
#endif

#define BM_REQ_CHAR_SIZE    (3)

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET         2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET              1
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static const uint16_t SizeCgm = 15;
static const uint16_t SizeCgf = 6;
static const uint16_t SizeCgs = 7;
static const uint16_t SizeCsst = 11;
static const uint16_t SizeCsrt = 4;
static const uint16_t SizeRacp = 20;
static const uint16_t SizeCfocp = 20;

static CGMS_Context_t CGMS_Context;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t CGMS_EventHandler(void *p_pckt);

/* USER CODE BEGIN PFP */
tBleStatus cgms_update_socp_characteristic(uint8_t opCode, uint8_t * pOperand, uint8_t operandLength);
// STM_TODO : cgms_update_racp_characteristic arguments to be changed to opcode + operand + operandlength
tBleStatus cgms_update_racp_characteristic(uint8_t * pData, uint8_t dataLength);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/* USER CODE BEGIN PFD */

/* USER CODE END PFD */

/* Private functions ----------------------------------------------------------*/

#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
do {\
    uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; uuid_struct[2] = uuid_2; uuid_struct[3] = uuid_3; \
    uuid_struct[4] = uuid_4; uuid_struct[5] = uuid_5; uuid_struct[6] = uuid_6; uuid_struct[7] = uuid_7; \
    uuid_struct[8] = uuid_8; uuid_struct[9] = uuid_9; uuid_struct[10] = uuid_10; uuid_struct[11] = uuid_11; \
    uuid_struct[12] = uuid_12; uuid_struct[13] = uuid_13; uuid_struct[14] = uuid_14; uuid_struct[15] = uuid_15; \
}while(0)

/* USER CODE BEGIN PF */
tBleStatus cgms_update_socp_characteristic(uint8_t opCode, uint8_t * pOperand, uint8_t operandLength)
{
  uint8_t cgmsocp_char_length = 0;
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  uint8_t buffer[CGMS_SOCP_RESPONSE_MAX_LENGTH + CGMS_CRC_LENGTH] ;
#else
  uint8_t buffer[CGMS_SOCP_RESPONSE_MAX_LENGTH] ;
#endif
  tBleStatus return_value = BLE_STATUS_SUCCESS;
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  uint16_t CRCValue = 0x0000;
#endif
  
  buffer[0] = opCode;
  cgmsocp_char_length++;
  memcpy(((uint8_t *)(buffer)) + cgmsocp_char_length, pOperand, operandLength);
  cgmsocp_char_length += operandLength;
    
#if (BLE_CFG_CGMS_CGM_FEATURE_E2E_CRC == 1)
  CRCValue = CGMS_APP_ComputeCRC(buffer, cgmsocp_char_length);
  
  LOG_INFO_APP("E2E_CRC: 0x%x\r\n", CRCValue);
  
  buffer[cgmsocp_char_length] = (uint8_t)(CRCValue & 0xFF);
  cgmsocp_char_length++;
  buffer[cgmsocp_char_length] = (uint8_t)((CRCValue >> 8) & 0xFF);
  cgmsocp_char_length++;
#endif
  
  return_value = aci_gatt_update_char_value(CGMS_Context.CgmsSvcHdle,
                                            CGMS_Context.CfocpCharHdle, 
                                            0,
                                            cgmsocp_char_length, 
                                            (uint8_t *) buffer);
  
  return return_value;
} /* end cgms_update_socp_characteristic() */

tBleStatus cgms_update_racp_characteristic(uint8_t * pData, uint8_t dataLength)
{
  tBleStatus return_value = BLE_STATUS_SUCCESS;
  
  return_value = aci_gatt_update_char_value(CGMS_Context.CgmsSvcHdle,
                                            CGMS_Context.RacpCharHdle, 
                                            0,
                                            dataLength, 
                                            (uint8_t *) pData);
  
  return return_value;
} /* end of cgms_update_racp_characteristic() */

/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static SVCCTL_EvtAckStatus_t CGMS_EventHandler(void *p_Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *p_event_pckt;
  evt_blecore_aci *p_blecore_evt;
  aci_gatt_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_write_permit_req_event_rp0   *p_write_perm_req;
  CGMS_NotificationEvt_t                 notification;
  /* USER CODE BEGIN Service1_EventHandler_1 */
  uint8_t error_code;
  CGMS_NotificationEvt_t AppEvent;
  /* USER CODE END Service1_EventHandler_1 */

  return_value = SVCCTL_EvtNotAck;
  p_event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)p_Event)->data);

  switch(p_event_pckt->evt)
  {
    case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
      p_blecore_evt = (evt_blecore_aci*)p_event_pckt->data;
      switch(p_blecore_evt->ecode)
      {
        case ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE:
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */
          p_attribute_modified = (aci_gatt_attribute_modified_event_rp0*)p_blecore_evt->data;
          notification.ConnectionHandle         = p_attribute_modified->Connection_Handle;
          notification.AttributeHandle          = p_attribute_modified->Attr_Handle;
          notification.DataTransfered.Length    = p_attribute_modified->Attr_Data_Length;
          notification.DataTransfered.p_Payload = p_attribute_modified->Attr_Data;
          if(p_attribute_modified->Attr_Handle == (CGMS_Context.CgmCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service1_Char_1 */

            /* USER CODE END Service1_Char_1 */
            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service1_Char_1_attribute_modified */

              /* USER CODE END Service1_Char_1_attribute_modified */

              /* Disabled Notification management */
              case (!(COMSVC_Notification)):
                /* USER CODE BEGIN Service1_Char_1_Disabled_BEGIN */
                
                /* USER CODE END Service1_Char_1_Disabled_BEGIN */
                notification.EvtOpcode = CGMS_CGM_NOTIFY_DISABLED_EVT;
                CGMS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_1_Disabled_END */

                /* USER CODE END Service1_Char_1_Disabled_END */
                break;

              /* Enabled Notification management */
              case COMSVC_Notification:
                /* USER CODE BEGIN Service1_Char_1_COMSVC_Notification_BEGIN */
                
                /* USER CODE END Service1_Char_1_COMSVC_Notification_BEGIN */
                notification.EvtOpcode = CGMS_CGM_NOTIFY_ENABLED_EVT;
                CGMS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_1_COMSVC_Notification_END */

                /* USER CODE END Service1_Char_1_COMSVC_Notification_END */
                break;

              default:
                /* USER CODE BEGIN Service1_Char_1_default */

                /* USER CODE END Service1_Char_1_default */
                break;
            }
          }  /* if(p_attribute_modified->Attr_Handle == (CGMS_Context.CgmCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(p_attribute_modified->Attr_Handle == (CGMS_Context.CgfCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service1_Char_2 */

            /* USER CODE END Service1_Char_2 */

            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service1_Char_2_attribute_modified */

              /* USER CODE END Service1_Char_2_attribute_modified */

              /* Disabled Indication management */
              case (!(COMSVC_Indication)):
                /* USER CODE BEGIN Service1_Char_2_Disabled_BEGIN */

                /* USER CODE END Service1_Char_2_Disabled_BEGIN */
                notification.EvtOpcode = CGMS_CGF_INDICATE_DISABLED_EVT;
                CGMS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_2_Disabled_END */

                /* USER CODE END Service1_Char_2_Disabled_END */
                break;

              /* Enabled Indication management */
              case COMSVC_Indication:
                /* USER CODE BEGIN Service1_Char_2_COMSVC_Indication_BEGIN */

                /* USER CODE END Service1_Char_2_COMSVC_Indication_BEGIN */
                notification.EvtOpcode = CGMS_CGF_INDICATE_ENABLED_EVT;
                CGMS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_2_COMSVC_Indication_END */

                /* USER CODE END Service1_Char_2_COMSVC_Indication_END */
                break;

              default:
                /* USER CODE BEGIN Service1_Char_2_default */

                /* USER CODE END Service1_Char_2_default */
                break;
            }
          }  /* if(p_attribute_modified->Attr_Handle == (CGMS_Context.CGFHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(p_attribute_modified->Attr_Handle == (CGMS_Context.RacpCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service1_Char_6 */

            /* USER CODE END Service1_Char_6 */

            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service1_Char_6_attribute_modified */

              /* USER CODE END Service1_Char_6_attribute_modified */

              /* Disabled Indication management */
              case (!(COMSVC_Indication)):
                /* USER CODE BEGIN Service1_Char_6_Disabled_BEGIN */
                
                /* USER CODE END Service1_Char_6_Disabled_BEGIN */
                notification.EvtOpcode = CGMS_RACP_INDICATE_DISABLED_EVT;
                CGMS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_6_Disabled_END */

                /* USER CODE END Service1_Char_6_Disabled_END */
                break;

              /* Enabled Indication management */
              case COMSVC_Indication:
                /* USER CODE BEGIN Service1_Char_6_COMSVC_Indication_BEGIN */
                
                /* USER CODE END Service1_Char_6_COMSVC_Indication_BEGIN */
                notification.EvtOpcode = CGMS_RACP_INDICATE_ENABLED_EVT;
                CGMS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_6_COMSVC_Indication_END */

                /* USER CODE END Service1_Char_6_COMSVC_Indication_END */
                break;

              default:
                /* USER CODE BEGIN Service1_Char_6_default */

                /* USER CODE END Service1_Char_6_default */
                break;
            }
          }  /* if(p_attribute_modified->Attr_Handle == (CGMS_Context.RACPHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(p_attribute_modified->Attr_Handle == (CGMS_Context.CfocpCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service1_Char_7 */

            /* USER CODE END Service1_Char_7 */

            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service1_Char_7_attribute_modified */

              /* USER CODE END Service1_Char_7_attribute_modified */

              /* Disabled Indication management */
              case (!(COMSVC_Indication)):
                /* USER CODE BEGIN Service1_Char_7_Disabled_BEGIN */

                /* USER CODE END Service1_Char_7_Disabled_BEGIN */
                notification.EvtOpcode = CGMS_CFOCP_INDICATE_DISABLED_EVT;
                CGMS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_7_Disabled_END */
                /* USER CODE END Service1_Char_7_Disabled_END */
                break;

              /* Enabled Indication management */
              case COMSVC_Indication:
                /* USER CODE BEGIN Service1_Char_7_COMSVC_Indication_BEGIN */

                /* USER CODE END Service1_Char_7_COMSVC_Indication_BEGIN */
                notification.EvtOpcode = CGMS_CFOCP_INDICATE_ENABLED_EVT;
                CGMS_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_7_COMSVC_Indication_END */
                /* USER CODE END Service1_Char_7_COMSVC_Indication_END */
                break;

              default:
                /* USER CODE BEGIN Service1_Char_7_default */

                /* USER CODE END Service1_Char_7_default */
                break;
            }
          }  /* if(p_attribute_modified->Attr_Handle == (CGMS_Context.CFOCPHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(p_attribute_modified->Attr_Handle == (CGMS_Context.CsstCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = CGMS_CSST_WRITE_EVT;
            /* USER CODE BEGIN Service1_Char_4_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

            /* USER CODE END Service1_Char_4_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            CGMS_Notification(&notification);
          } /* if(p_attribute_modified->Attr_Handle == (CGMS_Context.CsstCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
          else if(p_attribute_modified->Attr_Handle == (CGMS_Context.RacpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = CGMS_RACP_WRITE_EVT;
            /* USER CODE BEGIN Service1_Char_6_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

            /* USER CODE END Service1_Char_6_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            CGMS_Notification(&notification);
          } /* if(p_attribute_modified->Attr_Handle == (CGMS_Context.RacpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
          else if(p_attribute_modified->Attr_Handle == (CGMS_Context.CfocpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = CGMS_CFOCP_WRITE_EVT;
            /* USER CODE BEGIN Service1_Char_7_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

            /* USER CODE END Service1_Char_7_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            CGMS_Notification(&notification);
          } /* if(p_attribute_modified->Attr_Handle == (CGMS_Context.CfocpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

          /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */

          /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
          break;/* ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        }
        case ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE :
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ_BEGIN */

          /* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ_END */

          /* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ_END */
          break;/* ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE */
        }
        case ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE:
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_WRITE_PERMIT_REQ_BEGIN */
          p_write_perm_req = (aci_gatt_write_permit_req_event_rp0*)p_blecore_evt->data;
          if(p_write_perm_req->Attribute_Handle == (CGMS_Context.CsstCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service1_Char_4_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            LOG_INFO_APP("ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE CGMS_CGMSST, data length: %d\r\n", p_write_perm_req->Data_Length);
        
            error_code = CGMS_SST_CheckRequestValid(p_write_perm_req->Data, p_write_perm_req->Data_Length);
            if (error_code == 0x00)
            {
              aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                  p_write_perm_req->Attribute_Handle,
                                  0x00,
                                  0x00,
                                  p_write_perm_req->Data_Length,
                                  p_write_perm_req->Data);
          
              LOG_INFO_APP("ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE CGMS_CGMSST >>> PERMITTED\r\n");
              AppEvent.EvtOpcode = CGMS_CGM_SESSION_START_TIME_UPDATED;
              CGMS_Notification(&AppEvent);
            }
            else 
            {
              aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                  p_write_perm_req->Attribute_Handle,
                                  0x01,
                                  error_code,
                                  p_write_perm_req->Data_Length,
                                  p_write_perm_req->Data);
              LOG_INFO_APP("ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE CGMS_CGMSST >>> NOT PERMITTED (0x%02X)\r\n", error_code);
            }
            /*USER CODE END Service1_Char_4_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          } /*if(p_write_perm_req->Attribute_Handle == (CGMS_Context.CsstCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

          else if(p_write_perm_req->Attribute_Handle == (CGMS_Context.RacpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service1_Char_6_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            LOG_INFO_APP("ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE CGMS_RACP, data length: %d\r\n", p_write_perm_req->Data_Length);
        
            error_code = CGMS_RACP_CheckRequestValid(p_write_perm_req->Data, p_write_perm_req->Data_Length);
            if (error_code == 0x00)
            {
              aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                  p_write_perm_req->Attribute_Handle,
                                  0x00,
                                  0x00,
                                  p_write_perm_req->Data_Length,
                                  p_write_perm_req->Data);
          
              LOG_INFO_APP("ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE CGMS_RACP >>> PERMITTED\r\n");
              CGMS_RACP_RequestHandler(p_write_perm_req->Data, p_write_perm_req->Data_Length);
            }
            else 
            {
              aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                  p_write_perm_req->Attribute_Handle,
                                  0x01,
                                  error_code,
                                  p_write_perm_req->Data_Length,
                                  p_write_perm_req->Data);
              if(error_code == CGMS_ATT_ERROR_CODE_PROCEDURE_ALREADY_IN_PROGRESS)
              {
                LOG_INFO_APP("ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE CGMS_RACP >>> PROCEDURE ALREADY IN PROGRESS (0x%02X)\r\n", error_code);
              }
              else if(error_code == CGMS_ATT_ERROR_CODE_CLIENT_CHAR_CONF_DESC_IMPROPERLY_CONFIGURED)
              {
                LOG_INFO_APP("ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE CGMS_RACP >>> CLIENT CHAR CONF DESC IMPROPERLY CONFIGURED (0x%02X)\r\n", error_code);
              }
            }
            /*USER CODE END Service1_Char_6_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          } /*if(p_write_perm_req->Attribute_Handle == (CGMS_Context.RacpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

          else if(p_write_perm_req->Attribute_Handle == (CGMS_Context.CfocpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service1_Char_7_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            LOG_INFO_APP("ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE CGMS_CGMSOCP, data length: %d\r\n", p_write_perm_req->Data_Length);
            error_code = CGMS_SOCP_CheckRequestValid(p_write_perm_req->Data, p_write_perm_req->Data_Length);
            if (error_code == 0x00)
            {
              aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                  p_write_perm_req->Attribute_Handle,
                                  0x00,
                                  0x00,
                                  p_write_perm_req->Data_Length,
                                  p_write_perm_req->Data);
          
              LOG_INFO_APP("ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE CGMS_CGMSOCP >>> PERMITTED\r\n");
              CGMS_SOCP_RequestHandler(p_write_perm_req->Data, p_write_perm_req->Data_Length);
            }
            else 
            {
              aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                  p_write_perm_req->Attribute_Handle,
                                  0x01,
                                  error_code,
                                  p_write_perm_req->Data_Length,
                                  p_write_perm_req->Data);
              LOG_INFO_APP("ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE CGMS_CGMSOCP >>> NOT PERMITTED (0x%02X)\r\n", error_code);
            }
            /*USER CODE END Service1_Char_7_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          } /*if(p_write_perm_req->Attribute_Handle == (CGMS_Context.CfocpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

          /* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ_END */

          /* USER CODE END EVT_BLUE_GATT_WRITE_PERMIT_REQ_END */
          break;/* ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
        }
        case ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE:
        {
          aci_gatt_tx_pool_available_event_rp0 *p_tx_pool_available_event;
          p_tx_pool_available_event = (aci_gatt_tx_pool_available_event_rp0 *) p_blecore_evt->data;
          UNUSED(p_tx_pool_available_event);

          /* USER CODE BEGIN ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
          LOG_INFO_APP("ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE\n\r");
          /* USER CODE END ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
          break;/* ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE*/
        }
        case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
        {
          aci_att_exchange_mtu_resp_event_rp0 *p_exchange_mtu;
          p_exchange_mtu = (aci_att_exchange_mtu_resp_event_rp0 *)  p_blecore_evt->data;
          UNUSED(p_exchange_mtu);

          /* USER CODE BEGIN ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */

          /* USER CODE END ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
          break;/* ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
        }
        /* USER CODE BEGIN BLECORE_EVT */

        /* USER CODE END BLECORE_EVT */
        default:
          /* USER CODE BEGIN EVT_DEFAULT */

          /* USER CODE END EVT_DEFAULT */
          break;
      }
      /* USER CODE BEGIN EVT_VENDOR*/

      /* USER CODE END EVT_VENDOR*/
      break; /* HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE */

      /* USER CODE BEGIN EVENT_PCKT_CASES*/

      /* USER CODE END EVENT_PCKT_CASES*/

    default:
      /* USER CODE BEGIN EVENT_PCKT*/

      /* USER CODE END EVENT_PCKT*/
      break;
  }

  /* USER CODE BEGIN Service1_EventHandler_2 */

  /* USER CODE END Service1_EventHandler_2 */

  return(return_value);
}/* end CGMS_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void CGMS_Init(void)
{
  Char_UUID_t  uuid;
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  uint8_t max_attr_record;

  /* USER CODE BEGIN SVCCTL_InitService1Svc_1 */

  /* USER CODE END SVCCTL_InitService1Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  SVCCTL_RegisterSvcHandler(CGMS_EventHandler);

  /**
   * CGMS
   *
   * Max_Attribute_Records = 1 + 2*7 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
   * service_max_attribute_record = 1 for CGMS +
   *                                2 for CGM +
   *                                2 for CGF +
   *                                2 for CGS +
   *                                2 for CSST +
   *                                2 for CSRT +
   *                                2 for RACP +
   *                                2 for CFOCP +
   *                                1 for CGM configuration descriptor +
   *                                1 for CGF configuration descriptor +
   *                                1 for RACP configuration descriptor +
   *                                1 for CFOCP configuration descriptor +
   *                              = 19
   * This value doesn't take into account number of descriptors manually added
   * In case of descriptors added, please update the max_attr_record value accordingly in the next SVCCTL_InitService User Section
   */
  max_attr_record = 19;

  /* USER CODE BEGIN SVCCTL_InitService */
  /* max_attr_record to be updated if descriptors have been added */

  /* USER CODE END SVCCTL_InitService */

  uuid.Char_UUID_16 = 0x181f;
  ret = aci_gatt_add_service(UUID_TYPE_16,
                             (Service_UUID_t *) &uuid,
                             PRIMARY_SERVICE,
                             max_attr_record,
                             &(CGMS_Context.CgmsSvcHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_service command: CGMS, error code: 0x%x \n\r", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_service command: CGMS \n\r");
  }

  /**
   * CGM
   */
  uuid.Char_UUID_16 = 0x2aa7;
  ret = aci_gatt_add_char(CGMS_Context.CgmsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeCgm,
                          CHAR_PROP_NOTIFY,
                          ATTR_PERMISSION_AUTHEN_READ | ATTR_PERMISSION_AUTHEN_WRITE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(CGMS_Context.CgmCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : CGM, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : CGM\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char1 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char1 */

  /**
   * CGF
   */
  uuid.Char_UUID_16 = 0x2aa8;
  ret = aci_gatt_add_char(CGMS_Context.CgmsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeCgf,
                          CHAR_PROP_READ | CHAR_PROP_INDICATE,
                          ATTR_PERMISSION_AUTHEN_READ,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(CGMS_Context.CgfCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : CGF, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : CGF\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char2 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char2 */

  /**
   * CGS
   */
  uuid.Char_UUID_16 = 0x2aa9;
  ret = aci_gatt_add_char(CGMS_Context.CgmsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeCgs,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_AUTHEN_READ,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(CGMS_Context.CgsCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : CGS, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : CGS\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char3 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char3 */

  /**
   * CSST
   */
  uuid.Char_UUID_16 = 0x2aaa;
  ret = aci_gatt_add_char(CGMS_Context.CgmsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeCsst,
                          CHAR_PROP_READ | CHAR_PROP_WRITE,
                          ATTR_PERMISSION_AUTHEN_READ | ATTR_PERMISSION_AUTHEN_WRITE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(CGMS_Context.CsstCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : CSST, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : CSST\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char4 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char4 */

  /**
   * CSRT
   */
  uuid.Char_UUID_16 = 0x2aab;
  ret = aci_gatt_add_char(CGMS_Context.CgmsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeCsrt,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_AUTHEN_READ,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(CGMS_Context.CsrtCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : CSRT, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : CSRT\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char5 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char5 */

  /**
   * RACP
   */
  uuid.Char_UUID_16 = 0x2a52;
  ret = aci_gatt_add_char(CGMS_Context.CgmsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeRacp,
                          CHAR_PROP_WRITE | CHAR_PROP_INDICATE,
                          ATTR_PERMISSION_AUTHEN_READ | ATTR_PERMISSION_AUTHEN_WRITE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(CGMS_Context.RacpCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : RACP, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : RACP\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char6 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char6 */

  /**
   * CFOCP
   */
  uuid.Char_UUID_16 = 0x2aac;
  ret = aci_gatt_add_char(CGMS_Context.CgmsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeCfocp,
                          CHAR_PROP_WRITE | CHAR_PROP_INDICATE,
                          ATTR_PERMISSION_AUTHEN_READ | ATTR_PERMISSION_AUTHEN_WRITE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(CGMS_Context.CfocpCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : CFOCP, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : CFOCP\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char7 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char7 */

  /* USER CODE BEGIN SVCCTL_InitService1Svc_2 */

  /* USER CODE END SVCCTL_InitService1Svc_2 */

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  Service_Instance: Instance of the service to which the characteristic belongs
 *
 */
tBleStatus CGMS_UpdateValue(CGMS_CharOpcode_t CharOpcode, CGMS_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service1_App_Update_Char_1 */

  /* USER CODE END Service1_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case CGMS_CGM:
      ret = aci_gatt_update_char_value(CGMS_Context.CgmsSvcHdle,
                                       CGMS_Context.CgmCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value CGM command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value CGM command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_1*/

      /* USER CODE END Service1_Char_Value_1*/
      break;

    case CGMS_CGF:
      ret = aci_gatt_update_char_value(CGMS_Context.CgmsSvcHdle,
                                       CGMS_Context.CgfCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value CGF command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value CGF command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_2*/

      /* USER CODE END Service1_Char_Value_2*/
      break;

    case CGMS_CGS:
      ret = aci_gatt_update_char_value(CGMS_Context.CgmsSvcHdle,
                                       CGMS_Context.CgsCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value CGS command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value CGS command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_3*/

      /* USER CODE END Service1_Char_Value_3*/
      break;

    case CGMS_CSST:
      ret = aci_gatt_update_char_value(CGMS_Context.CgmsSvcHdle,
                                       CGMS_Context.CsstCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value CSST command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value CSST command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_4*/

      /* USER CODE END Service1_Char_Value_4*/
      break;

    case CGMS_CSRT:
      ret = aci_gatt_update_char_value(CGMS_Context.CgmsSvcHdle,
                                       CGMS_Context.CsrtCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value CSRT command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value CSRT command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_5*/

      /* USER CODE END Service1_Char_Value_5*/
      break;

    case CGMS_RACP:
      ret = aci_gatt_update_char_value(CGMS_Context.CgmsSvcHdle,
                                       CGMS_Context.RacpCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value RACP command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value RACP command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_6*/

      /* USER CODE END Service1_Char_Value_6*/
      break;

    case CGMS_CFOCP:
      ret = aci_gatt_update_char_value(CGMS_Context.CgmsSvcHdle,
                                       CGMS_Context.CfocpCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value CFOCP command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value CFOCP command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_7*/

      /* USER CODE END Service1_Char_Value_7*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service1_App_Update_Char_2 */

  /* USER CODE END Service1_App_Update_Char_2 */

  return ret;
}
