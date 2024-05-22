//
//  Kiwi.swift
//
//
//  Created by Jarrod Norwell on 21/2/2024.
//

import KiwiObjC
import Foundation

public struct Kiwi {
    public static let shared = Kiwi()
    
    fileprivate let kiwiObjC = KiwiObjC.shared()
    
    public func insert(game url: URL) {
        kiwiObjC.insert(game: url)
    }
    
    public func step() {
        kiwiObjC.step()
    }
    
    public func videoBuffer() -> UnsafeMutablePointer<UInt32> {
        kiwiObjC.videoBuffer()
    }
    
    public func virtualControllerButtonDown(_ button: UInt8) {
        kiwiObjC.virtualControllerButtonDown(button)
    }
    
    public func virtualControllerButtonUp(_ button: UInt8) {
        kiwiObjC.virtualControllerButtonUp(button)
    }
    
    public func useUpscalingFilter() -> Int32 {
        kiwiObjC.useUpscalingFilter()
    }
    
    public func setUpscalingFilter(_ upscalingFilter: Int32) {
        kiwiObjC.setUpscalingFilter(upscalingFilter)
    }
    
    public func useUpscalingFactor() -> Int32 {
        kiwiObjC.useUpscalingFactor()
    }
    
    public func setUpscalingFactor(_ upscalingFactor: Int32) {
        kiwiObjC.setUpscalingFactor(upscalingFactor)
    }
}
