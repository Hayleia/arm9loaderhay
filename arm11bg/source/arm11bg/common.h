#include "constants.h"
#include <inttypes.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define vu8 volatile u8
#define vu16 volatile u16
#define vu32 volatile u32
#define vu64 volatile u64

#define s8 int8_t
#define s16 int16_t
#define s32 int32_t
#define s64 int64_t

#define vs8 volatile s8
#define vs16 volatile s16
#define vs32 volatile s32
#define vs64 volatile s64

//default -1 arm11 done  
typedef struct {
    vu32 a11ControllValue;
    vu32 a11threadRunning;
    vu32 version;
    vu32 brightness;
    vu32 fbTopLeft;
    vu32 fbTopRigth;
    vu32 fbBottom;
    vs32 setBrightness;
    vs32 enableLCD;
    vu32 mode;
    vs32 changeMode;
} a11Commands;
