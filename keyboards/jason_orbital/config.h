#pragma once

#define SPLIT_USB_DETECT
#define SERIAL_USART_FULL_DUPLEX
#define SERIAL_USART_PIN_SWAP
// #define SERIAL_USART_TX_PIN GP0
// #define SERIAL_USART_RX_PIN GP1

#define MATRIX_ROW_PINS { GP3, GP4 }
#define MATRIX_COL_PINS { GP23, GP24, GP25 }

/* Encoders */
// #define ENCODERS_PAD_A { GP28, GP28 }
// #define ENCODERS_PAD_B { GP29, GP29 }

#define EE_HANDS
