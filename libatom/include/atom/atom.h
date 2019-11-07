#ifndef ATOM
#define ATOM

#include <mutex> // lock
#include <cassert> // assert
#include <type_traits> // operator existance checking
#include <iostream> // << >> overload

struct atomExceptions {
    struct NoValueStored : std::exception {
        // throw() is deprecated and only has been used before C++11.
        // Since C++11 it got changed to noexcept
        const char * what () const noexcept;
    };
};

#define OPERATOR_WITH_ARGUMENT(R, OP, type) R operator OP (type operatorArgument) { \
        std::lock_guard guard {this->lock}; \
        if (!this->stored) throw atomExceptions::NoValueStored(); \
        return this->value OP operatorArgument; \
    }

template<typename T>
class atomINTERNALCORE {
public:
    mutable std::mutex lock;
    T value;
    bool stored = false;

    template<typename To>
    To recast() {
        return To (value);
    }
};

template<typename T>
class atomINTERNAL: public atomINTERNALCORE<T> {
public:

    atomINTERNAL() {}

    atomINTERNAL(T data) {
        store(data);
    }

//        atomINTERNAL(initializer_list<T> c) {
//        assert(c.size() == 1);
//        value = c.begin()[0];
//        stored = true;
//    }

    void store(T data) {
        std::lock_guard guard{this->lock};
        this->value = data;
        this->stored = true;
    }

    T load() {
        std::lock_guard guard{this->lock};
        if (!this->stored) throw atomExceptions::NoValueStored();
        return this->value;
    }

    atomINTERNAL<T> &operator=(const atomINTERNAL<T> &v) {
        if (this == &v) return *this;
        std::lock_guard guard1{this->lock};
        std::lock_guard guard2{v.lock};
        if (!v.stored) throw atomExceptions::NoValueStored();
        this->value = v.value;
        this->stored = true;
        return *this;
    }

    atomINTERNAL<T> &operator=(const T &v) {
        std::lock_guard guard{this->lock};
        this->value = v;
        this->stored = true;
        return *this;
    }

    bool operator==(const atomINTERNAL<T> &v) const {
        if (this == &v) return true;
        std::lock_guard guard1{this->lock};
        std::lock_guard guard2{v.lock};
        if (!(this->stored && v.stored)) throw atomExceptions::NoValueStored();
        return this->value == v.value;
    }

    bool operator==(const T &v) const {
        std::lock_guard guard{this->lock};
        if (!this->stored) throw atomExceptions::NoValueStored();
        return this->value == v;
    }

    bool operator!=(const atomINTERNAL<T> &v) const {
        if (this == &v) return true;
        std::lock_guard guard1{this->lock};
        std::lock_guard guard2{v.lock};
        if (!(this->stored && v.stored)) throw atomExceptions::NoValueStored();
        return this->value != v.value;
    }

    bool operator!=(const T &v) const {
        std::lock_guard guard{this->lock};
        if (!this->stored) throw atomExceptions::NoValueStored();
        return this->value != v;
    }

    T fetch_add(T val) {
        std::lock_guard guard{this->lock};
        if (!this->stored) throw atomExceptions::NoValueStored();
        return std::exchange(this->value, this->value + val);
    }

    T fetch_sub(T val) {
        std::lock_guard guard{this->lock};
        if (!this->stored) throw atomExceptions::NoValueStored();
        return std::exchange(this->value, this->value - val);
    }

    T fetch_mul(T val) {
        std::lock_guard guard{this->lock};
        if (!this->stored) throw atomExceptions::NoValueStored();
        return std::exchange(this->value, this->value * val);
    }

    T fetch_div(T val) {
        std::lock_guard guard{this->lock};
        if (!this->stored) throw atomExceptions::NoValueStored();
        return std::exchange(this->value, this->value / val);
    }

    T fetch_and(T val) {
        std::lock_guard guard{this->lock};
        if (!this->stored) throw atomExceptions::NoValueStored();
        return std::exchange(this->value, this->value & val);
    }

    T fetch_or(T val) {
        std::lock_guard guard{this->lock};
        if (!this->stored) throw atomExceptions::NoValueStored();
        return std::exchange(this->value, this->value | val);
    }

    T fetch_xor(T val) {
        std::lock_guard guard{this->lock};
        if (!this->stored) throw atomExceptions::NoValueStored();
        return std::exchange(this->value, this->value ^ val);
    }

    T fetch_not(T val) {
        std::lock_guard guard{this->lock};
        if (!this->stored) throw atomExceptions::NoValueStored();
        return std::exchange(this->value, ~val);
    }

    T fetch_not() {
        std::lock_guard guard{this->lock};
        if (!this->stored) throw atomExceptions::NoValueStored();
        return std::exchange(this->value, ~this->value);
    }

    OPERATOR_WITH_ARGUMENT(T, +, T);

    OPERATOR_WITH_ARGUMENT(T, -, T);

    OPERATOR_WITH_ARGUMENT(T, *, T);

    OPERATOR_WITH_ARGUMENT(T, /, T);

    OPERATOR_WITH_ARGUMENT(T, &, T);

    OPERATOR_WITH_ARGUMENT(T, |, T);

    OPERATOR_WITH_ARGUMENT(T, ^, T);

    OPERATOR_WITH_ARGUMENT(T, +=, T);

    OPERATOR_WITH_ARGUMENT(T, -=, T);

    OPERATOR_WITH_ARGUMENT(T, *=, T);

    OPERATOR_WITH_ARGUMENT(T, /=, T);

    OPERATOR_WITH_ARGUMENT(T, &=, T);

    OPERATOR_WITH_ARGUMENT(T, |=, T);

    OPERATOR_WITH_ARGUMENT(T, ^=, T);

    T operator++(int) {
        std::lock_guard guard{this->lock};
        if (!this->stored) throw atomExceptions::NoValueStored();
        return this->value++;
    }

    T operator--(int) {
        std::lock_guard guard{this->lock};
        if (!this->stored) throw atomExceptions::NoValueStored();
        return this->value--;
    }

    friend std::ostream &operator<<(std::ostream &stream, const atomINTERNAL &v) {
        std::lock_guard guard{v.lock};
        if (!v.stored) throw atomExceptions::NoValueStored();
        return stream << v.value;
    };

    friend std::istream &operator>>(std::istream &stream, const atomINTERNAL &v) {
        std::lock_guard guard{v.lock};
        if (!v.stored) throw atomExceptions::NoValueStored();
        return stream >> v.value;
    };
};

// partial specialization on pointer types
template <typename T>
class atomINTERNAL<T*>: public atomINTERNALCORE<T*> {
public:

    T* value = nullptr; // explicitly initialize as nullptr, as a result, we do not need to throw

    atomINTERNAL() {}

    atomINTERNAL(T* data) {
        store(data);
    }

    atomINTERNAL(T data) {
        store(data);
    }

    void store(T* data) {
        std::lock_guard guard{this->lock};
        this->value = data;
        this->stored = true;
    }

    void store(T data) {
        std::lock_guard guard{this->lock};
        if (this->value == nullptr) this->value = new T;
        this->value[0] = data;
        this->stored = true;
    }

    T* load() {
        std::lock_guard guard{this->lock};
        return this->value;
    }

    atomINTERNAL<T*> &operator=(const atomINTERNAL<T*> &v) {
        if (this == &v) return *this;
        std::lock_guard guard1{this->lock};
        std::lock_guard guard2{v.lock};
        this->value = v.value;
        this->stored = true;
        return *this;
    }

    atomINTERNAL<T*> &operator=(const T* &v) {
        std::lock_guard guard{this->lock};
        this->value = v;
        this->stored = true;
        return *this;
    }

    atomINTERNAL<T*> &operator=(const T &v) {
        std::lock_guard guard{this->lock};
        if (this->value == nullptr) this->value = new T;
        this->value[0] = v;
        this->stored = true;
        return *this;
    }

    bool operator==(const atomINTERNAL<T*> &v) const {
        if (this == &v) return true;
        std::lock_guard guard1{this->lock};
        std::lock_guard guard2{v.lock};
        return this->value == v.value;
    }

    bool operator==(const T* &v) const {
        std::lock_guard guard{this->lock};
        return this->value == v;
    }

    bool operator!=(const atomINTERNAL<T*> &v) const {
        if (this == &v) return true;
        std::lock_guard guard1{this->lock};
        std::lock_guard guard2{v.lock};
        return this->value != v.value;
    }

    bool operator!=(const T* &v) const {
        std::lock_guard guard{this->lock};
        return this->value != v;
    }

    T* fetch_add(size_t val) {
        std::lock_guard guard{this->lock};
        return std::exchange(this->value, this->value + val);
    }

    T* fetch_sub(size_t val) {
        std::lock_guard guard{this->lock};
        return std::exchange(this->value, this->value - val);
    }

    OPERATOR_WITH_ARGUMENT(T*, +, size_t);

    OPERATOR_WITH_ARGUMENT(T*, -, size_t);

    OPERATOR_WITH_ARGUMENT(T*, +=, size_t);

    OPERATOR_WITH_ARGUMENT(T*, -=, size_t);

    T* operator++(int) {
        std::lock_guard guard{this->lock};
        return this->value++;
    }

    T* operator--(int) {
        std::lock_guard guard{this->lock};
        return this->value--;
    }

    friend std::ostream &operator<<(std::ostream &stream, const atomINTERNAL &v) {
        std::lock_guard guard{v.lock};
        return stream << v.value;
    };

    friend std::istream &operator>>(std::istream &stream, const atomINTERNAL &v) {
        std::lock_guard guard{v.lock};
        return stream >> v.value;
    };
};

#undef OPERATOR_WITH_ARGUMENT

// ensure self nestability

template <typename T>
struct atomInternalEnsureNestable {
    using type = atomINTERNAL<T>;
};
template <typename T>
struct atomInternalEnsureNestable<atomINTERNAL<T>> {
    using type = atomINTERNAL<T>;
};

template <typename T>
using atom = typename atomInternalEnsureNestable<T>::type;

template <typename T>
using Atom = atom<T>;

// externally required overloads

template<typename T>
bool operator == (const T & vv, const atom <T> & v) {
    std::lock_guard guard{v.lock};
    if (!v.stored) throw atomExceptions::NoValueStored();
    return vv == v.value;
}

template<typename T>
bool operator != (const T & vv, const atom <T> & v) {
    std::lock_guard guard{v.lock};
    if (!v.stored) throw atomExceptions::NoValueStored();
    return vv != v.value;
}

#endif