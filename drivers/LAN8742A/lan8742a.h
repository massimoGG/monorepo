#ifndef LAN8742A_H_
#define LAN8742A_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief function to write on the SMI */
typedef void (*lan8742a_readPhyRegister_t)(uint8_t reg, uint16_t *pVal);

/** @brief function to read from the SMI */
typedef void (*lan8742a_writePhyRegister_t)(uint8_t reg, uint16_t val);

/** @brief register 0 */
typedef enum {
  eLan8742a_powerDownNormal = 0,
  eLan8742a_powerDownGeneralPowerDown = 1,
} lan8742a_powerDown_e;

typedef enum {
  eLan8742a_modeHalfDuplex = 0,
  eLan8742a_modeFullDuplex = 1,
} lan8742a_duplexMode_e;

/** @brief register 1 */
typedef union {
  uint16_t reg;
  struct {
    uint16_t bit_supportsExtendedCapabilitiesRegisters : 1;
    uint16_t bit_jabberConditionDetected : 1;
    uint16_t bit_linkIsUp : 1;
    uint16_t bit_ableToPerformAutoNegotiation : 1;
    uint16_t bit_remoteFaultConditionDetected : 1;
    uint16_t bit_autoNegotiateProcessCompleted : 1;
    uint16_t : 2;
    uint16_t bit_extendedStatusInformationInRegister15 : 1;
    uint16_t bit_phyIsAbleToPerformHalfDuplex100BaseT2 : 1;
    uint16_t bit_phyIsAbleToPerformFullDuplex100BaseT2 : 1;
    uint16_t bit_10MbpsWithHalfDuplex : 1;
    uint16_t bit_10MbpsWithFullDuplex : 1;
    uint16_t bit_txWithHalfDuplex : 1;
    uint16_t bit_txWithFullDuplex : 1;
    uint16_t bit_t4Able : 1;
  };
} lan8742a_basicStatus_t;

/** @brief register 2 and 3 */
typedef struct {
  uint32_t phyIdNumber;
  uint8_t modelNumber;
  uint8_t revisionNumber;
} lan8742a_phyIdentifier_t;

/** @brief register 25 */
typedef enum {
  lan8742a_tdrCableTypeDefault = 0,
  lan8742a_tdrCableTypeShorted = 1,
  lan8742a_tdrCableTypeOpen = 2,
  lan8742a_tdrCableTypeMatch = 3,
} lan8742a_tdrChannelCableType_e;

/** @brief register 25 */
typedef struct {
  lan8742a_tdrChannelCableType_e channelCableType;
  uint8_t channelLength;
} lan8742a_tdrResult;

/** @brief register 27 */
typedef enum {
  eLan8742a_enableAutoMdix = 0,
  eLan8742a_disableAutoMdix = 1,
} lan8742a_amdixctrl_e;


/** @brief register 31  */
typedef enum {
  eLan8742a_autoNegotiationIsNotDoneOrDisabled = 0,
  eLan8742a_autoNegotiationIsDone = 1
} lan8742a_autoDone_e;

typedef enum {
  eLan8742a_10baseTHalfDuplex = 1,
  eLan8742a_100baseTxHalfDuplex = 2,
  eLan8742a_10baseTFullDuplex = 5,
  eLan8742a_100baseTxFullDuplex = 6,
} lan8742a_speedIndication_e;

typedef struct {
  lan8742a_autoDone_e autoDone;
  lan8742a_speedIndication_e speedIndication;
} lan8742a_phySpecialControlStatus_t;

void lan8742a_init(lan8742a_readPhyRegister_t readFn,
                   lan8742a_writePhyRegister_t writeFn);
void lan8742a_deinit(void);
void lan8742a_softReset(void);

void lan8742a_setPowerDown(lan8742a_powerDown_e mode);
void lan8742a_restartAutoNegotiation(void);
void lan8742a_setDuplexMode(lan8742a_duplexMode_e mode);
lan8742a_basicStatus_t lan8742a_getBasicStatus(void);
lan8742a_phyIdentifier_t lan8742a_getPhyIdentifier(void);
lan8742a_tdrResult lan8742a_performTdr(void);
uint16_t lan8742a_getSymbolErrorCounter(void);
void lan8742a_setAutoMdix(lan8742a_amdixctrl_e);
uint16_t lan8742a_getCableLength(void);
lan8742a_phySpecialControlStatus_t lan8742a_getPhySpecialControlStatus(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif