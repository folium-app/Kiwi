//
//  KiwiEmulator.h
//  Kiwi
//
//  Created by Jarrod Norwell on 12/8/2025.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface KiwiEmulator : NSObject
@property (nonatomic, strong, nullable) void (^audioCallback) (uint32_t*, NSInteger);
@property (nonatomic, strong, nullable) void (^videoCallback) (uint32_t*, NSInteger, NSInteger);

+(KiwiEmulator *) sharedInstance NS_SWIFT_NAME(shared());

-(void) insertCartridge:(NSURL *)url NS_SWIFT_NAME(insert(cartridge:));

-(void) pause;
-(void) start;
-(void) stop;
-(void) unpause;

-(BOOL) isPaused NS_SWIFT_NAME(paused());
-(BOOL) isRunning NS_SWIFT_NAME(running());

-(void) press:(uint32_t)button;
-(void) release:(uint32_t)button;

-(void) load:(NSURL *)url NS_SWIFT_NAME(load(state:));
-(void) save:(NSURL *)url NS_SWIFT_NAME(save(state:));

-(NSString *) title:(NSURL *)url NS_SWIFT_NAME(title(cartridge:));
@end

NS_ASSUME_NONNULL_END
