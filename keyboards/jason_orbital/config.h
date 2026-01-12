#pragma once

#define SPLIT_USB_DETECT
#define SERIAL_USART_FULL_DUPLEX
#define SERIAL_USART_PIN_SWAP
#define SERIAL_USART_TX_PIN GP0
#define SERIAL_USART_RX_PIN GP1

#define MATRIX_ROW_PINS { GP3, GP4 }
#define MATRIX_COL_PINS { GP23, GP24, GP25 }
#define EE_HANDS
#define DEBOUNCE 5   // or 2 or even 0 if your switches are clean
#define ENCODER_DEBOUNCE 5   // or 2 or even 0 if your switches are clean
// #undef MATRIX_IO_DELAY   // or make sure it’s 0
// #define USB_POLLING_INTERVAL_MS 1
