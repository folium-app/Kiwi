//
//  KiwiEmulator.mm
//  Kiwi
//
//  Created by Jarrod Norwell on 12/8/2025.
//

#import "KiwiEmulator.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#include <gambatte/gambatte.h>
#include <gambatte/common/ringbuffer.h>
#include <gambatte/common/rateest.h>
#include <gambatte/common/resample/resampler.h>
#include <gambatte/common/resample/resamplerinfo.h>
#include <gambatte/common/scoped_ptr.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_AudioStream* stream = nullptr;

std::jthread thread;
gambatte::GB gameboy{};
uint32_t* fb;
uint32_t* ab;

std::atomic<bool> paused;
std::mutex mutex;
std::condition_variable_any cv;

uint32_t activeInput[8];

class GetInput : public gambatte::InputGetter {
public:
    unsigned operator()() {
        return activeInput[0];
    }
} static GetInput;

@implementation KiwiEmulator
+(KiwiEmulator *) sharedInstance {
    static KiwiEmulator *sharedInstance = NULL;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[self alloc] init];
    });
    return sharedInstance;
}

-(void) insertCartridge:(NSURL *)url {
    NSURL* saveDirectoryURL = [[[[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] firstObject] URLByAppendingPathComponent:@"Kiwi"] URLByAppendingPathComponent:@"states"];
    
    gameboy.setInputGetter(&GetInput);
    gameboy.load([url.path UTF8String], gambatte::GB::MULTICART_COMPAT);
    gameboy.setSaveDir({[saveDirectoryURL.path UTF8String]});
    
    fb = new uint32_t[160 * 144 * 4];
    ab = new uint32_t[2064 * 4 * 4];
}

-(void) start {
    thread = std::jthread([&](std::stop_token token) {
        using namespace std::chrono;
        
        const auto frameDuration = duration<double>(1.0 / 60.0);
        
        while (!token.stop_requested()) {
            {
                std::unique_lock lock(mutex);
                cv.wait(lock, token, []() {
                    return !paused.load();
                });
                
                if (token.stop_requested())
                    break;
            }
            
            auto frameStart = steady_clock::now();
            
            size_t samples = 2064;
            while (gameboy.runFor(fb, 160, ab, samples) == -1) {
                if (auto buffer = [[KiwiEmulator sharedInstance] ab])
                    buffer(ab, samples);
            };
            
            if (auto buffer = [[KiwiEmulator sharedInstance] fb])
                buffer(fb, 160, 144);

            // Limit FPS
            auto frameEnd = steady_clock::now();
            auto elapsed = frameEnd - frameStart;
            if (elapsed < frameDuration)
                std::this_thread::sleep_for(frameDuration - elapsed);
        }
    });
}

-(void) stop {
    thread.request_stop();
    if (thread.joinable())
        thread.join();
    
    SDL_DestroyAudioStream(stream);
    
    delete [] ab;
    delete [] fb;
    
    paused.store(false);
}

-(BOOL) isPaused {
    return paused.load();
}

-(void) pause:(BOOL)pause {
    if (pause)
        paused.store(true);
    else {
        paused.store(false);
        cv.notify_all();
    }
}

-(NSString *) title:(NSURL *)url {
    gameboy.load([url.path UTF8String]);
    return [NSString stringWithCharacters:(const unichar*)gameboy.romTitle().c_str() length:gameboy.romTitle().length()];
}

-(void) button:(uint8_t)button player:(int)player pressed:(BOOL)pressed {
    if (pressed)
        activeInput[0] |= button;
    else
        activeInput[0] &= ~button;
}

-(BOOL) loadState {
    gameboy.selectState(0);
    return gameboy.loadState();
}

-(BOOL) saveState {
    gameboy.selectState(0);
    return gameboy.saveState(nullptr, 0);
}

-(void) load:(NSURL *)url {
    gameboy.loadState([url.path UTF8String]);
}

-(void) save:(NSURL *)url {
    gameboy.saveState(nullptr, 0, [url.path UTF8String]);
}
@end
