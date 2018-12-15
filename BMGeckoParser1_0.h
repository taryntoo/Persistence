// BMGeckoParser1_0
//
//  Created by taryn on 8/13. (C) 2013

/** @brief Parsing and packing library for Gecko (Bluetooth LE) 1.0 packets
 
  Goals:
    Converting the bitpacked Gecko packets to data structures easily accessed by high level languages.
    Reversing that process, turning C structures to bitpacked gecko packets.
    Posix compliance in API data types and internal code to allow portability across all modern systems. 
    Limiting special case requirements by driving the process with tables describing the data layouts.
    Reducing complexity by utilizing a union of "sparse" structures, whose similar elements are always
    positioned at identical offsets.


  Tables:
    Can be almost identical to the spreadsheets used to describe the process.
 
 
  Notes:
    Time formats: 
        The parsed structures retain the strange minute-based times of the packets.
        Our device epoch began at 2013:01:01:00:00.
        Max times actually encoded in any packet: 2 in GEC_DEVICE_TIME?
     
        Some Packets have multiple minutes worth of data, e.g. GEC_ALG_<mumble> variants. Should the
        substructures describing these events each have their own (calculated) timestamp, e.g. GEC_ALG_2's 
        minute2 and minute3 values, or should we require the lib caller to calculate the times, only 
        delivering the minute1 time? For now, we'll include the minute in each substruct.
     
        The actual units for members named either "kCal" or "Calorie" in the spreadsheet are all really kCals.

  Pending Questions:
 
    Distance: Packet val/2 = meters/min? Packet val = meters/tick *2?   Reconcile spreadsheed comments.
 
    Write versus read: Should the packet-to-struct func always assume this was a read?
        should struct-to-packet always set the write bit of the type?
 
    The automatic offset increment into the struct seemed like a way to save a lot of entries in the parse tables
    but is probably an awful idea, a  source of weird bugs. I should just finish declaring all the type structures 
    and clean up the tables.
 
  
*/    


#ifndef BMGeckoParser1_0_h
#define BMGeckoParser1_0_h

#include <stddef.h>

#include "BMplatform.h"

#include "BMGeckoPacketDefs.h"

/** @brief errors returned from the parser
 Arbitrarily, we will root parse failures at -199,
 note that the subsequent values will number -198, 197, etc.
 MARK: BMGeckoParseFailures_t
 */
typedef enum
{
    BMGeckoParseFail_UnknownPacketType = -199,  // Not possible to parse
    BMGeckoParseFail_SanityFailure,             // Ill-made packet
    BMGeckoParseFail_BadParameter,
    BMGeckoParseFail_UnimplementedSpecialHandler,
    BMGeckoParseFail_195,
    BMGeckoParseFail_194,
    BMGeckoParseFail_193,
    BMGeckoParseFail_192,
    BMGeckoParseFail_NoMemory, // malloc failure.
    
    // All failures should be above this and negative.
    BMGeckoParseSuccess = 0 // this should never actually be seen, parsing returns the parsed type.
} BMGeckoParseFailures_t;



/** @brief an object containing a single Gecko Packet.
 Declared only for type checking, and to allow correct allocation, callers need know nothing about it. */
typedef union BMGeckoLEPacketUnionTag {
    uint32_t u32[5];
    uint16_t u16[10];
    uint8_t  u8[20];
} BMGeckoLEPacketUnion;



 
/** @brief Structures and sub-structures returned from and passed to the parser calls. 
 Because all targeted platforms, including smartphones, tablets, and desktop devices
 have a word size of 32 bits or more, all the parsed structure members have a size of 
 at least 32 bits.
 */

/** @brief Base indexes into the BMDeviceMessageUnion
 */
typedef enum
{
    BMuType = 0,
    BMu__1,
    BMu__2,
    BMu__3,
    BMu__4,
    BMu__5,
    BMu__6,
    BMu__7,
    BMu__8,
    BMu__9,
    BMu_10,
    BMu_11,
    BMu_12,
    BMu_13,
    BMu_14,
    BMu_15,
    BMu_16,
    BMu_17,
    BMu_18,
    BMu_19,
    BMu_20,
} BMUMindices;



/** @brief Unified structure to access all Algorithm data elements minute-by-minute
  Question, should each minute contain it's own time? Yes, for now.
 */
typedef struct BMDeviceAlgorithmMessageTag 
{
    int32_t minute; // (since epoch) Signed because I can spare the bits, and negative times might be needed.
    int32_t kCals;  // mostly this is kCals/min and could fit in fewer bits, but a few spots need summations.
    int32_t steps;  //
    int32_t distance; // Always delivered here in meters, calculation comments seem contradictory.
    // 16 bytes for first four members
    
    int32_t heartrate; // Beats/minute This can fit in 8 bits (0-255) for humans, but not for many beasts.
    int32_t heartvariability; // Need to understand these units.
    // 24 bytes 
    
    // We want this struct to end on a 64 bit address offset if we can.
    // That may require this struct to be packed if we want to avoid automatic padding.
    // Make them bigger, if that's the case.
    int32_t activity; // enumerated value from activity table.
    int32_t validity; // a bool, packed with a byte
} BMDeviceAlgorithmMessage;

/** @brief structure to access the weird time of Gecko devices 
 */
typedef struct BMDeviceTimeTag // each time instance is 12 bytes if unpadded
{
    int32_t minutes; // (since epoch) Signed because I can spare the bits, and negative times might be needed.
    int32_t seconds;
    int32_t ticks;   // 1/32 second
    // total 12 bytes
} BMDeviceTime;


/** brief A "base structure" containing all common packet variants. 
 Do not be concerned with the silly element names, callers will always be using 
 meaningful names to reference the structure members of any BMDeviceUnifiedMessage instance. 
 As much as possible symbols and element names match existing objects in Adam's BMLE<mumble>Packet files.
 
 This has been padded heavily to reduce the clutter of multiple, almost identical, structure declarations.
 Many elements have been declared oversize to ensure fast resolution on systems with poor unaligned addressing
 Struct reordering by the compiler continues to be forbidden since K&R days (ISO/IEC 9899:TC2, 6.7.2.1.12)
 so that's a non issue.
 */
typedef struct BMDeviceUnifiedMessageTag
{
    int32_t      packetType;        // The "direction" bit is never set in this field, only the type.
    int32_t      int32AtOffset4;    // A filler, 
    BMDeviceTime BMDtimeAtOffset8;  // A large fraction of packets have one set of time data,
    int32_t      int32AtOffset20;    // A filler,
    BMDeviceTime BMDtimeAtOffset24; // Some have two time-like datasets
    
} BMDeviceUnifiedMessage;


/* Reminder to self, offsetof() can prove packing assumptions are correct in all target platforms, 
 but it appears there's a bug in clang regarding this, so revisit it to make it work
 http://www.embedded.com/design/prototyping-and-development/4024941/Learn-a-new-trick-with-the-offsetof--macro
#if(offsetof(BMDeviceUnifiedMessageTag,BMDtimeAtOffset8)!=8
# error Struct packing failure in BMDeviceUnifiedMessage
#endif // offsetof(...
*/
// static size_t testoffset = offsetof(BMDeviceUnifiedMessage,BMDtimeAtOffset24);


/** TODO: Bug! Compute this as max len of union structs! */

#define MAX_BMDeviceMessage_LENGTH 200

typedef union BMDeviceMessageUnionTag
{
    uint32_t u32[MAX_BMDeviceMessage_LENGTH];
    uint32_t messagetype;
    float   f32[MAX_BMDeviceMessage_LENGTH];
    BMDeviceAlgorithmMessage algmsg;
    BMDeviceUnifiedMessage unimsg;
} BMDeviceMessageUnion;




/** @brief turn a Gecko packet into directly accesible structures
 BMDevMessage, a union of structs, is fully transformed
 for the current platform's endianness, data sizes, etc.
 Will either calloc and populate a BMDeviceMessageUnion, passing it back
 in *ppDevMessage, or clear and populate an existing one, depending on
 whether ppDevMessage is nil or valid.
 */
int BMGeckoCopyPacketToStruct(BMGeckoLEPacketUnion* pPacket,   // As delivered over BT
                              BMDeviceMessageUnion** ppDevMessage);   // Filled out with union whose type
                                                                // is identified both by the return value of
                                                                // call, and first member of the union.

/** @brief assemble a packet from a passed in Device Message union
 Will either calloc and populate a packet, passing it back
 in *ppPacket, or clear and populate an existing one, depending on
 whether ppPacket is nil or valid.
 */
int BMGeckoCopyStructToPacket(const BMDeviceMessageUnion* pDevMessage,  // created by the caller.
                              BMGeckoLEPacketUnion** ppPacket);      // prepare to deliver over BT


/** @brief Check the parser's tables for sanity 
Intended to test apps, to be run at build time.
 */
int GeckolateValidateParseTables(/* BMGeckoPackets*/ void* gp); // return true if table sane 


/** @brief Runtime sanity checking combined with debug mode assertions.
 Test a condition, if true, return true.
 else log the insanity, fire an assertion, and return false.
 */
bool IsSane(intptr_t condition, char* format,...);





#endif // BMGeckoParser1_0_h
