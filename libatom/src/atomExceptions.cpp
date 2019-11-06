//
// Created by brothercomplex on 3/11/19.
//

#include <atom/atom.h>

const char * atomExceptions::NoValueStored::what() const noexcept {
    return "no value has been stored, please store a value first using the store() function, = operator, or {} brace initialization list operator";
}