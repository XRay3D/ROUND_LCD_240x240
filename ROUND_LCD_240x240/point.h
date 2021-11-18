#pragma once

#include <algorithm>
//#include <concepts>
#include <stdint.h>

constexpr static inline bool isNull(double d) {
    union {
        double d;
        uint64_t u;
    } val { .d = d };
    return (val.u & uint64_t(0x7fffffffffffffff)) == 0;
}

constexpr static inline bool isNull(float f) {
    union {
        float f;
        uint32_t u;
    } val { .f = f };
    return (val.u & uint32_t(0x7fffffff)) == 0;
}

template <class T>
constexpr T abs_(T val) { return val < 0 ? -val : +val; }

constexpr static inline bool fuzzyCompare(double p1, double p2) {
    return (abs_(p1 - p2) * 1000000000000. <= std::min(abs_(p1), abs_(p2)));
}

constexpr static inline bool fuzzyCompare(float p1, float p2) {
    return (abs_(p1 - p2) * 100000.f <= std::min(abs_(p1), abs_(p2)));
}

constexpr static inline bool fuzzyIsNull(double d) {
    return abs_(d) <= 0.000000000001;
}

constexpr static inline bool fuzzyIsNull(float f) {
    return abs_(f) <= 0.00001f;
}

template <class /*std::integral*/ T>
constexpr inline T round_(double d) { return d >= 0.0 ? T(d + 0.5) : T(d - double(T(d - 1)) + 0.5) + T(d - 1); }
template <class /*std::integral*/ T>
constexpr inline T round_(float d) { return d >= 0.0f ? T(d + 0.5f) : T(d - float(T(d - 1)) + 0.5f) + T(d - 1); }

/*************************************************/

template <class T>
class Point {
public:
    constexpr Point();
    constexpr Point(T xpos, T ypos);
    //    template <class U>
    //    constexpr Point(const Point<U>& p)
    //        : xp(p.x())
    //        , yp(p.y()) { }
    constexpr inline T manhattanLength() const;

    inline bool isNull() const;

    constexpr inline T x() const;
    constexpr inline T y() const;
    constexpr inline void setX(T x);
    constexpr inline void setY(T y);

    constexpr inline T& rx();
    constexpr inline T& ry();
    template <class U>
    constexpr inline Point<T>& operator+=(const Point<U>& p);
    template <class U>
    constexpr inline Point<T>& operator-=(const Point<U>& p);
    template <class U>
    constexpr inline Point<T>& operator*=(U c);
    template <class U>
    constexpr inline Point<T>& operator/=(U c);

    template <class U, class W>
    constexpr static inline T dotProduct(const Point<U>& p1, const Point<W>& p2) { return p1.xp * p2.xp + p1.yp * p2.yp; }

    template <class U, class W>
    friend constexpr inline bool operator==(const Point<U>&, const Point<W>&);
    template <class U, class W>
    friend constexpr inline bool operator!=(const Point<U>&, const Point<W>&);
    template <class U, class W>
    friend constexpr inline const Point<std::common_type_t<U, T>> operator+(const Point<U>&, const Point<W>&);
    template <class U, class W>
    friend constexpr inline const Point<std::common_type_t<U, T>> operator-(const Point<U>&, const Point<W>&);
    template <class U>
    friend constexpr inline const Point<T> operator*(U, const Point<T>&);
    template <class U>
    friend constexpr inline const Point<T> operator*(const Point<T>&, U);
    template <class U>
    friend constexpr inline const Point<T> operator+(const Point<U>&);
    template <class U>
    friend constexpr inline const Point<T> operator-(const Point<U>&);
    template <class U>
    friend constexpr inline const Point<T> operator/(const Point<T>&, U);
    template <class U>
    constexpr Point<U> toPoint() const;

private:
    T xp;
    T yp;
};

/*****************************************************************************
  Point inline functions
 *****************************************************************************/
template <class T>
constexpr inline Point<T>::Point()
    : xp(0)
    , yp(0) { }

template <class T>
constexpr inline Point<T>::Point(T xpos, T ypos)
    : xp(xpos)
    , yp(ypos) { }

//template <class T>
//template <class U>
//constexpr inline Point<T>::Point(const Point<U>& p)
//    : xp(p.x())
//    , yp(p.y()) { }

template <class T>
constexpr inline T Point<T>::manhattanLength() const { return abs_(xp) + abs_(yp); }

template <class T>
inline bool Point<T>::isNull() const { return isNull(xp) && isNull(yp); }

template <class T>
constexpr inline T Point<T>::x() const {
    return xp;
}

template <class T>
constexpr inline T Point<T>::y() const {
    return yp;
}

template <class T>
constexpr inline void Point<T>::setX(T xpos) {
    xp = xpos;
}

template <class T>
constexpr inline void Point<T>::setY(T ypos) {
    yp = ypos;
}

template <class T>
constexpr inline T& Point<T>::rx() {
    return xp;
}

template <class T>
constexpr inline T& Point<T>::ry() {
    return yp;
}

template <class T>
template <class U>
constexpr inline Point<T>& Point<T>::operator+=(const Point<U>& p) {
    xp += p.xp;
    yp += p.yp;
    return *this;
}

template <class T>
template <class U>
constexpr inline Point<T>& Point<T>::operator-=(const Point<U>& p) {
    xp -= p.xp;
    yp -= p.yp;
    return *this;
}

template <class T>
template <class U>
constexpr inline Point<T>& Point<T>::operator*=(U c) {
    xp *= c;
    yp *= c;
    return *this;
}

template <class U, class W>
constexpr inline bool operator==(const Point<U>& p1, const Point<W>& p2) {
    return ((!p1.xp || !p2.xp) ? fuzzyIsNull(p1.xp - p2.xp) : fuzzyCompare(p1.xp, p2.xp))
        && ((!p1.yp || !p2.yp) ? fuzzyIsNull(p1.yp - p2.yp) : fuzzyCompare(p1.yp, p2.yp));
}

template <class U, class W>
constexpr inline bool operator!=(const Point<U>& p1, const Point<W>& p2) {
    return !(p1 == p2);
}

template <class U, class W>
constexpr inline const Point<std::common_type_t<U, W>> operator+(const Point<U>& p1, const Point<W>& p2) {
    return Point(p1.xp + p2.xp, p1.yp + p2.yp);
}

template <class U, class W>
constexpr inline const Point<std::common_type_t<U, W>> operator-(const Point<U>& p1, const Point<W>& p2) {
    return Point(p1.xp - p2.xp, p1.yp - p2.yp);
}

template <class T, class U>
constexpr inline const Point<T> operator*(const Point<T>& p, U c) {
    return Point(p.xp * c, p.yp * c);
}

template <class T, class U>
constexpr inline const Point<T> operator*(T c, const Point<U>& p) {
    return Point(p.xp * c, p.yp * c);
}

template <class T, class U>
constexpr inline const Point<T> operator+(const Point<U>& p) {
    return p;
}

template <class T, class U>
constexpr inline const Point<T> operator-(const Point<U>& p) {
    return Point(-p.xp, -p.yp);
}

template <class T>
template <class U>
constexpr inline Point<T>& Point<T>::operator/=(U divisor) {
    xp /= divisor;
    yp /= divisor;
    return *this;
}

template <class T, class U>
constexpr inline const Point<T> operator/(const Point<T>& p, U divisor) {
    return Point<T>(p.xp / divisor, p.yp / divisor);
}

template <class T>
template <class U>
constexpr inline Point<U> Point<T>::toPoint() const {
    if constexpr (std::is_floating_point_v<T> && std::is_integral_v<U>)
        return Point<U>(round_<U>(xp), round_<U>(yp));
    return Point<U>(xp, yp);
}

using PointU16 = Point<uint16_t>;
