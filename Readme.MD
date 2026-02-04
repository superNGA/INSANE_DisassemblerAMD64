# INSANE Disassembler AMD64

<p align="center">
  <img src="Images/logo.png" width="180" height="180" alt="INSANE Disassembler Logo">
</p>

<p align="center">
  A lightweight, high-performance linear-sweep disassembler for x86-64 (long mode) instructions.
</p>

## Overview

INSANE Disassembler AMD64 is a fast and minimalistic disassembler targeting the x86-64 architecture in long mode. It supports the most commonly used instruction encodings in modern binaries:

- Legacy (pre-AVX)
- VEX-encoded (AVX/AVX2)
- EVEX-encoded (AVX-512)

The design prioritizes performance and simplicity while delivering output quality comparable to established tools such as Zydis and ndisasm.

## Features

- **High Performance**: Competitive with or surpassing leading open-source disassemblers on real-world binaries.
- **Two-Pass Architecture**: The first pass decodes raw bytes into structured instruction representations; the second pass handles formatting and output. This separation enables clean code and potential parallelization of the formatting stage.
- **Single-Threaded Core**: Currently single-threaded, with straightforward opportunities for multithreading the formatting pass to achieve significant speedups on multi-core systems.
- **Minimal Dependencies**: Pure C++ implementation with no external runtime libraries required beyond the standard library.

## Requirements

- C++17-compliant compiler (e.g., MSVC, GCC, Clang)
- NASM assembler (optional, for assembling test binaries or examples)

## Architecture

The disassembler employs a two-pass linear-sweep design:

1. **Decoding Pass**: Scans the input byte stream and decodes instructions into an internal representation, determining instruction boundaries and extracting all necessary metadata.
2. **Formatting Pass**: Converts the decoded instructions into human-readable assembly output.

This approach provides clear separation of concerns, excellent cache locality in the decoding phase, and flexibility for future enhancements (e.g., multithreaded formatting or additional analysis passes).

## Performance

Benchmarks were conducted on a 1,000,000-byte code section extracted from a real-world x86-64 application (`client.dll` from Counter-Strike 2).

- **INSANE Disassembler AMD64**: 8.2 seconds
- **Zydis** (latest version): 8.0 seconds

Measurements were taken using high-resolution timers (`QueryPerformanceCounter` on Windows) on identical hardware and input, covering full decoding and formatting. The near-parity with Zydis—one of the fastest and most mature open-source x86-64 disassemblers—demonstrates the effectiveness of the optimized implementation.

Further improvements (e.g., merging passes or multithreading) are expected to yield additional gains.

## Building and Usage

*(Add your specific build instructions here, e.g., Visual Studio solution, CMake commands, or command-line compilation with `cl.exe`.)*

Example usage (once built):
