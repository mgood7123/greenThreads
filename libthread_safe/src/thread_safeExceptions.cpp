//
// Created by brothercomplex on 3/11/19.
//

#include <thread_safe/thread_safe.h>

template <typename Mutex>
struct MutexPointer {
    Mutex* ptr = nullptr;

    constexpr MutexPointer() noexcept = default;
    constexpr MutexPointer(Mutex& m) noexcept : ptr(&m) {}

    constexpr bool unlock_if() {
        if (ptr) {
            ptr->unlock();
        }
        return true;
    }
};

template <typename... Mutexes>
class scoped_lock {
    std::tuple<MutexPointer<Mutexes>...> m_locks{};

    static auto get_locks(Mutexes&... ms) {
        std::lock(ms...);
        return std::tuple<MutexPointer<Mutexes>...>(ms...);
    }

    template <size_t... Is>
    void release_locks(std::index_sequence<Is...>) {
        (void)((std::get<Is>( m_locks ).unlock_if( )) && ...);
    }

public:
    scoped_lock(Mutexes&... ms) : m_locks(get_locks(ms...)) {}

    ~scoped_lock() {
        release_locks(std::make_index_sequence<sizeof...(Mutexes)>{});
    }
};
template <typename... Mutexes>
scoped_lock(Mutexes&...) -> scoped_lock<Mutexes...>;

const char * thread_safeExceptions::NoValueStored::what() const noexcept {
    return "no value has been stored, please store a value first using the store() function, = operator, or {} brace initialization list operator";
}