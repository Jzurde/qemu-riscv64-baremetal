#include "baremetal_printfs.h"

int main() {
    int i = 5;
    float f = 3.1415; // 定義は可能
    printf("Hello, baremetal world! This is the %dth time of trying!\n", i);
    printf("Value of i: %d\n", i);
    while (1);
    return 0;
}