#ifndef _EC_SERVER_WHEEL_OD_H_
#define _EC_SERVER_WHEEL_OD_H_






#define SEQ_CONTROL_WORD "101"
#define OD_CONTROL_WORD "0x6040 0"
#define DT_CONTROL_WORD "i16"

#define SEQ_ACTUAL_POSITION "201"
#define OD_ACTUAL_POSITION "0x6063 0"
#define DT_ACTUAL_POSITION "i32"

#define SEQ_ACTUAL_POSITION_CLEAR "202"
#define OD_ACTUAL_POSITION_CLEAR "0x607C 2"
#define DT_ACTUAL_POSITION_CLEAR "u8" 

#define SEQ_TARGET_VELOCITY "301"
#define OD_TARGET_VELOCITY "0x60FF 0"
#define DT_TARGET_VELOCITY "i32"

#define SEQ_ACCELERATION "401"
#define OD_ACCELERATION "0x6083 0"
#define DT_ACCELERATION "u32"

#define SEQ_DECELERATION "402"
#define OD_DECELERATION "0x6084 0"
#define DT_DECELERATION "u32"

#define SEQ_ROTATION_DIRECTION "501"
#define OD_ROTATION_DIRECTION "0x6410 0x13"
#define DT_ROTATION_DIRECTION "u8"


#define ROTATION_CLOCKWISE 1
#define ROTATION_COUNTER_CLOCKWISE 0


#endif