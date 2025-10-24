#include "squid_plugin.h"

// Store the amount sent in the form of a string, without any ticker or decimals. These will be
// added when displaying.
static void handle_amount_sent(ethPluginProvideParameter_t *msg, squid_parameters_t *context) {
    memcpy(context->amount_sent, msg->parameter, AMOUNT_LENGTH);
}

// Stores the address of the token sent
static void handle_token_sent(ethPluginProvideParameter_t *msg, squid_parameters_t *context) {
    memset(context->token_sent, 0, sizeof(context->token_sent));
    memcpy(context->token_sent, msg->parameter, ADDRESS_LENGTH);
}

// Stores the destination chain as a string
static void handle_dest_chain(ethPluginProvideParameter_t *msg, squid_parameters_t *context) {
    memcpy(context->dest_chain, msg->parameter, DEST_CHAIN_LENGTH);
}

// Stores the token symbol of the asset we are bridging to as a string
static void handle_token_symbol(ethPluginProvideParameter_t *msg, squid_parameters_t *context) {
    memcpy(context->token_symbol, msg->parameter, MAX_TICKER_LEN);
}

// Stores the recipient address as a string
static void handle_recipient(ethPluginProvideParameter_t *msg, squid_parameters_t *context) {
    memcpy(context->recipient, msg->parameter, INT256_LENGTH);
}

// Stores the second half of recipient address as a string
static void handle_recipient_2(ethPluginProvideParameter_t *msg, squid_parameters_t *context) {
    memcpy(context->recipient + INT256_LENGTH, msg->parameter, RECIPIENT_SECOND_HALF_LENGTH);
}

static void handle_call_bridge_call(ethPluginProvideParameter_t *msg, squid_parameters_t *context) {
    switch (context->next_param) {
        case TOKEN_SENT:
            handle_token_sent(msg, context);
            printf_hex_array("Token sent: 0x", ADDRESS_LENGTH, context->token_sent);
            context->next_param = AMOUNT_SENT;
            break;
        case AMOUNT_SENT:
            handle_amount_sent(msg, context);
            printf_hex_array("Amount sent:", AMOUNT_LENGTH, context->amount_sent);
            context->next_param = SAVE_CHAIN_OFFSET;
            break;
        case SAVE_CHAIN_OFFSET:
            context->offset = U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->offset));
            PRINTF("offset = %d\n", context->offset);
            context->next_param = SKIP;
            break;
        case SKIP:
            // Skip num of characters declaration for dest chain string
            // Already skipped 1 by going in this case
            context->next_param = DEST_CHAIN;
            break;
        case DEST_CHAIN:
            handle_dest_chain(msg, context);
            PRINTF("dest chain: %s\n", context->dest_chain);
            // go to previously saved offset
            context->offset = context->saved_offset_1;
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_bridge_call(ethPluginProvideParameter_t *msg, squid_parameters_t *context) {
    switch (context->next_param) {
        case SAVE_CHAIN_OFFSET:
            context->saved_offset_1 =
                U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->saved_offset_1));
            PRINTF("saved offset dest chain = %d\n", context->saved_offset_1);
            context->next_param = SAVE_SYMBOL_OFFSET;
            break;
        case SAVE_SYMBOL_OFFSET:
            context->saved_offset_2 =
                U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->offset));
            PRINTF("saved offset token symbol = %d\n", context->saved_offset_2);
            context->next_param = AMOUNT_SENT;
            break;
        case AMOUNT_SENT:
            handle_amount_sent(msg, context);
            printf_hex_array("Amount sent:", AMOUNT_LENGTH, context->amount_sent);
            context->offset = context->saved_offset_1;
            context->next_param = SKIP;
            break;
        case SKIP:
            // Skip num of characters declaration for dest chain string
            // Already skipped 1 by going in this case
            context->next_param = DEST_CHAIN;
            break;
        case DEST_CHAIN:
            handle_dest_chain(msg, context);
            PRINTF("dest chain: %s\n", context->dest_chain);
            context->offset = context->saved_offset_2;
            context->next_param = SKIP_2;
            break;
        case SKIP_2:
            // Skip num of characters declaration for token symbol string
            // Already skipped 1 by going in this case
            context->next_param = TOKEN_SYMBOL;
            break;
        case TOKEN_SYMBOL:
            handle_token_symbol(msg, context);
            PRINTF("token symbol: %s\n", context->token_symbol);
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_send_token(ethPluginProvideParameter_t *msg, squid_parameters_t *context) {
    switch (context->next_param) {
        case SAVE_CHAIN_OFFSET:
            context->saved_offset_1 =
                U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->saved_offset_1));
            PRINTF("saved offset dest chain = %d\n", context->saved_offset_1);
            context->next_param = SAVE_SYMBOL_OFFSET;
            break;
        case SAVE_SYMBOL_OFFSET:
            context->saved_offset_2 =
                U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->offset));
            PRINTF("saved offset token symbol = %d\n", context->saved_offset_2);
            context->next_param = SAVE_RECIPIENT_OFFSET;
            break;
        case SAVE_RECIPIENT_OFFSET:
            context->saved_offset_3 =
                U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->offset));
            PRINTF("saved offset token symbol = %d\n", context->saved_offset_3);
            context->next_param = AMOUNT_SENT;
            break;
        case AMOUNT_SENT:
            handle_amount_sent(msg, context);
            printf_hex_array("Amount sent:", AMOUNT_LENGTH, context->amount_sent);
            context->offset = context->saved_offset_1;
            context->next_param = SKIP;
            break;
        case SKIP:
            // Skip num of characters declaration for dest chain string
            // Already skipped 1 by going in this case
            context->next_param = DEST_CHAIN;
            break;
        case DEST_CHAIN:
            handle_dest_chain(msg, context);
            PRINTF("dest chain: %s\n", context->dest_chain);
            context->offset = context->saved_offset_2;
            context->next_param = SKIP_2;
            break;
        case SKIP_2:
            // Skip num of characters declaration for token symbol string
            // Already skipped 1 by going in this case
            context->next_param = RECIPIENT_FIRST_HALF;
            break;
        case RECIPIENT_FIRST_HALF:
            handle_recipient(msg, context);
            PRINTF("recipient 1: %s\n", context->recipient);
            context->next_param = RECIPIENT_SECOND_HALF;
            break;
        case RECIPIENT_SECOND_HALF:
            handle_recipient_2(msg, context);
            PRINTF("recipient 2: %s\n", context->recipient);
            context->offset = context->saved_offset_3;
            context->next_param = SKIP_3;
            break;
        case SKIP_3:
            // Skip num of characters declaration for token symbol string
            // Already skipped 1 by going in this case
            context->next_param = TOKEN_SYMBOL;
            break;
        case TOKEN_SYMBOL:
            handle_token_symbol(msg, context);
            PRINTF("token symbol: %s\n", context->token_symbol);
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_call_bridge(ethPluginProvideParameter_t *msg, squid_parameters_t *context) {
    switch (context->next_param) {
        case TOKEN_SENT:
            handle_token_sent(msg, context);
            printf_hex_array("Token sent: 0x", ADDRESS_LENGTH, context->token_sent);
            context->next_param = AMOUNT_SENT;
            break;
        case AMOUNT_SENT:
            handle_amount_sent(msg, context);
            printf_hex_array("Amount sent:", AMOUNT_LENGTH, context->amount_sent);
            context->next_param = SAVE_CHAIN_OFFSET;
            break;
        case SAVE_CHAIN_OFFSET:
            context->saved_offset_1 =
                U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->saved_offset_1));
            // Skip destAddress offset
            PRINTF("SAVE_CHAIN_OFFSET\n");
            context->skip += 1;
            context->next_param = SAVE_SYMBOL_OFFSET;
            break;
        case SAVE_SYMBOL_OFFSET:
            // Go to dest chain offset next
            context->offset = context->saved_offset_1;
            // Save token symbol offset
            context->saved_offset_1 =
                U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->offset));
            PRINTF("SAVE_SYMBOL_OFFSET\n");
            context->next_param = SKIP;
            break;
        case SKIP:
            // Skip num of characters declaration for dest chain string
            // Already skipped 1 by going in this case
            context->next_param = DEST_CHAIN;
            break;
        case DEST_CHAIN:
            handle_dest_chain(msg, context);
            PRINTF("dest chain: %s\n", context->dest_chain);
            context->offset = context->saved_offset_1;
            context->next_param = SKIP_2;
            break;
        case SKIP_2:
            // Skip num of characters declaration for token symbol string
            // Already skipped 1 by going in this case
            context->next_param = TOKEN_SYMBOL;
            break;
        case TOKEN_SYMBOL:
            handle_token_symbol(msg, context);
            PRINTF("token symbol: %s\n", context->token_symbol);
            context->next_param = NONE;
            break;
        case NONE:
            break;
        default:
            PRINTF("Param not supported\n");
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

void handle_provide_parameter(ethPluginProvideParameter_t *msg) {
    squid_parameters_t *context = (squid_parameters_t *) msg->pluginContext;
    printf_hex_array("Squid plugin provide parameter: ", PARAMETER_LENGTH, msg->parameter);
    PRINTF("param offset: %d\n", msg->parameterOffset);

    msg->result = ETH_PLUGIN_RESULT_OK;

    if (context->skip) {
        // Skip this step, and don't forget to decrease skipping counter.
        context->skip--;
    } else {
        if ((context->offset) && msg->parameterOffset != context->offset + SELECTOR_SIZE) {
            PRINTF("offset: %d, parameterOffset: %d\n", context->offset, msg->parameterOffset);
            return;
        }
        context->offset = 0;
        switch (context->selectorIndex) {
            case CALL_BRIDGE_CALL:
                handle_call_bridge_call(msg, context);
                break;
            case BRIDGE_CALL:
                handle_bridge_call(msg, context);
                break;
            case CALL_BRIDGE:
                handle_call_bridge(msg, context);
                break;
            case SEND_TOKEN:
                handle_send_token(msg, context);
                break;
            default:
                PRINTF("Selector Index %d not supported\n", context->selectorIndex);
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                break;
        }
    }
}
