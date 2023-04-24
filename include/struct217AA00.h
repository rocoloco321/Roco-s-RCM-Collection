#pragma once

#include "stateMachine.h"
#include "race/raceConfig.h"

typedef struct
{
    u8 field0[6];
    u16 field6;
} struct_217AA00_field44_t;

typedef struct
{
    u16 nickName[10];
    u8 emblem[512];
    u8 hasEmblem;
    u16 field216;
    DWCAccFriendData exchangeToken;
    u8 unlockBits[4];
    u8 field228 : 4;
    u8 : 4;
    u8 field229;
    u8 field22A;
    u8 field22B;
} struct_217AA00_field45C_t;

typedef struct
{
    u16 field0[3];
    s16 field6;
} struct_217AA00_field1E4C_entry_t;

typedef struct
{
    u16 field0;
    u16 tp;
    u8 gap4[2];
    u8 flags : 7;
    u8 mic : 1;
    u8 field7;
} struc_252;

typedef struct
{
    WMDataSharingInfo* wmDataSharingInfo;
    void* field4;
    void* field8;
    struc_252* fieldC;
    u32 field10[8];
    u8 field30;
    u8 gap31[0x34 - 0x31];
    u32 field34;
    u16 field38;
    u8 field3A;
    u8 field3B;
    u16 field3C[7];
    u8 field4A;
} struc_396;

typedef struct
{
    struct_217AA00_field1E4C_entry_t field0[RACE_DRIVER_COUNT_MAX];
    u16 field40[RACE_DRIVER_COUNT_MAX];
    u8 gap50[0x60 - 0x50];
    u8 field60;
    u8 gap61[0x64 - 0x61];
    u32 field64;
    u8 field68;
} struct_3;

typedef enum
{
    STRUCT_217AA00_FIELD0_0 = 0,
    STRUCT_217AA00_FIELD0_1 = 1,
    STRUCT_217AA00_FIELD0_2 = 2,
} Struct217AA00Field0;

typedef enum
{
    STRUCT_217AA00_FIELD4_0 = 0,
    STRUCT_217AA00_FIELD4_1 = 1,
    STRUCT_217AA00_FIELD4_2 = 2,
    STRUCT_217AA00_FIELD4_3 = 3,

    STRUCT_217AA00_FIELD4_6 = 6,
    STRUCT_217AA00_FIELD4_7 = 7,
    STRUCT_217AA00_FIELD4_8 = 8,
    STRUCT_217AA00_FIELD4_9 = 9,

    STRUCT_217AA00_FIELD4_11 = 11,

    STRUCT_217AA00_FIELD4_13 = 13
} Struct217AA00Field4;

typedef enum
{
    STRUCT_217AA00_FIELDC_0 = 0,
    STRUCT_217AA00_FIELDC_1 = 1,
    STRUCT_217AA00_FIELDC_2 = 2,
    STRUCT_217AA00_FIELDC_3 = 3,
    STRUCT_217AA00_FIELDC_4 = 4,
    STRUCT_217AA00_FIELDC_5 = 5,
    STRUCT_217AA00_FIELDC_6 = 6,
    STRUCT_217AA00_FIELDC_7 = 7,
    STRUCT_217AA00_FIELDC_8 = 8,
    STRUCT_217AA00_FIELDC_9 = 9,
    STRUCT_217AA00_FIELDC_10 = 10,
    STRUCT_217AA00_FIELDC_11 = 11,
    STRUCT_217AA00_FIELDC_12 = 12,
    STRUCT_217AA00_FIELDC_13 = 13,
    STRUCT_217AA00_FIELDC_14 = 14,
    STRUCT_217AA00_FIELDC_15 = 15,
    STRUCT_217AA00_FIELDC_16 = 16,
    STRUCT_217AA00_FIELDC_17 = 17,
    STRUCT_217AA00_FIELDC_18 = 18,
    STRUCT_217AA00_FIELDC_19 = 19,
    STRUCT_217AA00_FIELDC_20 = 20,
    STRUCT_217AA00_FIELDC_21 = 21,
    STRUCT_217AA00_FIELDC_22 = 22,
    STRUCT_217AA00_FIELDC_23 = 23,
    STRUCT_217AA00_FIELDC_24 = 24,
    STRUCT_217AA00_FIELDC_25 = 25,

    STRUCT_217AA00_FIELDC_27 = 27,
    STRUCT_217AA00_FIELDC_28 = 28,
    STRUCT_217AA00_FIELDC_29 = 29,
    STRUCT_217AA00_FIELDC_30 = 30,
    STRUCT_217AA00_FIELDC_31 = 31,
    STRUCT_217AA00_FIELDC_32 = 32,
    STRUCT_217AA00_FIELDC_33 = 33,
    STRUCT_217AA00_FIELDC_34 = 34,
    STRUCT_217AA00_FIELDC_35 = 35,
    STRUCT_217AA00_FIELDC_36 = 36,
    STRUCT_217AA00_FIELDC_37 = 37,
    STRUCT_217AA00_FIELDC_38 = 38,
    STRUCT_217AA00_FIELDC_39 = 39,
    STRUCT_217AA00_FIELDC_40 = 40,
    STRUCT_217AA00_FIELDC_41 = 41,
    STRUCT_217AA00_FIELDC_42 = 42,
    STRUCT_217AA00_FIELDC_43 = 43,
    STRUCT_217AA00_FIELDC_44 = 44,
    STRUCT_217AA00_FIELDC_45 = 45,
    STRUCT_217AA00_FIELDC_46 = 46
} Struct217AA00FieldC;

typedef struct
{
    Struct217AA00Field0 field0;
    Struct217AA00Field4 field4;
    u32 field8;
    Struct217AA00FieldC fieldC;
    u32 field10;
    void* field14;
    void* field18;
    void* field1C;
    void* field20;
    void* field24;
    void* field28;
    void* field2C;
    u32 sizeOfField34;
    void* field34;
    u32 sizeOfField3C;
    void* field3C;
    void* field40;
    struct_217AA00_field44_t field44[7];
    void* field7C;
    u16 field80;
    u16 field82;
    u8 field84;
    u8 field85;
    u8 field86;
    u8 field87;
    u8 field88;
    u8 gap89;
    u8 field8A;
    u8 field8B;
    u8 gap8C[0x432 - 0x8C];
    u16 field432;
    u16 field434;
    u16 field436;
    u8 field438;
    u8 field439;
    u8 field43A;
    s8 field43B;
    u8 field43C;
    u8 field43D;
    u8 unk43E[0x454 - 0x43E];
    u16 field454;
    u8 field456;
    u8 field457;
    u8 field458;
    u8 unk459[0x45c - 0x459];
    struct_217AA00_field45C_t field45C[RACE_DRIVER_COUNT_MAX];
    struc_396 field15BC;
    state_machine_t field1608;
    u8 gap161C[0x1658 - 0x161C];
    u32 field1658;
    u32 field165C;
    u32 field1660;
    u32 field1664;
    u8 gap1668[0x1699 - 0x1668];
    u8 field1699;
    u8 gap169A[0x1D04 - 0x169A];
    NNSFndHeapHandle field1D04;
    u8 gap1D08[0x1D5C - 0x1D08];
    u16 field1D5C;
    u8 gap1D5E[0x1DA0 - 0x1D5E];
    u32* field1DA0[16];
    u32 field1DE0[16];
    u32 field1E20;
    u32 field1E24;
    u32 field1E28;
    u32 field1E2C;
    u8 gap1E30[0x1E38 - 0x1E30];
    u32 field1E38;
    u32 field1E3C;
    u16 field1E40;
    u16 field1E42;
    u8 field1E44;
    s8 field1E45;
    u8 field1E46;
    u8 gap1E47;
    u32 field1E48;
    struct_3 field1E4C;
    u8 unk1EB8[0x1EE8 - 0x1EB8];
    MATHRandContext32 field1EE8;
} struct_217AA00_t;

extern struct_217AA00_t* gStruct217AA00;

static inline Struct217AA00Field0 struct217AA00_getField0(void)
{
    return !gStruct217AA00 ? STRUCT_217AA00_FIELD0_0 : gStruct217AA00->field0;
}

static inline BOOL struct217AA00Inline(void)
{
    BOOL result;
    Struct217AA00Field0 field0;
    result = FALSE;
    if (!gStruct217AA00)
        result = field0 = STRUCT_217AA00_FIELD0_0;
    else
        field0 = gStruct217AA00->field0;
    if (field0 != STRUCT_217AA00_FIELD0_0 && gStruct217AA00->field4 != STRUCT_217AA00_FIELD4_13)
        result = TRUE;
    return result;
}