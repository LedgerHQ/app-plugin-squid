#include <stdbool.h>
#include "squid_plugin.h"

// Set UI for the "Send" screen.
static bool set_send_ui(ethQueryContractUI_t *msg, squid_parameters_t *context) {
    strlcpy(msg->title, "Send", msg->titleLength);
    switch (context->selectorIndex) {
        // fall through
        case CALL_BRIDGE_CALL:
        case CALL_BRIDGE:
            // set network ticker (ETH, BNB, etc) if needed
            if (ADDRESS_IS_NETWORK_TOKEN(context->token_sent)) {
                strlcpy(context->ticker_sent, msg->network_ticker, sizeof(context->ticker_sent));
            }
            break;
        // fall through
        case BRIDGE_CALL:
        case SEND_TOKEN:
            break;
        default:
            PRINTF("Unhandled selector Index: %d\n", context->selectorIndex);
            return false;
    }
    if (!amountToString(context->amount_sent,
                        AMOUNT_LENGTH,
                        context->decimals_sent,
                        context->ticker_sent,
                        msg->msg,
                        msg->msgLength)) {
        return false;
    }
    PRINTF("AMOUNT SENT: %s\n", msg->msg);
    return true;
}

// Set UI for "To Asset" screen.
static bool set_to_asset_ui(ethQueryContractUI_t *msg, squid_parameters_t *context) {
    PRINTF("To Asset: %s\n", context->token_symbol);
    strlcpy(msg->title, "To Asset", msg->titleLength);
    strlcpy(msg->msg, context->token_symbol, msg->msgLength);
    return true;
}

// Set UI for "Destination Chain" screen.
static bool set_dest_chain_ui(ethQueryContractUI_t *msg, squid_parameters_t *context) {
    strlcpy(msg->title, "To Chain", msg->titleLength);
    for (size_t i = 0; i < NUM_SUPPORTED_CHAINS; i++) {
        if (!memcmp(context->dest_chain, SQUID_SUPPORTED_CHAINS[i].chain_id, MAX_CHAIN_ID_LEN)) {
            strlcpy(msg->msg, SQUID_SUPPORTED_CHAINS[i].chain_name, msg->msgLength);
            return true;
        }
    }
    strlcpy(msg->msg, context->dest_chain, msg->msgLength);
    return true;
}

// Set UI for "Recipient" screen.
static bool set_recipient_ui(ethQueryContractUI_t *msg, squid_parameters_t *context) {
    strlcpy(msg->title, "Recipient", msg->titleLength);
    strlcpy(msg->msg, context->recipient, msg->msgLength);
    return true;
}

// Set UI for "Warning" screen for the token.
static bool set_token_warning_ui(ethQueryContractUI_t *msg,
                                 const squid_parameters_t *context __attribute__((unused))) {
    strlcpy(msg->title, "WARNING", msg->titleLength);
    strlcpy(msg->msg, "Unknown token", msg->msgLength);
    return true;
}

// Set UI for "Warning" screen for the destination chain.
static bool set_chain_warning_ui(ethQueryContractUI_t *msg,
                                 const squid_parameters_t *context __attribute__((unused))) {
    strlcpy(msg->title, "WARNING", msg->titleLength);
    strlcpy(msg->msg, "Unsupported chain", msg->msgLength);
    return true;
}

// Helper function that returns the enum corresponding to the screens that should be displayed
// for each methods.
static screens_t get_screen(ethQueryContractUI_t *msg,
                            squid_parameters_t *context __attribute__((unused))) {
    uint8_t index = msg->screenIndex;

    bool token_sent_found = context->tokens_found & TOKEN_SENT_FOUND;
    bool chain_supported = is_chain_supported(context);

    PRINTF("token_sent_found: %d\n", token_sent_found);
    PRINTF("chain_supported: %d\n", chain_supported);

    switch (context->selectorIndex) {
        case CALL_BRIDGE:
            switch (index) {
                case 0:
                    if (token_sent_found) {
                        return SEND_SCREEN;
                    } else {
                        return WARN_TOKEN_SCREEN;
                    }
                case 1:
                    if (token_sent_found) {
                        return TO_ASSET_SCREEN;
                    } else {
                        return SEND_SCREEN;
                    }
                case 2:
                    if (token_sent_found && chain_supported) {
                        return DEST_CHAIN_SCREEN;
                    } else if (token_sent_found && !chain_supported) {
                        return WARN_CHAIN_SCREEN;
                    } else if (!token_sent_found) {
                        return TO_ASSET_SCREEN;
                    } else {
                        return ERROR_SCREEN;
                    }
                case 3:
                    if (token_sent_found && chain_supported) {
                        return ERROR_SCREEN;
                    } else if (!token_sent_found && chain_supported) {
                        return DEST_CHAIN_SCREEN;
                    } else if (token_sent_found && !chain_supported) {
                        return DEST_CHAIN_SCREEN;
                    } else if (!token_sent_found && !chain_supported) {
                        return WARN_CHAIN_SCREEN;
                    } else {
                        return ERROR_SCREEN;
                    }
                case 4:
                    if (!token_sent_found && !chain_supported) {
                        return DEST_CHAIN_SCREEN;
                    } else {
                        return ERROR_SCREEN;
                    }
                default:
                    return ERROR_SCREEN;
            }
            break;
        // fall through
        case CALL_BRIDGE_CALL:
        case BRIDGE_CALL:
            switch (index) {
                case 0:
                    if (token_sent_found) {
                        return SEND_SCREEN;
                    } else {
                        return WARN_TOKEN_SCREEN;
                    }
                case 1:
                    if (token_sent_found && chain_supported) {
                        return DEST_CHAIN_SCREEN;
                    } else if (token_sent_found && !chain_supported) {
                        return WARN_CHAIN_SCREEN;
                    } else if (!token_sent_found) {
                        return SEND_SCREEN;
                    } else {
                        return ERROR_SCREEN;
                    }
                case 2:
                    if (token_sent_found && chain_supported) {
                        return ERROR_SCREEN;
                    } else if (!token_sent_found && chain_supported) {
                        return DEST_CHAIN_SCREEN;
                    } else if (token_sent_found && !chain_supported) {
                        return DEST_CHAIN_SCREEN;
                    } else if (!token_sent_found && !chain_supported) {
                        return WARN_CHAIN_SCREEN;
                    } else {
                        return ERROR_SCREEN;
                    }
                case 3:
                    if (!token_sent_found && !chain_supported) {
                        return DEST_CHAIN_SCREEN;
                    } else {
                        return ERROR_SCREEN;
                    }
                default:
                    return ERROR_SCREEN;
            }
            break;
        case SEND_TOKEN:
            switch (index) {
                case 0:
                    if (token_sent_found) {
                        return SEND_SCREEN;
                    } else {
                        return WARN_TOKEN_SCREEN;
                    }
                case 1:
                    if (token_sent_found && chain_supported) {
                        return DEST_CHAIN_SCREEN;
                    } else if (token_sent_found && !chain_supported) {
                        return WARN_CHAIN_SCREEN;
                    } else if (!token_sent_found) {
                        return SEND_SCREEN;
                    } else {
                        return ERROR_SCREEN;
                    }
                case 2:
                    if (token_sent_found && chain_supported) {
                        return RECIPIENT_SCREEN;
                    } else if (!token_sent_found && chain_supported) {
                        return DEST_CHAIN_SCREEN;
                    } else if (token_sent_found && !chain_supported) {
                        return DEST_CHAIN_SCREEN;
                    } else if (!token_sent_found && !chain_supported) {
                        return WARN_CHAIN_SCREEN;
                    } else {
                        return ERROR_SCREEN;
                    }
                case 3:
                    if (!token_sent_found && !chain_supported) {
                        return DEST_CHAIN_SCREEN;
                    } else if (!token_sent_found && chain_supported) {
                        return RECIPIENT_SCREEN;
                    } else if (token_sent_found && !chain_supported) {
                        return RECIPIENT_SCREEN;
                    } else {
                        return ERROR_SCREEN;
                    }
                case 4:
                    if (!token_sent_found && !chain_supported) {
                        return RECIPIENT_SCREEN;
                    } else {
                        return ERROR_SCREEN;
                    }
                default:
                    return ERROR_SCREEN;
            }
            break;
        default:
            PRINTF("Unhandled selector Index: %d\n", context->selectorIndex);
            return ERROR_SCREEN;
    }
    return ERROR_SCREEN;
}

void handle_query_contract_ui(ethQueryContractUI_t *msg) {
    squid_parameters_t *context = (squid_parameters_t *) msg->pluginContext;
    bool ret = false;

    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);

    screens_t screen = get_screen(msg, context);
    switch (screen) {
        case SEND_SCREEN:
            ret = set_send_ui(msg, context);
            break;
        case TO_ASSET_SCREEN:
            ret = set_to_asset_ui(msg, context);
            break;
        case DEST_CHAIN_SCREEN:
            ret = set_dest_chain_ui(msg, context);
            break;
        case RECIPIENT_SCREEN:
            ret = set_recipient_ui(msg, context);
            break;
        case WARN_TOKEN_SCREEN:
            ret = set_token_warning_ui(msg, context);
            break;
        case WARN_CHAIN_SCREEN:
            ret = set_chain_warning_ui(msg, context);
            break;
        default:
            PRINTF("Received an invalid screenIndex %d\n", screen);
    }
    msg->result = ret ? ETH_PLUGIN_RESULT_OK : ETH_PLUGIN_RESULT_ERROR;
}
