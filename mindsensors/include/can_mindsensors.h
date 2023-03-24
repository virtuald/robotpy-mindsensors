#ifndef _MINDSENSORS_CAN_H__
#define _MINDSENSORS_CAN_H__

// "The Reserved manufacturer identifiers in the Message Id."
// mindsensors manufacturer ID
#define CAN_MSGID_MFR_MS  0x00070000

// "The masks of the fields that are used in the message identifier."
#define CAN_MSGID_FULL_M  0x1fffffff
#define CAN_MSGID_API_S   6

// "The Reserved system control API numbers in the Message Id."
#define CAN_MSGID_API_SYSHALT    0x00000000
#define CAN_MSGID_API_SYSRST     0x00000040
#define CAN_MSGID_API_HEARTBEAT  0x08041C00
#define CAN_MSGID_API_SYSRESUME  0x00000280

#define BTL_DATA  0x00
#define BTL_CMD   0x02

#define BTL_CMD_WRITE_UNLOCK  10
#define BTL_CMD_LOAD_ADDRESS  11
#define BTL_CMD_LOAD_DATA     12 /* not used */
#define BTL_CMD_WRITE_DATA    13
#define BTL_CMD_ERASE         14
#define BTL_CMD_RESET         15 /* not used */
#define BTL_CMD_CHKSUM        16 /* not used */
#define BTL_CMD_RUN           17

#define FIRMWARE    0x00003000
#define CHANGE_ID   0x00003400
#define DEVNAME     0x00003800
#define BOOTLOADER  0x00003C00
#define BLINK       0x00004000
#define DEVSERNO    0x00004400
#define FLIP        0x00004800 /* not used */

#endif
