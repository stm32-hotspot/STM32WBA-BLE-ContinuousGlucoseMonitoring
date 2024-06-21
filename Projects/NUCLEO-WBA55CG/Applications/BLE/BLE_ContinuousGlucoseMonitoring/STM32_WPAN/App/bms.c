/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service3.c
  * @author  MCD Application Team
  * @brief   service3 definition.
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
#include "bms.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  BmsSvcHdle;                  /**< Bms Service Handle */
  uint16_t  BmcpCharHdle;                  /**< BMCP Characteristic Handle */
  uint16_t  BmfCharHdle;                  /**< BMF Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}BMS_Context_t;

/* Private defines -----------------------------------------------------------*/
#define UUID_128_SUPPORTED  1

#if (UUID_128_SUPPORTED == 1)
#define BM_UUID_LENGTH  UUID_TYPE_128
#else
#define BM_UUID_LENGTH  UUID_TYPE_16
#endif

#define BM_REQ_CHAR_SIZE    (3)

/* USER CODE BEGIN PD */
/* Record Access Control Point: fields position */
#define BMS_CNTL_POINT_OP_CODE_POSITION                                      (0)

#define BMS_CNTL_POINT_OP_CODE_RFU                                        (0x00)
#define BMS_CNTL_POINT_OP_DELETE_BOND_OF_REQUESTING_DEVICE                (0x03)
#define BMS_CNTL_POINT_OP_DELETE_ALL_BONDS_ON_SERVER                      (0x06)
#define BMS_CNTL_POINT_OP_DELETE_ALL_BUT_THE_ACTIVE_BOND_ON_SERVER        (0x09)

#define BMS_CNTL_POINT_COMMAND_SUPPORTED                                  (0x00)
#define BMS_CNTL_POINT_COMMAND_NOT_SUPPORTED                              (0x80)
#define BMS_CNTL_POINT_OPERATION_FAILED                                   (0x81)
#define BMS_CNTL_POINT_INSUFFICIENT_AUTHORIZATION                         (0x08)

#define CGMS_RACP_RESPONSE_LENGTH                (4)  /* RACP response length */
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
static const uint16_t SizeBmcp = 512;
static const uint16_t SizeBmf = 10;

static BMS_Context_t BMS_Context;

/* USER CODE BEGIN PV */
uint8_t AuthCode[12] = 
{
  0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t BMS_EventHandler(void *p_pckt);

/* USER CODE BEGIN PFP */

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
/**
* @brief Bond Management Control Point request handler 
* @param requestData: pointer to received BMCP request data
* @param requestDataLength: received BMCP request length
* @retval None
*/
static void BMS_BMCP_RequestHandler(uint8_t * pRequestData, uint8_t requestDataLength)
{
  LOG_INFO_APP("BMCP Request, request data length: %d\r\n", requestDataLength);
  
  /* Check and Process the OpCode */
  switch(pRequestData[BMS_CNTL_POINT_OP_CODE_POSITION])
  {
    case BMS_CNTL_POINT_OP_DELETE_ALL_BONDS_ON_SERVER:
      {
        tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
        
        LOG_INFO_APP("BMS_BMCP_OP_DELETE_ALL_BONDS_ON_SERVER\r\n");
  
        /* Clear Security Database */
        ret = aci_gap_clear_security_db();
        if (ret != BLE_STATUS_SUCCESS)
        {
          LOG_INFO_APP("==>> aci_gap_clear_security_db - Fail, result: %d \n", ret);
        }
        else
        {
          LOG_INFO_APP("==>> aci_gap_clear_security_db - Success\n");
        }
      }
      break;
    
    case BMS_CNTL_POINT_OP_DELETE_BOND_OF_REQUESTING_DEVICE:
      {
        tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
        
        LOG_INFO_APP("BMS_CNTL_POINT_OP_DELETE_BOND_OF_REQUESTING_DEVICE\r\n");
  
        /* Clear Security Database */
        ret = aci_gap_clear_security_db();

        if (ret != BLE_STATUS_SUCCESS)
        {
          LOG_INFO_APP("==>> aci_gap_clear_security_db - Fail, result: %d \n", ret);
        }
        else
        {
          LOG_INFO_APP("==>> aci_gap_clear_security_db - Success\n");
        }
      }
      break;
    
    default:
      break;
  }
} /* end of CGMS_RACP_RequestHandler() */
/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static SVCCTL_EvtAckStatus_t BMS_EventHandler(void *p_Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *p_event_pckt;
  evt_blecore_aci *p_blecore_evt;
  aci_gatt_attribute_modified_event_rp0 *p_attribute_modified;
  aci_gatt_write_permit_req_event_rp0   *p_write_perm_req;
  BMS_NotificationEvt_t                 notification;
  /* USER CODE BEGIN Service3_EventHandler_1 */
  uint8_t i;
  tBleStatus hciCmdResult = BLE_STATUS_SUCCESS;
  uint16_t data_length;
  uint8_t buffer[3];
  uint32_t feature = 0;
  /* USER CODE END Service3_EventHandler_1 */

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
          if(p_attribute_modified->Attr_Handle == (BMS_Context.BmcpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = BMS_BMCP_WRITE_EVT;
            /* USER CODE BEGIN Service3_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

            /* USER CODE END Service3_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            BMS_Notification(&notification);
          } /* if(p_attribute_modified->Attr_Handle == (BMS_Context.BmcpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

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
          if(p_write_perm_req->Attribute_Handle == (BMS_Context.BmcpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /*USER CODE BEGIN Service3_Char_1_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
            LOG_INFO_APP ("ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE: Op Code = %d, Operand = \r\n",
                          p_write_perm_req->Data[0]);
            for(i = 1; i < p_write_perm_req->Data_Length; i++)
            {
              LOG_INFO_APP ("0x%02X ", p_write_perm_req->Data[i]);
            }
            LOG_INFO_APP("\n");
            
            hciCmdResult = aci_gatt_read_handle_value(BMS_Context.BmfCharHdle + 1, 
                                                      0, 
                                                      3,
                                                      &data_length, 
                                                      &data_length, 
                                                      &(buffer[0]));
            if(hciCmdResult != BLE_STATUS_SUCCESS)
            {
              LOG_INFO_APP("BMS Feature Characteristic read failed with error: 0x%X !\r\n",
                              hciCmdResult);
            }
            else
            {
              feature = buffer[0] +(buffer[1] << 8) + (buffer[2] << 16);
              LOG_INFO_APP("BMS Feature Characteristic read successfully 0x%X !\r\n", feature);
            }
            
            if(((p_write_perm_req->Data[0] == 0) || (p_write_perm_req->Data[0] > 9))               ||
               ((p_write_perm_req->Data[0] == 1) && (!(feature & 1)     && !(feature & 2)))      ||
               ((p_write_perm_req->Data[0] == 2) && (!(feature & 4)     && !(feature & 8)))      ||
               ((p_write_perm_req->Data[0] == 3) && (!(feature & 16)    && !(feature & 32)))     ||
               ((p_write_perm_req->Data[0] == 4) && (!(feature & 64)    && !(feature & 128)))    ||
               ((p_write_perm_req->Data[0] == 5) && (!(feature & 256)   && !(feature & 512)))    ||
               ((p_write_perm_req->Data[0] == 6) && (!(feature & 1024)  && !(feature & 2048)))   ||
               ((p_write_perm_req->Data[0] == 7) && (!(feature & 4096)  && !(feature & 8192)))   ||
               ((p_write_perm_req->Data[0] == 8) && (!(feature & 16384) && !(feature & 32768)))  ||
               ((p_write_perm_req->Data[0] == 9) && (!(feature & 65535) && !(feature & 131072))))
            {
              /* received a not supported value for BM control point char */
              aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                  p_write_perm_req->Attribute_Handle,
                                  0x01, //* write_status = 1 (error))
                                  (uint8_t)BMS_CNTL_POINT_COMMAND_NOT_SUPPORTED, //* err_code 
                                  p_write_perm_req->Data_Length,
                                  (uint8_t *)&p_write_perm_req->Data[0]);
              LOG_INFO_APP("BMS_CNTL_POINT_COMMAND_NOT_SUPPORTED !");
            }
            else
            {
              i = 1;
              while((p_write_perm_req->Data[i] == AuthCode[i-1]) &&
                    (i < p_write_perm_req->Data_Length))
              {
                i++;
              }
              
              if(i == p_write_perm_req->Data_Length)
              {
                /* received a correct value for BM control point char */
                aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                    p_write_perm_req->Attribute_Handle,
                                    0x00, //* write_status = 1 (No error))
                                    (uint8_t)BMS_CNTL_POINT_COMMAND_SUPPORTED, //* err_code 
                                    p_write_perm_req->Data_Length,
                                    (uint8_t *)&p_write_perm_req->Data[0]);
                LOG_INFO_APP("BMS_CNTL_POINT_COMMAND_SUPPORTED ! \n");
                BMS_BMCP_RequestHandler(p_write_perm_req->Data, p_write_perm_req->Data_Length);
              }
              else
              {
                /* received a not supported value for BM control point char */
                aci_gatt_write_resp(p_write_perm_req->Connection_Handle,
                                    p_write_perm_req->Attribute_Handle,
                                    0x01, //* write_status = 1 (error))
                                    (uint8_t)BMS_CNTL_POINT_INSUFFICIENT_AUTHORIZATION, //* err_code 
                                    p_write_perm_req->Data_Length,
                                    (uint8_t *)&p_write_perm_req->Data[0]);
                LOG_INFO_APP("BMS_CNTL_POINT_INSUFFICIENT_AUTHORIZATION ! 0x%x != 0x%x",
                                p_write_perm_req->Data[i],
                                AuthCode[i]);
              }
            }
            /*USER CODE END Service3_Char_1_ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE*/
          } /*if(p_write_perm_req->Attribute_Handle == (BMS_Context.BmcpCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

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

  /* USER CODE BEGIN Service3_EventHandler_2 */

  /* USER CODE END Service3_EventHandler_2 */

  return(return_value);
}/* end BMS_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void BMS_Init(void)
{
  Char_UUID_t  uuid;
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  uint8_t max_attr_record;

  /* USER CODE BEGIN SVCCTL_InitService3Svc_1 */

  /* USER CODE END SVCCTL_InitService3Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  SVCCTL_RegisterSvcHandler(BMS_EventHandler);

  /**
   * BMS
   *
   * Max_Attribute_Records = 1 + 2*2 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
   * service_max_attribute_record = 1 for BMS +
   *                                2 for BMCP +
   *                                2 for BMF +
   *                              = 5
   * This value doesn't take into account number of descriptors manually added
   * In case of descriptors added, please update the max_attr_record value accordingly in the next SVCCTL_InitService User Section
   */
  max_attr_record = 5;

  /* USER CODE BEGIN SVCCTL_InitService */
  /* max_attr_record to be updated if descriptors have been added */

  /* USER CODE END SVCCTL_InitService */

  uuid.Char_UUID_16 = 0x181e;
  ret = aci_gatt_add_service(UUID_TYPE_16,
                             (Service_UUID_t *) &uuid,
                             PRIMARY_SERVICE,
                             max_attr_record,
                             &(BMS_Context.BmsSvcHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_service command: BMS, error code: 0x%x \n\r", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_service command: BMS \n\r");
  }

  /**
   * BMCP
   */
  uuid.Char_UUID_16 = 0x2aa4;
  ret = aci_gatt_add_char(BMS_Context.BmsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeBmcp,
                          CHAR_PROP_WRITE,
                          ATTR_PERMISSION_AUTHEN_WRITE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE | GATT_NOTIFY_WRITE_REQ_AND_WAIT_FOR_APPL_RESP,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(BMS_Context.BmcpCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : BMCP, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : BMCP\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService3Char1 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService3Char1 */

  /**
   * BMF
   */
  uuid.Char_UUID_16 = 0x2aa5;
  ret = aci_gatt_add_char(BMS_Context.BmsSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeBmf,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_NONE/*ATTR_PERMISSION_AUTHEN_READ*/,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(BMS_Context.BmfCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : BMF, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : BMF\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService3Char2 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService3Char2 */

  /* USER CODE BEGIN SVCCTL_InitService3Svc_2 */

  /* USER CODE END SVCCTL_InitService3Svc_2 */

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  Service_Instance: Instance of the service to which the characteristic belongs
 *
 */
tBleStatus BMS_UpdateValue(BMS_CharOpcode_t CharOpcode, BMS_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service3_App_Update_Char_1 */

  /* USER CODE END Service3_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case BMS_BMCP:
      ret = aci_gatt_update_char_value(BMS_Context.BmsSvcHdle,
                                       BMS_Context.BmcpCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value BMCP command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value BMCP command\n");
      }
      /* USER CODE BEGIN Service3_Char_Value_1*/

      /* USER CODE END Service3_Char_Value_1*/
      break;

    case BMS_BMF:
      ret = aci_gatt_update_char_value(BMS_Context.BmsSvcHdle,
                                       BMS_Context.BmfCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value BMF command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value BMF command\n");
      }
      /* USER CODE BEGIN Service3_Char_Value_2*/

      /* USER CODE END Service3_Char_Value_2*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service3_App_Update_Char_2 */

  /* USER CODE END Service3_App_Update_Char_2 */

  return ret;
}
