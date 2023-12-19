# CRC-32 Collisions Finder

This program is designed to find CRC-32 checksum collisions. It searches all possible 32-bit values and calculates CRC-32 values for them. If it finds a collision, it displays information about it and stops working.

## Prerequisites

The following libraries are used in the project and should be present on your system:

- `<stdio.h>`
- `<stdint.h>`
- `<stdlib.h>`
- `<string.h>`
- `<time.h>`
- `<windows.h>`

Currently, this program is designed to work only on Windows
due to its usage of Windows-specific threading functions.

**However, I welcome pull requests to modify the program to use `pthreads` to allow it to run on Linux as well.**

## Why did I create this program?

I created this program because I wanted to make sure that CRC32 
isn't vulnerable to the birthday attack, and that it will detect 
corruption of a few bytes (<= 4) in a larger structure.
