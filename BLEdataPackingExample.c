//
//  BLEdataPackingExample.c
///
//  Created by taryn on 8/13. (C) 2013
//

#include <stdio.h>

// One of several dozen message types used in a BLE-enabled medical sensor
// Note that the third element is the starting bit offset for the field
//  and the 4th is the field size. These messages are fully bit-packed,
//  completely ignoring word, byte, nibble, or odd/even bounds.

static const BMGeckoPacketParseTable GEC_SENSORS_CALPacketMap = { GEC_SENSORS_CAL,    0, GEC_SENSORS_CALMapEntries};

// MARK: GEC_SENSORS_ACCEL
// offsetB, offsetb, startb, fieldSize, Bytes, mask, offsetIntoUnion, bytesInUnion,  littleEndian,  geckoler,  Field name    Units/Range,       Packet,  Discussion, Storage considerations, Upload considerations
static const BMGeckolatorMapEntry GEC_SENSORS_ACCELMapEntries[] = {   /* Calibrated Accelerometer Readings,   Calibrated Accelerometer 0x16,  60,   minutes per hour, */
    { 0,   7,   0,    8,   1,       0xFFFF, BMuType,   4,   0,   0,    "Type = GEC_SENSORS_ACCEL" },    /* ,   Stream,     packets per hour 1476.92307692308,   seconds to upload */
    { 1,   7,   8,   24,   3,       0xFFFF, BMu__1,    4,   0,   0,    "Time: Minutes" },      /* Minutes since epoch,   Record to Sensor,    24.6153846153847,   minutes */
    { 4,   7,  32,    6,   0.75,    0xFFFF, BMu__2,    4,   0,   0,    "Time: Seconds" },       /* Seconds within minute, */
    { 4,   1,  38,    5,   0.625,   0xFFFF, BMu__3,    4,   0,   0,    "Time: Ticks" },       /* 32Hz ticks within second, */
    { 5,   4,  43,   13,   1.625,   0xFFFF, BMu__4,    4,   0,   0,    "Sample 1 - Accelerometer Longitudinal" },  /* milli-g: +/- 4095, 13 bits for +/- 4095 milli-g, 20 fixed packet size,*/
    { 7,   7,  56,   13,   1.625,   0xFFFF, BMu__5,    4,   0,   0,    "Sample 1 - Accelerometer Transverse" },       /* milli-g: +/- 4095, ,   0,   bytes per hour */
    { 8,   2,  69,   13,   1.625,   0xFFFF, BMu__6,    4,   0,   0,    "Sample 1 - Accelerometer Forward" },       /* milli-g: +/- 4095 */
    { 10,  5,  82,   13,   1.625,   0xFFFF, BMu__7,    4,   0,   0,    "Sample 2 - Accelerometer Longitudinal" },        /* "L,   T,   &F all zero for invalid samples" */
    { 11,  0,  95,   13,   1.625,   0xFFFF, BMu__8,    4,   0,   0,    "Sample 2 - Accelerometer Transverse" },
    { 13,  3, 108,   13,   1.625,   0xFFFF, BMu__9,    4,   0,   0,    "Sample 2 - Accelerometer Forward" },
    { 15,  6, 121,   13,   1.625,   0xFFFF, BMu_10,    4,   0,   0,    "Sample 3 - Accelerometer Longitudinal" },        /* "L,   T,   &F all zero for invalid samples" */
    { 16,  1, 134,   13,   1.625,   0xFFFF, BMu_11,    4,   0,   0,    "Sample 3 - Accelerometer Transverse" },
    { 18,  4, 147,   13,   1.625,   0xFFFF, BMu_12,    4,   0,   0,    "Sample 3 - Accelerometer Forward" },
    {GECKO_PACKET_LENGTH}
}; // ends GEC_SENSORS_ACCELMapEntries

static const BMGeckoPacketParseTable GEC_SENSORS_ACCELPacketMap = { GEC_SENSORS_ACCEL,    0,  GEC_SENSORS_ACCELMapEntries};


