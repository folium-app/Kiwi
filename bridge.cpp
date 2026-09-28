//
//  bridge.cpp
//  Kiwi
//
//  Created by Jarrod Norwell on 2/7/2026.
//

#include "bridge.h"
#include "mesence.h"

#include "Shared/EmuSettings.h"
#include "Shared/MessageManager.h"
#include "Shared/SaveStateManager.h"
#include "Utilities/FolderUtilities.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <thread>

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "Kiwi-Swift.h"
using namespace Kiwi;

struct cntnr_k {
    KiwiCommon kiwiCommon{KiwiCommon::init()};
    KiwiSystem kiwiSystem{KiwiSystem::init()};
    
    std::unique_ptr<Emulator> emulator;
    std::unique_ptr<GBInput> input;
    std::unique_ptr<iOSRenderer> renderer;
    std::unique_ptr<iOSSink> sink;
    
    GameboyConfig config;
    
    std::condition_variable_any cv;
    std::mutex mutex;
    std::atomic<bool> paused, running;
    std::jthread thread;
    
    uint32_t height, width;
    
    std::filesystem::path kiwi_path, debugger_path, firmware_path;
    std::filesystem::path hd_packs_path, recent_games_path, saves_path;
    std::filesystem::path save_states_path, screenshots_path, system_data_path;
} cntnr_k;

void kiwi::print_about(void) {
    printf("Welcome to Kiwi\n");
    printf("Game Boy emulation provided by MesenCE\n");
}

void kiwi::initialize_paths(void) {
    auto kiwiDirectoryURL{cntnr_k.kiwiCommon.getKiwiDirectoryURL()};
    if (kiwiDirectoryURL.isSome()) {
        auto kiwi_path{std::filesystem::path{kiwiDirectoryURL.get()}};
        
        cntnr_k.kiwi_path = kiwi_path;
        cntnr_k.debugger_path = kiwi_path / "debugger";
        cntnr_k.firmware_path = kiwi_path / "firmware";
        cntnr_k.hd_packs_path = kiwi_path / "hd_packs";
        cntnr_k.recent_games_path = kiwi_path / "recent_games";
        cntnr_k.saves_path = kiwi_path / "saves";
        cntnr_k.save_states_path = kiwi_path / "save_states";
        cntnr_k.screenshots_path = kiwi_path / "screenshots";
        cntnr_k.system_data_path = kiwi_path / "system_data";
    }
}

void kiwi::initialize_system(void) {
    auto mm{std::make_unique<iOSMessageManager>()};
    MessageManager::SetOptions(false, true);
    MessageManager::RegisterMessageManager(mm.get());
    
    cntnr_k.emulator = std::make_unique<Emulator>();
    cntnr_k.emulator->Initialize(false);
    
    cntnr_k.input = std::make_unique<GBInput>();
    cntnr_k.renderer = std::make_unique<iOSRenderer>(cntnr_k.emulator, 144, 160);
    cntnr_k.sink = std::make_unique<iOSSink>(cntnr_k.emulator, 48000);
    
    cntnr_k.config = cntnr_k.emulator->GetSettings()->GetGameboyConfig();
    cntnr_k.config.Controller.Type = ControllerType::GameboyController;
    cntnr_k.emulator->GetSettings()->SetGameboyConfig(cntnr_k.config);
}


void kiwi::destroy_system(void) {
    kiwi::initialize_system();
}


void kiwi::insert_disc(std::string path) {
    FolderUtilities::SetHomeFolder(cntnr_k.kiwi_path.string());
    FolderUtilities::SetFolderOverrides({}, {}, {}, cntnr_k.system_data_path);
    
    cntnr_k.emulator->LoadRom({path}, {});
    cntnr_k.emulator->RegisterInputProvider(cntnr_k.input.get());
}


bool kiwi::is_paused(bool change, bool set_paused) {
    if (change)
        cntnr_k.paused.store(set_paused);
    
    if (change)
        set_paused ? cntnr_k.emulator->Pause() : cntnr_k.emulator->Resume();
    
    if (change && !set_paused)
        cntnr_k.cv.notify_one();
    
    return cntnr_k.paused.load();
}

bool kiwi::is_running(bool change, bool set_running) {
    if (change)
        cntnr_k.running.store(set_running);
    return cntnr_k.running.load();
}


void kiwi::start(void) {
    cntnr_k.thread = std::jthread([&](std::stop_token token) {
        using namespace std::chrono;
        
        const auto frameDuration = duration<double>(1.0 / 60.0);
        
        while (!token.stop_requested()) {
            {
                std::unique_lock lock(cntnr_k.mutex);
                cntnr_k.cv.wait(lock, token, []() {
                    return !cntnr_k.paused.load();
                });
                
                if (token.stop_requested())
                    break;
            }
            
            auto frameStart = steady_clock::now();
            
            std::vector<uint32_t> data{0};
            if (cntnr_k.renderer->GetFrameIfReady(data, cntnr_k.height, cntnr_k.width))
                kiwi::video_callback(kiwi::context, data.data(), 0);

            // Limit FPS
            auto frameEnd = steady_clock::now();
            auto elapsed = frameEnd - frameStart;
            if (elapsed < frameDuration)
                std::this_thread::sleep_for(frameDuration - elapsed);
        }
    });
}

void kiwi::stop(void) {
    cntnr_k.emulator->Stop(false, true);
    
    cntnr_k.thread.request_stop();
    if (cntnr_k.thread.joinable())
        cntnr_k.thread.join();
    
    cntnr_k.paused.store(false);
    cntnr_k.running.store(false);
}


int kiwi::framebuffer_height(void) {
    return cntnr_k.height;
}

int kiwi::framebuffer_width(void) {
    return cntnr_k.width;
}


void kiwi::audio_buffer_callback(kiwi::AudioVideoBufferCallback callback) {
    kiwi::audio_callback = callback;
}

void kiwi::video_buffer_callback(kiwi::AudioVideoBufferCallback callback) {
    kiwi::video_callback = callback;
}


void kiwi::press_button(uint32_t button) {
    cntnr_k.input->keys |= button;
}

void kiwi::release_button(uint32_t button) {
    cntnr_k.input->keys &= ~button;
}


void kiwi::set_context(void* context) {
    kiwi::context = context;
}

void kiwi::set_setting(SETTING setting, bool value) {
    cntnr_k.config = cntnr_k.emulator->GetSettings()->GetGameboyConfig();
    
    switch (setting) {
        case SETTING::ADJUST_COLOURS:
            cntnr_k.config.GbcAdjustColors = value;
            break;
        case SETTING::BLEND_FRAMES:
            cntnr_k.config.BlendFrames = value;
            break;
    }
    
    cntnr_k.emulator->GetSettings()->SetGameboyConfig(cntnr_k.config);
}


bool kiwi::save_state_exists(int index) {
    if (const auto& save_state_manager = cntnr_k.emulator->GetSaveStateManager()) {
        const auto& path{save_state_manager->GetSaveStatePath(index)};
        return std::filesystem::exists(path) && std::filesystem::file_size(path) > 0;
    } return false;
}

std::string kiwi::save_state_path(int index) {
    if (const auto& save_state_manager = cntnr_k.emulator->GetSaveStateManager()) {
        return save_state_manager->GetSaveStatePath(index);
    } return {};
}

void kiwi::load_state(int index) {
    if (const auto& save_state_manager = cntnr_k.emulator->GetSaveStateManager())
        save_state_manager->LoadState(index);
}

void kiwi::save_state(int index) {
    if (const auto& save_state_manager = cntnr_k.emulator->GetSaveStateManager())
        save_state_manager->SaveState(index);
}
