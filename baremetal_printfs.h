#include <stdint.h>
#include <stdarg.h>

#ifndef BAREMETAL_PRINTFS_H
#define BAREMETAL_PRINTFS_H

#define UART_BASE 0x10000000
volatile uint8_t* const uart = (uint8_t*)UART_BASE;

// 文字列を UART に出力
void print(const char* str) {
    while (*str) {
        *uart = *str++;
    }
}

// 整数を UART に出力
void print_int(int value) {
    char buf[12]; // -2147483648 まで対応
    int i = 0;

    if (value < 0) {
        *uart = '-';
        value = -value;
    }

    if (value == 0) {
        *uart = '0';
        return;
    }

    while (value > 0) {
        buf[i++] = '0' + (value % 10);
        value /= 10;
    }

    while (i--) {
        *uart = buf[i];
    }
}

// // 浮動小数点を UART に出力（小数4桁固定）
// void print_float(float f) {
//     int int_part = (int)f;
//     int frac_part = (int)((f - int_part) * 10000); // 小数4桁

//     print_int(int_part);
//     *uart = '.';

//     if (frac_part < 0) frac_part = -frac_part;

//     // 桁揃え（小数4桁固定）
//     int div = 1000;
//     for (int i = 0; i < 4; i++) {
//         *uart = '0' + (frac_part / div);
//         frac_part %= div;
//         div /= 10;
//     }
// }

// 簡易 printf
void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            if (*fmt == 'd') {
                int val = va_arg(args, int);
                print_int(val);
            } else if (*fmt == 's') {
                const char* s = va_arg(args, const char*);
                print(s);
            // } else if (*fmt == 'f') {
            //     float f = (float)va_arg(args, double); // float は promote で double になる
            //     print_float(f);
            } else {
                *uart = *fmt; // 未対応フォーマットはそのまま出力
            }
        } else {
            *uart = *fmt;
        }
        fmt++;
    }

    va_end(args);
}

#endif // BAREMETAL_PRINTFS_H


