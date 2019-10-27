#include "direction.h"
#include "stack.h"
int stack::internal::getStackDirection(int *addr) {
    int fun_local;
    if (addr < &fun_local) {
        printf("Stack grows upward\n");
        return SDU;
    }
    printf("Stack grows downward\n");
    return SDD;
}

int stack::getStackDirection() {
    int main_local;
    return internal().getStackDirection(&main_local);
}
