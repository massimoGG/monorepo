#ifndef ETHERNET_LL__
#define ETHERNET_LL__

#ifdef __cplusplus
extern "C" {
#endif

#include <error_codes.h>
#include <stdint.h>

void eth_ll_init(void);
error_e eth_ll_deinit(void);
error_e eth_ll_start(void);
void eth_ll_stop(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif