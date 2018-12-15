// Generated by the protocol buffer compiler.  DO NOT EDIT!

#import "ProtocolBuffers.h"

// @@protoc_insertion_point(imports)

@class RawActivity;
@class RawActivityBuilder;
@class RawActivityUploadRequest;
@class RawActivityUploadRequestBuilder;
#ifndef __has_feature
  #define __has_feature(x) 0 // Compatibility with non-clang compilers.
#endif // __has_feature

#ifndef NS_RETURNS_NOT_RETAINED
  #if __has_feature(attribute_ns_returns_not_retained)
    #define NS_RETURNS_NOT_RETAINED __attribute__((ns_returns_not_retained))
  #else
    #define NS_RETURNS_NOT_RETAINED
  #endif
#endif


@interface RawActivityUploadRequestRoot : NSObject {
}
+ (PBExtensionRegistry*) extensionRegistry;
+ (void) registerAllExtensions:(PBMutableExtensionRegistry*) registry;
@end

@interface RawActivityUploadRequest : PBGeneratedMessage {
@private
  NSMutableArray * rawActivityArray;
}
@property (readonly, strong) NSArray * rawActivity;
- (RawActivity*)rawActivityAtIndex:(NSUInteger)index;

+ (RawActivityUploadRequest*) defaultInstance;
- (RawActivityUploadRequest*) defaultInstance;

- (BOOL) isInitialized;
- (void) writeToCodedOutputStream:(PBCodedOutputStream*) output;
- (RawActivityUploadRequestBuilder*) builder;
+ (RawActivityUploadRequestBuilder*) builder;
+ (RawActivityUploadRequestBuilder*) builderWithPrototype:(RawActivityUploadRequest*) prototype;
- (RawActivityUploadRequestBuilder*) toBuilder;

+ (RawActivityUploadRequest*) parseFromData:(NSData*) data;
+ (RawActivityUploadRequest*) parseFromData:(NSData*) data extensionRegistry:(PBExtensionRegistry*) extensionRegistry;
+ (RawActivityUploadRequest*) parseFromInputStream:(NSInputStream*) input;
+ (RawActivityUploadRequest*) parseFromInputStream:(NSInputStream*) input extensionRegistry:(PBExtensionRegistry*) extensionRegistry;
+ (RawActivityUploadRequest*) parseFromCodedInputStream:(PBCodedInputStream*) input;
+ (RawActivityUploadRequest*) parseFromCodedInputStream:(PBCodedInputStream*) input extensionRegistry:(PBExtensionRegistry*) extensionRegistry;
@end

@interface RawActivityUploadRequestBuilder : PBGeneratedMessageBuilder {
@private
  RawActivityUploadRequest* result;
}

- (RawActivityUploadRequest*) defaultInstance;

- (RawActivityUploadRequestBuilder*) clear;
- (RawActivityUploadRequestBuilder*) clone;

- (RawActivityUploadRequest*) build;
- (RawActivityUploadRequest*) buildPartial;

- (RawActivityUploadRequestBuilder*) mergeFrom:(RawActivityUploadRequest*) other;
- (RawActivityUploadRequestBuilder*) mergeFromCodedInputStream:(PBCodedInputStream*) input;
- (RawActivityUploadRequestBuilder*) mergeFromCodedInputStream:(PBCodedInputStream*) input extensionRegistry:(PBExtensionRegistry*) extensionRegistry;

- (NSMutableArray *)rawActivity;
- (RawActivity*)rawActivityAtIndex:(NSUInteger)index;
- (RawActivityUploadRequestBuilder *)addRawActivity:(RawActivity*)value;
- (RawActivityUploadRequestBuilder *)setRawActivityArray:(NSArray *)array;
- (RawActivityUploadRequestBuilder *)clearRawActivity;
@end

@interface RawActivity : PBGeneratedMessage {
@private
  BOOL hasCharging_:1;
  BOOL hasNonUse_:1;
  BOOL hasScore_:1;
  BOOL hasCalories_:1;
  BOOL hasStrides_:1;
  BOOL hasDistance_:1;
  BOOL hasTime_:1;
  BOOL hasType_:1;
  BOOL hasIntensity_:1;
  BOOL charging_:1;
  BOOL nonUse_:1;
  Float32 score;
  Float32 calories;
  Float32 strides;
  Float32 distance;
  SInt32 time;
  SInt32 type;
  SInt32 intensity;
}
- (BOOL) hasTime;
- (BOOL) hasType;
- (BOOL) hasIntensity;
- (BOOL) hasScore;
- (BOOL) hasCalories;
- (BOOL) hasStrides;
- (BOOL) hasCharging;
- (BOOL) hasNonUse;
- (BOOL) hasDistance;
@property (readonly) SInt32 time;
@property (readonly) SInt32 type;
@property (readonly) SInt32 intensity;
@property (readonly) Float32 score;
@property (readonly) Float32 calories;
@property (readonly) Float32 strides;
- (BOOL) charging;
- (BOOL) nonUse;
@property (readonly) Float32 distance;

+ (RawActivity*) defaultInstance;
- (RawActivity*) defaultInstance;

- (BOOL) isInitialized;
- (void) writeToCodedOutputStream:(PBCodedOutputStream*) output;
- (RawActivityBuilder*) builder;
+ (RawActivityBuilder*) builder;
+ (RawActivityBuilder*) builderWithPrototype:(RawActivity*) prototype;
- (RawActivityBuilder*) toBuilder;

+ (RawActivity*) parseFromData:(NSData*) data;
+ (RawActivity*) parseFromData:(NSData*) data extensionRegistry:(PBExtensionRegistry*) extensionRegistry;
+ (RawActivity*) parseFromInputStream:(NSInputStream*) input;
+ (RawActivity*) parseFromInputStream:(NSInputStream*) input extensionRegistry:(PBExtensionRegistry*) extensionRegistry;
+ (RawActivity*) parseFromCodedInputStream:(PBCodedInputStream*) input;
+ (RawActivity*) parseFromCodedInputStream:(PBCodedInputStream*) input extensionRegistry:(PBExtensionRegistry*) extensionRegistry;
@end

@interface RawActivityBuilder : PBGeneratedMessageBuilder {
@private
  RawActivity* result;
}

- (RawActivity*) defaultInstance;

- (RawActivityBuilder*) clear;
- (RawActivityBuilder*) clone;

- (RawActivity*) build;
- (RawActivity*) buildPartial;

- (RawActivityBuilder*) mergeFrom:(RawActivity*) other;
- (RawActivityBuilder*) mergeFromCodedInputStream:(PBCodedInputStream*) input;
- (RawActivityBuilder*) mergeFromCodedInputStream:(PBCodedInputStream*) input extensionRegistry:(PBExtensionRegistry*) extensionRegistry;

- (BOOL) hasTime;
- (SInt32) time;
- (RawActivityBuilder*) setTime:(SInt32) value;
- (RawActivityBuilder*) clearTime;

- (BOOL) hasType;
- (SInt32) type;
- (RawActivityBuilder*) setType:(SInt32) value;
- (RawActivityBuilder*) clearType;

- (BOOL) hasIntensity;
- (SInt32) intensity;
- (RawActivityBuilder*) setIntensity:(SInt32) value;
- (RawActivityBuilder*) clearIntensity;

- (BOOL) hasScore;
- (Float32) score;
- (RawActivityBuilder*) setScore:(Float32) value;
- (RawActivityBuilder*) clearScore;

- (BOOL) hasCalories;
- (Float32) calories;
- (RawActivityBuilder*) setCalories:(Float32) value;
- (RawActivityBuilder*) clearCalories;

- (BOOL) hasStrides;
- (Float32) strides;
- (RawActivityBuilder*) setStrides:(Float32) value;
- (RawActivityBuilder*) clearStrides;

- (BOOL) hasCharging;
- (BOOL) charging;
- (RawActivityBuilder*) setCharging:(BOOL) value;
- (RawActivityBuilder*) clearCharging;

- (BOOL) hasNonUse;
- (BOOL) nonUse;
- (RawActivityBuilder*) setNonUse:(BOOL) value;
- (RawActivityBuilder*) clearNonUse;

- (BOOL) hasDistance;
- (Float32) distance;
- (RawActivityBuilder*) setDistance:(Float32) value;
- (RawActivityBuilder*) clearDistance;
@end


// @@protoc_insertion_point(global_scope)
