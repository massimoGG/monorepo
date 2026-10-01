/**
 * @file lan8742a.c
 * @author Massimo Giardina (dev@massimog.net)
 * @brief this is the driver code for the MicroChip LAN8742A, included in all
 * STM NUCLEO boards of type MB1137
 * @date 2026-08-04
 *
 * @copyright Copyright (c) 2026
 *
 */
#include <assert.h>
#include <stdint.h>
#include <stm32f4xx.h>
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_gpio.h>

#include "lan8742a.h"
#include <debug.h>

/** Register 1 */
#define BCR_SOFTRESET_Pos (15)
#define BCR_SOFTRESET_Mask (0x1U << BCR_SOFTRESET_Pos)
#define BCR_SOFTRESET BCR_SOFTRESET_Mask

#define BCR_LOOPBACK_Pos (14)
#define BCR_LOOPBACK_Mask (0x1U << BCR_LOOPBACK_Pos)
#define BCR_LOOPBACK BCR_LOOPBACK_Mask

#define BCR_SPEEDSELECT_Pos (13)
#define BCR_SPEEDSELECT_Mask (0x1U << BCR_SPEEDSELECT_Pos)
#define BCR_SPEEDSELECT BCR_SPEEDSELECT_Mask

#define BCR_AUTONEGOTIATION_ENABLE_Pos (12)
#define BCR_AUTONEGOTIATION_ENABLE_Msk (0x1U << BCR_AUTONEGOTIATION_ENABLE_Pos)
#define BCR_AUTONEGOTIATION_ENABLE BCR_AUTONEGOTIATION_ENABLE_Msk

#define BCR_POWERDOWN_Pos (11)
#define BCR_POWERDOWN_Msk (0x1U << BCR_POWERDOWN_Pos)
#define BCR_POWERDOWN BCR_POWERDOWN_Msk

#define BCR_ISOLATE_Pos (10)
#define BCR_ISOLATE_Msk (0x1U << BCR_ISOLATE_Pos)
#define BCR_ISOLATE BCR_ISOLATE_Msk

#define BCR_RESTART_AUTONEGOTIATION_Pos (9)
#define BCR_RESTART_AUTONEGOTIATION_Msk                                        \
  (0x1U << BCR_RESTART_AUTONEGOTIATION_Pos)
#define BCR_RESTART_AUTONEGOTIATION BCR_RESTART_AUTONEGOTIATION_Msk

#define BCR_DUPLEX_MODE_Pos (8)
#define BCR_DUPLEX_MODE_Msk (0x1U << BCR_DUPLEX_MODE_Pos)
#define BCR_DUPLEX_MODE BCR_DUPLEX_MODE_Msk

/** Register 25 TDR CONTROL/STATUS REGISTER */
#define TDRCSR_TDR_ENABLE_Pos (15)
#define TDRCSR_TDR_ENABLE_Msk (0x1U << TDRCSR_TDR_ENABLE_Pos)
#define TDRCSR_TDR_ENABLE TDRCSR_TDR_ENABLE_Msk

#define TDRCSR_TDR_ADF_ENABLE_Pos (14)
#define TDRCSR_TDR_ADF_ENABLE_Msk (0x1U << TDRCSR_TDR_ADF_ENABLE_Pos)
#define TDRCSR_TDR_ADF_ENABLE TDRCSR_TDR_ADF_ENABLE_Msk

#define TDRCSR_TDR_CHANNEL_CABLE_TYPE_Pos (9)
#define TDRCSR_TDR_CHANNEL_CABLE_TYPE_Msk                                      \
  (0x3U << TDRCSR_TDR_CHANNEL_CABLE_TYPE_Pos)
#define TDRCSR_TDR_CHANNEL_CABLE_TYPE TDRCSR_TDR_CHANNEL_CABLE_TYPE_Msk

#define TDRCSR_TDR_CHANNEL_STATUS_Pos (8)
#define TDRCSR_TDR_CHANNEL_STATUS_Msk (0x1U << TDRCSR_TDR_CHANNEL_STATUS_Pos)
#define TDRCSR_TDR_CHANNEL_STATUS TDRCSR_TDR_CHANNEL_STATUS_Msk

#define TDRCSR_TDR_CHANNEL_LENGTH_Pos (0)
#define TDRCSR_TDR_CHANNEL_LENGTH_Msk (0xFF << TDRCSR_TDR_CHANNEL_LENGTH_Pos)
#define TDRCSR_TDR_CHANNEL_LENGTH TDRCSR_TDR_CHANNEL_LENGTH_Msk

typedef enum {
  eLan8742a_basicControlRegister = 0,
  eLan8742a_basicStatusRegister = 1,
  eLan8742a_phyIdentifier1Register = 2,
  eLan8742a_phyIdentifier2Register = 3,
  eLan8742a_autoNegotiationAdvertisementRegister = 4,
  eLan8742a_autoNegotiationLinkPartnerAbilityRegister = 5,
  eLan8742a_autoNegotiationExpansionRegister = 6,
  eLan8742a_autoNegotiationNextPageTxRegister = 7,
  eLan8742a_autoNegotiationNextPageRxRegister = 8,
  eLan8742a_mmdAccessControlRegister = 13,
  eLan8742a_mmdAccessAddressDataRegister = 14,
  eLan8742a_edpdNlpCrossoverTimerRegister = 16,
  eLan8742a_modeControlStatusRegister = 17,
  eLan8742a_specialModesRegister = 18,
  eLan8742a_tdrPatternsDelayControlRegister = 24,
  eLan8742a_tdrControlStatusRegister = 25,
  eLan8742a_symbolErrorCounterRegister = 26,
  eLan8742a_specialControlStatusIndicationsRegister = 27,
  eLan8742a_cableLengthRegister = 28,
  eLan8742a_interruptSourceFlagRegister = 29,
  eLan8742a_interruptMaskRegister = 30,
  eLan8742a_phySpecialControlStatusRegister = 31,
} lan8742aRegisters_e;

static uint8_t s_phyAddr = 0;
static lan8742a_readPhyRegister_t s_readFn = NULL;
static lan8742a_writePhyRegister_t s_writeFn = NULL;

static void modifyPhyRegister(uint8_t reg, uint16_t clearMask,
                              uint16_t setMask);

/**
 * @brief initializes the PHY
 * @param[in] readFn function pointer to read from PHY
 * @param[in] writeFn function pointer to write to PHY
 */
void lan8742a_init(lan8742a_readPhyRegister_t readFn,
                   lan8742a_writePhyRegister_t writeFn, uint8_t phyAddr) {
  s_readFn = readFn;
  s_writeFn = writeFn;
  s_phyAddr = phyAddr;
}

/** @brief soft resets the PHY */
void lan8742a_softReset(void) {
  modifyPhyRegister(eLan8742a_basicControlRegister,
                                BCR_SOFTRESET_Mask, BCR_SOFTRESET);
}

void lan8742a_setPowerDown(lan8742a_powerDown_e mode) {

  const uint16_t setMask =
      (mode == eLan8742a_powerDownNormal) ? 0 : BCR_POWERDOWN;

  modifyPhyRegister(eLan8742a_basicControlRegister,
                                BCR_POWERDOWN_Msk, setMask);
}

void lan8742a_restartAutoNegotiation(void) {
  modifyPhyRegister(eLan8742a_basicControlRegister,
                                BCR_RESTART_AUTONEGOTIATION_Msk,
                                BCR_RESTART_AUTONEGOTIATION);
}

void lan8742a_setDuplexMode(lan8742a_duplexMode_e mode) {
  const uint16_t setMask =
      (mode == eLan8742a_modeFullDuplex) ? BCR_DUPLEX_MODE_Msk : 0;

  modifyPhyRegister(eLan8742a_basicControlRegister,
                                BCR_DUPLEX_MODE_Msk, setMask);
}

lan8742a_basicStatus_t lan8742a_getBasicStatus(void) {
  const uint8_t addr = eLan8742a_basicStatusRegister;

  lan8742a_basicStatus_t ret = {};
  s_readFn(s_phyAddr, addr, &ret.reg);

  return ret;
}

lan8742a_phyIdentifier_t lan8742a_getPhyIdentifier(void) {

  /* Assigned to the 3rd through 18th bits of the Organizationally Unique
  Identifier (OUI), respectively. */
  uint16_t id1 = 0;
  s_readFn(s_phyAddr, eLan8742a_phyIdentifier1Register, &id1);

  /* Assigned to the 19th through 24th bits of the OUI. */
  uint16_t id2 = 0;
  s_readFn(s_phyAddr, eLan8742a_phyIdentifier2Register, &id2);

  lan8742a_phyIdentifier_t ret = {
      .phyIdNumber = (id1 << 2) | ((id2 & 0xFC) << 19),
      .modelNumber = (id2 >> 4) & 0x1F,
      .revisionNumber = (id2 & 0x0F),
  };
  return ret;
}

/**
 * @brief this field is not valid during a match cable condition
 * @return lan8742a_tdrResult
 */
lan8742a_tdrResult lan8742a_performTdr(void) {

  const uint8_t addr = eLan8742a_tdrControlStatusRegister;

  enum {
    tdrChannelLength = 0,
    tdrChannelStatus = 8,
    tdrChannelCableType = 9,
    tdrAnalogToDigitalFilterEnable = 14,
    tdrEnable = 15,
  };

  uint16_t regValue = 0;
  s_readFn(s_phyAddr, addr, &regValue);

  /* Bit will stay high until reset or new TDR op restarted */
  while (!((regValue >> tdrChannelStatus) & 0x01)) {
    /* Start TDR */
    s_writeFn(s_phyAddr, addr, 0xC0);
    s_readFn(s_phyAddr, addr, &regValue);
  }

  lan8742a_tdrResult ret = {
      .channelCableType =
          (lan8742a_tdrChannelCableType_e)((regValue >> 9) & 0x03),
      .channelLength = (regValue & 0x7F)};
  return ret;
}

uint16_t lan8742a_getSymbolErrorCounter(void) {
  const uint8_t addr = eLan8742a_symbolErrorCounterRegister;
  uint16_t ret = 0;
  s_readFn(s_phyAddr, addr, &ret);
  return ret;
}

void lan8742a_setAutoMdix(lan8742a_amdixctrl_e val) {
  const uint8_t addr = eLan8742a_specialControlStatusIndicationsRegister;

  uint16_t regValue = 0;
  s_readFn(s_phyAddr, addr, &regValue);
  if (val == eLan8742a_enableAutoMdix) {
    CLEAR_BIT(regValue, 0x80);
  } else {
    SET_BIT(regValue, 0x80);
  }
  s_writeFn(s_phyAddr, addr, regValue);
}

uint16_t lan8742a_getCableLength(void) {
  uint16_t regValue = 0;
  s_readFn(s_phyAddr, eLan8742a_cableLengthRegister, &regValue);
  return regValue >> 12;
}

lan8742a_phySpecialControlStatus_t lan8742a_getPhySpecialControlStatus(void) {
  const uint8_t addr = eLan8742a_phySpecialControlStatusRegister;
  uint16_t regValue = 0;
  s_readFn(s_phyAddr, addr, &regValue);

  lan8742a_phySpecialControlStatus_t ret = {.autoDone = (regValue >> 12) & 0x01,
                                            .speedIndication =
                                                (regValue >> 2) & 0x07};

  return ret;
}

/**
 * @brief modifies specific bits of the phy register
 * @param[in] reg       register to modify
 * @param[in] clearMask clear mask
 * @param[in] setMask   set mask
 */
static void modifyPhyRegister(uint8_t reg, uint16_t clearMask,
                              uint16_t setMask) {
  uint16_t value = 0;
  s_readFn(s_phyAddr, reg, &value);
  MODIFY_REG(value, clearMask, setMask);
  s_writeFn(s_phyAddr, reg, value);
}