#ifndef ETHERNET_DESCRIPTORS_H__
#define ETHERNET_DESCRIPTORS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#define TDES0_OWN_Pos (31) /* Own bit */
#define TDES0_OWN_Msk (0x1U << TDES0_OWN_Pos)
#define TDES0_IC_Pos (30) /* Interrupt on completion */
#define TDES0_IC_Msk (0x1U << TDES0_IC_Pos)
#define TDES0_LS_Pos (29) /* Last segment */
#define TDES0_LS_Msk (0x1U << TDES0_LS_Pos)
#define TDES0_FS_Pos (28) /* First segment */
#define TDES0_FS_Msk (0x1U << TDES0_FS_Pos)
#define TDES0_DC_Pos (27) /* Disable CRC */
#define TDES0_DC_Msk (0x1U << TDES0_DC_Pos)
#define TDES0_DP_Pos (26) /* Disable pad */
#define TDES0_DP_Msk (0x1U << TDES0_DP_Pos)
#define TDES0_TTSE_Pos (25) /* Transmit time stamp enable */
#define TDES0_CIC_Pos (22)  /* Checksum insertion control */
#define TDES0_CIC_Msk (0x3 << TDES0_CIC_Pos)
#define TDES0_TER_Pos (21) /* Transmit end of ring */
#define TDES0_TER_Msk (0x1U << TDES0_TER_Pos)
#define TDES0_TCH_Pos (20)                  /* Second address chained */
#define TDES0_TTSS_Pos (17)                 /* Transmit time stamp status */
#define TDES0_IHE_Pos (16)                  /* IP header error */
#define TDES0_ES_Pos (15)                   /* Error summary */
#define TDES0_JT_Pos (14)                   /* Jabberr timeout */
#define TDES0_FF_Pos (13)                   /* Frame flushed */
#define TDES0_IPE_Pos (12)                  /* IP payload error */
#define TDES0_LCA_Pos (11)                  /* Loss of Carrier */
#define TDES0_NC_Pos (10)                   /* No carrier */
#define TDES0_LCO_Pos (9)                   /* Late collision */
#define TDES0_EC_Pos (8)                    /* Excessive collision */
#define TDES0_VF_Pos (7)                    /* VLAN frame */
#define TDES0_CC_Pos (3)                    /* Collision count */
#define TDES0_CC_Msk (0x0F << TDES0_CC_Pos) /* Collision count */
#define TDES0_ED_Pos (2)                    /* Excessive deferral */
#define TDES0_UF_Pos (1)                    /* Underflow error */
#define TDES0_DB_Pos (0)                    /* Deferred bit */

#define TDES1_TBS2_Pos (16) /* Transmit buffer 2 size */
#define TDES1_TBS2_Msk (0x1FFF << TDES1_TBS2_Pos)
#define TDES1_TBS1_Pos (0) /* Transmit buffer 1 size */
#define TDES1_TBS1_Msk (0x1FFF << TDES1_TBS1_Pos)

#define TDES2_TBAP1_Pos (0)
#define TDES2_TBAP1_Msk (0xFFFFFFFF << TDES2_TBAP1_Pos)

#define TDES3_TBAP2_Pos (0)
#define TDES3_TBAP2_Msk (0xFFFFFFFF << TDES3_TBAP2_Pos)

#define TDES6_TTSL_Pos (0)
#define TDES6_TTSL_Msk (0xFFFFFFFF << TDES6_TTSL_Pos)

#define TDES7_TTSH_Pos (0)
#define TDES7_TTSH_Msk (0xFFFFFFFF << TDES7_TTSH_Pos)

#define RDES0_OWN_Pos (31) /* Owning bit*/
#define RDES0_OWN_Msk (0x1U << RDES0_OWN_Pos)
#define RDES0_AFM_Pos (30) /* Descriptor address fail */
#define RDES0_AFM_Msk (0x1U << RDES0_AFM_Pos)
#define RDES0_FRAMELENGTH_Pos (16)
#define RDES0_FRAMELENGTH_Msk (0x1FFF << RDES0_FRAMELENGTH_Pos)
#define RDES0_ES_Pos (15) /* Error Summary */
#define RDES0_ES_Msk (0x1U << RDES0_ES_Pos)
#define RDES0_DE_Pos (14) /* Descriptor Error */
#define RDES0_DE_Msk (0x1U << RDES0_DE_Pos)
#define RDES0_SAF_Pos (13) /* Source Address Filter Fail */
#define RDES0_SAF_Msk (0x1U << RDES0_SAF_Pos)
#define RDES0_LE_Pos (12) /* Length Error */
#define RDES0_LE_Msk (0x1U << RDES0_LE_Pos)
#define RDES0_OE_Pos (11) /* Overflow Error */
#define RDES0_OE_Msk (0x1U << RDES0_OE_Pos)
#define RDES0_VLAN_Pos (10) /* VLAN tag */
#define RDES0_VLAN_Msk (0x1U << RDES0_VLAN_Pos)
#define RDES0_FS_Pos (9) /* First descriptor */
#define RDES0_FS_Msk (0x1U << RDES0_FS_Pos)
#define RDES0_LS_Pos (8) /* Last descriptor */
#define RDES0_LS_Msk (0x1U << RDES0_LS_Pos)
#define RDES0_IPHCE_TSV_Pos                                                    \
  (7) /* IPv header checksum error / time stamp valid */
#define RDES0_IPHCE_TSV_Msk (0x1U << RDES0_IPHCE_TSV_Pos)
#define RDES0_LCO_Pos (6) /* Late collision */
#define RDES0_LCO_Msk (0x1U << RDES0_LCO_Pos)
#define RDES0_FT_Pos (5) /* Frame Type - FT >= 0x600*/
#define RDES0_FT_Msk (0x1U << RDES0_FT_Pos)
#define RDES0_RWT_Pos (4) /* Receive watchdog timeout */
#define RDES0_RWT_Msk (0x1U << RDES0_RWT_Pos)
#define RDES0_RE_Pos (3) /* Receive error */
#define RDES0_RE_Msk (0x1U << RDES0_RE_Pos)
#define RDES0_DRE_Pos (2) /* Dribble bit error */
#define RDES0_DRE_Msk (0x1U << RDES0_DRE_Pos)
#define RDES0_CE_Pos (1) /* CRC error */
#define RDES0_CE_Msk (0x1U << RDES0_CE_Pos)
#define RDES0_PCE_ESA_Pos                                                      \
  (0) /* Payload checksum error/extended status available */
#define RDES0_PCE_ESA_Msk (0x1U << RDES0_PCE_ESA_Pos)

#define RDES1_DIC_Pos (31)  /* Disable Interrupt on completion */
#define RDES1_DIC_Msk (0x1U << RDES1_DIC_Pos)
#define RDES1_RBS2_Pos (16) /* Receive buffer size 2 */
#define RDES1_RBS2_Msk (0x1FFF << RDES1_RBS2_Pos)
#define RDES1_RER_Pos (15) /* Receive end of Ring */
#define RDES1_RER_Msk (0x01 << RDES1_RER_Pos)
#define RDES1_RCH_Pos (14) /* Second address chained */
#define RDES1_RCH_Msk (0x01 << RDES1_RCH_Pos)
#define RDES1_RBS1_Pos (0) /* Receive buffer size 1 */
#define RDES1_RBS1_Msk (0x1FFF << RDES1_RBS1_Pos)

#define RDES2_RBAP1_Pos (0) /* Receive buffer 1 pointer address */
#define RDES2_RBAP1_Msk (0xFFFFFFFF << RDES2_RBAP1_Pos)

#define RDES3_RBAP2_Pos (0) /* Receive buffer 2 pointer address */
#define RDES3_RBAP2_Msk (0xFFFFFFFF << RDES3_RBAP2_Pos)

#define RDES4_PV_Pos (13)  /* PIP version */
#define RDES4_PFT_Pos (12) /* PIP frame type */
#define RDES4_PMT_Pos (8)  /* PTP message type */
#define RDES4_PMT_Msk (0xF << RDES4_PMT_Pos)
#define RDES4_IPV6PR_Pos (7) /* IPv6 packet received */
#define RDES4_IPV4PR_Pos (6) /* IPv4 packet received */
#define RDES4_IPCB_Pos (5)   /* IP Checksum bypassed */
#define RDES4_IPPE_Pos (4)   /* IP payload error */
#define RDES4_IPHE_Pos (3)   /* IP header error */
#define RDES4_IPPT_Pos (0)   /* IP payload type */
#define RDES4_IPPT_Msk (0x7 << RDES4_IPPT_Pos)

#define RDES6_RTSL_Pos (0) /* Receive timestamp low */
#define RDES6_RTSL_Msk (0xFFFFFFFF << RDES6_RTSL_Pos)

#define RDES7_RTSH_Pos (0) /* Receive timestamp high */
#define RDES7_RTSH_Msk (0xFFFFFFFF << RDES7_RTSH_Pos)

/**====================PUBLIC TYPEDEFS====================*/

typedef struct {
  /* RDES0 Own[31] Status [30:0] */
  volatile uint32_t RDES0;
  /* RDES1 CTRL[31] Reserved[30:29] Buffer 2 byte count[28:!6] CTRL[15:13]
   * Buffer1 byte count[12:0] */
  volatile uint32_t RDES1;
  /* RDES2 Buffer 1 address [31:0] */
  volatile uint32_t RDES2;
  /* RDES3 Buffer 2 address [31:0] or Next descriptor address */
  volatile uint32_t RDES3;

  /* RDES4 Reserved */
  volatile uint32_t RDES4;
  /* RDES5 Reserved */
  volatile uint32_t RDES5;
  /* RDES6 Time stamp low */
  volatile uint32_t RDES6;
  /* RDES7 Time stmap high */
  volatile uint32_t RDES7;
} rxDescriptor_t;

typedef struct {
  /**
   * TDES0
   * OWN[31] CTRL[30:26] TTSE[25] RES[24] CTRL[23:20] Reserved[19:18]
   * TTSS[17]  Status[16:0] */
  volatile uint32_t TDES0;
  /**
   * TDES1
   * Reserved[31:29] Buffer 2 byte count[28:16] Reserved[15:13] Buffer 1 byte
   * count [12:0]
   */
  volatile uint32_t TDES1;
  /**
   * TDES2
   * Buffer 1 address[31:0]/Time stamp low [31:0]
   */
  volatile uint32_t TDES2;
  /**
   * TDES3
   * Buffer 2 address[31:0] or Next descriptor address[31:0] / Timestamp high
   * [31:0]
   */
  volatile uint32_t TDES3;

  /* TDES4 Reserved */
  volatile uint32_t TDES4;
  /* TDES5 Reserved */
  volatile uint32_t TDES5;
  /* TDES6 Time stamp low */
  volatile uint32_t TDES6;
  /* TDES7 Time stmap high */
  volatile uint32_t TDES7;
} txDescriptor_t;

/**====================PUBLIC FUNCTIONS====================*/

void eth_initRxDescriptors(void);
void eth_initTxDescriptors(void);

volatile rxDescriptor_t *eth_getRxDescriptor(void);
volatile txDescriptor_t *eth_getTxDescriptor(void);

void eth_clearRxDescriptor(volatile rxDescriptor_t *pDesc);
void eth_clearTxDescriptor(volatile txDescriptor_t *pDesc);

uint16_t eth_getRxPacket(volatile rxDescriptor_t *pDesc, uint8_t **ppData);
void eth_setTxPacket(volatile txDescriptor_t *pDesc, const uint8_t *pData,
                          uint16_t length);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif