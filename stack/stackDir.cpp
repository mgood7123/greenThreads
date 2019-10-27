#include "stackDir.h"

int internal_getStackDir(int *addr) {
    int fun_local;
    if (addr < &fun_local) {
        printf("Stack grows upward\n");
        return SDU;
    }
    printf("Stack grows downward\n");
    return SDD;
}

int getStackDirection() {
    // fun's local variable
    int main_local;

    return internal_getStackDir(&main_local);
}
