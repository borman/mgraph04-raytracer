#ifndef V3_GEN_H
#define V3_GEN_H

#include <cmath>
#include <algorithm>

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

  F scalar() const { return m_x; }
  F x() const { return m_x; }
  F y() const { return m_y; }
  F z() const { return m_z; }

  v3 operator-() const { return v3(-m_x, -m_y, -m_z); }
  v3 operator+(v3 op) const { return v3(m_x+op.m_x, m_y+op.m_y, m_z+op.m_z); }
  v3 operator-(v3 op) const { return v3(m_x-op.m_x, m_y-op.m_y, m_z-op.m_z); }
  v3 operator*(v3 op) const { return v3(m_x*op.m_x, m_y*op.m_y, m_z*op.m_z); }
  v3 operator/(v3 op) const { return v3(m_x/op.m_x, m_y/op.m_y, m_z/op.m_z); }
  v3 operator*(F op) const { return v3(m_x*op, m_y*op, m_z*op); }
  v3 operator/(F op) const { return v3(m_x/op, m_y/op, m_z/op); }

  void operator+=(v3 op) { m_x+=op.m_x; m_y+=op.m_y; m_z+=op.m_z; }
  void operator*=(v3 op) { m_x*=op.m_x; m_y*=op.m_y; m_z*=op.m_z; }
  void operator/=(v3 op) { m_x/=op.m_x; m_y/=op.m_y; m_z/=op.m_z; }

  F m_x, m_y, m_z;
};

// Reversed scalar-multiplication
OP v3<F> operator*(F s, v3<F> v) { return v*s; }

// Dot product
OP v3<F> vdot(v3<F> u, v3<F> v) { return v3<F>(u.m_x*v.m_x + u.m_y*v.m_y + u.m_z*v.m_z); }

// Cross product
OP v3<F> vcross(v3<F> u, v3<F> v)
{
  return v3<F>(u.m_y*v.m_z - u.m_z*v.m_y,
               u.m_z*v.m_x - u.m_x*v.m_z,
               u.m_x*v.m_y - u.m_y*v.m_x);
}

// Higher-level functions
OP v3<F> vlength(v3<F> v) { return v3<F>(sqrtf(vdot(v, v).scalar())); }
OP v3<F> vnormalize(v3<F> v) { return v / vlength(v); }

OP v3<F> vmax(v3<F> a, v3<F> b) { return v3<F>(max(a.m_x, b.m_x), max(a.m_y, b.m_y), max(a.m_z, b.m_z)); }
OP v3<F> vmin(v3<F> a, v3<F> b) { return v3<F>(min(a.m_x, b.m_x), min(a.m_y, b.m_y), min(a.m_z, b.m_z)); }
OP v3<F> vmaxcomp(v3<F> u) { return v3<F>(max(max(u.m_x, u.m_y), u.m_z)); }

OP v3<F> vabs(v3<F> v)
{
  return v3<F>(abs(v.m_x), abs(v.m_y), abs(v.m_z));
}

#undef OP

#endif // V3_GEN_H
