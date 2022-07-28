---
layout: post
title:  "MKW-SP Programming Tutorial - Part 1"
date:   2022-05-26 13:30:00 +0200
author: stebler
---

This tutorial aims to give the reader the necessary tools to write code for MKW-SP (although it can easily be transposed to other MKW mods and patching systems). Basic programming knowledge is assumed (ideally C or C++). This will be somewhat higher level than other available resources, both from the reverse-engineering and the programming side. This first part will present an overview of the hardware and software.

## Hardware

### CPU

The main processor of the Wii is a single-core semi-custom IBM [PowerPC](https://en.wikipedia.org/wiki/PowerPC) 750CL clocked at a fixed 729 MHz, nicknamed [Broadway](https://en.wikipedia.org/wiki/Broadway_(microprocessor)). Both memory addresses and instructions are always 32-bit. It features superscalar out-of-order execution, a 32 KiB L1 instruction cache, a 32 KiB L1 data cache, a 256 KiB unified L2 cache and 2x32-bit floating point [SIMD](https://en.wikipedia.org/wiki/Single_instruction,_multiple_data) "paired singles" extensions. Its [MMU](https://en.wikipedia.org/wiki/Memory_management_unit) supports both page (4 KiB) address translation (PAT) and block (128 KiB to 256 MiB) address translation (BAT), with most Wii software opting for the latter. Its manual is available [here](https://pokeacer.xyz/wii/pdf/BroadwayUserManual.pdf). It is backwards compatible with the [Gekko](https://en.wikipedia.org/wiki/Gekko_(microprocessor)) (PowerPC 750CXe) used by the GameCube and the Triforce. In a similar way, the tri-core [Espresso](https://en.wikipedia.org/wiki/Espresso_(microprocessor)) of the Wii U is backwards compatible with the Broadway and the Gekko.

### GPU

The ATI-designed GPU of the Wii is called [Hollywood](https://en.wikipedia.org/wiki/Hollywood_(graphics_chip)) and is clocked at 243 MHz for a theoretical performance of 14.1 GFLOPS. It has 3 MiB of eDRAM and a configurable fixed-function TEV pipeline. Again, it is backwards-compatible with the Flipper GPU of the GameCube. Commands are sent from the CPU to the GPU via a FIFO (concretely simply by writing to a memory-mapped address) and acknowledged using interrupts. The Revolution[^revolution] SDK contains wrapper functions for most common operations.

### I/O and security coprocessor

Unlike on the GameCube, most peripherals aren't accessed directly by the main CPU but by an [ARM926EJ-S](https://en.wikipedia.org/wiki/ARM9) coprocessor unofficially named [Starlet](https://wiibrew.org/wiki/Hardware/Starlet) (officially IOP) and running its own operating system known as IOS. Like the Broadway, the Starlet is a 32-bit big-endian system. It runs at 243 MHz like the GPU. There are various versions of IOS with different feature sets, and several revisions for each version to patch bugs and vulnerabilities. Each game or channel (known as "titles") has a fixed major version of IOS, but versions are largely compatible with each others: for instance MKW originally runs on IOS36, but MKW-SP runs on IOS58 for its USB 2.0 capabilities instead, without any change to the PPC-side code aside from USB (and SD) additions. The Starlet runs at a higher security level than the Broadway: it runs the boot programs of the Wii before bootstrapping it, and can restrict the memory and the registers available to it. It also contains dedicated hardware for cryptography (AES, SHA-1). As such it is responsible for preventing unauthorized software from running on the Wii, even if the Broadway is compromised (there is no such split on the Broadway between user and system code, and everything runs in kernel mode). At a low level, the Starlet and the Broadway communicate through shared registers. At a higher level, IOS exposes a UNIX-like API for the NAND filesystem, and special files for other peripherals (e.g. `/dev/sdio/slot0` for the SD card). The Revolution SDK wraps it in a higher level interface (here SDI), and as such the Starlet and IOS become an implementation detail.

### Memory

The Wii contains 24 MiB of 1T-SRAM inherited for the GameCube, known as MEM1 and typically mapped between 0x80000000 and 0x81800000 and 64 MiB of subsequently added GDDR3 SDRAM, known as MEM2 and typically mapped between 0x90000000 and 0x94000000. The Wii features unified memory and as such all 3 aforementioned processors can access all of it (although there are a few limitations in practice). MEM1 is about 3 times faster than MEM2 for main CPU reads. For that reason, code and game logic structures are typically in MEM1, while assets are in MEM2. Both MEM1 and MEM2 are doubled on devkits.

### NAND storage

The Wii contains a 512 MiB NAND chip, which is used to store IOSes, save data and channels notably. The filesystem is encrypted and only available through IOS. A permission system handles sharing of files between titles.

### SDIO and Wi-Fi

The SD interface is wired to the front SD card slot (`/dev/sdio/slot0`), and to the Wi-Fi module (`/dev/wl0`, exposed via a common interface with the USB Ethernet adapter by IOS).

### USB and Bluetooth

The USB bus is connected to the 2 external ports and to the Bluetooth adapter. Only IOS58 and IOS59 support USB 2.0, the other ones are limited to USB 1.1.

### Optical drive interface

The last hardware component worth mentioning: it can read GameCube discs, Wii discs, and even regular DVDs, although that last functionality was ultimately disabled for licensing cost reasons. It is exposed by IOS as `/dev/di`.

## Software

### Versions

There are 4 versions of the game (in release order): NTSC-J, PAL, NTSC-U and NTSC-K (they are often shorted by their differing game id letter, J, P, E and K respectively). There are differences between the versions, both for the code and for the assets. The vast majority of research and development by the community happens on the PAL version of the game. Several porting tools are available to translate memory addresses from one version to another.

### Apploader

The [apploader](https://wiibrew.org/wiki/Apploader) is the first piece of software from a disc to be run by the [System Menu](https://wiibrew.org/wiki/System_Menu). It tells it how to load the main.dol into memory.

### main.dol

The main.dol[^dolphin] (or simply "the dol") is the main game executable. Its [format](https://wiki.tockdom.com/wiki/DOL_(File_Format)) is fairly simple. In Mario Kart Wii, it mainly contains library code.

### StaticR.rel

The StaticR.rel (or simply "the rel") is a relocatable (can be put anywhere in memory) executable. As such its [format](https://wiki.tockdom.com/wiki/REL_(File_Format)) is somewhat more complex. Rel files can access dol functions and data once linked. In Mario Kart Wii, it contains most game-specific code. Other games use multiple rels (e.g. one per level) to reduce memory usage, but Mario Kart Wii doesn't make use of that feature. That's why it's quite curious that a rel was used at all, especially since the channel is just one dol.

### Sections

Both the dol and the rel are made of several sections of different types: text sections contain the actual PowerPC instructions, rodata and data sections contain read-only and read-write data respectively, while bss sections are zero-initialized. Content from all types of data sections can be directly referenced by PowerPC load and store instructions. A map of the sections for all versions of both the dol and the rel is available [here](https://github.com/stblr/mkw-sp/blob/main/port.py).

### The C and C++ compilation model

Each C or C++ file (known as a compilation unit or [translation unit](https://en.wikipedia.org/wiki/Translation_unit_(programming)) after running the preprocessor) is transformed into an object file by the compiler. Then all object files are packed together into a binary by the linker. This allows for faster build times if only a subset of the files have been modified. The linker generally doesn't mix files with each other[^ntsc-u], which means that the file structure can still be understood from the binary.

### Libraries

The dol contains several libraries used by the game. They are statically linked and contiguous.

#### MSL

The C standard library notably.

#### Revolution SDK and RevoEX

Written in C. Consists of low level code to interact with the Wii's hardware. RevoEX is an extension for networking mainly.

#### GameSpy SDK

Written in C. Used to access the GameSpy matchmaking servers used by WFC titles, as well as for P2P connections. The code as used in MKW is in large part available [here](https://github.com/riidefi/mkw/tree/master/source/gamespy).

#### DWC[^dwc]

Written in C. Wraps the GameSpy SDK.

#### RFL (Revolution Face Library)

Written in C. Provides functions to manage and render Miis.

#### NW4R (NintendoWare For Revolution)

Written in C++. A set of components to handle various file formats, notably: DB (debugging), EF (particle effects), G3D (3D graphics), LYT (layout), MATH (math utilities), SND (sound), UT (generic utilities).

#### EGG

Written in C++. Used by first-party Nintendo games. Provides game engine foundations such as graphics, threading or memory management, on top of NW4R and the SDK.

### Debugging information

For performance and size reasons, and to make reverse-engineering harder, most debugging information is removed from the retail version of the game. However, there is still a lot we can make use of.

#### Symbol maps

A symbol map is a text file containing the addresses of functions and globals of an executable. Unlike MKDD, MK7 or MK8, it was not kept in the ROM. Other Wii games using the same libraries (and notably Big Brain Academy: Wii Degree for EGG) do have one. It was then possible to port symbols from them to MKW, either manually or using tools.

#### Assertions and prints

A few leftover assertions contain the name of the file and/or the function in which they are used. A few `OSReport` (RVL SDK `printf` wrapper) calls are also still there.

#### Destructors

CodeWarrior (the official PowerPC compiler for Wii software) isn't good at optimizing destructors out, allowing us to easily trace C++ class inheritance.

#### Static constructors and destructors

C++ globals that cannot be initialized at compile-time are instead constructed before running the main function by a set of functions known as static constructors (and are destroyed in a similar way). All the constructors needed by a particular C++ file are grouped in a single function at the very end of the code of that file, and pointers to all these functions are put in an array in a dedicated section. This can help find boundaries between files in the binary.

#### RTTI helpers

MKW's UI code uses a custom [RTTI](https://en.wikipedia.org/wiki/Run-time_type_information) system and `dynamic_cast` equivalent based on NW4R. Presumably to help debugging, Nintendo also added a function to return the name of the type to some classes, which is still here in some cases (notably for all classes based on `UIControl`).

#### Alphabetical ordering

Thanks to all of this, we can actually infer that files are in fact sorted grouped by folder and sorted alphabetically. This can help to understand the code structure from a high level, and to give sensible names to classes and files.

### Program structure

Mario Kart Wii is written in C++, but it doesn't make use of the C++ standard library. Several parts of the C standard libaray are also non-existent. Instead it relies on the RVL SDK, EGG or its own code.

#### Scene and UI hierarchy

At the top level, we have the `SceneManager` (based on EGG). It handles `Scene`s and `Fader`s. As the name suggests, a `Fader` is responsible for fading transitions between scenes. A `Scene` notably handles memory heaps, available resources and what graphics can be drawn. They can be nested. There are 5 of them: the `StrapScene` displays the health and safety warning and loads the rel from the disc, the `RootScene` then replaces it and is the parent of one of the other 3 scenes, the `GlobeScene` can display the Earth graphic, the `RaceScene` can render actual races while the `MenuScene` is used for in the remaining cases. At the top level of the UI subsystem, there is the `SectionManager`. It can hold one `Section` at a time. Each section has an associated `Scene`, and contains a fixed set of `Page`s. `Page`s are all initialized as part of the `Section`, but they can be activated and deactivated as will, with up to 10 active pages at once in a layered fashion. `Page`s from the lower layers may not have graphical elements at all, and instead handle background processing tasks. 2 additional `SystemPage`s are drawn at the very top for the Wii Remote cursor and the controller disconnection UI. `Page`s themselves contain a set of `UIControl`s, which can sometimes themselves contain other controls.

#### Main loop

The main loop first runs `SceneManager::draw` to send commands to the GPU (in green on the [performance monitor](/assets/performance-monitor.png)). While the GPU is executing them (in blue), the CPU then runs `SceneManager::calc` (in pink).

#### Subsystems

MKW uses the [singleton pattern](https://en.wikipedia.org/wiki/Singleton_pattern) for basically all of its subsystems. They are either initialized on game start by the `RootScene` (such as `ResourceManager` or `SaveManager` but also `RaceConfig`), or when entering one of its child `Scene`s (such as `ItemManager` or `GlobeManager`).

#### Memory management

While C libraries in MKW make very little use of dynamic memory allocation (there is no `malloc`), MKW is very reliant on it. The `new` and `delete` operators are wired to a multi-heap system based on EGG. The `RootScene` has one heap for MEM1 and one for MEM2 for permanent allocations. Each of its 3 child scenes has 2 heaps per memory chip, with only the latter being wiped when reloading a scene (e.g. on race restart). Heaps are locked after initialization, which means that no direct allocations are performed outside of loading sequences (some subsystems allocate a child heap if they need it nevertheless). The advantages of such a system are that the memory usage is quite predictable, and that it is possible to erase the memory of a subsystem very efficiently.

#### Multithreading

The Broadway is single core, but Mario Kart Wii uses multithreading a lot. This is implemented with a periodic decrementer interrupt, whose routine can change the currently running thread. Multithreading is notably important for tasks that need to wait on an external device (such as anything going through IOS): in this case they can explicitly yield execution to another thread. Another use case is periodic background tasks. The thread system is implemented by the SDK. It also provides utilities such as `OSMutex` and `OSMessageQueue` to handle data sharing and communication between threads. Because there is only one physical CPU, it is common to define a critical section simply by disabling decrementer and external interrupts. Each thread has its own [stack](https://en.wikipedia.org/wiki/Call_stack) and a configurable priority between 0 (highest) and 31.

## Additional resources

[WiiBrew](https://wiibrew.org/wiki/Main_Page): various details about Wii hardware and software.

[Compiler](https://wiki.tockdom.com/wiki/Compiler) page on Tockdom: lower level details about the ABI, memory management and multithreading.

## Now what?

Part 2 can be found [here]({% post_url 2022-07-08-mkw-sp-programming-tutorial-part-2 %}).

[^revolution]: Revolution (RVL) is the codename of the Wii.
[^dolphin]: Dolphin (DOL) is the codename of the GameCube.
[^ntsc-u]: The NTSC-U version of the StaticR.rel has many weird reorderings for some unknown reason.
[^dwc]: Likely DS Wireless Communication.
