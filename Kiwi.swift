//
//  Kiwi.swift
//  Kiwi
//
//  Created by Jarrod Norwell on 2/9/2026.
//

import Foundation

public enum KiwiButton : UInt32 {
    case a = 0x1
    case b = 0x2
    case select = 0x4
    case start = 0x8
    case right = 0x10
    case left = 0x20
    case up = 0x40
    case down = 0x80
    
    var uint32: UInt32 { rawValue }
}

public class KiwiCommon {
    public init() {}
    
    public static var documentDirectoryURL: URL? {
        FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first
    }
    
    public static var kiwiDirectoryURL: String? {
        if let documentDirectoryURL {
            documentDirectoryURL.appending(component: "Kiwi").path
        } else {
            nil
        }
    }
}

public actor KiwiSystem {
    private var fileManager: FileManager = .default
    
    public init() {}
    
    public func printAbout() {
        kiwi.print_about()
    }
    
    public func initializePaths() {
        kiwi.initialize_paths()
    }
    
    public func initializeSystem() {
        kiwi.initialize_system()
    }
    
    public func destroySystem() {
        kiwi.destroy_system()
    }
    
    public func insertDisc(at url: URL) {
        kiwi.insert_disc(std.string(url.path))
    }
    
    public func set(change: Bool = false, isRunning: Bool = false) {
        if change {
            running = isRunning
        }
    }
    
    public var running: Bool {
        get {
            kiwi.is_running()
        }
        set {
            kiwi.is_running(true, newValue)
        }
    }
    
    public func set(change: Bool = false, isPaused: Bool = false) {
        if change {
            paused = isPaused
        }
    }
    
    public var paused: Bool {
        get {
            kiwi.is_paused()
        }
        set {
            kiwi.is_paused(true, newValue)
        }
    }
    
    
    public func start() {
        kiwi.start()
    }
    
    public func stop() {
        kiwi.stop()
    }
    
    
    public var framebufferHeight: Int32 {
        kiwi.framebuffer_height()
    }
    
    public var framebufferWidth: Int32 {
        kiwi.framebuffer_width()
    }
    
    
    public nonisolated func press(button: KiwiButton) {
        kiwi.press_button(button.uint32)
    }
    
    public nonisolated func release(button: KiwiButton) {
        kiwi.release_button(button.uint32)
    }
    
    
    public nonisolated func audioBuffer(callback: kiwi.AudioVideoBufferCallback) {
        kiwi.audio_buffer_callback(callback)
    }
    
    public nonisolated func videoBuffer(callback: kiwi.AudioVideoBufferCallback) {
        kiwi.video_buffer_callback(callback)
    }
    
    
    public func setContext(context: UnsafeMutableRawPointer) {
        kiwi.set_context(context)
    }
    
    
    public func setSetting<T>(setting: kiwi.SETTING, value: T) {
        switch value {
        case let boolSetting as Bool:
            kiwi.set_setting(setting, boolSetting)
        default:
            break
        }
    }
    
    
    public nonisolated func boxartURLString(for url: URL) -> String? {
        let title: String = url.deletingPathExtension().lastPathComponent
        
        let systemName: String = switch url.pathExtension.localizedLowercase {
        case "gb":
            "Game Boy"
        case "gbc":
            "Game Boy Color"
        default:
            "Game Boy"
        }
        
        return "https://raw.githubusercontent.com/libretro/libretro-thumbnails/refs/heads/master/Nintendo - \(systemName)/Named_Boxarts/\(title).png"
    }
}
