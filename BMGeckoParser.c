//  BMGeckoParser.c
//
//  Created by taryn 8/13. (c) 2013
//

#include "BMGeckoParser1_0.h"          // also picks up BMGeckoPacketDefs.h
#include "BMGeckoParserPrivateDefs.h"


/* MARK: Private functions */


/** @brief Choose breakpoints or assertions when in debug mode
 Set true to make IsSane() throw exceptions in debug mode. */
bool AssertIfInsane = false;


/** @brief Runtime sanity checking combined with debug mode assertions.
 Test a condition,
 if true, return true.
 else log the insanity, then either pass through a breakable point or fire an assertion, and finally return false.
 Uses the intptr_t type so it can test any integral type.
 Annoying feature: the condition tested and the error text have opposed state,
 which makes the calls weird and ungrammatical, kinda like the ternary operator in c.
 Someday I'll think through that, but i only use it for debugging anyway.
 */
bool IsSane(intptr_t condition, char* format, ...)
{
    if(condition) {
        return true;

    } else {
        va_list arg_marker;
        va_start(arg_marker, format);
        vfprintf(stderr, format, arg_marker);
        va_end( arg_marker);
        fprintf(stderr,"\n");
    }
    if(AssertIfInsane) {
        assert(condition);
    } else {
        fprintf(stderr,"Continuing, though insane...\n"); // breakpoint here if you want to stop on condition failure
    }
    return false;
}   // ends IsSane(...





/** @brief extract numbits, (max 32) starting at start (max 31) from an uint32
 Flaw: Cannot return true failure, returns 0 if bad parms
 K&R first ed, pp 45 is more succinct than this, but I want to examine intermediates.
 Note that ~ is bitwise negation (I always forget :(
*/
static uint32_t BMgeckoBitsToInt(uint numbits, uint start, uint32_t source)
{
    uint32_t final = 0; // a failure return
    if( IsSane( (numbits && (numbits<=32) && (start < 32)) , "%s, Bad parameter",__func__)){
        
        uint32_t shifted = source >> start;  // modulo is silly, 
        
        // uint32_t shifted = source >> (start+1 - numbits);
        uint32_t mask = ~0 << numbits;
        final = shifted & ~mask;
        
    }
    return final;
}   // ends BMgeckobitsToInt(...


/** @brief Insert numbits (max 32) of newbits, starting at start (max 31) into a uint32
 TODO: toss intermediates after validating logic
 */
void BMgeckoStuffBitsIntoUInt(uint32_t newbits, uint numbits, uint start, uint32_t* thisUint)
{
    uint32_t hotbits = ~(~0 << numbits);        // yields numbits of 1s
    uint32_t shifted = hotbits << start;        // move mask into place
    uint32_t mask = ~shifted;                   // mask in all but our hotbits, which are the target for newbits    
                                                // trim newbits to size, (should never be needed...but...
    uint32_t bitsofinterest = (newbits & hotbits) << start; // then shift newbits to fit into cleared hotbits

    *thisUint &= mask;                          // clear hotbits, leaving balance of target unchanged
    *thisUint |= bitsofinterest;                // and combine.                                     
}




/*  Convert int from Gecko network order to host order (or not, possibly :)
 We'd normally assume that the compiler/platform is best at quickly and properly
 converting to/from network byte order, via ntoh() and her sistren but the fields
 of size 17-24 bits hose that possibility, so just do it ourselves as needed...
 Logic:      |  hostBig  | hostLittle|
 ----------- -----------
 DataBig     |  assign   |   swap    |   A classic XOR table, but C has only bitwise XOR
 DataLittle  |   swap    |   assign  |   bool true ^ true == false is guaranteed (indirectly) for c99
 */

uint32_t BMGeckoToHost(uint32_t geckbits, int fieldsize, bool isLittle)
{
    bool hostBig = BIG_ENDIAN;
    if( hostBig ^ isLittle ) {   // note that both must be forced to bool types before test
        return geckbits;         // Don't swap bigendian data unless field marked as little 
    }
    // Swap little endian data unless this field is tagged little-endian
    return ReEndianOfSize(geckbits,fieldsize);
}

/** Going to a gecko packet requires the same logic, 
 but provide a rational name to go the other way
 uint32_t BMGeckoToNetwork(uint32_t geckbits, int fieldsize, bool isLittle)
    { ... }
*/ 
#define BMGeckoToNetwork(a,b,c) BMGeckoToHost(a,b,c)




/** @brief Turned a fixed point or encoded number into a float, based on reference values in a shapetable
 formed like "s,m,f,o"
 Fraction is misnamed, in these instances, it is the significand or the mantissa
 */
#if WeNeedBMGeckoFloatFromCompressedBits
float BMGeckoFloatFromCompressedBits(const int bits, const char* shapetable)
{
    float flo = 0.0;
    int sbc,mbc,fbc, offset;   // sign, multiplier, fraction, (bit counts), offset
    
    int sign = 0;
    int multiplier = 0;
    int fraction = 0;
    if(bits && shapetable) {
        int count = sscanf(shapetable, "%i,%i,%i,%i",&sbc, &mbc, &fbc, &offset);
        if(count == 4){
            if(fbc) {
                fraction = BMgeckoBitsToInt(fbc, fbc-1, bits);
            }
            if(mbc) {
                multiplier = BMgeckoBitsToInt(mbc, (mbc+fbc)-1, bits);
            }
            
            if(sbc) {
                sign = BMgeckoBitsToInt(sbc, (sbc+mbc+fbc)-1, bits);
            }
            
            // float tmp =
            
                // this may be pointless, there are so few of these I should
                // probably just handle each in a custom func.
        }
    }
    return flo;
}   // ends BMGeckoFloatFromCompressedBits()
#endif


/** @brief Traverse a ParseTable and a Gecko packet, assembling a BMDevMessage
 Note, this depends upon the caller to validate its parameters, ONLY CALL FROM 
 BMGeckoCopyPacketToStruct() or other function which guarantees the parameters.
 Returns the packet type or a BMGeckoParseFailures_t 
 */
static int UnpackGeckoPacket(const BMGeckoPacketParseTable* pParseTable,
                                   BMGeckoLEPacketUnion*    pPacket, // incoming, packet to parse.
                                   BMDeviceMessageUnion*    pMsg)    // outgoing struct to fill out
{
    int state = BMGeckoParseFail_SanityFailure;
    int entryIndex = 0;         // for traversing parsetable field entries
    BMGeckolatorMapEntry const* pRow = &pParseTable->entry[entryIndex];
    
    uint32_t* ppack =  pPacket->u32;
    uint32_t* punion = pMsg->u32;           // handling both pPacket and pmsg as uint32_t[]
    uint unionIndex = 0;
    
    uint spannedlongs;
    
    // loop through the translation table, one data element per line.
    while(pRow->offsetByte < GECKO_PACKET_LENGTH) { // the entry* array is terminated by an entry offset of 20
        
        uint32_t thesebits = 0;     // holds bits for this field
        uint packIndex = 0;         // into the uint32[5] or the uint8[20], as needed
        
        // First question, do our source bits span a troublesome 32 bit boundary?
        // Assumption: for any platform, pPacket has been allocated to start at an address that allows efficent
        // access to 32 bit types (probably a quadbyte boundary)
        // or the 
        
        uint firstlong = pRow->startBitCounter/32;
        uint lastlong = (pRow->startBitCounter + pRow->fieldSize - 1) / 32;
        spannedlongs = (lastlong + 1) - firstlong;
        
        // Extract bits
        if (spannedlongs == 1) {    // this can be managed in a single stroke
            thesebits = BMgeckoBitsToInt(pRow->fieldSize, pRow->startBitCounter % 32, ppack[firstlong]);
            
        } else if((spannedlongs == 2) && (pRow->fieldSize < 32) ) {
			int i;
            // copy the 1-4 bytes we know contain our bits into an intermediate long 
            BMintOrBytesUnion rowVal = {0}; // an intermediate container for bits spanning an uin32 boundary
            uint8_t* packetbytes = (uint8_t*)pPacket;
            packIndex = pRow->startBitCounter/8;
            for(i = 0; (i < 4) && (packIndex < GECKO_PACKET_LENGTH); i++, packIndex++ ) {
                rowVal.b[i] = packetbytes[packIndex];
            }
            thesebits = BMgeckoBitsToInt(pRow->fieldSize, pRow->startBitCounter % 8,rowVal.u32);
            
        } else {    // any field greater than 32 bits can't be handled directly,  
            IsSane((intptr_t)pRow->geckoler, "%s, %s, is %u bits long, but has no handler func.",
                   __func__,pRow->fieldName, pRow->fieldSize);
        }
        
        // Test endianess and possibly convert from network order to host order
        punion[unionIndex] = BMGeckoToHost(thesebits, pRow->fieldSize, pRow->littleEndian);

        if(pRow->geckoler) {                                    // does this field require special processing?
            state = (pRow->geckoler)(false, pRow, pPacket, pMsg, &unionIndex); // geckoler returns the packet type on success
            if( (state != *(uint8_t*)pPacket)) {                             // Success?
                return state;                                   // No, the function failed to parse, abandon.
            }
        }
        pRow = &pParseTable->entry[++entryIndex];
        if(pRow->offsetIntoUnion) {                     // Weird business allows us to generally ignore pRow->offsetIntoUnion
            unionIndex = pRow->offsetIntoUnion;         // in our parse tables, but if needed, it can be re-positioned
        } else {                                        // by the parsetable entry.
            unionIndex++;                               // (Didn't bother with ternary operator 'cause I needed the room :)
        }
    }
    return punion[0];
}   // ends UnpackGeckoPacket(...

    
/** @brief Traverse a ParseTable and a BMDevMessage, assembling a Gecko packet 
 Note, this depends upon the caller to validate its parameters.
 Returns the packet type or a BMGeckoParseFailures_t
 */
static int PackGeckoPacket(const BMGeckoPacketParseTable* pParseTable,
                           BMGeckoLEPacketUnion* pPacket,     // outgoing, packet to create.
                           const BMDeviceMessageUnion* pMsg)  // incoming, struct to pack as Gecko data
{
    int state = BMGeckoParseFail_SanityFailure;
    int entryIndex = 0;         // for traversing parsetable field entries
    BMGeckolatorMapEntry const* pRow = &pParseTable->entry[entryIndex];
    
    uint32_t* ppack =  pPacket->u32;
    uint32_t const* punion = pMsg->u32;           // handling both pPacket and pmsg as uint32_t[]
    uint unionIndex = 0;
    
    uint spannedlongs;
    
    // loop through the translation table, one data element per line.
    while(pRow->offsetByte < GECKO_PACKET_LENGTH) { // the entry* array is terminated by an entry offset of 20
        
        uint32_t thesebits;         // holds bits for this field
        uint firstlong = pRow->startBitCounter/32;
        uint lastlong = (pRow->startBitCounter + pRow->fieldSize - 1) / 32;
        //  uint packIndex = 0;         // into the uint32[5] or the uint8[20], as needed
                
        // Test endianess and possibly convert from host order to network order
        thesebits = BMGeckoToNetwork(punion[unionIndex], pRow->fieldSize, pRow->littleEndian);
        
        // Do our source bits span a troublesome 32 bit boundary?
        // Assumption: for any platform, pPacket has been allocated to start at an address that allows efficent
        // access to 32 bit types (probably a quadbyte boundary)
        
        spannedlongs = (lastlong + 1) - firstlong;
        
        /// TODO: this test for int spanning fields is stupid when packing,
        //  It's a loop, just think through the masking and shifting across the boundary
        
        // Extract bits
        if (spannedlongs == 1) {    // this can be managed in a single stroke
            
            BMgeckoStuffBitsIntoUInt(thesebits, pRow->fieldSize, pRow->startBitCounter % 32, &(ppack[firstlong]));
            state = pMsg->messagetype;
            
        } else if((spannedlongs == 2) && (pRow->fieldSize < 32) ) {
            
            // which of our bits go into which word?
            uint32_t lastwidth = (pRow->startBitCounter % 32 +  pRow->fieldSize) % 32;
            uint32_t firstwidth = pRow->fieldSize - lastwidth;
            uint32_t somebits = thesebits >> firstwidth;
            BMgeckoStuffBitsIntoUInt(thesebits, firstwidth, pRow->startBitCounter % 32, &(ppack[firstlong]));
            BMgeckoStuffBitsIntoUInt(somebits, lastwidth, 0, &(ppack[lastlong]));
            state = pMsg->messagetype;
            
        } else {    // any field greater than 32 bits can't be handled directly,
            if(pRow->geckoler) {  // does this field have special processing?
                state = (pRow->geckoler)(true, pRow, pPacket, pMsg, unionIndex);
            } else {
                IsSane((intptr_t)pRow->geckoler, "%s, %s, is %u bits long, but has no handler func.",
                   __func__,pRow->fieldName, pRow->fieldSize);
                state = BMGeckoParseFail_SanityFailure;
            }
        }
        if(state != pMsg->messagetype) {                        // Success?
            return state;                                   // No, the function failed to parse, abandon.
        }

        pRow = &pParseTable->entry[++entryIndex];
        if(pRow->offsetIntoUnion) {                     // Weird business allows us to generally ignore pRow->offsetIntoUnion
            unionIndex = pRow->offsetIntoUnion;         // in our parse tables, but if needed, it can be re-positioned
        } else {                                        // by the parsetable entry.
            unionIndex++;                               // (Didn't bother with ternary operator 'cause I needed the room :)
        }
    }
    return state;
}   // ends PackGeckoPacket(...



/* MARK: Public functions */

/** @brief turn a Gecko packet into directly accesible structures
 Will either calloc and populate a BMDeviceMessageUnion, passing it back
 in *ppDevMessage, or clear and populate an existing one, depending on
 whether ppDevMessage is nil or valid.
 Does sanity checking and memory allocation, then passes the actual work to GeckolatePacket()
 TODO: BMGeckoCopyPacketToStruct has too many exit points, reorganize
 it with a central success exit and a bottom fail exit
 */
int BMGeckoCopyPacketToStruct(BMGeckoLEPacketUnion* pPacket,         // As delivered over BT
                              BMDeviceMessageUnion** ppDevMessage)    // Filled out with with data from packet
{
    if(pPacket && ppDevMessage) {                       // vaguely sane arguments?
        uint8_t type = *((uint8_t*)pPacket) & GECKO_PACKET_TYPE_BITS;  // We need the type, but don't want the write bit.
        
        // Note that this technique partly depends on the types not being sparse,
        // but even worst case, with the type as byte, we'll only need 127 pointers in the table
        if((type > GEC_PACKET_CLEARED) && (type < GEC_TYPE_TAIL_FENCE)) {   // reasonable type?
            // Pick up the table that will drive the parsing for this packet
            BMGeckoPacketParseTable const* pPacketTable = BMGeckoPackets_1_0_MasterParseTable.entry[type];
            
            // Establish and clear our output buffer, failing if no memory
            if(*ppDevMessage) {
                bzero(*ppDevMessage,sizeof(BMDeviceMessageUnion));
            } else {
                
                if(!(*ppDevMessage = calloc(1,sizeof(BMDeviceMessageUnion)))) {
                    return BMGeckoParseFail_NoMemory; // Malloc failure exit.
                }
            }
            
            
            //BMGeckoPackets const*  master = &BMGeckoPackets_1_0_MasterParseTable;
            // Table describing one packet type
            //BMGeckoPacketParseTable const* pPacketTable = master->entry[type];
            
            
            if(pPacketTable){
                // Good exit for this func, though UnpackGeckoPacket might return a fail.
                return UnpackGeckoPacket(pPacketTable, pPacket, *ppDevMessage);
            } else {
                return BMGeckoParseFail_UnknownPacketType; // // Can't parse, failure exit.
            }
        } else {
            return BMGeckoParseFail_UnknownPacketType;
        }
    }
    return BMGeckoParseFail_BadParameter;                                           
}   // ends BMGeckoCopyPacketToStruct(...




/** @brief assemble a packet from a passed in Device Message union
 Will either calloc and populate a packet, passing it back
 in *ppPacket, or clear and populate an existing one, depending on
 whether ppPacket is nil or valid.
 */

int BMGeckoCopyStructToPacket(const BMDeviceMessageUnion*  pDevMessage,    // created by the caller.
                              BMGeckoLEPacketUnion**      ppPacket)      // ready to deliver over BT
{
    if(ppPacket && pDevMessage) {                         // vaguely sane arguments?
        
        uint unionIndex = 0;   // treating pmsg as uint32_t[]
        uint32_t* punion = (uint32_t*)pDevMessage;
        
        uint8_t type = (uint8_t)punion[unionIndex] & GECKO_PACKET_TYPE_BITS;  // We need the type, but don't want the write bit for lookup.

        if((type > GEC_PACKET_CLEARED) && (type < GEC_TYPE_TAIL_FENCE)) {   // reasonable type?

            // Pick up the table that will drive the parsing for this packet c
            const BMGeckoPacketParseTable* pPacketTable = BMGeckoPackets_1_0_MasterParseTable.entry[type];

            // TODO: test the type and refuse to convert types that will never be sent.
            
            // Establish and clear our output buffer, failing if no memory
            if(*ppPacket) {
                bzero(*ppPacket,sizeof(BMGeckoLEPacketUnion));
            } else {
                
                if(!(*ppPacket = calloc(1,sizeof(BMGeckoLEPacketUnion)))) {
                    return BMGeckoParseFail_NoMemory;               // Malloc failure exit.
                }
            }
            
            if(pPacketTable){
                // Good exit for this func, though PackGeckoPacket might return a fail.
                return PackGeckoPacket(pPacketTable, *ppPacket, pDevMessage);
            } else {
                return BMGeckoParseFail_UnknownPacketType; // // Can't parse, failure exit.
            }
        } else {
            return BMGeckoParseFail_UnknownPacketType;
        }

    }
    return BMGeckoParseFail_BadParameter;
}   // ends BMGeckoCopyStructToPacket(...
