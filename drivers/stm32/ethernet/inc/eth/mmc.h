#ifndef MMC_H_
#define MMC_H_

#ifdef __cplusplus
extern "C" {
#endif

void mmc_init(void);
void mmc_irqHandler(void);
void mmc_debug(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif