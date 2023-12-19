// Copyright (c) 2023 Jan Pydych

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "crc32.h"

// NOTE: Remember to align the code of the worker function if you want to change this value.
#define THREADS 16

// TODO: Use a bitmap instead.
uint8_t *hashtable;
uint8_t worker(uint8_t e);

static uint8_t xchg(uint8_t *addr, uint8_t val)
{
    uint8_t result;

    asm volatile(
       "lock; xchgb %0, %1" :
       "+m" (*addr), "=a" (result) :
       "1" (val) :
       "cc"
    );

    return result;
}

int main()
{
    // README: For some reason that I don't know, trying to allocate 4 GiB of memory results in SIGSEGV...
    //         That's why I allocate one byte less.
    hashtable = malloc(0xFFFFFFFF);
    memset(hashtable, 0, 0xFFFFFFFF);

    DWORD  thread_id_array[THREADS];
    HANDLE thread_array[THREADS];

    // TIP: You can also use this program to reverse CRC-32.
    //      This can be used, for example, to crack passwords for ZIPs
    //      (not useful in practice though, it only works on files up to 4 bytes,
    //      but can be used for CTFs, like this one: https://ctflearn.com/challenge/1022).
    hashtable[0xDEADC0DE] = 1;

    for (uint8_t i = 0; i < THREADS; i++) {
        thread_array[i] = CreateThread(
            NULL,
            0,
            worker,
            i,
            0,
            &thread_id_array[i]);
    }

    const time_t start = time(NULL);

    WaitForMultipleObjects(THREADS, thread_array, TRUE, INFINITE);

    const time_t end = time(NULL);

    // About 39 seconds on Intel Core i7-10700K (8 cores with HT).
    // NOTE: We do not measure the creation time of threads,
    //       so the total program execution time may be slightly longer.

    printf("Time elapsed: %lld seconds\n", end - start);
    printf("No collisions found!\n");

    for (int i = 0; i < THREADS; i++)
    {
        CloseHandle(thread_array[i]);
    }

    free(hashtable);

    return 0;
}

uint8_t bufsize = 8;

constexpr uint8_t a_offset = 2;
constexpr uint8_t b_offset = 3;
constexpr uint8_t c_offset = 4;
constexpr uint8_t d_offset = 5;

uint8_t worker(const uint8_t e) {
    // Look at: https://en.wikipedia.org/wiki/Hexspeak
    uint8_t buf[] = {
        0xBA,
        0xAD,

        0xFA,
        0xCE,
        0xB0,
        0x0C,

        0xCA,
        0xFE
    };

    for (register int a = 0; a < 256; a++) {
        for (register int b = 0; b < 256; b++) {
            for (register int c = 0; c < 256; c++) {
                for (register int d = 0; d < 16; d++) {
                    buf[a_offset] = a;
                    buf[b_offset] = b;
                    buf[c_offset] = c;
                    buf[d_offset] = d * 16 + e;

                    const uint32_t crc = crc32(buf, bufsize);

                    if (xchg(hashtable + crc, 1) != 0) {
                        printf("Collision found!!!\n");
                        printf("CRC32: %08X, value: %08X", crc, a << 24 | b << 16 | c << 8 | d);
                        exit(1);

                        return 1;
                    }
                }
            }
        }
    }

    return 0;
}
