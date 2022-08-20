#pragma once

#include "pch.h"

namespace fuse::math {
    template<typename T>
    struct vec2 {
        T x = 0, y = 0;

        FUSE_INLINE vec2(T x, T y) : x(x), y(y) {}
        FUSE_INLINE vec2(T v) : x(v), y(v) {}
        FUSE_INLINE vec2() = default;

        FUSE_INLINE vec2<T> operator+(const vec2<T> &v) const {
            return vec2<T>(x + v.x, y + v.y);
        }

        FUSE_INLINE vec2<T> &operator+=(const vec2<T> &v) {
            x += v.x;
            y += v.y;
            return *this;
        }

        FUSE_INLINE vec2<T> operator-(const vec2<T> &v) const {
            return vec2<T>(x - v.x, y - v.y);
        }

        FUSE_INLINE vec2<T> &operator-=(const vec2<T> &v) {
            x -= v.x;
            y -= v.y;
            return *this;
        }

        FUSE_INLINE vec2<T> operator*(T s) const {
            return vec2<T>(x * s, y * s);
        }

        FUSE_INLINE vec2<T> &operator*=(T s) {
            x *= s;
            y *= s;
            return *this;
        }
    };
}

namespace fuse {
    using vec2u = math::vec2<uint32_t>;
    using vec2f = math::vec2<float>;
    using vec2i = math::vec2<int>;
}