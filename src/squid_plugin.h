#pragma once

#include <string.h>
#include "eth_plugin_interface.h"

#define PARAMETER_LENGTH 32
#define SELECTOR_SIZE    4

#define RECIPIENT_ADDRESS_LENGTH     46
#define RECIPIENT_SECOND_HALF_LENGTH 13

#define AMOUNT_LENGTH     32
#define DEST_CHAIN_LENGTH 26

#define RUN_APPLICATION 1

#define NUM_SQUID_SELECTORS 4

#define PLUGIN_NAME "Squid"

#define TOKEN_SENT_FOUND 1

#define NUM_SUPPORTED_CHAINS 25
#define NUM_SUPPORTED_TOKENS 18

extern const uint8_t NULL_ETH_ADDRESS[ADDRESS_LENGTH];

typedef struct tokenSymbolToDecimals_t {
    char token_symbol[MAX_TICKER_LEN];
    uint8_t decimals_sent;
} tokenSymbolToDecimals_t;

// Longest chain id is currently "ASSETMANTLE"
#define MAX_CHAIN_ID_LEN 12
// Longest chain name is currently "Avalanche C-Chain"
#define MAX_CHAIN_NAME_LEN 18

typedef struct chainIdToChainName_t {
    char chain_id[MAX_CHAIN_ID_LEN];
    char chain_name[MAX_CHAIN_NAME_LEN];
} chainIdToChainName_t;

extern const struct chainIdToChainName_t SQUID_SUPPORTED_CHAINS[NUM_SUPPORTED_CHAINS];
extern const struct tokenSymbolToDecimals_t SQUID_SUPPORTED_TOKENS[NUM_SUPPORTED_TOKENS];

// Returns 1 if corresponding address is the address for the chain token (ETH, BNB, MATIC,
#define ADDRESS_IS_NETWORK_TOKEN(_addr) (!memcmp(_addr, NULL_ETH_ADDRESS, ADDRESS_LENGTH))

typedef enum { CALL_BRIDGE_CALL, BRIDGE_CALL, CALL_BRIDGE, SEND_TOKEN } pluginSelector_t;

extern const uint8_t *const SQUID_SELECTORS[NUM_SQUID_SELECTORS];

typedef enum {
    SEND_SCREEN,
    TO_ASSET_SCREEN,
    DEST_CHAIN_SCREEN,
    RECIPIENT_SCREEN,
    WARN_TOKEN_SCREEN,
    WARN_CHAIN_SCREEN,
    ERROR_SCREEN,
} screens_t;

#define AMOUNT_SENT           0  // Amount sent by the user to the contract.
#define TOKEN_SENT            1
#define SAVE_SYMBOL_OFFSET    2
#define SAVE_CHAIN_OFFSET     3
#define SAVE_RECIPIENT_OFFSET 4
#define SKIP                  5
#define DEST_CHAIN            6
#define SKIP_2                7
#define TOKEN_SYMBOL          8
#define SKIP_3                9
#define RECIPIENT_FIRST_HALF  10
#define RECIPIENT_SECOND_HALF 11
#define NONE                  12  // Placeholder variant to be set when parsing is done.

// Number of decimals used when the token wasn't found in the CAL.
#define DEFAULT_DECIMAL WEI_TO_ETHER

// Ticker used when the token wasn't found in the CAL.
#define DEFAULT_TICKER ""

// Shared global memory with Ethereum app. Must be at most 5 * 32 bytes.
typedef struct squid_parameters_t {
    char recipient[RECIPIENT_ADDRESS_LENGTH];
    char dest_chain[DEST_CHAIN_LENGTH];
    uint8_t amount_sent[AMOUNT_LENGTH];
    uint8_t token_sent[ADDRESS_LENGTH];
    char token_symbol[MAX_TICKER_LEN];
    char ticker_sent[MAX_TICKER_LEN];

    uint16_t offset;
    uint16_t saved_offset_1;
    uint16_t saved_offset_2;
    uint16_t saved_offset_3;
    uint8_t next_param;
    uint8_t tokens_found;
    uint8_t decimals_sent;
    uint8_t selectorIndex;
    uint8_t skip;
} squid_parameters_t;  // Remove any variable not used
// 32*1 + 1*27 + 1*20 + 11*2 + 1*45 = 146
// 2*4 + 1*5 = 13
// 13+146 = 159

// Piece of code that will check that the above structure is not bigger than 5 * 32.
// Do not remove this check.
_Static_assert(sizeof(squid_parameters_t) <= 5 * 32, "Structure of parameters too big.");

static inline void printf_hex_array(const char *title __attribute__((unused)),
                                    size_t len __attribute__((unused)),
                                    const uint8_t *data __attribute__((unused))) {
    PRINTF(title);
    for (size_t i = 0; i < len; ++i) {
        PRINTF("%02x", data[i]);
    };
    PRINTF("\n");
}

static inline bool is_chain_supported(squid_parameters_t *context) {
    for (size_t i = 0; i < NUM_SUPPORTED_CHAINS; i++) {
        if (!strncmp(context->dest_chain, SQUID_SUPPORTED_CHAINS[i].chain_id, MAX_CHAIN_ID_LEN)) {
            return 1;
        }
    }
    return 0;
}
