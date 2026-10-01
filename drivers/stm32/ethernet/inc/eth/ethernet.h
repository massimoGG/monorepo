#ifndef ETHERNET_H_
#define ETHERNET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <error_codes.h>
#include <stdint.h>

/**====================PUBLIC TYPEDEFS====================*/

/** @brief All supported ether protos */
typedef enum {
  etherType_ipv4 = 0x0800,
  etherType_arp = 0x0806,
  etherType_ipv6 = 0x86DD,
  etherType_lldp = 0x88CC,
} etherType_e;

/** @brief MAC address */
typedef struct {
  uint8_t addr[6];
} mac_t;

typedef struct {
  /**
   * The first LSB in dest indicates it individual (I/G = 0) or group address
   * (I/G = 1)
   * The second bit (U/L) distinguishes between locally (U/L = 1) or globally
   * administrered addresses (U/L = 0) BROADCAST = 1,
   *
   * MSB 46-bit adddress | U/L | I/G LSB
   * <------------------- BIT TRANSMISSION
   * */
  uint8_t dst[6];
  uint8_t src[6];

  /**
   * @brief MAC length/type
   * - If value <= maxValidFrame (= 0d1500) then this field indicates length of
   *    the data in bytes   -> Length interpretation
   * - If value >= minTypeValue (=0d1536 = 0x0600) then this field indicates the
   *    nature of the protocol -> Type interpretation
   * - If Type interpretation == 0x8100 = 802.1Q Tag Protocol Type
   */
  uint16_t macLengthType;

  /**
   * n-byte data field
   * If the value <minimum length -> PAD after the data field but prior to the
   * FCS (Frame Check Sequence)
   *
   * The min and max of the DATA and PAD fields are :
   * Maximum length = 1500
   * Minium length of untagged = 46 bytes
   * Maximum length of tagged = 42 bytes
   */

  /* After data comes the Frame Check Sequence = 4bytes CRC value: Based on all
   * fields except preamble and SFD
   * The polynomial
   */

} eth_header_t;

/** If the macLengthType in ethernet_header_t is 0x8100 -> The next bytes are
 * this header */
typedef struct {
  uint16_t tagControl;
  uint16_t macLengthType; /* Length of data in this Q frame */
} eth_qHeader_t;

/**====================PUBLIC FUNCTIONS====================*/

void eth_irqHandler(void);

error_e eth_init(void);
error_e eth_deinit(void);
error_e eth_start(void);
error_e eth_stop(void);

void eth_debug(void);

error_e eth_readData(uint8_t *pBuf, uint16_t *pLength);
error_e eth_writeData(const uint8_t *pBuf, uint16_t length);

uint16_t eth_createPacket(uint8_t *pBuf, mac_t dst, mac_t src, etherType_e type, const void *pData, uint16_t length);

error_e eth_getMacConfig(void);
error_e eth_setMacConfig(void);

error_e eth_getDmaConfig(void);
error_e eth_setDmaConfig(void);

// error_e eth_setRxVlan(void);
// error_e eth_getMacFilterConfig(void);
// error_e eth_setMacFilterConfig(void);
// error_e eth_setHashTable(void);
mac_t eth_getMacAddr(void);
error_e eth_setMacAddr(mac_t addr);

error_e eth_processFrame(void *pData);

// error_e eth_getState(void);
// error_e eth_getError(void);
// error_e eth_getDmaError(void);
// error_e eth_getMacError(void);

void eth_writePhyRegister(uint8_t reg, uint16_t val);
void eth_readPhyRegister(uint8_t reg, uint16_t *pVal);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif