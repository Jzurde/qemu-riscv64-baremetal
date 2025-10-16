#include <stdint.h>
#include <stdarg.h>

#ifndef BAREMETAL_PRINTFS_H
#define BAREMETAL_PRINTFS_H

#define UART_BASE 0x10000000

void print(const char* str);
void print_int(int value);
void print_float(float f, int decimal_precision);
int get_pow(int base, int exp);
void printf(const char* fmt, ...);
void print_uint64(uint64_t value);
int get_int_digits(int value);

volatile uint8_t* const uart = (uint8_t*)UART_BASE;

inline void print_int(int value) {
    char buf[12];
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

inline void print_uint64(uint64_t value) {
    char buf[21];
    int i = 0;

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

inline void print_float(float f, int decimal_precision=3) {    
    uint32_t bits = *(uint32_t*)&f;

    printf("\n===== float =====\n");
    printf("\n[");
    for(int i = 31; i >= 0; i--){
        printf("%d", (bits >> i) & 1);
        if(i % 4 == 0) printf(" ");
    }
    printf("]\n");

    int sign = (bits >> 31) & 1;
    int raw_exp = (bits >> 23) & 0xFF;
    int exp  = raw_exp - 127;

    if(raw_exp == 0x00){
        if((bits & 0x7FFFFF) == 0){
            if(sign) printf("-");
            printf("0.");
            for(int i = 0; i < decimal_precision; i++) printf("0");
            return;
        } else {
            printf("0.");
            for(int i = 0; i < decimal_precision; i++) printf("0");
            return;
        }
    } else if(raw_exp == 0xFF){
        if((bits & 0x7FFFFF) == 0){
            if(sign) printf("-");
            printf("inf");
            return;
        } else {
            printf("NaN");
            return;
        }
    }
    if(exp < -22){
        if(sign) printf("-");
        printf("0.");
        for(int i = 0; i < decimal_precision; i++) printf("0");
        return;
    } else if(exp > 30){
        if(sign) printf("-");
        printf("overfw");
        return;
    }

    uint32_t frac = bits & 0x7FFFFF;
    uint64_t mantissa = frac | 0x800000;
    int int_part = (int)(mantissa >> (23 - exp));
    int int_part_digits = get_int_digits(int_part);
    int frac_max_digits = (int_part_digits < 7) ? 7 - int_part_digits : 0;
    if(decimal_precision > frac_max_digits) decimal_precision = frac_max_digits;
    
    uint64_t frac_part_bits = mantissa & ((1LL << (23 - exp)) - 1);

    uint64_t frac_top=0, frac_bottom;
    int scale_exp = 23;
    for(int i = 0; i < scale_exp; i++){
        uint64_t bit = (frac_part_bits >> (22 - exp - i)) & 1;
        // printf("bit %d: %d\n", i, bit);
        frac_top = bit << (scale_exp - 1 - i) | frac_top; // bit * 1/2^(i+1) + frac_top
    }
    frac_bottom = 1 << scale_exp;

    uint64_t frac_part = (frac_top * get_pow(10, decimal_precision)) / frac_bottom;
    int most_right_digit = ((frac_top * get_pow(10, decimal_precision) * 10) / frac_bottom) % 10;
    if(most_right_digit >= 5) frac_part++; // rounding
    int display_bias = decimal_precision - get_int_digits(frac_part);

    if (sign) printf("-");
    print_int(int_part);
    printf(".");
    for(int i = 0; i < display_bias; i++) printf("0");
    print_uint64(frac_part);
}

inline void printf(const char* fmt, ...) {
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
            //     double d = va_arg(args, double);
            //     uint64_t bits = (*(uint64_t*)&d);
            //     float f = *(float*)&bits;
            //     print_float(f);
            } else {
                *uart = *fmt;
            }
        } else {
            *uart = *fmt;
        }
        fmt++;
    }

    va_end(args);
}

// DO NOT USE FROM OTHER FILES
void print(const char* str) {
    while (*str) {
        *uart = *str++;
    }
}

int get_int_digits(int value){
    int digits = 0;
    if(value == 0) return 1;
    while(value > 0){
        digits++;
        value /= 10;
    }
    return digits;
}

int get_pow(int base, int exp){
    int result = 1;
    for(int i = 0; i < exp; i++) result *= base;
    return result;
}

#endif // BAREMETAL_PRINTFS_H


