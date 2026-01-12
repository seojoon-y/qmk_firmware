# Build Options
#   change yes to no to disable

AUDIO_SUPPORTED = no
RGB_MATRIX_SUPPORTED = no
RGBLIGHT_SUPPORTED = no
BOOTMAGIC_ENABLE = yes
EXTRAKEY_ENABLE = yes
SPLIT_KEYBOARD = yes
SERIAL_DRIVER = vendor

# Trackball
POINTING_DEVICE_ENABLE = no
# POINTING_DEVICE_DRIVER = pmw3389 # Change to pmw3360 if you use that

# Encoder
ENCODER_ENABLE = yes
ENCODER_MAP_ENABLE = no

# Orbital mouse
SRC += features/orbital_mouse.c
MOUSE_ENABLE = yes
DIGITIZER_ENABLE = yes

ANALOG_DRIVER_REQUIRED = yes
CONSOLE_ENABLE = yes
