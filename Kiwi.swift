//
//  Kiwi.swift
//  Kiwi
//
//  Created by Jarrod Norwell on 12/8/2025.
//

public enum KiwiKey : UInt8 {
    case a = 0b00000001
    case b = 0b00000010
    case select = 0b00000100
    case start = 0b00001000
    case right = 0b00010000
    case left = 0b00100000
    case up = 0b01000000
    case down = 0b10000000
}

public actor Kiwi {
    public var emulator: KiwiEmulator = .shared()
    
    public init() {}
    
    public func insert(_ cartridge: URL) {
        emulator.insert(cartridge)
    }
    
    public func start() {
        emulator.start()
    }
    
    public func stop() {
        emulator.stop()
    }
    
    public var isPaused: Bool {
        get {
            emulator.isPaused()
        }
        set {
            pause(newValue)
        }
    }
    
    public func pause(_ pause: Bool) {
        emulator.pause(pause)
    }
    
    public func title(_ cartridge: URL) -> String {
        emulator.title(from: cartridge)
    }
    
    public func load(state url: URL) {
        emulator.load(url)
    }
    
    public func save(state url: URL) {
        emulator.save(url)
    }
    
    public func ab(_ buffer: @escaping (UnsafeMutablePointer<UInt32>, Int) -> Void) {
        emulator.ab = buffer
    }
    
    public func fb(_ buffer: @escaping (UnsafeMutablePointer<UInt32>, _ width: Int, _ height: Int) -> Void) {
        emulator.fb = buffer
    }
    
    public func button(button: KiwiKey, player: Int, pressed: Bool) {
        emulator.button(button.rawValue, player: .init(player), pressed: pressed)
    }
    
    public func loadState(_ completionHandler: @escaping (Bool) -> Void) {
        completionHandler(emulator.loadState())
    }
    
    public func saveState(_ completionHandler: @escaping (Bool) -> Void) {
        completionHandler(emulator.saveState())
    }
}
