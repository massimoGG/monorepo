#include <stm32f4xx.h>
#include <string.h>

#include "ethernet_descriptors.h"

#ifndef NUM_DESCRIPTORS
#define NUM_DESCRIPTORS 4
#endif

#ifndef MAX_BUFSIZE
/* IEEE802.3 Max frame length + CRC */
#define MAX_BUFSIZE (0x600 + 4)
#endif

/**====================LOCAL TYPEDEFS====================*/

/* Typedef for one buffer in a descriptor */
typedef uint8_t descriptorBuf_t[MAX_BUFSIZE];

/* Typedef for the two buffers in a descriptor */
typedef descriptorBuf_t descriptorBuffers_t[2];

/**====================LOCAL VARIABLES====================*/

/** Storage of descriptors */
static volatile rxDescriptor_t v_rxDescriptors[NUM_DESCRIPTORS]
    __attribute__((aligned(4)));
static volatile txDescriptor_t v_txDescriptors[NUM_DESCRIPTORS]
    __attribute__((aligned(4)));

static volatile descriptorBuffers_t v_rxBuffers[NUM_DESCRIPTORS]
    __attribute__((aligned(4)));
static volatile descriptorBuffers_t v_txBuffers[NUM_DESCRIPTORS]
    __attribute__((aligned(4)));

/**====================LOCAL PROTOTYPES====================*/

static size_t eth_getDescriptorsNum(void);
static volatile void *eth_getRxDescriptorsPtr(void);
static volatile void *eth_getTxDescriptorsPtr(void);

/**====================PUBLIC FUNCTIONS====================*/


/** @brief initializes the RX descriptor list to ready state for DMA */
void eth_initRxDescriptors(void) {
  for (uint32_t idx = 0; idx < NUM_DESCRIPTORS; idx++) {
    volatile rxDescriptor_t *pDescr = &v_rxDescriptors[idx];

    /* Reset descriptor */
    WRITE_REG(pDescr->RDES0, 0);
    WRITE_REG(pDescr->RDES1, 0);
    WRITE_REG(pDescr->RDES2, 0);
    WRITE_REG(pDescr->RDES3, 0);

    /* Reset Enhanced */
    WRITE_REG(pDescr->RDES6, 0);
    WRITE_REG(pDescr->RDES7, 0);

    /* Enable Ethernet DMA Rx Descriptor interrupt */
    CLEAR_BIT(pDescr->RDES1, RDES1_DIC_Msk);

    /* Clear Receive buffer 2 size = 0 bytes*/
    MODIFY_REG(pDescr->RDES1, RDES1_RBS2_Msk, 0);

    /* Configure Receive end of ring if last element */
    if (idx < NUM_DESCRIPTORS - 1) {
      CLEAR_BIT(pDescr->RDES1, RDES1_RER_Msk);
    } else {
      SET_BIT(pDescr->RDES1, RDES1_RER_Msk);
    }

    /* Set receive buffer 1 size */
    MODIFY_REG(pDescr->RDES1, RDES1_RBS1_Msk,
               MAX_BUFSIZE << RDES1_RBS1_Pos);

    /* Set buffer 1 address */
    WRITE_REG(pDescr->RDES2, (uint32_t)&v_rxBuffers[idx][0]);

    /* Set Own bit to indicate that this descriptor is owned by the DMA */
    SET_BIT(pDescr->RDES0, RDES0_OWN_Msk);
  }

  /* Set the first descriptor's address */
  ETH->DMARDLAR = (uint32_t)&v_rxDescriptors[0];
}

/** @brief initializes the TX descriptor list to ready state for DMA */
void eth_initTxDescriptors(void) {
  for (uint32_t idx = 0; idx < NUM_DESCRIPTORS; idx++) {
    volatile txDescriptor_t *pDescr = &v_txDescriptors[idx];

    /* Reset descriptor */
    WRITE_REG(pDescr->TDES0, 0);
    WRITE_REG(pDescr->TDES1, 0);
    WRITE_REG(pDescr->TDES2, 0);
    WRITE_REG(pDescr->TDES3, 0);

    /* Reset Enhanced */
    WRITE_REG(pDescr->TDES6, 0);
    WRITE_REG(pDescr->TDES7, 0);

    /* Enable Ethernet DMA Tx Descriptor interrupt */
    SET_BIT(pDescr->TDES0, TDES0_IC_Msk);

    /* Checksum insertion control */
    MODIFY_REG(pDescr->TDES0, TDES0_CIC_Msk, 0); /* Disable checksum Insertion */

    if (idx < NUM_DESCRIPTORS - 1) {
      CLEAR_BIT(pDescr->TDES0, TDES0_TER_Msk);
    } else {
      SET_BIT(pDescr->TDES0, TDES0_TER_Msk);
    }

    /* Set buffer 1 size */
    MODIFY_REG(pDescr->TDES1, TDES1_TBS1_Msk,
               0 << TDES1_TBS1_Pos);

    /* Set buffer 1 address */
    WRITE_REG(pDescr->TDES2, (uint32_t)&v_txBuffers[idx][0]);

    /* Clear Receive buffer 2 size = 0 bytes*/
    MODIFY_REG(pDescr->TDES1, TDES1_TBS2_Msk, 0);
/* Clear own bit */
    CLEAR_BIT(pDescr->TDES0, TDES0_OWN_Msk);
  }

  /* Set descriptor address */
  ETH->DMATDLAR = (uint32_t)&v_txDescriptors[0];
}

/**
 * @brief returns an available RX descriptor
 * @retval pointer to descriptor 
 * @retval NULL if no descriptor is available
 */
volatile rxDescriptor_t *eth_getRxDescriptor(void)
{
  for (size_t idx = 0; idx < eth_getDescriptorsNum(); idx++) {
    volatile rxDescriptor_t *pDesc_tmp = eth_getRxDescriptorsPtr() + idx;

    if (0 == READ_BIT(pDesc_tmp->RDES0, RDES0_OWN_Msk)) {
      /* DMA made this descriptor available for CPU */
      return pDesc_tmp;
    }
  }

  return NULL;
}

/**
 * @brief returns an available TX descriptor
 * @retval pointer to descriptor 
 * @retval NULL if no descriptor is available
 */
volatile txDescriptor_t *eth_getTxDescriptor(void)
{
  for (size_t idx = 0; idx < eth_getDescriptorsNum(); idx++) {
    volatile txDescriptor_t *pDesc_tmp = eth_getTxDescriptorsPtr() + idx;

    if (0 == READ_BIT(pDesc_tmp->TDES0, TDES0_OWN_Msk)) {
      /* DMA made this descriptor available for CPU */
      return pDesc_tmp;
    }
  }

  return NULL;
}

/**
 * @brief makes given RX descriptor free for the DMA
 * @param[in] pDesc 
 */
void eth_clearRxDescriptor(volatile rxDescriptor_t *pDesc)
{
  /* Let DMA reclaim this descriptor */
  SET_BIT(pDesc->RDES0, RDES0_OWN_Msk);
}

/**
 * @brief makes given TX descriptor free for the DMA
 * @param[in] pDesc 
 */
void eth_clearTxDescriptor(volatile txDescriptor_t *pDesc)
{
  /* Let DMA reclaim this descriptor */
  SET_BIT(pDesc->TDES0, TDES0_OWN_Msk);
}

/**
 * @brief gets the packet's length from a given descriptor
 * @param[in] pDesc   pointer to descriptor
 * @param[out] ppData  pointer to data
 * @return uint16_t
 */
uint16_t eth_getRxPacket(volatile rxDescriptor_t *pDesc,
                                    uint8_t **ppData) {
  uint16_t length = 0;
  const uint32_t c_status = pDesc->RDES0;

  /* Frame length is given if Last Descriptor (RDES0[8]) is set and Descriptor
   * Error is not set (RDES0[14]) */
  if (READ_BIT(c_status, RDES0_LS_Msk) && !READ_BIT(c_status, RDES0_DE_Msk)) {
    /* Frame length indicates the byte length of the received frame including
     * CRC */
    length = (c_status & RDES0_FRAMELENGTH_Msk) >> RDES0_FRAMELENGTH_Pos;
    *ppData = (void *)pDesc->RDES2;

  } else {
    /* Indicates the accumulated number of bytes that have beeen transfered to
     * the current frame */
    if (READ_BIT(c_status, RDES0_FS_Msk)) {
      /* When set -> Check first buffer */
      length = (pDesc->RDES1 & RDES1_RBS1_Msk) >> RDES1_RBS1_Pos;
      *ppData = (void *)pDesc->RDES2;

      if (length == 0) {
        /* If size of first buffer =0 -> Second buffer contains beginning of
         * frame -> IF SIZE OF SECOND IS ALSO 0 -> See next descriptor for
         * beginning of the frame */
        length = (pDesc->RDES1 & RDES1_RBS2_Msk) >> RDES1_RBS2_Pos;
        *ppData = (void *)pDesc->RDES3;
      }
    }
  }

  return length;
}

/**
 * @brief sets the packet data to the TX descriptor
 * @param[in] pDesc 
 * @param[in] pData 
 * @param[in] length 
 */
void eth_setTxPacket(volatile txDescriptor_t *pDesc, const uint8_t *pData, uint16_t length)
{
  /* Copy packet data */
  memcpy((void *)pDesc->TDES2, pData, length);

  /* Set packet length */
  MODIFY_REG(pDesc->TDES1, TDES1_TBS1_Msk,
             length << TDES1_TBS1_Pos);

  /* Set interrupt on completion */
  SET_BIT(pDesc->TDES0, TDES0_IC_Msk);

  /* Enable auto CRC and padding */
  CLEAR_BIT(pDesc->TDES0, TDES0_DC_Msk);

  /* Set buffer 2 size to 0 */
  MODIFY_REG(pDesc->TDES1, TDES1_TBS2_Msk, 0);

  /* Mark First and Last */
  SET_BIT(pDesc->TDES0, TDES0_LS_Msk);
  SET_BIT(pDesc->TDES0, TDES0_FS_Msk);

  /* Grant the DMA the descriptor */
  SET_BIT(pDesc->TDES0, TDES0_OWN_Msk);
}

/**====================LOCAL FUNCTIONS====================*/


/**
 * @brief returns the number of descriptors
 * @return size_t 
 */
static size_t eth_getDescriptorsNum(void)
{
  return NUM_DESCRIPTORS;
}

/**
 * @brief returns a pointer to the first element of the RX descriptors 
 * @return volatile* 
 */
static volatile void *eth_getRxDescriptorsPtr(void)
{
    return &v_rxDescriptors[0];
}

/**
 * @brief returns a pointer to the first element of the TX descriptors
 * @return volatile* 
 */
static volatile void *eth_getTxDescriptorsPtr(void)
{
    return &v_txDescriptors[0];
}