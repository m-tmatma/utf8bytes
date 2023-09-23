/*
    g++ -std=c++11  utf8bytes.cpp
*/
#include <iostream>
#include <vector>
#include <cstring>
#include <stdint.h>
#include <stdio.h>

#define DEBUG_PRINT(fmt, args...) printf("%s:%d " fmt, __func__, __LINE__, ##args)

/*
    https://datatracker.ietf.org/doc/html/rfc3629#section-3
        Char. number range  |        UTF-8 octet sequence
            (hexadecimal)   |              (binary)
        --------------------+---------------------------------------------
        0000 0000-0000 007F | 0xxxxxxx
        0000 0080-0000 07FF | 110xxxxx 10xxxxxx
        0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
        0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

    https://datatracker.ietf.org/doc/html/rfc3629#section-4
        UTF8-octets = *( UTF8-char )
        UTF8-char   = UTF8-1 / UTF8-2 / UTF8-3 / UTF8-4
        UTF8-1      = %x00-7F
        UTF8-2      = %xC2-DF UTF8-tail
        UTF8-3      = %xE0 %xA0-BF UTF8-tail /
                      %xE1-EC 2( UTF8-tail ) /
                      %xED %x80-9F UTF8-tail /
                      %xEE-EF 2( UTF8-tail )
        UTF8-4      = %xF0 %x90-BF 2( UTF8-tail ) /
                      %xF1-F3 3( UTF8-tail ) /
                      %xF4 %x80-8F 2( UTF8-tail )
        UTF8-tail   = %x80-BF
*/
int get_utf8_bytes(const unsigned char* p, uint32_t *val)
{
    DEBUG_PRINT("start: 0x%02x\n", *p);
    *val = 0;
    if ( (*p & 0x80) == 0) {
        DEBUG_PRINT("1: 0x%02x\n", *p);
        *val = *p;
        return 1;
    } else if ( (*p & 0xE0) == 0xC0) {
        DEBUG_PRINT("2: 0x%02x\n", *p);
        if ( (*(p + 1) & 0xC0) == 0x80){
            *val = *p << 8 | *(p + 1);
            return 2;
        }
    } else if ( (*p & 0xF0) == 0xE0) {
        DEBUG_PRINT("3: 0x%02x\n", *p);
        if ( (*(p + 1) & 0xC0) == 0x80 && (*(p + 2) & 0xC0) == 0x80) {
            *val = *p << 16 | *(p + 1) << 8 | *(p + 2);
            return 3;
        }
    } else if ( (*p & 0xF8) == 0xF0) {
        DEBUG_PRINT("4: 0x%02x\n", *p);
        if ( (*(p + 1) & 0xC0) == 0x80 && (*(p + 2) & 0xC0) == 0x80 && (*(p + 3) & 0xC) == 0x80) {
            *val = *p << 24 | *(p + 1) << 16 | *(p + 2) << 8 | *(p + 3);
            return 4;
        }
        return 4;
    }
    return 0;
}

int main() {
    unsigned char str[] = "Hello, 世界";
    unsigned char* p = str;
    int bytes = 1;
    while(*p != '\0' && bytes > 0) {
        uint32_t val = 0;
        bytes = get_utf8_bytes(p, &val);
        p += bytes;

        printf("val = %08x, bytes = %d\n", val, bytes);
    }

    return 0;
}