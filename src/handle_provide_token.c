#include "squid_plugin.h"

static void sent_network_token(squid_parameters_t *context) {
    context->decimals_sent = WEI_TO_ETHER;
    context->tokens_found |= TOKEN_SENT_FOUND;
}

void handle_provide_token(ethPluginProvideInfo_t *msg) {
    squid_parameters_t *context = (squid_parameters_t *) msg->pluginContext;
    PRINTF("Plugin provide tokens : 0x%p, 0x%p\n", msg->item1, msg->item2);

    switch (context->selectorIndex) {
        // fall through
        case CALL_BRIDGE_CALL:
        case CALL_BRIDGE:
            if (ADDRESS_IS_NETWORK_TOKEN(context->token_sent)) {
                sent_network_token(context);
            } else if (msg->item1 != NULL) {
                context->decimals_sent = msg->item1->token.decimals;
                strlcpy(context->ticker_sent,
                        (char *) msg->item1->token.ticker,
                        sizeof(context->ticker_sent));
                context->tokens_found |= TOKEN_SENT_FOUND;
            } else {
                // CAL did not find the token and token is not ETH.
                context->decimals_sent = DEFAULT_DECIMAL;
                strlcpy(context->ticker_sent, DEFAULT_TICKER, sizeof(context->ticker_sent));
                // // We will need an additional screen to display a warning message.
                msg->additionalScreens++;
            }
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        // Skip this for bridgeCall and sendToken
        // fall through
        case BRIDGE_CALL:
        case SEND_TOKEN:
            msg->result = ETH_PLUGIN_RESULT_OK;
            break;
        default:
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}
