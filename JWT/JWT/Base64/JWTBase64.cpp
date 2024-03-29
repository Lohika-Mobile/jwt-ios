//
//  JWTBase64.cpp
//
//  Created by Alexander Trishyn on 7/6/15.
//  Copyright (c) 2015 Lohika. BSD license.
//

#include "JWTBase64.h"

namespace jwt {

namespace {
/*
 * http://code.google.com/p/stringencoders/
 *
 * This uses a "URL-safe" or "WEB-safe" encoding.  THe standard
 * base 64 encoding uses the characters '+', '/' and '=' have special
 * restrictions when used inside a URL.
 *
 * This uses "+" to "-", "/" to "_", and "=" to "." as the replacement
 * alphabet.
 *
 * It's easy to change this to use "URL safe" characters and to remove
 * padding.  See the modp_b64.c source code for details.
*/
    
#define CHAR62 '-'
#define CHAR63 '_'
#define CHARPAD '='

#define BADCHAR 0x01FFFFFF
    
/**
 * you can control if we use padding by commenting out this
 * next line.  However, I highly recommend you use padding and not
 * using it should only be for compatability with a 3rd party.
 * Also, 'no padding' is not tested!
*/

// #define DOPAD 1

/*
 * if we aren't doing padding
 * set the pad character to NULL
*/
    
#ifndef DOPAD
#undef CHARPAD
#define CHARPAD '\0'
#endif
    
static const uint8_t e0[256] = {
    'A',  'A',  'A',  'A',  'B',  'B',  'B',  'B',  'C',  'C',
    'C',  'C',  'D',  'D',  'D',  'D',  'E',  'E',  'E',  'E',
    'F',  'F',  'F',  'F',  'G',  'G',  'G',  'G',  'H',  'H',
    'H',  'H',  'I',  'I',  'I',  'I',  'J',  'J',  'J',  'J',
    'K',  'K',  'K',  'K',  'L',  'L',  'L',  'L',  'M',  'M',
    'M',  'M',  'N',  'N',  'N',  'N',  'O',  'O',  'O',  'O',
    'P',  'P',  'P',  'P',  'Q',  'Q',  'Q',  'Q',  'R',  'R',
    'R',  'R',  'S',  'S',  'S',  'S',  'T',  'T',  'T',  'T',
    'U',  'U',  'U',  'U',  'V',  'V',  'V',  'V',  'W',  'W',
    'W',  'W',  'X',  'X',  'X',  'X',  'Y',  'Y',  'Y',  'Y',
    'Z',  'Z',  'Z',  'Z',  'a',  'a',  'a',  'a',  'b',  'b',
    'b',  'b',  'c',  'c',  'c',  'c',  'd',  'd',  'd',  'd',
    'e',  'e',  'e',  'e',  'f',  'f',  'f',  'f',  'g',  'g',
    'g',  'g',  'h',  'h',  'h',  'h',  'i',  'i',  'i',  'i',
    'j',  'j',  'j',  'j',  'k',  'k',  'k',  'k',  'l',  'l',
    'l',  'l',  'm',  'm',  'm',  'm',  'n',  'n',  'n',  'n',
    'o',  'o',  'o',  'o',  'p',  'p',  'p',  'p',  'q',  'q',
    'q',  'q',  'r',  'r',  'r',  'r',  's',  's',  's',  's',
    't',  't',  't',  't',  'u',  'u',  'u',  'u',  'v',  'v',
    'v',  'v',  'w',  'w',  'w',  'w',  'x',  'x',  'x',  'x',
    'y',  'y',  'y',  'y',  'z',  'z',  'z',  'z',  '0',  '0',
    '0',  '0',  '1',  '1',  '1',  '1',  '2',  '2',  '2',  '2',
    '3',  '3',  '3',  '3',  '4',  '4',  '4',  '4',  '5',  '5',
    '5',  '5',  '6',  '6',  '6',  '6',  '7',  '7',  '7',  '7',
    '8',  '8',  '8',  '8',  '9',  '9',  '9',  '9',  '-',  '-',
    '-',  '-',  '_',  '_',  '_',  '_'
};
    
static const uint8_t e1[256] = {
    'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',
    'K',  'L',  'M',  'N',  'O',  'P',  'Q',  'R',  'S',  'T',
    'U',  'V',  'W',  'X',  'Y',  'Z',  'a',  'b',  'c',  'd',
    'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',
    'o',  'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',
    'y',  'z',  '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',
    '8',  '9',  '-',  '_',  'A',  'B',  'C',  'D',  'E',  'F',
    'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',  'P',
    'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',
    'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',
    'k',  'l',  'm',  'n',  'o',  'p',  'q',  'r',  's',  't',
    'u',  'v',  'w',  'x',  'y',  'z',  '0',  '1',  '2',  '3',
    '4',  '5',  '6',  '7',  '8',  '9',  '-',  '_',  'A',  'B',
    'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',
    'M',  'N',  'O',  'P',  'Q',  'R',  'S',  'T',  'U',  'V',
    'W',  'X',  'Y',  'Z',  'a',  'b',  'c',  'd',  'e',  'f',
    'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',  'p',
    'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',
    '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',
    '-',  '_',  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',
    'I',  'J',  'K',  'L',  'M',  'N',  'O',  'P',  'Q',  'R',
    'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',  'a',  'b',
    'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',
    'm',  'n',  'o',  'p',  'q',  'r',  's',  't',  'u',  'v',
    'w',  'x',  'y',  'z',  '0',  '1',  '2',  '3',  '4',  '5',
    '6',  '7',  '8',  '9',  '-',  '_'
};
    
static const uint8_t e2[256] = {
    'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',
    'K',  'L',  'M',  'N',  'O',  'P',  'Q',  'R',  'S',  'T',
    'U',  'V',  'W',  'X',  'Y',  'Z',  'a',  'b',  'c',  'd',
    'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',
    'o',  'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',
    'y',  'z',  '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',
    '8',  '9',  '-',  '_',  'A',  'B',  'C',  'D',  'E',  'F',
    'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',  'P',
    'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',
    'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',
    'k',  'l',  'm',  'n',  'o',  'p',  'q',  'r',  's',  't',
    'u',  'v',  'w',  'x',  'y',  'z',  '0',  '1',  '2',  '3',
    '4',  '5',  '6',  '7',  '8',  '9',  '-',  '_',  'A',  'B',
    'C',  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',
    'M',  'N',  'O',  'P',  'Q',  'R',  'S',  'T',  'U',  'V',
    'W',  'X',  'Y',  'Z',  'a',  'b',  'c',  'd',  'e',  'f',
    'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',  'p',
    'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',  'z',
    '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',
    '-',  '_',  'A',  'B',  'C',  'D',  'E',  'F',  'G',  'H',
    'I',  'J',  'K',  'L',  'M',  'N',  'O',  'P',  'Q',  'R',
    'S',  'T',  'U',  'V',  'W',  'X',  'Y',  'Z',  'a',  'b',
    'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',
    'm',  'n',  'o',  'p',  'q',  'r',  's',  't',  'u',  'v',
    'w',  'x',  'y',  'z',  '0',  '1',  '2',  '3',  '4',  '5',
    '6',  '7',  '8',  '9',  '-',  '_'
};
    
    
/* SPECIAL DECODE TABLES FOR LITTLE ENDIAN (INTEL) CPUS */

static const uint32_t d0[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x000000f8, 0x01ffffff, 0x01ffffff,
    0x000000d0, 0x000000d4, 0x000000d8, 0x000000dc, 0x000000e0, 0x000000e4,
    0x000000e8, 0x000000ec, 0x000000f0, 0x000000f4, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00000004, 0x00000008, 0x0000000c, 0x00000010, 0x00000014, 0x00000018,
    0x0000001c, 0x00000020, 0x00000024, 0x00000028, 0x0000002c, 0x00000030,
    0x00000034, 0x00000038, 0x0000003c, 0x00000040, 0x00000044, 0x00000048,
    0x0000004c, 0x00000050, 0x00000054, 0x00000058, 0x0000005c, 0x00000060,
    0x00000064, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x000000fc,
    0x01ffffff, 0x00000068, 0x0000006c, 0x00000070, 0x00000074, 0x00000078,
    0x0000007c, 0x00000080, 0x00000084, 0x00000088, 0x0000008c, 0x00000090,
    0x00000094, 0x00000098, 0x0000009c, 0x000000a0, 0x000000a4, 0x000000a8,
    0x000000ac, 0x000000b0, 0x000000b4, 0x000000b8, 0x000000bc, 0x000000c0,
    0x000000c4, 0x000000c8, 0x000000cc, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff
};
    
    
static const uint32_t d1[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x0000e003, 0x01ffffff, 0x01ffffff,
    0x00004003, 0x00005003, 0x00006003, 0x00007003, 0x00008003, 0x00009003,
    0x0000a003, 0x0000b003, 0x0000c003, 0x0000d003, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00001000, 0x00002000, 0x00003000, 0x00004000, 0x00005000, 0x00006000,
    0x00007000, 0x00008000, 0x00009000, 0x0000a000, 0x0000b000, 0x0000c000,
    0x0000d000, 0x0000e000, 0x0000f000, 0x00000001, 0x00001001, 0x00002001,
    0x00003001, 0x00004001, 0x00005001, 0x00006001, 0x00007001, 0x00008001,
    0x00009001, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x0000f003,
    0x01ffffff, 0x0000a001, 0x0000b001, 0x0000c001, 0x0000d001, 0x0000e001,
    0x0000f001, 0x00000002, 0x00001002, 0x00002002, 0x00003002, 0x00004002,
    0x00005002, 0x00006002, 0x00007002, 0x00008002, 0x00009002, 0x0000a002,
    0x0000b002, 0x0000c002, 0x0000d002, 0x0000e002, 0x0000f002, 0x00000003,
    0x00001003, 0x00002003, 0x00003003, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff
};
    
    
static const uint32_t d2[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00800f00, 0x01ffffff, 0x01ffffff,
    0x00000d00, 0x00400d00, 0x00800d00, 0x00c00d00, 0x00000e00, 0x00400e00,
    0x00800e00, 0x00c00e00, 0x00000f00, 0x00400f00, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00400000, 0x00800000, 0x00c00000, 0x00000100, 0x00400100, 0x00800100,
    0x00c00100, 0x00000200, 0x00400200, 0x00800200, 0x00c00200, 0x00000300,
    0x00400300, 0x00800300, 0x00c00300, 0x00000400, 0x00400400, 0x00800400,
    0x00c00400, 0x00000500, 0x00400500, 0x00800500, 0x00c00500, 0x00000600,
    0x00400600, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00c00f00,
    0x01ffffff, 0x00800600, 0x00c00600, 0x00000700, 0x00400700, 0x00800700,
    0x00c00700, 0x00000800, 0x00400800, 0x00800800, 0x00c00800, 0x00000900,
    0x00400900, 0x00800900, 0x00c00900, 0x00000a00, 0x00400a00, 0x00800a00,
    0x00c00a00, 0x00000b00, 0x00400b00, 0x00800b00, 0x00c00b00, 0x00000c00,
    0x00400c00, 0x00800c00, 0x00c00c00, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff
};
    
    
static const uint32_t d3[256] = {
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x003e0000, 0x01ffffff, 0x01ffffff,
    0x00340000, 0x00350000, 0x00360000, 0x00370000, 0x00380000, 0x00390000,
    0x003a0000, 0x003b0000, 0x003c0000, 0x003d0000, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x00000000,
    0x00010000, 0x00020000, 0x00030000, 0x00040000, 0x00050000, 0x00060000,
    0x00070000, 0x00080000, 0x00090000, 0x000a0000, 0x000b0000, 0x000c0000,
    0x000d0000, 0x000e0000, 0x000f0000, 0x00100000, 0x00110000, 0x00120000,
    0x00130000, 0x00140000, 0x00150000, 0x00160000, 0x00170000, 0x00180000,
    0x00190000, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x003f0000,
    0x01ffffff, 0x001a0000, 0x001b0000, 0x001c0000, 0x001d0000, 0x001e0000,
    0x001f0000, 0x00200000, 0x00210000, 0x00220000, 0x00230000, 0x00240000,
    0x00250000, 0x00260000, 0x00270000, 0x00280000, 0x00290000, 0x002a0000,
    0x002b0000, 0x002c0000, 0x002d0000, 0x002e0000, 0x002f0000, 0x00300000,
    0x00310000, 0x00320000, 0x00330000, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff,
    0x01ffffff, 0x01ffffff, 0x01ffffff, 0x01ffffff
};

/**
 * Given a source string of length len, this returns the amount of
 * memory the destination string should have.
 *
 * remember, this is integer math
 * 3 bytes turn into 4 chars
 * ceiling[len / 3] * 4 + 1
 *
 * +1 is for any extra null.
 */
#define modp_b64r_encode_len(A) ((A+2)/3 * 4 + 1)
    
/**
 * Given a base64 string of length len,
 *   this returns the amount of memory required for output string
 *  It maybe be more than the actual number of bytes written.
 * NOTE: remember this is integer math
 * this allocates a bit more memory than traditional versions of b64r
 * decode  4 chars turn into 3 bytes
 * floor[len * 3/4] + 2
 */
#define modp_b64r_decode_len(A) (A / 4 * 3 + 2)
    
/**
 * Will return the strlen of the output from encoding.
 * This may be less than the required number of bytes allocated.
 *
 * This allows you to 'deserialized' a struct
 * \code
 * char* b64rencoded = "...";
 * int len = strlen(b64rencoded);
 *
 * struct datastuff foo;
 * if (modp_b64r_encode_strlen(sizeof(struct datastuff)) != len) {
 *    // wrong size
 *    return false;
 * } else {
 *    // safe to do;
 *    if (modp_b64r_decode((char*) &foo, b64rencoded, len) == -1) {
 *      // bad characters
 *      return false;
 *    }
 * }
 * // foo is filled out now
 * \endcode
 */
#define modp_b64r_encode_strlen(A) ((A + 2)/ 3 * 4)

/**
 * Encode a raw binary string into web-safe base 64.
 * \param[out] dest should be allocated by the caller to contain
 *   at least modp_b64w_encode_len(len) bytes (see below)
 *   This will contain the null-terminated b64w encoded result
 * \param[in] src contains the bytes
 * \param[in] len contains the number of bytes in the src
 * \return length of the destination string plus the ending null byte
 *    i.e.  the result will be equal to strlen(dest) + 1
 *
 * Example
 *
 * \code
 * char* src = ...;
 * int srclen = ...; //the length of number of bytes in src
 * char* dest = (char*) malloc(modp_b64w_encode_len);
 * int len = modp_b64w_encode(dest, src, sourcelen);
 * if (len == -1) {
 *   printf("Error\n");
 * } else {
 *   printf("b64w = %s\n", dest);
 * }
 * \endcode
 *
 */

inline size_t modp_b64r_encode(char* dest, const char* str, size_t len)
{
    size_t i = 0;
    const uint8_t* s = (const uint8_t*) str;
    uint8_t* p = (uint8_t*) dest;
    
    /* unsigned here is important! */
    /* uint8_t is fastest on G4, amd */
    /* uint32_t is fastest on Intel */
    uint32_t t1, t2, t3;
    
    if (len > 2) {
        for (i = 0; i < len - 2; i += 3) {
            t1 = s[i]; t2 = s[i+1]; t3 = s[i+2];
            *p++ = e0[t1];
            *p++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
            *p++ = e1[((t2 & 0x0F) << 2) | ((t3 >> 6) & 0x03)];
            *p++ = e2[t3];
        }
    }
    
    switch (len - i) {
        case 0:
            break;
        case 1:
            t1 = s[i];
            *p++ = e0[t1];
            *p++ = e1[(t1 & 0x03) << 4];
            *p++ = CHARPAD;
            *p++ = CHARPAD;
            break;
        default: /* case 2 */
            t1 = s[i]; t2 = s[i+1];
            *p++ = e0[t1];
            *p++ = e1[((t1 & 0x03) << 4) | ((t2 >> 4) & 0x0F)];
            *p++ = e2[(t2 & 0x0F) << 2];
            *p++ = CHARPAD;
    }
    
    *p = '\0';
    return (size_t)(p - (uint8_t*)dest);
}
    
/**
 * Decode a web-safe base64 encoded string
 *
 * \param[out] dest should be allocated by the caller to contain at least
 *    len * 3 / 4 bytes.
 * \param[in] src should contain exactly len bytes of b64r characters.
 *     if src contains -any- non-base characters (such as white
 *     space, -1 is returned.
 * \param[in] len is the length of src
 *
 * \return the length (strlen) of the output, or -1 if unable to
 * decode
 *
 * \code
 * char* src = ...;
 * int srclen = ...; // or if you don't know use strlen(src)
 * char* dest = (char*) malloc(modp_b64r_decode_len(srclen));
 * int len = modp_b64r_decode(dest, src, sourcelen);
 * if (len == -1) { error }
 * \endcode
 */
    
size_t modp_b64r_decode(char* dest, const char* src, size_t len)
{
    size_t i;
    size_t leftover;
    size_t chunks;
    
    uint8_t* p;
    uint32_t x;
    uint32_t* destInt;
    const uint32_t* srcInt = (const uint32_t*) src;
    uint32_t y = *srcInt++;
    
    if (len == 0) return 0;
    
#ifdef DOPAD
    /*
     * if padding is used, then the message must be at least
     * 4 chars and be a multiple of 4
     */
    if (len < 4 || (len % 4 != 0)) {
        return (size_t)-1; /* error */
    }
    /* there can be at most 2 pad chars at the end */
    if (src[len-1] == CHARPAD) {
        len--;
        if (src[len -1] == CHARPAD) {
            len--;
        }
    }
#endif
    
    leftover = len % 4;
    chunks = (leftover == 0) ? len / 4 - 1 : len /4;
    
    p = (uint8_t*) dest;
    x = 0;
    destInt = (uint32_t*) p;
    srcInt = (const uint32_t*) src;
    y = *srcInt++;
    for (i = 0; i < chunks; ++i) {
        x = d0[y & 0xff] |
        d1[(y >> 8) & 0xff] |
        d2[(y >> 16) & 0xff] |
        d3[(y >> 24) & 0xff];
        
        if (x >= BADCHAR) {
            return (size_t)-1;
        }
        *destInt = x ;
        p += 3;
        destInt = (uint32_t*)p;
        y = *srcInt++;}
    
    
    switch (leftover) {
        case 0:
            x = d0[y & 0xff] |
            d1[(y >> 8) & 0xff] |
            d2[(y >> 16) & 0xff] |
            d3[(y >> 24) & 0xff];
            
            if (x >= BADCHAR) {
                return (size_t)-1;
            }
            *p++ =  ((uint8_t*)(&x))[0];
            *p++ =  ((uint8_t*)(&x))[1];
            *p =    ((uint8_t*)(&x))[2];
            return (chunks+1)*3;
#ifndef DOPAD
        case 1:  /* with padding this is an impossible case */
            x = d0[y & 0xff];
            *p = *((uint8_t*)(&x)); /* i.e. first char/byte in int */
            break;
#endif
        case 2: /* case 2, 1  output byte */
            x = d0[y & 0xff] | d1[y >> 8 & 0xff];
            *p = *((uint8_t*)(&x)); /* i.e. first char */
            break;
        default: /* case 3, 2 output bytes */
            x = d0[y & 0xff] |
            d1[y >> 8 & 0xff ] |
            d2[y >> 16 & 0xff];  /* 0x3c */
            *p++ =  ((uint8_t*)(&x))[0];
            *p =  ((uint8_t*)(&x))[1];
            break;
    }
    
    if (x >= BADCHAR) {
        return (size_t)-1;
    }
    
    return 3*chunks + (6*leftover)/8;
}

/** \brief b64r encode a cstr with len
 *
 * \param[in] s the input string to encode
 * \param[in] len the length of the input string
 * \return a newly allocated b64r string.  Empty if failed.
 */
static std::string b64r_encode(const char* s, size_t len)
{
    std::string x(modp_b64r_encode_len(len), '\0');
    size_t d = modp_b64r_encode(const_cast<char*>(x.data()), s, static_cast<int>(len));
    if (d == (size_t)-1) {
        x.clear();
    } else {
        x.erase(d, std::string::npos);
    }
    return x;
}

/** \brief b64r encode a cstr
 *
 * \param[in] s the input string to encode
 * \return a newly allocated b64r string.  Empty if failed.
 */
inline std::string b64r_encode(const char* s)
{
    return b64r_encode(s, static_cast<int>(strlen(s)));
}

/** \brief b64r encode a const std::string
 *
 * \param[in] s the input string to encode
 * \return a newly allocated b64r string.  Empty if failed.
 */
inline std::string b64r_encode(const std::string& s)
{
    return b64r_encode(s.data(), s.size());
}

/** \brief self-modifing b64r encode
 *
 * web-safe base 64 decode a string (self-modifing)
 * On failure, the string is empty.
 *
 * \param[in,out] s the string to be decoded
 * \return a reference to the input string
 */
inline std::string& b64r_encode(std::string& s)
{
    std::string x(b64r_encode(s.data(), s.size()));
    s.swap(x);
    return s;
}

inline std::string b64r_decode(const char* src, size_t len)
{
    std::string x(modp_b64r_decode_len(len)+1, '\0');
    size_t d = modp_b64r_decode(const_cast<char*>(x.data()), src,
                                static_cast<int>(len));
    if (d == (size_t)-1) {
        x.clear();
    } else {
        x.erase(d, std::string::npos);
    }
    return x;
}

inline std::string b64r_decode(const char* src)
{
    return b64r_decode(src, strlen(src));
}

/**
 * base 64 decode a string (self-modifing)
 * On failure, the string is empty.
 *
 * This function is for C++ only (duh)
 *
 * \param[in,out] s the string to be decoded
 * \return a reference to the input string
 */
inline std::string& b64r_decode(std::string& s)
{
    std::string x(b64r_decode(s.data(), s.size()));
    s.swap(x);
    return s;
}

inline std::string b64r_decode(const std::string& s)
{
    return b64r_decode(s.data(), s.size());
}
    
} // namespace

#pragma mark-
    
std::string
Base64::decode(const std::string& data)
{
   return b64r_decode(data);
}

std::string
Base64::encode(const std::string& data)
{
    return b64r_encode(data);
}
    
}