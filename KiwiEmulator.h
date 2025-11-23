//
//  KiwiEmulator.h
//  Kiwi
//
//  Created by Jarrod Norwell on 12/8/2025.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface KiwiEmulator : NSObject
@property (nonatomic, strong, nullable) void (^ab) (uint32_t*, size_t);
@property (nonatomic, strong, nullable) void (^fb) (uint32_t*, NSInteger, NSInteger);

+(KiwiEmulator *) sharedInstance NS_SWIFT_NAME(shared());

-(void) insertCartridge:(NSURL *)url NS_SWIFT_NAME(insert(_:));

-(void) start;
-(void) stop;
-(BOOL) isPaused;
-(void) pause:(BOOL)pause;

-(NSString *) title:(NSURL *)url NS_SWIFT_NAME(title(from:));

-(void) button:(uint8_t)button player:(int)player pressed:(BOOL)pressed;

-(BOOL) loadState;
-(BOOL) saveState;

-(void) load:(NSURL *)url;
-(void) save:(NSURL *)url;
@end

NS_ASSUME_NONNULL_END
