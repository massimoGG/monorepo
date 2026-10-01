#include <assert.h>
#include <stdint.h>
#include <stm32f4xx.h>
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_dma.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_rcc.h>
#include <stm32f4xx_ll_system.h>

#include "error_codes.h"
#include "ethernet_ll.h"

#define PHY_SMI_CLOCK (ETH_MACMIIAR_CR_Div102)

static void eth_ll_writellPhyRegister(uint8_t devAddr, uint8_t reg);

/** @brief initializes the ethernet ll */
void eth_ll_init(void) {

  /* Keep peripherals under reset MAC clocks */
  LL_AHB1_GRP1_ForceReset(LL_AHB1_GRP1_PERIPH_ETHMAC);
  LL_AHB1_GRP1_ForceReset(LL_AHB1_GRP1_PERIPH_ETHMACRX);
  LL_AHB1_GRP1_ForceReset(LL_AHB1_GRP1_PERIPH_ETHMACTX);
  LL_AHB1_GRP1_ForceReset(LL_AHB1_GRP1_PERIPH_ETHMACPTP);

  /* Initialize low level hardware (GPIO, Clock NVIC)*/
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /* PA1 - RMII_REF_CLK */
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_1, LL_GPIO_AF_11);
  LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_1, LL_GPIO_SPEED_FREQ_VERY_HIGH);
  /* PA2 - RMII_MDIO */
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_2, LL_GPIO_AF_11);
  LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_VERY_HIGH);
  /* PA7 - RMII_CRS_DV */
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_7, LL_GPIO_AF_11);
  LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_VERY_HIGH);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
  /* PB13 - RMII_TXD1 */
  LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_13, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetAFPin_8_15(GPIOB, LL_GPIO_PIN_13, LL_GPIO_AF_11);
  LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_13, LL_GPIO_SPEED_FREQ_VERY_HIGH);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  /* PC1 - RMII_MDC */
  LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_1, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetAFPin_0_7(GPIOC, LL_GPIO_PIN_1, LL_GPIO_AF_11);
  LL_GPIO_SetPinSpeed(GPIOC, LL_GPIO_PIN_1, LL_GPIO_SPEED_FREQ_VERY_HIGH);

  /* PC4 - RMII_RXD0 */
  LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_4, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetAFPin_0_7(GPIOC, LL_GPIO_PIN_4, LL_GPIO_AF_11);
  LL_GPIO_SetPinSpeed(GPIOC, LL_GPIO_PIN_4, LL_GPIO_SPEED_FREQ_VERY_HIGH);

  /* PC5 - RMII_RXD1 */
  LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_5, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetAFPin_0_7(GPIOC, LL_GPIO_PIN_5, LL_GPIO_AF_11);
  LL_GPIO_SetPinSpeed(GPIOC, LL_GPIO_PIN_5, LL_GPIO_SPEED_FREQ_VERY_HIGH);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOG);
  /* PG11 - RMII_TX_EN */
  LL_GPIO_SetPinMode(GPIOG, LL_GPIO_PIN_11, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetAFPin_8_15(GPIOG, LL_GPIO_PIN_11, LL_GPIO_AF_11);
  LL_GPIO_SetPinSpeed(GPIOG, LL_GPIO_PIN_11, LL_GPIO_SPEED_FREQ_VERY_HIGH);

  /* PG13 - RMII_TXDD0 */
  LL_GPIO_SetPinMode(GPIOG, LL_GPIO_PIN_13, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetAFPin_8_15(GPIOG, LL_GPIO_PIN_13, LL_GPIO_AF_11);
  LL_GPIO_SetPinSpeed(GPIOG, LL_GPIO_PIN_13, LL_GPIO_SPEED_FREQ_VERY_HIGH);

  /* Enable Syscfg */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  /* Set RMII mode  */
  LL_SYSCFG_SetPHYInterface(LL_SYSCFG_PMC_ETHRMII);

  /* Dummy read to sync SYSCFG with ETH */
  (void)SYSCFG->PMC;

  /* Take peripherals out of reset */
  LL_AHB1_GRP1_ReleaseReset(LL_AHB1_GRP1_PERIPH_ETHMAC);
  LL_AHB1_GRP1_ReleaseReset(LL_AHB1_GRP1_PERIPH_ETHMACRX);
  LL_AHB1_GRP1_ReleaseReset(LL_AHB1_GRP1_PERIPH_ETHMACTX);
  LL_AHB1_GRP1_ReleaseReset(LL_AHB1_GRP1_PERIPH_ETHMACPTP);

  /* Enable Ethernet MAC */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ETHMAC);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ETHMACRX);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ETHMACTX);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ETHMACPTP);

  /* Configure NVIC */
  NVIC_EnableIRQ(ETH_IRQn);
}

/**
 * @brief deinitializes the ethernet ll
 * @retval eError_ok      if successful
 * @retval eError_timeout timeout occurred
 */
error_e eth_ll_deinit(void) {
  /* Software Reset MAC */
  SET_BIT(ETH->DMABMR, ETH_DMABMR_SR);
  while (READ_BIT(ETH->DMABMR, ETH_DMABMR_SR)) {
    /**
     * @todo add timeout
     * @retval eError_timeout
     */
  }

  /* Disable MAC clocks */
  LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_ETHMAC);
  LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_ETHMACRX);
  LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_ETHMACTX);

  return eError_ok;
}

/**
 * @brief starts the ethernet DMA and MAC
 * @return error_e
 */
error_e eth_ll_start(void) {

  uint32_t tmpReg = 0;

  {
    /* 2. Write to the ETH_DMAIER register to mask unnecessary interrupt causes
     */
    tmpReg = ETH->DMAIER;
    /* Normal interrupt */
    SET_BIT(tmpReg, ETH_DMAIER_NISE);
    /* Abnormal interrupt */
    SET_BIT(tmpReg, ETH_DMAIER_AISE);
    /* Fatal bus error */
    SET_BIT(tmpReg, ETH_DMAIER_FBEIE);
    /* Receive interrupt */
    SET_BIT(tmpReg, ETH_DMAIER_RIE);
    /* Transmit underflow */
    SET_BIT(tmpReg, ETH_DMAIER_TUIE);
    /* Overflow interrupt */
    SET_BIT(tmpReg, ETH_DMAIER_ROIE);
    /* Transmmit interrupt */
    SET_BIT(tmpReg, ETH_DMAIER_TIE);
    /* write register */
    ETH->DMAIER = tmpReg;
  }

  /* Start MAC - Enable transmitter & receiver*/
  SET_BIT(ETH->MACCR, ETH_MACCR_TE | ETH_MACCR_RE);

  /* Start DMA - Start transmission & receiver */
  SET_BIT(ETH->DMAOMR, ETH_DMAOMR_ST | ETH_DMAOMR_SR);

  return eError_ok;
}

/** @brief stops the ethernet DMA and MAC */
void eth_ll_stop(void) {

  /* Stop MAC - Disable transmitter & receiver*/
  CLEAR_BIT(ETH->MACCR, ETH_MACCR_TE | ETH_MACCR_RE);

  /* Stop DMA - Disable transmission & receiver */
  CLEAR_BIT(ETH->DMAOMR, ETH_DMAOMR_ST | ETH_DMAOMR_SR);
}

/**
 * @brief writes a register to the PHY
 * @param[in] devAddr address of PHY
 * @param[in] reg register to write to
 * @param[in] val register value to write
 */
void eth_writePhyRegister(uint8_t devAddr, uint8_t reg, uint16_t val) {

  eth_ll_writellPhyRegister(devAddr, reg);

  /* write data value */
  ETH->MACMIIDR |= (val & ETH_MACMIIDR_MD_Msk);

  /* Mark write mode & Start transfer */
  SET_BIT(ETH->MACMIIAR, ETH_MACMIIAR_MW | ETH_MACMIIAR_MB);

  /* Block until transfer complete */
  while (READ_BIT(ETH->MACMIIAR, ETH_MACMIIAR_MB)) {
  }
}

/**
 * @brief reads a register from the PHY
 * @param[in] devAddr address of PHY
 * @param[in] reg   register to read from
 * @param[out] pVal value in register
 */
void eth_readPhyRegister(uint8_t devAddr, uint8_t reg, uint16_t *pVal) {

  eth_ll_writellPhyRegister(devAddr, reg);

  uint32_t tmpReg = ETH->MACMIIAR;

  /* Mark read mode */
  CLEAR_BIT(tmpReg, ETH_MACMIIAR_MW);

  /* Start transfer */
  SET_BIT(tmpReg, ETH_MACMIIAR_MB);

  ETH->MACMIIAR = tmpReg;

  /* Block until transfer complete */
  while (READ_BIT(ETH->MACMIIAR, ETH_MACMIIAR_MB)) {
  }

  *pVal = ETH->MACMIIDR & ETH_MACMIIDR_MD_Msk;
}

/**
 * @brief common ETH->MACMIIAR preparation for write and read
 * @param[in] devAddr address of PHY
 * @param[in] reg register to addres
 */
static void eth_ll_writellPhyRegister(uint8_t devAddr, uint8_t reg) {

  /* Block while busy */
  while (READ_BIT(ETH->MACMIIAR, ETH_MACMIIAR_MB)) {
  }

  uint32_t tmpReg = ETH->MACMIIAR;

  /* Reset bit to 0 during a write to ETH_MACMIIAR */
  CLEAR_BIT(tmpReg, ETH_MACMIIAR_MB);

  /* Write phy address */
  MODIFY_REG(tmpReg, ETH_MACMIIAR_PA_Msk, (devAddr & 0x1F) << ETH_MACMIIAR_PA_Pos);
  /* Write register address */
  MODIFY_REG(tmpReg, ETH_MACMIIAR_MR_Msk, reg << ETH_MACMIIAR_MR_Pos);

  /* Clock Register */
  MODIFY_REG(tmpReg, ETH_MACMIIAR_CR_Msk, PHY_SMI_CLOCK);

  ETH->MACMIIAR = tmpReg;
}