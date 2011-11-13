#ifndef V3_GEN_H
#define V3_GEN_H

#include <cmath>
#include <algorithm>
#include <QDebug>

using std::min;
using std::max;

// Hide template-specific visual garbage
#define OP template<typename F> inline static

// A generalized 3-vector
template<typename F>
struct v3
{
  explicit v3(F _x, F _y, F _z) : m_x(_x), m_y(_y), m_z(_z) {}
  explicit v3(F _w = F()): m_x(_w), m_y(_w), m_z(_w) {}

  float x() const { return m_x; }
  float y() const { return m_y; }
  float z() const { return m_z; }

  v3 operator-() const { return v3(-m_x, -m_y, -m_z); }
  v3 operator+(v3 op) const { return v3(m_x+op.m_x, m_y+op.m_y, m_z+op.m_z); }
  v3 operator-(v3 op) const { return v3(m_x-op.m_x, m_y-op.m_y, m_z-op.m_z); }
  v3 operator*(v3 op) const { return v3(m_x*op.m_x, m_y*op.m_y, m_z*op.m_z); }
  v3 operator*(F op) const { return v3(m_x*op, m_y*op, m_z*op); }
  v3 operator/(F op) const { return v3(m_x/op, m_y/op, m_z/op); }

  v3 &operator+=(v3 op) { m_x+=op.m_x; m_y+=op.m_y; m_z+=op.m_z; return *this; }
  v3 &operator*=(v3 op) { m_x*=op.m_x; m_y*=op.m_y; m_z*=op.m_z; return *this; }
  v3 &operator/=(F op) { m_x/=op; m_y/=op; m_z/=op; return *this; }

  F m_x, m_y, m_z;
};

// Reversed scalar-multiplication
OP v3<F> operator*(F s, v3<F> v) { return v*s; }

// Dot product
OP F dot(v3<F> u, v3<F> v) { return u.m_x*v.m_x + u.m_y*v.m_y + u.m_z*v.m_z; }

// Cross product
OP v3<F> cross(v3<F> u, v3<F> v)
{
  return v3<F>(u.m_y*v.m_z - u.m_z*v.m_y,
               u.m_z*v.m_x - u.m_x*v.m_z,
               u.m_x*v.m_y - u.m_y*v.m_x);
}

// Higher-level functions
OP F square(v3<F> v) { return dot(v, v); }
OP F length(v3<F> v) { return sqrtf(dot(v, v)); }
OP v3<F> normalize(v3<F> v) { return v / length(v); }

OP v3<F> vmax(v3<F> a, v3<F> b) { return v3<F>(max(a.m_x, b.m_x), max(a.m_y, b.m_y), max(a.m_z, b.m_z)); }
OP v3<F> vmin(v3<F> a, v3<F> b) { return v3<F>(min(a.m_x, b.m_x), min(a.m_y, b.m_y), min(a.m_z, b.m_z)); }

OP v3<F> vabs(v3<F> v)
{
  return v3<F>(abs(v.m_x), abs(v.m_y), abs(v.m_z));
}


// Debug output
OP QDebug operator<<(QDebug dbg, v3<F> p)
{
  dbg.nospace() << "(" << p.m_x << "," << p.m_y << "," << p.m_z << ")";
  return dbg.space();
}

#undef OP

#endif // V3_GEN_H
