#include "squid_plugin.h"

// Called once to init.
void handle_init_contract(ethPluginInitContract_t *msg) {
    if (msg->interfaceVersion != ETH_PLUGIN_INTERFACE_VERSION_LATEST) {
        msg->result = ETH_PLUGIN_RESULT_UNAVAILABLE;
        return;
    }

    if (msg->pluginContextLength < sizeof(squid_parameters_t)) {
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }

    squid_parameters_t *context = (squid_parameters_t *) msg->pluginContext;
    memset(context, 0, sizeof(*context));

    // Assign an invalid value to selectorIndex by default and only set it to a valid
    // value when a selector matches.
    context->selectorIndex = NUM_SQUID_SELECTORS;

    // Determine a function to call
    size_t i;
    for (i = 0; i < NUM_SQUID_SELECTORS; i++) {
        if (memcmp((uint8_t *) PIC(SQUID_SELECTORS[i]), msg->selector, SELECTOR_SIZE) == 0) {
            context->selectorIndex = i;
            break;
        }
    }

    if (i == NUM_SQUID_SELECTORS) {
        // Selector was not found
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }

    // Set `next_param` to be the first field we expect to parse.
    switch (context->selectorIndex) {
        // fall through
        case CALL_BRIDGE_CALL:
        case CALL_BRIDGE:
            context->next_param = TOKEN_SENT;
            break;
        // fall through
        case BRIDGE_CALL:
        case SEND_TOKEN:
            context->next_param = SAVE_CHAIN_OFFSET;
            break;
        default:
            PRINTF("Missing selectorIndex\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}
