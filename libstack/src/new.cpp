#include <stack/stack.h>
void Stack::alloc(atom<size_t> size_) {
    stack = new char[size_];
    direction = getStackDirection();
    if (direction == SDU) top = stack;
    else top = stack + (size*sizeof(char));
    size = size;
}

void Stack::free() {
    if (stack == nullptr) return;
    delete[] stack;
    stack = nullptr;
    top = nullptr;
    size = 0;
    direction = 0;
}
