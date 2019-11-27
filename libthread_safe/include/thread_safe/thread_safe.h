#ifndef THREAD_SAFE
#define THREAD_SAFE

#include <mutex> // lock
#include <cassert> // assert
#include <type_traits> // operator existance checking
#include <iostream> // << >> overload
#include <functional> // std::function, function pointer templating
#include "thread_safe_lock.h"

struct thread_safeExceptions {
    // throw() is deprecated and only has been used before C++11.
    // Since C++11 it got changed to noexcept
    struct NoValueStored : std::exception {
        const char * what () const noexcept;
    };
    struct NullptrDereference : std::exception {
        const char * what () const noexcept;
    };
    struct NonPointerInstantiationDisallowed : std::exception {
        const char * what () const noexcept;
    };
    struct AllLocksNotUnique : std::exception {
        const char * what () const noexcept;
    };
};

/* TODO: resolve deadlock
 *  https://cpplang.slack.com/archives/C21PKDHSL/p1573630236047600
 *  suppose you have two of these, A and B.
 *  and suppose you have thread 1 calling `A += B` and thread 2 calling `B += A`.
 *  1 locks A while 2 locks B. then 1 tries to lock B and 2 tries to lock A but neither is ever released.
 */

#define OPERATOR_WITH_ARGUMENT(R, type, rightType, leftValue, OP, rightValue) R operator OP (type rightType) { \
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock(); \
        if (!this->stored) throw thread_safeExceptions::NoValueStored(); \
        return (leftValue) OP (rightValue); \
    }

#define OPERATOR_WITH_ARGUMENT_NON_CONST(R, type, rightType, leftValue, OP, rightValue) R operator OP (type rightType) { \
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock(); \
        if (!this->stored) throw thread_safeExceptions::NoValueStored(); \
        return (leftValue) OP (rightValue); \
    }

#define POINTER_OPERATOR_WITH_ARGUMENT(R, type, rightType, leftValue, OP, rightValue) R operator OP (type rightType) { \
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock(); \
        /* nullptr translates to 0 when cast as an integer type */ \
        return reinterpret_cast<R>(reinterpret_cast<uintptr_t>(leftValue) OP ((rightValue) * sizeof(*(leftValue)))); \
    }

#define POINTER_OPERATOR_WITH_ARGUMENT_SELF(R, type, TYPE, leftType, rightType, leftValue, OP, rightValue) R operator OP (type rightType) { \
        if (&(leftType) != &(rightType)) { \
            Lock::SCOPED_LOCK guard = (leftType).lock.scoped_lock((rightType).lock); \
            /* nullptr translates to 0 when cast as an integer type */ \
            return reinterpret_cast<R>(reinterpret_cast<uintptr_t>(leftValue) OP ((rightValue) * sizeof(*(leftValue)))); \
        } else { \
            Lock::SCOPED_LOCK guard = (leftType).lock.scoped_lock(); \
            return reinterpret_cast<R>(reinterpret_cast<uintptr_t>(leftValue) OP (rightValue * sizeof(*(leftValue)))); \
        } \
    }

#define POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT(R, type, rightType, leftValue, OP, rightValue) R operator OP##= (type rightType) { \
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock(); \
        /* nullptr translates to 0 when cast as an integer type */ \
        this->value = reinterpret_cast<R>(reinterpret_cast<uintptr_t>(leftValue) OP ((rightValue) * sizeof(*(leftValue)))); \
        return this->value; \
    }

#define POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(R, type, TYPE, leftType, rightType, leftValue, OP, rightValue) R operator OP##= (type rightType) { \
        if (&(leftType) != &(rightType)) { \
            Lock::SCOPED_LOCK guard = (leftType).lock.scoped_lock((rightType).lock); \
            /* nullptr translates to 0 when cast as an integer type */ \
            this->value = reinterpret_cast<R>(reinterpret_cast<uintptr_t>(leftValue) OP ((rightValue) * sizeof(*(leftValue)))); \
            return this->value; \
        } else { \
            Lock::SCOPED_LOCK guard = (leftType).lock.scoped_lock(); \
            /* nullptr translates to 0 when cast as an integer type */ \
            this->value = reinterpret_cast<R>(reinterpret_cast<uintptr_t>(leftValue) OP ((rightValue) * sizeof(*(leftValue)))); \
            return this->value; \
        } \
    }

template<typename T>
struct thread_safe_CORE {
    mutable Lock lock;

    T value;
    bool stored = false;

    template<typename To>
    To recast() {
        return reinterpret_cast<To>(value);
    }
};

template<typename T>
struct thread_safe_POINTER_AND_VALUE: public thread_safe_CORE<T> {
    thread_safe_POINTER_AND_VALUE() {}

    thread_safe_POINTER_AND_VALUE(T data) {
        store(data);
    }

    // COPY CONSTRUCTOR
    thread_safe_POINTER_AND_VALUE(const thread_safe_POINTER_AND_VALUE &p2) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        this->value = p2.value;
        this->stored = p2.stored;
    }

    void store(T data) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        this->value = data;
        this->stored = true;
    }

    T load() {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return this->value;
    }

    thread_safe_POINTER_AND_VALUE<T> &operator=(thread_safe_POINTER_AND_VALUE<T> &data) {
        if (this == &data) return *this;
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock(data.lock);
        if (!data.stored) throw thread_safeExceptions::NoValueStored();
        this->value = data.value;
        this->stored = true;
        return *this;
    }

    thread_safe_POINTER_AND_VALUE<T> &operator=(const T &data) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        this->value = data;
        this->stored = true;
        return *this;
    }

    T fetch_add(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, this->value + val);
    }

    T fetch_sub(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, this->value - val);
    }

    T fetch_mul(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, this->value * val);
    }

    T fetch_div(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, this->value / val);
    }

    T fetch_and(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, this->value & val);
    }

    T fetch_or(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, this->value | val);
    }

    T fetch_xor(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, this->value ^ val);
    }

    T fetch_not(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, ~val);
    }

    OPERATOR_WITH_ARGUMENT(T, thread_safe_POINTER_AND_VALUE, x, this->value, +, x.value);
    OPERATOR_WITH_ARGUMENT(T, thread_safe_POINTER_AND_VALUE, x, this->value, -, x.value);
    OPERATOR_WITH_ARGUMENT(T, thread_safe_POINTER_AND_VALUE, x, this->value, *, x.value);
    OPERATOR_WITH_ARGUMENT(T, thread_safe_POINTER_AND_VALUE, x, this->value, /, x.value);
    OPERATOR_WITH_ARGUMENT(T, thread_safe_POINTER_AND_VALUE, x, this->value, &, x.value);
    OPERATOR_WITH_ARGUMENT(T, thread_safe_POINTER_AND_VALUE, x, this->value, |, x.value);
    OPERATOR_WITH_ARGUMENT(T, thread_safe_POINTER_AND_VALUE, x, this->value, ^, x.value);
    OPERATOR_WITH_ARGUMENT_NON_CONST(T, thread_safe_POINTER_AND_VALUE, x, this->value, +=, x.value);
    OPERATOR_WITH_ARGUMENT_NON_CONST(T, thread_safe_POINTER_AND_VALUE, x, this->value, -=, x.value);
    OPERATOR_WITH_ARGUMENT_NON_CONST(T, thread_safe_POINTER_AND_VALUE, x, this->value, *=, x.value);
    OPERATOR_WITH_ARGUMENT_NON_CONST(T, thread_safe_POINTER_AND_VALUE, x, this->value, /=, x.value);
    OPERATOR_WITH_ARGUMENT_NON_CONST(T, thread_safe_POINTER_AND_VALUE, x, this->value, &=, x.value);
    OPERATOR_WITH_ARGUMENT_NON_CONST(T, thread_safe_POINTER_AND_VALUE, x, this->value, |=, x.value);
    OPERATOR_WITH_ARGUMENT_NON_CONST(T, thread_safe_POINTER_AND_VALUE, x, this->value, ^=, x.value);

    OPERATOR_WITH_ARGUMENT(T, T, x, this->value, +, x);
    OPERATOR_WITH_ARGUMENT(T, T, x, this->value, -, x);
    OPERATOR_WITH_ARGUMENT(T, T, x, this->value, *, x);
    OPERATOR_WITH_ARGUMENT(T, T, x, this->value, /, x);
    OPERATOR_WITH_ARGUMENT(T, T, x, this->value, &, x);
    OPERATOR_WITH_ARGUMENT(T, T, x, this->value, |, x);
    OPERATOR_WITH_ARGUMENT(T, T, x, this->value, ^, x);
    OPERATOR_WITH_ARGUMENT_NON_CONST(T, T, x, this->value, +=, x);
    OPERATOR_WITH_ARGUMENT_NON_CONST(T, T, x, this->value, -=, x);
    OPERATOR_WITH_ARGUMENT_NON_CONST(T, T, x, this->value, *=, x);
    OPERATOR_WITH_ARGUMENT_NON_CONST(T, T, x, this->value, /=, x);
    OPERATOR_WITH_ARGUMENT_NON_CONST(T, T, x, this->value, &=, x);
    OPERATOR_WITH_ARGUMENT_NON_CONST(T, T, x, this->value, |=, x);
    OPERATOR_WITH_ARGUMENT_NON_CONST(T, T, x, this->value, ^=, x);
    
    T operator++(int) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return this->value++;
    }

    T operator--(int) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return this->value--;
    }

    friend std::ostream &operator<<(std::ostream &stream, const thread_safe_POINTER_AND_VALUE<T> &data) {
        Lock::SCOPED_LOCK guard = data.lock.scoped_lock();
        if (!data.stored) throw thread_safeExceptions::NoValueStored();
        return stream << data.value;
    };

    friend std::istream &operator>>(std::istream &stream, const thread_safe_POINTER_AND_VALUE<T> &data) {
        Lock::SCOPED_LOCK guard = data.lock.scoped_lock();
        if (!data.stored) throw thread_safeExceptions::NoValueStored();
        return stream >> data.value;
    };
};

template<typename T>
struct thread_safe_POINTER_AND_VALUE<T*>: public thread_safe_CORE<T> {

    bool stored_by_index = false; // should this be made private?
    T* original_address = nullptr; // should this be made private?

    T* value = nullptr;

    thread_safe_POINTER_AND_VALUE() {}

    thread_safe_POINTER_AND_VALUE(std::nullptr_t data) {
        store(data);
    }

    thread_safe_POINTER_AND_VALUE(T data) {
        store(0, data);
    }

    thread_safe_POINTER_AND_VALUE(T* data) {
        store(data);
    }

    // COPY CONSTRUCTOR
    thread_safe_POINTER_AND_VALUE(const thread_safe_POINTER_AND_VALUE<T*> &p2) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        this->value = p2.value;
        this->stored = p2.stored;
    }

    void store(std::nullptr_t data) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (this->stored_by_index) {
            delete this->original_address;
            this->original_address = nullptr;
            this->stored_by_index = false;
        }
        this->value = data;
        this->stored = true;
    }

    void store(int index, T data) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (this->value == nullptr) {
            this->value = new T;
            this->original_address = this->value;
        }
        this->value[index] = const_cast<T>(data);
        this->stored = true;
        this->stored_by_index = true;
    }

    void store(T* data) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (this->stored_by_index) {
            delete this->original_address;
            this->original_address = nullptr;
            this->stored_by_index = false;
        }
        this->value = const_cast<T *>(data);
        this->stored = true;
    }

    T* load() {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        return this->value;
    }

    T * load(int index) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (this->value == nullptr) throw thread_safeExceptions::NullptrDereference();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return this->value[index];
    }

    thread_safe_POINTER_AND_VALUE<T*> &operator=(const std::nullptr_t &data) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (this->stored_by_index) {
            delete this->original_address;
            this->original_address = nullptr;
            this->stored_by_index = false;
        }
        this->value = data;
        this->stored = true;
        return *this;
    }

    thread_safe_POINTER_AND_VALUE<T*> &operator=(thread_safe_POINTER_AND_VALUE<T*> &data) {
        if (this == &data) return *this;
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock(data.lock);
        if (this->stored_by_index) {
            delete this->original_address;
            this->original_address = nullptr;
            this->stored_by_index = false;
        }
        if (!data.stored) throw thread_safeExceptions::NoValueStored();
        this->value = data.value;
        this->stored = true;
        return *this;
    }

    thread_safe_POINTER_AND_VALUE<T*> &operator=(const T * data) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (this->stored_by_index) {
            delete this->original_address;
            this->original_address = nullptr;
            this->stored_by_index = false;
        }
        this->value = const_cast<T *>(data);
        this->stored = true;
        return *this;
    }

    thread_safe_POINTER_AND_VALUE<T*> &operator=(const T &data) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (this->stored_by_index) {
            delete this->original_address;
            this->original_address = nullptr;
            this->stored_by_index = false;
        }
        this->value[0] = const_cast<T *>(data);
        this->stored = true;
        return *this;
    }

    T* fetch_add(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this->value) + val));
    }

    T fetch_add(int index, T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (this->value == nullptr) throw thread_safeExceptions::NullptrDereference();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value[index], this->value[index] + val);
    }

    T* fetch_sub(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this->value) - val));
    }

    T fetch_sub(int index, T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (this->value == nullptr) throw thread_safeExceptions::NullptrDereference();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value[index], this->value[index] - val);
    }

    T* fetch_mul(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this->value) * val));
    }

    T fetch_mul(int index, T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (this->value == nullptr) throw thread_safeExceptions::NullptrDereference();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value[index], this->value[index] * val);
    }

    T* fetch_div(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this->value) / val));
    }

    T fetch_div(int index, T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (this->value == nullptr) throw thread_safeExceptions::NullptrDereference();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value[index], this->value[index] / val);
    }

    T* fetch_and(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, this->value & val);
    }

    T fetch_and(int index, T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (this->value == nullptr) throw thread_safeExceptions::NullptrDereference();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value[index], this->value[index] & val);
    }

    T* fetch_or(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, this->value | val);
    }

    T fetch_or(int index, T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (this->value == nullptr) throw thread_safeExceptions::NullptrDereference();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value[index], this->value[index] | val);
    }

    T* fetch_xor(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, this->value ^ val);
    }

    T fetch_xor(int index, T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (this->value == nullptr) throw thread_safeExceptions::NullptrDereference();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value[index], this->value[index] ^ val);
    }

    T* fetch_not(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, ~val);
    }

    T fetch_not(int index, T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (this->value == nullptr) throw thread_safeExceptions::NullptrDereference();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value[index], ~val);
    }

    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, +, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, -, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, *, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, /, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, &, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, |, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, ^, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, +, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, -, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, *, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, /, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, &, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, |, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, ^, reinterpret_cast<uintptr_t>(x.value));

    POINTER_OPERATOR_WITH_ARGUMENT(T*, T, x, this->value, +, x);
    POINTER_OPERATOR_WITH_ARGUMENT(T*, T, x, this->value, -, x);
    POINTER_OPERATOR_WITH_ARGUMENT(T*, T, x, this->value, *, x);
    POINTER_OPERATOR_WITH_ARGUMENT(T*, T, x, this->value, /, x);
    POINTER_OPERATOR_WITH_ARGUMENT(T*, T, x, this->value, &, x);
    POINTER_OPERATOR_WITH_ARGUMENT(T*, T, x, this->value, |, x);
    POINTER_OPERATOR_WITH_ARGUMENT(T*, T, x, this->value, ^, x);
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT(T*, T, x, this->value, +, x);
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT(T*, T, x, this->value, -, x);
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT(T*, T, x, this->value, *, x);
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT(T*, T, x, this->value, /, x);
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT(T*, T, x, this->value, &, x);
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT(T*, T, x, this->value, |, x);
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT(T*, T, x, this->value, ^, x);

    T* operator++(int) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this->value) + sizeof(*this->value)));
    }

    T* operator--(int) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this->value) - sizeof(*this->value)));
    }

    friend std::ostream &operator<<(std::ostream &stream, const thread_safe_POINTER_AND_VALUE<T*> &data) {
        Lock::SCOPED_LOCK guard = data.lock.scoped_lock();
        return stream << data.value;
    };

    friend std::istream &operator>>(std::istream &stream, const thread_safe_POINTER_AND_VALUE<T*> &data) {
        Lock::SCOPED_LOCK guard = data.lock.scoped_lock();
        return stream >> data.value;
    };

};


template<typename T>
struct thread_safe_POINTER_ONLY: public thread_safe_CORE<T> {
    // prevent instantiation for thread_safe_POINTER_ONLY<int>
    static_assert(std::is_pointer_v<T>, "this type can only be instantiated on pointers, required: T* , given: T");

    T* value = nullptr;

    thread_safe_POINTER_ONLY() {}

    thread_safe_POINTER_ONLY(std::nullptr_t data) {
        store(data);
    }

    thread_safe_POINTER_ONLY(T* data) {
        store(data);
    }

    // COPY CONSTRUCTOR
    thread_safe_POINTER_ONLY(const thread_safe_POINTER_ONLY<T*> &p2) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        this->value = p2.value;
        this->stored = p2.stored;
    }

    void store(std::nullptr_t data) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        this->value = data;
        this->stored = true;
    }

    void store(T* data) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        this->value = data;
        this->stored = true;
    }

    T* load() {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        return this->value;
    }

    thread_safe_POINTER_ONLY<T*> &operator=(const std::nullptr_t &data) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        this->value = const_cast<std::nullptr_t>(data);
        this->stored = true;
        return *this;
    }

    thread_safe_POINTER_ONLY<T*> &operator=(thread_safe_POINTER_ONLY<T*> &data) {
        if (this == &data) return *this;
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock(data.lock);
        if (!data.stored) throw thread_safeExceptions::NoValueStored();
        this->value = data.value;
        this->stored = true;
        return *this;
    }

    thread_safe_POINTER_ONLY<T*> &operator=(const T * data) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        this->value = const_cast<T*>(data);
        this->stored = true;
        return *this;
    }

    T* fetch_add(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this->value) + val));
    }

    T* fetch_sub(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this->value) - val));
    }

    T* fetch_mul(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this->value) * val));
    }

    T* fetch_div(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this->value) / val));
    }

    T* fetch_and(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, this->value & val);
    }

    T* fetch_or(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, this->value | val);
    }

    T* fetch_xor(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, this->value ^ val);
    }

    T* fetch_not(T val) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, ~val);
    }
    
    // valid for thread_safe_POINTER_AND_VALUE only if it references a pointer

    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, +, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, -, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, *, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, /, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, &, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, |, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, ^, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, +, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, -, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, *, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, /, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, &, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, |, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_AND_VALUE<T*>, T, *this, x, this->value, ^, reinterpret_cast<uintptr_t>(x.value));

    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_ONLY<T*>, T, *this, x, this->value, +, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_ONLY<T*>, T, *this, x, this->value, -, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_ONLY<T*>, T, *this, x, this->value, *, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_ONLY<T*>, T, *this, x, this->value, /, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_ONLY<T*>, T, *this, x, this->value, &, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_ONLY<T*>, T, *this, x, this->value, |, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ARGUMENT_SELF(T*, thread_safe_POINTER_ONLY<T*>, T, *this, x, this->value, ^, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_ONLY<T*>, T, *this, x, this->value, +, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_ONLY<T*>, T, *this, x, this->value, -, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_ONLY<T*>, T, *this, x, this->value, *, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_ONLY<T*>, T, *this, x, this->value, /, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_ONLY<T*>, T, *this, x, this->value, &, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_ONLY<T*>, T, *this, x, this->value, |, reinterpret_cast<uintptr_t>(x.value));
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT_SELF(T*, thread_safe_POINTER_ONLY<T*>, T, *this, x, this->value, ^, reinterpret_cast<uintptr_t>(x.value));

    POINTER_OPERATOR_WITH_ARGUMENT(T*, T, x, this->value, +, x);
    POINTER_OPERATOR_WITH_ARGUMENT(T*, T, x, this->value, -, x);
    POINTER_OPERATOR_WITH_ARGUMENT(T*, T, x, this->value, *, x);
    POINTER_OPERATOR_WITH_ARGUMENT(T*, T, x, this->value, /, x);
    POINTER_OPERATOR_WITH_ARGUMENT(T*, T, x, this->value, &, x);
    POINTER_OPERATOR_WITH_ARGUMENT(T*, T, x, this->value, |, x);
    POINTER_OPERATOR_WITH_ARGUMENT(T*, T, x, this->value, ^, x);
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT(T*, T, x, this->value, +, x);
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT(T*, T, x, this->value, -, x);
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT(T*, T, x, this->value, *, x);
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT(T*, T, x, this->value, /, x);
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT(T*, T, x, this->value, &, x);
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT(T*, T, x, this->value, |, x);
    POINTER_OPERATOR_WITH_ASSIGNMENT_ARGUMENT(T*, T, x, this->value, ^, x);

    T* operator++(int) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this->value) + sizeof(*this->value)));
    }

    T* operator--(int) {
        Lock::SCOPED_LOCK guard = this->lock.scoped_lock();
        if (!this->stored) throw thread_safeExceptions::NoValueStored();
        return std::exchange(this->value, reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this->value) - sizeof(*this->value)));
    }

    friend std::ostream &operator<<(std::ostream &stream, const thread_safe_POINTER_ONLY<T*> &data) {
        Lock::SCOPED_LOCK guard = data.lock.scoped_lock();
        return stream << data.value;
    };

    friend std::istream &operator>>(std::istream &stream, const thread_safe_POINTER_ONLY<T*> &data) {
        Lock::SCOPED_LOCK guard = data.lock.scoped_lock();
        return stream >> data.value;
    };

};

// ensure self nestability

template <typename T>
struct thread_safeInternalEnsureNestable_POINTER_AND_VALUE {
    using type = thread_safe_POINTER_AND_VALUE<T>;
};
template <typename T>
struct thread_safeInternalEnsureNestable_POINTER_AND_VALUE<thread_safe_POINTER_AND_VALUE<T>> {
    using type = thread_safe_POINTER_AND_VALUE<T>;
};

template <typename T>
using thread_safe = typename thread_safeInternalEnsureNestable_POINTER_AND_VALUE<T>::type;

template <typename T>
using Thread_Safe = thread_safe <T>;

template <typename T>
using thread_safeParameterType = thread_safe_POINTER_AND_VALUE <T>;

template <typename T>
using Thread_SafeParameterType = thread_safe_POINTER_AND_VALUE <T>;

// pointer only

template <typename T>
struct thread_safeInternalEnsureNestable_POINTER_ONLY {
    using type = thread_safe_POINTER_ONLY<T>;
};
template <typename T>
struct thread_safeInternalEnsureNestable_POINTER_ONLY<thread_safe_POINTER_ONLY<T>> {
    using type = thread_safe_POINTER_ONLY<T>;
};

template <typename T>
using thread_safePointer = typename thread_safeInternalEnsureNestable_POINTER_ONLY<T>::type;

template <typename T>
using Thread_SafePointer = thread_safePointer<T>;

template <typename T>
using thread_safePointerParameterType = thread_safe_POINTER_ONLY<T>;

template <typename T>
using Thread_SafePointerParameterType = thread_safe_POINTER_ONLY<T>;

// externally overloads

// ==

// thread_safe == thread_safe

template<typename T1, typename T2>
bool operator == (const thread_safeParameterType <T1> &a, const thread_safeParameterType <T2> &b) {
    if (&a == &b) return true;
    Lock::SCOPED_LOCK guard = a.lock.scoped_lock(b.lock);
    if (!(a.stored && b.stored)) throw thread_safeExceptions::NoValueStored();
    return a.value == b.value;
}

template<typename T1, typename T2>
bool operator == (const thread_safeParameterType <T1*> &a, const thread_safeParameterType <T2*> &b) {
    if (&a == &b) return true;
    Lock::SCOPED_LOCK guard = a.lock.scoped_lock(b.lock);
    return a.value == b.value;
}

template<typename T1, typename T2>
bool operator == (const thread_safePointerParameterType <T1*> &a, const thread_safePointerParameterType <T2*> &b) {
    if (&a == &b) return true;
    Lock::SCOPED_LOCK guard = a.lock.scoped_lock(b.lock);
    return a.value == b.value;
}

// thread_safe == T

template<typename T1, typename T2>
bool operator == (const thread_safeParameterType <T1> &a, const T2 & b) {
    Lock::SCOPED_LOCK guard = a.lock.scoped_lock();
    if (!a.stored) throw thread_safeExceptions::NoValueStored();
    return a.value == b;
}

template<typename T1, typename T2>
bool operator == (const thread_safeParameterType <T1*> &a, const T2* b) {
    Lock::SCOPED_LOCK guard = a.lock.scoped_lock();
    return a.value == b;
}

template<typename T1, typename T2>
bool operator == (const thread_safePointerParameterType <T1*> &a, const T2* b) {
    Lock::SCOPED_LOCK guard = a.lock.scoped_lock();
    return a.value == b;
}

// T == thread_safe

template<typename T1, typename T2>
bool operator == (const T1 &a, const thread_safeParameterType <T2> & b) {
    Lock::SCOPED_LOCK guard = b.lock.scoped_lock();
    if (!b.stored) throw thread_safeExceptions::NoValueStored();
    return a == b.value;
}

template<typename T1, typename T2>
bool operator == (const T1* &a, const thread_safeParameterType <T2*> b) {
    Lock::SCOPED_LOCK guard = b.lock.scoped_lock();
    return a == b.value;
}

template<typename T1, typename T2>
bool operator == (const T1* &a, const thread_safePointerParameterType <T2*> b) {
    Lock::SCOPED_LOCK guard = b.lock.scoped_lock();
    return a == b.value;
}

// !=

// thread_safe != thread_safe

template<typename T1, typename T2>
bool operator != (const thread_safeParameterType <T1> &a, const thread_safeParameterType <T2> &b) {
    if (&a == &b) return false;
    Lock::SCOPED_LOCK guard = a.lock.scoped_lock(b.lock);
    if (!(a.stored && b.stored)) throw thread_safeExceptions::NoValueStored();
    return a.value[0] != b.value[0];
}

template<typename T1, typename T2>
bool operator != (const thread_safeParameterType <T1*> &a, const thread_safeParameterType <T2*> &b) {
    if (&a == &b) return false;
    Lock::SCOPED_LOCK guard = a.lock.scoped_lock(b.lock);
    return a.value != b.value;
}

template<typename T1, typename T2>
bool operator != (const thread_safePointerParameterType <T1*> &a, const thread_safePointerParameterType <T2*> &b) {
    if (&a == &b) return false;
    Lock::SCOPED_LOCK guard = a.lock.scoped_lock(b.lock);
    return a.value != b.value;
}

// thread_safe != T

template<typename T1>
bool operator != (const thread_safeParameterType <T1> &a, const std::nullptr_t & b) {
    Lock::SCOPED_LOCK guard = a.lock.scoped_lock();
    return a.value != b;
}

template<typename T1, typename T2>
bool operator != (const thread_safeParameterType <T1> &a, const T2 & b) {
    Lock::SCOPED_LOCK guard = a.lock.scoped_lock();
    if (!a.stored) throw thread_safeExceptions::NoValueStored();
    return a.value[0] != b;
}

template<typename T1, typename T2>
bool operator != (const thread_safeParameterType <T1*> &a, const T2* b) {
    Lock::SCOPED_LOCK guard = a.lock.scoped_lock();
    return a.value != b;
}

template<typename T1, typename T2>
bool operator != (const thread_safePointerParameterType <T1*> &a, const T2* b) {
    Lock::SCOPED_LOCK guard = a.lock.scoped_lock();
    return a.value != b;
}

// T != thread_safe

template<typename T2>
bool operator != (const std::nullptr_t &a, const thread_safeParameterType <T2> & b) {
    Lock::SCOPED_LOCK guard = b.lock.scoped_lock();
    return a != b.value;
}

template<typename T1, typename T2>
bool operator != (const T1 &a, const thread_safeParameterType <T2> & b) {
    Lock::SCOPED_LOCK guard = b.lock.scoped_lock();
    if (!b.stored) throw thread_safeExceptions::NoValueStored();
    return a != b.value[0];
}

template<typename T1, typename T2>
bool operator != (const T1* &a, const thread_safeParameterType <T2*> b) {
    Lock::SCOPED_LOCK guard = b.lock.scoped_lock();
    return a != b.value;
}

template<typename T1, typename T2>
bool operator != (const T1* &a, const thread_safePointerParameterType <T2*> b) {
    Lock::SCOPED_LOCK guard = b.lock.scoped_lock();
    return a != b.value;
}

#endif