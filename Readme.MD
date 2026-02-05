<h1 align="center"> INSANE Disassembler AMD64 </h1>

<p align="center">
  <img src="Images/logo.png" width="180" height="180" alt="INSANE Disassembler Logo">
</p>

<p align="center"><i>
  A lightweight, wanna-be high-performance linear-sweep disassembler for x86-64 (long mode) instructions.
</i></p>

## Overview

INSANE Disassembler AMD64 is a fast and minimalistic disassembler targeting the x86-64 architecture in long mode. It supports the most commonly used instruction encodings in modern binaries:

- Legacy (pre-AVX)
- VEX-encoded (AVX/AVX2)
- EVEX-encoded (AVX-512)

The design tries to prioritizes performance and simplicity, but the natures of x86 doesn't allow me to do that. Still fairly readable tho 🙂. 

## Features

- **Two-Pass Architecture**: The first pass decodes raw bytes into structured instruction representations; the second pass handles formatting and output. This separation enables clean code and potential parallelization of the formatting stage.
- **Single-Threaded Core**: Currently single-threaded, with straightforward opportunities for multithreading the formatting pass to achieve significant speedups on multi-core systems.
- **Minimal Dependencies**: Pure C++ implementation with no external runtime libraries required beyond the standard library.

## Requirements

- C++17-compliant compiler (e.g., MSVC, GCC, Clang)
- NASM assembler (modify CMakeList.txt if using other assembler)

## Architecture

The disassembler employs a two-pass linear-sweep design:

1. **Decoding Pass**: Scans the input byte stream and decodes instructions into an internal representation, determining instruction boundaries and extracting all necessary metadata.
2. **Formatting Pass**: Converts the decoded instructions into human-readable assembly output.

This approach provides clear separation of concerns, excellent cache locality in the decoding phase, and flexibility for future enhancements (e.g., multithreaded formatting or additional analysis passes).
