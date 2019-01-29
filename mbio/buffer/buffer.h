#pragma once
#include <cstddef>
#include <string>
#include "../if/if.h"

namespace mbio {

class BufferView : public MutableBuffer<BufferView> {
    char* buf_;
    size_t len_;

public:
    BufferView(char* buf, size_t len) : buf_(buf), len_(len) {}

    explicit BufferView(std::string & s) : buf_(s.data()), len_(s.size()) {}

    template <typename T>
    BufferView(MutableBuffer<T> const& b) : buf_(b.data()), len_(b.size()) {}

    const char* data() const {
        return buf_;
    }

    char* data() {
        return buf_;
    }

    size_t size() const {
        return len_;
    }
};

} //namespace mbio
