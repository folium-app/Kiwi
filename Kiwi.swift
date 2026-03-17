//
//  Kiwi.swift
//  Kiwi
//
//  Created by Jarrod Norwell on 12/8/2025.
//

import Foundation

@objcMembers
public class KiwiCommon : NSObject {
    public static var documentDirectoryURL: URL? {
        FileManager.default.urls(for: .documentDirectory, in: .userDomainMask).first
    }
    
    public static var kiwiDirectoryURL: URL? {
        if let documentDirectoryURL {
            documentDirectoryURL.appending(component: "Kiwi")
        } else {
            nil
        }
    }
    
    public static var statesDirectoryURL: URL? {
        if let kiwiDirectoryURL {
            kiwiDirectoryURL.appending(component: "states")
        } else {
            nil
        }
    }
}

public actor Kiwi {
    public let emulator: KiwiEmulator = KiwiEmulator.shared()
    
    public init() {}
    
    
    public func insert(cartridge: URL) {
        emulator.insert(cartridge: cartridge)
    }
    
    
    public func pause() {
        emulator.pause()
    }
    
    public func start() {
        emulator.start()
    }
    
    public func stop() {
        emulator.stop()
    }
    
    public func unpause() {
        emulator.unpause()
    }
    
    
    public var paused: Bool {
        emulator.paused()
    }
    
    public var running: Bool {
        emulator.running()
    }
    
    
    public func press(button: UInt32) {
        emulator.press(button)
    }
    
    public func release(button: UInt32) {
        emulator.release(button)
    }
    
    
    public func load(state: URL) {
        emulator.load(state: state)
    }
    
    public func save(state: URL) {
        emulator.save(state: state)
    }
    
    
    public func audioCallback(output: @escaping (UnsafeMutablePointer<UInt32>, Int) -> Void) {
        emulator.audioCallback = output
    }
    
    public func videoCallback(output: @escaping (UnsafeMutablePointer<UInt32>, Int, Int) -> Void) {
        emulator.videoCallback = output
    }
    
    
    public func title(cartridge: URL) -> String {
        emulator.title(cartridge: cartridge)
    }
}
