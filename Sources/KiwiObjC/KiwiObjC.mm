//
//  KiwiObjC.mm
//
//
//  Created by Jarrod Norwell on 21/2/2024.
//

#import "KiwiObjC.h"

#include <hqx/hqx.h>
#include <xbrz/xbrz.h>

#include "emulator.hpp"
#include "settings.hpp"

std::unique_ptr<Emulator> kiwiEmulator;

@implementation KiwiObjC
-(KiwiObjC *) init {
    if (self = [super init]) {
        hqxInit();
        
        NSURL *documentsDirectory = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] firstObject];
        NSURL *grapeDirectory = [documentsDirectory URLByAppendingPathComponent:@"Kiwi"];
        
        if (!KiwiSettings::load([[[grapeDirectory URLByAppendingPathComponent:@"config"]
                              URLByAppendingPathComponent:@"config.ini"].path UTF8String])) {
            KiwiSettings::save();
        }
    } return self;
}

+(KiwiObjC *) sharedInstance {
    static dispatch_once_t onceToken;
    static KiwiObjC *sharedInstance = NULL;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[self alloc] init];
    });
    return sharedInstance;
}

-(void) insertGame:(NSURL *)url {
    kiwiEmulator = std::make_unique<Emulator>([url.path UTF8String]);
    [self reset];
}

-(void) reset {
    kiwiEmulator->reset();
}

-(void) step {
    kiwiEmulator->step();
}

-(uint32_t *) videoBuffer {
    static std::vector<uint32_t> framebuffer(256 * 240);
    memcpy(framebuffer.data(), kiwiEmulator->get_screen_buffer(), 256 * 240 * sizeof(uint32_t));
    
    switch ([self useUpscalingFilter]) {
        case 0: {
            const auto upscalingFactor = [self useUpscalingFactor];
            static std::vector<uint32_t> upscaled((256 * upscalingFactor) * (240 * upscalingFactor));
            switch (upscalingFactor) {
                case 2:
                default:
                    hq2x_32(framebuffer.data(), upscaled.data(), 256, 240);
                    break;
                case 3:
                    hq3x_32(framebuffer.data(), upscaled.data(), 256, 240);
                    break;
                case 4:
                    hq4x_32(framebuffer.data(), upscaled.data(), 256, 240);
                    break;
            }
            return upscaled.data();
        }
        case 1: {
            const auto upscalingFactor = [self useUpscalingFactor];
            static std::vector<uint32_t> upscaled((256 * upscalingFactor) * (240 * upscalingFactor));
            xbrz::scale(upscalingFactor, framebuffer.data(), upscaled.data(), 256, 240, xbrz::ColorFormat::ARGB);
            return upscaled.data();
        }
        case -1:
        default:
            return framebuffer.data();
    }
}

-(void) virtualControllerButtonDown:(uint8_t)button {
    kiwiEmulator->get_controller(0)[0] |= button;
}

-(void) virtualControllerButtonUp:(uint8_t)button {
    kiwiEmulator->get_controller(0)[0] &= ~button;
}

-(int) useUpscalingFilter {
    return KiwiSettings::upscalingFilter;
}

-(void) setUpscalingFilter:(int)upscalingFilter {
    KiwiSettings::upscalingFilter = upscalingFilter;
    KiwiSettings::save();
}

-(int) useUpscalingFactor {
    return KiwiSettings::upscalingFactor;
}

-(void) setUpscalingFactor:(int)upscalingFactor {
    KiwiSettings::upscalingFactor = upscalingFactor;
    KiwiSettings::save();
}
@end
