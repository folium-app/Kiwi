// swift-tools-version: 5.10
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "Kiwi",
    products: [
        // Products define the executables and libraries a package produces, making them visible to other packages.
        .library(name: "Kiwi", targets: ["Kiwi"]),
        .library(name: "KiwiCXX", targets: ["KiwiCXX"]),
        .library(name: "KiwiObjC", targets: ["KiwiObjC"])
    ],
    dependencies: [
        .package(url: "https://github.com/jarrodnorwell/HQx", branch: "main"),
        .package(url: "https://github.com/jarrodnorwell/XBRZ", branch: "main")
    ],
    targets: [
        // Targets are the basic building blocks of a package, defining a module or a test suite.
        // Targets can depend on other targets in this package and products from dependencies.d
        .target(name: "Kiwi", dependencies: ["KiwiObjC"]),
        .target(name: "KiwiCXX", dependencies: ["HQx", "XBRZ"], sources: ["", "bus", "cartridge", "controller", "cpu", "mappers", "ppu"],
                publicHeadersPath: "include", cxxSettings: [
                    .unsafeFlags([
                        "-Wno-conversion"
                    ])
                ], swiftSettings: [
                    .interoperabilityMode(.Cxx)
                ]),
        .target(name: "KiwiObjC", dependencies: ["KiwiCXX"], publicHeadersPath: "include", swiftSettings: [
            .interoperabilityMode(.Cxx)
        ])
    ],
    cLanguageStandard: .c2x,
    cxxLanguageStandard: .cxx2b
)
