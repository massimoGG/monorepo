/**
 * @brief This Ethernet driver implements the STM32 Ethernet in chained
 * descriptor mode Buffers are statically allocated Store-and-forward mode is
 * used
 * @details
 * This driver creates 2 descriptor lists (for TX and RX) configured in Ring
 * Mode. The last descriptor has it's RER and TER bit set
 */
#include <assert.h>
#include <complex.h>
#include <stdint.h>
#include <string.h>

#include <debug.h>
#include <error_codes.h>

#include "ethernet_descriptors.h"
#include "ethernet_ll.h"
#include <eth/ethernet.h>

#include <stm32f4xx.h>

/**====================LOCAL DEFINES====================*/
/**====================LOCAL TYPEDEFS====================*/
/**====================STATIC VARIABLES====================*/

/** The DMA status */
static volatile uint32_t v_dmaStatus = 0;

/**====================LOCAL PROTOTYPES====================*/

static uint16_t swapU16(uint16_t param);

static error_e eth_resetMac(void);

/*** UNUSED - IRQ mode ***/
static void irqProcessRxPackets(void);

/**====================PUBLIC FUNCTIONS====================*/

/**
 * @brief Ethernet IRQ handler
 * @warning in IRQ context
 */
void eth_irqHandler(void) {
  /**
   * The interrupt register bits only indicate the block from which the event is
   * reported. You have to read the corresponding status registers and other
   * registers to clear the interrupt. For example, bit 3 of the Interrupt
   * register, set high, indicates that the Magic packet or Wake-on- LAN frame
   * is received in Power-down mode. You must read the ETH_MACPMTCSR register to
   * clear this interrupt event.
   * @todo Handle \p macStatus
   */
  const uint32_t macStatus = ETH->MACSR;
  UNUSED(macStatus);

  DBG_DBG("%s", __func__);

  /** @brief handle DMA Status Register */
  v_dmaStatus = ETH->DMASR;

  /* Normal DMA interrupt -> Bit 0, 2, 6, 14 */
  if (READ_BIT(v_dmaStatus, ETH_DMASR_NIS)) {

    /* Early receive status */
    if (READ_BIT(v_dmaStatus, ETH_DMASR_ERS)) {
      /* -> Nothing to do, Receive status bit auto clears this bit */
      DBG_DBG("Early receive status");
    }

    /* Received a packet! */
    if (READ_BIT(v_dmaStatus, ETH_DMASR_RS)) {
      irqProcessRxPackets();
    }

    /* Transmit buffer unavailable status */
    if (READ_BIT(v_dmaStatus, ETH_DMASR_TBUS)) {
      /* -> Next descriptor is owned by host -> Transmission suspended -> All
       * packets transfered */
      DBG_DBG("Transmit buffer unavailable");
    }

    /* Transmit status */
    if (READ_BIT(v_dmaStatus, ETH_DMASR_TS)) {
      /* Frame transmission is finished */
      /* TDES1[31] is set in the first descriptor */
      DBG_DBG("Frame transmit finished");
    }

    /* Clear sticky bit */
    SET_BIT(ETH->DMASR, ETH_DMASR_NIS);
  }

  /* Abnormal DMA interrupt -> Bit 1, 3, 4, 5, 7, 8, 9, 10, 13 */
  if (READ_BIT(v_dmaStatus, ETH_DMASR_AIS)) {
    /* Fatal bus error status */
    if (READ_BIT(v_dmaStatus, ETH_DMASR_FBES)) {
      /* Read Error bit status */
      if (READ_BIT(v_dmaStatus, ETH_DMASR_EBS_DataTransfTx)) {
        DBG_ERR("Error during data transfer by txDMA");
      } else {
        DBG_ERR("Error during data transfer by rxDMA");
      }

      if (READ_BIT(v_dmaStatus, ETH_DMASR_EBS_ReadTransf)) {
        DBG_ERR("Error during read transfer ");
      } else {
        DBG_ERR("Error during write transfer ");
      }

      if (READ_BIT(v_dmaStatus, ETH_DMASR_EBS_DescAccess)) {
        DBG_ERR("Error during descriptor access");
      } else {
        DBG_ERR("Error during data buffer access");
      }
    }

    if (READ_BIT(v_dmaStatus, ETH_DMASR_ETS)) {
      DBG_ERR("Early transmit status");
    }

    if (READ_BIT(v_dmaStatus, ETH_DMASR_RWTS)) {
      DBG_ERR("Receive watchdog timeout status");
    }

    if (READ_BIT(v_dmaStatus, ETH_DMASR_RPSS)) {
      DBG_ERR("Receive process stopped status");
    }

    if (READ_BIT(v_dmaStatus, ETH_DMASR_RBUS)) {
      DBG_ERR("Receive buffer unavailable status");
    }

    if (READ_BIT(v_dmaStatus, ETH_DMASR_TUS)) {
      DBG_ERR("Transmit underflow status");
    }

    if (READ_BIT(v_dmaStatus, ETH_DMASR_ROS)) {
      DBG_ERR("Receive overflow status");
    }

    if (READ_BIT(v_dmaStatus, ETH_DMASR_TJTS)) {
      DBG_ERR("Transmit jabber timeout status");
    }

    if (READ_BIT(v_dmaStatus, ETH_DMASR_TPSS)) {
      DBG_ERR("Transmit process stoppped status");
    }

    /* Clear sticky bit */
    SET_BIT(ETH->DMASR, ETH_DMASR_AIS);
  }
}

/**
 * @brief initializes the ethernet driver
 * @retval eError_ok
 */
error_e eth_init(void) {

  eth_ll_init();

  /* MAC, MTL, and DMA default configuration */
  eth_resetMac();
  eth_setDmaConfig();
  eth_setMacConfig();

  /* 3. Create transmit and receive descriptor lists. */
  eth_initRxDescriptors();
  eth_initTxDescriptors();

  return eError_ok;
}

/**
 * @brief deinitializes the ethernet driver
 * @retval eError_ok
 */
error_e eth_deinit(void) {

  eth_ll_deinit();

  return eError_ok;
}

/**
 * @brief starts the ethernet MAC & DMA
 * @retval eError_ok
 */
error_e eth_start(void) {

  eth_ll_start();

  return eError_ok;
}

/**
 * @brief stops the ethernet MAC & DMA
 * @retval eError_ok
 */
error_e eth_stop(void) {
  eth_ll_stop();

  return eError_ok;
}

/**
 * @brief logs debug info
 * @return error_e
 */
void eth_debug(void) {

  DBG_INFO("\n********** MAC **********");
  const mac_t macAddress = eth_getMacAddr();
  DBG_INFO("\nMAC address is %02X:%02X:%02X:%02X:%02X:%02X", macAddress.addr[0],
           macAddress.addr[1], macAddress.addr[2], macAddress.addr[3],
           macAddress.addr[4], macAddress.addr[5]);

  /* Read MAC debug */
  const uint32_t macDebug = ETH->MACDBGR;
  {
    /* Receiver status */
    if (!READ_BIT(macDebug, ETH_MACDBGR_RFWRA)) {
      DBG_ERR("MAC RX FIFO write controller not active");
    }
  }

  {
    char *miiMode;
    if (READ_BIT(ETH->MACCR, ETH_MACCR_FES)) {
      miiMode = "100 Mbit";
    } else {
      miiMode = "10 Mbit";
    }

    char *duplexMode;
    if (READ_BIT(ETH->MACCR, ETH_MACCR_DM)) {
      duplexMode = "Full-duplex";
    } else {
      duplexMode = "Half-duplex";
    }

    DBG_DBG("Negotiated speed: %s, duplex mode: %s\n", miiMode, duplexMode);
  }

  /* Read status */
  const uint32_t v_dmaStatus = ETH->DMASR;
  DBG_INFO("DMASR: %08X", v_dmaStatus);

  {
    const uint32_t transmitStatus =
        (v_dmaStatus & ETH_DMASR_TPS_Msk) >> ETH_DMASR_TPS_Pos;
    char *str_transmitStatus;
    switch (transmitStatus) {
    case 0:
      str_transmitStatus = "Stopped: Reset or Stop Transmit Command issued";
      break;
    case 1:
      str_transmitStatus = "Running: Fetching transmit transfer descriptor";
      break;
    case 2:
      str_transmitStatus = "Running: Waiting for status";
      break;
    case 3:
      str_transmitStatus =
          "Running: Reading Data from host mem and queuing to transmit buffer";
      break;
    case 6:
      str_transmitStatus = "Suspended: Transmit descriptor unavailable or "
                           "transmit buffer underflow";
      break;
    case 7:
      str_transmitStatus = "Running: Closing transmit descriptor";
      break;
    default:
      str_transmitStatus = "Unknown state";
    }
    DBG_DBG("DMA Transmit status: %s", str_transmitStatus);
  }

  {
    const uint32_t receiveStatus =
        (v_dmaStatus & ETH_DMASR_RPS_Msk) >> ETH_DMASR_RPS_Pos;
    char *str_receiveStatus;
    switch (receiveStatus) {
    case 0:
      str_receiveStatus = "Stopped: Reset or Stop Receive Command issued";
      break;
    case 1:
      str_receiveStatus = "Running: Fetching receive transfer descriptor";
      break;
    case 3:
      str_receiveStatus = "Running: Waiting for receive packet";
      break;
    case 4:
      str_receiveStatus = "Suspended: Receive descriptor unavailable";
      break;
    case 5:
      str_receiveStatus = "Running: Closing receive descriptor";
      break;
    case 7:
      str_receiveStatus = "Running: Transferring the receive packet data";
      break;
    default:
      str_receiveStatus = "Unknown state";
    }
    DBG_DBG("DMA Receive status: %s", str_receiveStatus);
  }
}

/**
 * @brief reads a packet from the RX descriptors if available, resets the
 * descriptor after reading
 * @param[in,out] pBuf
 * @param[out] pLength
 * @retval eError_ok
 * @retval eError_noData
 */
error_e eth_readData(uint8_t *pBuf, uint16_t *pLength) {
  error_e ret = eError_ok;

  volatile rxDescriptor_t *pDesc = eth_getRxDescriptor();
  if (NULL == pDesc) {
    return eError_noData;
  }

  uint8_t *pData = NULL;
  *pLength = eth_getRxPacket(pDesc, &pData);

  if (0 == *pLength) {
    ret = eError_noData;
  } else {
    memcpy(pBuf, pData, *pLength);
  }

  eth_clearRxDescriptor(pDesc);

  return ret;
}

/**
 * @brief writes a packet to an available TX descriptor
 * @param[in] pBuf
 * @param[in] length
 * @todo  The same frame should be retransmitted from SOF on observing a Retry
 * request (in the Status) from the MAC. The MAC issues an underflow status if
 * the data are not provided continuously during the transmission.
 * @retval eError_busy  no space available
 * @retval eError_ok    successfully sent
 */
error_e eth_writeData(const uint8_t *pBuf, uint16_t length) {
  error_e ret = eError_ok;

  /* Fetch the current TX descriptor ptr */
  const uint32_t currentTxPtr = ETH->DMACHTDR;

  // volatile txDescriptor_t *pDesc = eth_getTxDescriptor();
  volatile txDescriptor_t *pDesc = (volatile txDescriptor_t *)currentTxPtr;
  if (NULL == pDesc) {
    return eError_busy;
  }
  DBG_DBG("current TX PTR: %08X %08X", currentTxPtr, (uint32_t)pDesc);

  eth_setTxPacket(pDesc, pBuf, length);

  /* Start TX transmission */
  SET_BIT(ETH->DMAOMR, ETH_DMAOMR_ST);

  /* Wake up DMA TX if it went into suspend */
  ETH->DMATPDR = ETH->DMACHTDR;

  /* Block until the descriptor is given back to the CPU */
  while (READ_BIT(pDesc->TDES0, TDES0_OWN_Msk)) {
  }

  /* Read back potential error */
  DBG_NOTICE("%s: TDES0: %08X", __func__, pDesc->TDES0);

  return ret;
}

/**
 * @brief forms a packet
 * @param[out] pBuf
 * @param[in] dst
 * @param[in] src
 * @param[in] type
 * @param[in] pData
 * @param[in] length
 * @return uint16_t total size of packet
 */
uint16_t eth_createPacket(uint8_t *pBuf, mac_t dst, mac_t src, etherType_e type,
                          const void *pData, uint16_t length) {
  uint16_t totalLength = 0;

  /* Destination MAC */
  memcpy(pBuf, &dst.addr[0], sizeof(dst.addr));

  /* Source MAC*/
  memcpy(pBuf + 6, &src.addr[0], sizeof(src.addr));

  /* EtherType */
  uint16_t type_v = (uint16_t)type;
  *(pBuf + 12) = (uint8_t)(type_v >> 8);
  *(pBuf + 13) = (uint8_t)(type_v & 0xFF);

  totalLength = 14;

  if (pData != NULL && length != 0) {
    memcpy(pBuf + 14, pData, length);
    totalLength += length;
  } else {
    DBG_WARN("Zero length data! Must generate padding of 48?");
  }

  return totalLength;
}

/**
 * @brief gets the current MAC configuration
 * @return error_e
 */
error_e eth_getMacConfig(void) { return eError_unsupported; }

/**
 * @brief sets the MAC configuration
 * @return error_e
 */
error_e eth_setMacConfig(void) {

  uint32_t tmpReg = 0;
  {
    tmpReg = ETH->MACFFR;
    /* 4. Write to MAC regs 1,2,3 to choose the desired filtering options */

    /* Enable perfect destination filtering */
    SET_BIT(tmpReg, ETH_MACFFR_HPF); /* Enable hash or perfect filter */

    /*** ETHERNET MAC FRAME FILTER  */
    /* Don't Receive all frames */
    CLEAR_BIT(tmpReg, ETH_MACFFR_RA);
    /* Don't Pass all broadcast */
    SET_BIT(tmpReg, ETH_MACFFR_BFD);
    /* Don't Pass all multicast */
    CLEAR_BIT(tmpReg, ETH_MACFFR_PAM);

    /* Write register */
    ETH->MACFFR = tmpReg;
  }

  {
    tmpReg = ETH->MACCR;

    /* 5. Write to the MAC ETH->MACCR reg to configure and enable the transmit
    and receive operating modes. The PS and DM bits are set based on the
    auto-negotation result read from the PHY */

    /** @todo read from PHY or config. For now hardcode :) */
    /* Set 100Mbit */
    SET_BIT(tmpReg, ETH_MACCR_FES);
    /* Set Full-Duplex */
    SET_BIT(tmpReg, ETH_MACCR_DM);

    /* IPv4 checksum offload */
    SET_BIT(tmpReg, ETH_MACCR_IPCO);
    /* Enable transmitter */
    MODIFY_REG(tmpReg, ETH_MACCR_TE_Msk, ETH_MACCR_TE);
    /* Enable receiver */
    MODIFY_REG(tmpReg, ETH_MACCR_RE_Msk, ETH_MACCR_RE);

    ETH->MACCR = tmpReg;
  }

  return eError_ok;
}

/**
 * @brief gets the current DMA configuration
 * @return error_e
 */
error_e eth_getDmaConfig(void) { return eError_unsupported; }

/**
 * @brief sets the DMA configuration
 * @return error_e
 */
error_e eth_setDmaConfig(void) {

  uint32_t tmpReg = 0;
  {
    tmpReg = ETH->DMABMR;

    // CLEAR_BIT(tmp_dmaBmr, ETH_DMABMR_MB); /* Disable mixed-bursts */
    /* Fixed-Bursts */
    // SET_BIT(tmp_dmaBmr, ETH_DMABMR_FB);
    /* Programmable Burst Length - Set to 16 beats of transfer */
    // MODIFY_REG(tmp_dmaBmr, ETH_DMABMR_RDP_16Beat, ETH_DMABMR_RDP_16Beat);
    /* Enhanced Descriptors (Set => IPv4 checksum offloaded) */
    SET_BIT(tmpReg, ETH_DMABMR_EDE);
    /* Descriptor skip length - If 0 = DMA ring mode otherwise it's the number
     * of words to skip */
    // MODIFY_REG(tmp_dmaBmr, ETH_DMABMR_DSL_Msk, 0<<ETH_DMABMR_DSL_Pos);   /*
    // Ignoring DSL cuz idk what it does */

    ETH->DMABMR = tmpReg;
  }

  {
    tmpReg = ETH->DMAOMR;

    /* Store-and-forward RX */
    SET_BIT(tmpReg, ETH_DMAOMR_RSF);
    /* Enable flushing of RX frames if no descriptors available */
    CLEAR_BIT(tmpReg, ETH_DMAOMR_DFRF);
    /* Store-and-forward TX */
    SET_BIT(tmpReg, ETH_DMAOMR_TSF);
    /* Don't operator on second frame */
    CLEAR_BIT(tmpReg, ETH_DMAOMR_OSF);

    ETH->DMAOMR = tmpReg;
  }

  return eError_ok;
}

/**
 * @brief gets the current set first MAC filter address
 * @return mac_t
 */
mac_t eth_getMacAddr(void) {
  mac_t ret = {.addr = {
                   [0] = ETH->MACA0LR & 0xFF,
                   [1] = (ETH->MACA0LR >> 8) & 0xFF,
                   [2] = (ETH->MACA0LR >> 16) & 0xFF,
                   [3] = (ETH->MACA0LR >> 24) & 0xFF,
                   [4] = (ETH->MACA0HR) & 0xFF,
                   [5] = (ETH->MACA0HR >> 8) & 0xFF,
               }};

  return ret;
}

/**
 * @brief configures the first MAC filter to the given address
 * @param[in] addr
 * @return error_e
 */
error_e eth_setMacAddr(mac_t addr) {
  const uint32_t macHigh = (addr.addr[5] << 8) | (addr.addr[4]);
  const uint32_t macLow = (addr.addr[3] << 24) | (addr.addr[2] << 16) |
                          (addr.addr[1] << 8) | (addr.addr[0]);

  MODIFY_REG(ETH->MACA0HR, 0x0000FFFF, macHigh);
  MODIFY_REG(ETH->MACA0LR, 0xFFFFFFFF, macLow);

  return eError_ok;
}

/**
 * @brief processes an ethernet frame
 * @param pData
 * @return error_e
 */
error_e processFrame(void *pData) {
  /* Read Destination MAC */
  const mac_t *pDestMac = (mac_t *)pData;

  /* Read Source MAC */
  const mac_t *pSrcMac = (mac_t *)(pData + sizeof(mac_t));

  /* Read Frame Length */
  const uint16_t *pFrameLength = (uint16_t *)(pData + 2 * sizeof(mac_t));

  uint16_t length = swapU16(*pFrameLength);

  DBG_NOTICE("Received frame from [%02X:%02X:%02X:%02X:%02X:%02X] -> "
             "[%02X:%02X:%02X:%02X:%02X:%02X] (0x%04X)",
             pSrcMac->addr[0], pSrcMac->addr[1], pSrcMac->addr[2],
             pSrcMac->addr[3], pSrcMac->addr[4], pSrcMac->addr[5],
             pDestMac->addr[0], pDestMac->addr[1], pDestMac->addr[2],
             pDestMac->addr[3], pDestMac->addr[4], pDestMac->addr[5], length);

#if 0
  if (READ_BIT(pDesc->Status, RDES0_FT_Msk)) {
    DBG_DBG("Ethernet frame");
    /* Frame type = >= 0x0600 -> Ethernet-type*/
    /** If reset -> IEEE802.3 frame */
  } else {
    DBG_DBG("IEEE802.3 frame");
  }
#endif

  return eError_ok;
}

/**====================LOCAL FUNCTIONS====================*/

static error_e eth_resetMac(void) {

  /* Software Reset MAC */
  SET_BIT(ETH->DMABMR, ETH_DMABMR_SR);

  /* Read a 0 value before re-programming any register of the core */
  while (READ_BIT(ETH->DMABMR, ETH_DMABMR_SR)) {
    /** @todo Add timeout */
  }

  return eError_ok;
}

static uint16_t swapU16(uint16_t param) {
  return ((param & 0xFF) << 8) | (param >> 8);
}

/**
 * @brief Processes all received packets
 * @warning In IRQ context
 */
static void irqProcessRxPackets(void) {

  /**
   * @todo iterate through all received packets,
   * If not (FIRST AND LAST frame) -> Drop. We don't support > 1540 MTU
   * If OK RX packet -> Put in RB queue
   * eth_readPacket() reads from RB the first X bytes for IP header
   * identification Then read IPv4 header or IPv6 header Read length of bytes
   */

#if 0
  /* Fetch the current descriptor */
  volatile rxDescriptor_t *pDesc = &v_rxDescriptors[0] + sv_currentRxIndex;

  const uint32_t c_status = pDesc->Status;

  if (READ_BIT(c_status, RDES0_OWN_Msk)) {
    /* Currently owned by DMA */
    DBG_ERR("Current RXD owned by DMA");
    /** @todo iterate through all descriptors to correct \p sv_currentRxIndex ?
     */
    return;
  }

  /* Check for any errors */
  if (READ_BIT(c_status, RDES0_ES_Msk)) {
    DBG_ERR("Error in packet");
  } else {
    /* Frame length is given if Last Descriptor (RDES0[8]) is set and Descriptor
     * Error is not set (RDES0[14]) */
    if (READ_BIT(c_status, RDES0_LS_Msk) && !READ_BIT(c_status, RDES0_DE_Msk)) {
      /* Frame length indicates the byte length of the received frame including
       * CRC */
      const uint16_t frameLength =
          (c_status & RDES0_FRAMELENGTH_Msk) >> RDES0_FRAMELENGTH_Pos;
      DBG_NOTICE("RX Frame length: %db", frameLength);

    } else {
      /* Indicates the accumulated number of bytes that have beeen transfered to
       * the current frame */
      if (READ_BIT(c_status, RDES0_FS_Msk)) {
        DBG_WARN("First descriptor");
        /* When set -> Check first buffer */
        const uint32_t firstBufferLength =
            (pDesc->ControlBufferSize & RDES1_RBS1_Msk) >> RDES1_RBS1_Pos;
        DBG_WARN("First buffer length: %db", firstBufferLength);

        /* If size of first buffer =0 -> Second buffer contains beginning of
         * frame -> IF SIZE OF SECOND IS ALSO 0 -> See next descriptor for
         * beginning of the frame */
        const uint32_t secondBufferLength =
            (pDesc->ControlBufferSize & RDES1_RBS2_Msk) >> RDES1_RBS2_Pos;
        DBG_WARN("Second buffer length: %db ( SHOULD BE ZERO AS THE SECOND "
                 "BUFFER IS A POINTER IN CHAINED MODE )",
                 secondBufferLength);
      }
    }
    if (READ_BIT(c_status, RDES0_LS_Msk)) {
      /* Last descriptor */
      DBG_DBG("Last descriptor ");
    }
  }

  /* Reset descriptor for DMA */
  SET_BIT(pDesc->Status, RDES0_OWN_Msk);

  /* Progress pointer to the next */
  sv_currentRxIndex++;
  if (sv_currentRxIndex >= NUM_DESCRIPTORS) {
    sv_currentRxIndex = 0;
  }
#endif
}
