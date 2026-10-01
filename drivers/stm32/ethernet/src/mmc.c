#include <stdint.h>
#include <stm32f4xx.h>

#include <eth/mmc.h>
#include <debug.h>

/** @brief initializes the MMC */
void mmc_init(void) {
  uint32_t tmpReg = ETH->MMCRIMR;

  /* Mask receive interrupt */
  SET_BIT(tmpReg, ETH_MMCRIMR_RGUFM); /* Received good unicast frames */
  SET_BIT(tmpReg, ETH_MMCRIMR_RFAEM); /* Received frames alignment error */
  SET_BIT(tmpReg, ETH_MMCRIMR_RFCEM); /* Received frames CRC error */

  /* Mask transmit interrupt */
  SET_BIT(tmpReg, ETH_MMCTIMR_TGFM);    /* Transmit good frames */
  SET_BIT(tmpReg, ETH_MMCTIMR_TGFMSCM); /* Transmit good frames more
                                                 single collision mask */
  SET_BIT(tmpReg,
          ETH_MMCTIMR_TGFSCM); /* Transmit good frames single collision mask*/

  ETH->MMCRIMR = tmpReg;
}

/**
 * @brief Reads MMC Interrupt status
 * @warning in IRQ context
 */
void mmc_irqHandler(void) { 
const uint32_t rir = ETH->MMCRIR;
const uint32_t tir = ETH->MMCTIR;

  /* Ethernet MMC receive interrupt register generates an interrupt when receive
   * statistic counter reach half their maximum value.*/
  /* Interrupt bit is cleared when the counter that caused the interrupt is read
   */
  if (rir & ETH_MMCRIR_RGUFS) {
    /* Received good unicast frames status reached half */
  }
  if (rir & ETH_MMCRIR_RFAES) {
    /* Received frames alignment error status reached half */
  }
  if (rir & ETH_MMCRIR_RFCES) {
    /* Received frames CRC error status reached half */
  }
  if (tir & ETH_MMCTIR_TGFS) {
    /* Transmitted good frames reached half */
  }
  if (tir & ETH_MMCTIR_TGFMSCS) {
    /* Transmitted good frames more single collision status */
  }
  if (tir & ETH_MMCTIR_TGFSCS) {
    /* Transmitted good frames signle collision status */
  }
}

/** @brief Debug logs MMC */
void mmc_debug(void) {
  const uint32_t receivedGoodUnicastFrames = ETH->MMCRGUFCR;
  const uint32_t transmittedGoodUnicastFrames = ETH->MMCTGFCR;
  DBG_DBG("MMC Stats:\n"
          "\tReceived frames\n\tGood: %lu\n"
          "\tTransmitted frames\n\tGood: %lu\n",
          receivedGoodUnicastFrames, transmittedGoodUnicastFrames);
  DBG_DBG(
      "\n\tReceived with CRC error: %lu\n\tReceived with alignment error: %lu",
      ETH->MMCRFCECR, ETH->MMCRFAECR);
}