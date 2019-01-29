#pragma once
#include <cstddef>

namespace mbio {

template <typename T>
struct ConstBuffer {
    const char* data() const {
        return static_cast<const T*>(this)->data();
    }

    size_t size() const {
        return static_cast<const T*>(this)->size();
    }
};

template <typename T>
struct MutableBuffer : public ConstBuffer<T> {
    char* data() {
        return static_cast<T*>(this)->data();
    }
};

template <typename T>
struct Reader {
    template <typename B>
    ssize_t read(MutableBuffer<B> & buf) {
        return static_cast<T*>(this)->read(buf);
    }
};

template <typename T>
struct Writer {
    template <typename B>
    ssize_t write(ConstBuffer<B> const& buf) {
        return static_cast<T*>(this)->write(buf);
    }
};


template <typename T>
struct Closer {
    enum class eCloseType { CLOSE_READ, CLOSE_WRITE, CLOSE_ALL };

    int close(eCloseType type) {
        return static_cast<T*>(this)->close(type);
    }
};

} // namespace mbio
