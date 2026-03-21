//
//  KiwiEmulator.mm
//  Kiwi
//
//  Created by Jarrod Norwell on 12/8/2025.
//

#import "KiwiEmulator.h"
#import "Kiwi-Swift.h"

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

std::jthread thread;
gambatte::GB gameboy{};
uint32_t* fb;
uint32_t* ab;

std::atomic<bool> paused, running;
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


-(void) insertCartridge:(NSURL *)url { // TODO: rewrite
    gameboy.setInputGetter(&GetInput);
    gameboy.load([url.path UTF8String], gambatte::GB::MULTICART_COMPAT);
    gameboy.setSaveDir(std::string{[KiwiCommon.statesDirectoryURL.path UTF8String]});
    
    fb = new uint32_t[160 * 144 * 4];
    ab = new uint32_t[2064 * 2 * 4];
}


-(void) pause {
    paused.store(true);
}

-(void) start { // TODO: rewrite
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
            while (gameboy.runFor(fb, 160, ab, samples) == -1)
                if (auto buffer = [[KiwiEmulator sharedInstance] audioCallback])
                    buffer(ab, samples);
            
            if (auto buffer = [[KiwiEmulator sharedInstance] videoCallback])
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
    
    delete [] ab;
    delete [] fb;
    
    paused.store(false);
    running.store(false);
}

-(void) unpause {
    paused.store(false);
    cv.notify_all();
}


-(BOOL) isPaused {
    return paused.load();
}

-(BOOL) isRunning {
    return running.load();
}


-(void) press:(uint32_t)button {
    activeInput[0] |= button;
}

-(void) release:(uint32_t)button {
    activeInput[0] &= ~button;
}


-(void) load:(NSURL *)url {
    gameboy.loadState(std::string{[url.path UTF8String]});
}

-(void) save:(NSURL *)url {
    gameboy.saveState(nullptr, 0, std::string{[url.path UTF8String]});
}


-(NSString *) title:(NSURL *)url {
    auto result = gameboy.load(std::string{[url.path UTF8String]}, gambatte::GB::MULTICART_COMPAT);
    return [NSString stringWithCString:gameboy.romTitle().c_str() encoding:NSUTF8StringEncoding];
}
@end
