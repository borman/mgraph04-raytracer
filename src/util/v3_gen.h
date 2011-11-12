#ifndef V3_GEN_H
#define V3_GEN_H

#include <cmath>
#include <QDebug>

// Hide template-specific visual garbage
#define OP template<typename F> inline static

// A generalized 3-vector
template<typename F>
struct v3
{
  explicit v3(F _x = F(), F _y = F(), F _z = F()) : x(_x), y(_y), z(_z) {}

  v3 operator-() const { return v3(-x, -y, -z); }
  v3 operator+(v3 op) const { return v3(x+op.x, y+op.y, z+op.z); }
  v3 operator-(v3 op) const { return v3(x-op.x, y-op.y, z-op.z); }
  v3 operator*(v3 op) const { return v3(x*op.x, y*op.y, z*op.z); }
  v3 operator*(F op) const { return v3(x*op, y*op, z*op); }
  v3 operator/(F op) const { return v3(x/op, y/op, z/op); }

  v3 &operator+=(v3 op) { x+=op.x; y+=op.y; z+=op.z; return *this; }
  v3 &operator*=(v3 op) { x*=op.x; y*=op.y; z*=op.z; return *this; }
  v3 &operator/=(F op) { x/=op; y/=op; z/=op; return *this; }

  F x, y, z;
};

// Reversed scalar-multiplication
OP v3<F> operator*(F s, v3<F> v) { return v*s; }

// Dot product
OP F dot(v3<F> u, v3<F> v) { return u.x*v.x + u.y*v.y + u.z*v.z; }

// Cross product
OP v3<F> cross(v3<F> u, v3<F> v)
{
  return v3<F>(u.y*v.z - u.z*v.y,
               u.z*v.x - u.x*v.z,
               u.x*v.y - u.y*v.x);
}

// Higher-level functions
OP F square(v3<F> v) { return dot(v, v); }
OP F length(v3<F> v) { return sqrtf(dot(v, v)); }
OP v3<F> normalize(v3<F> v) { return v / length(v); }

// Debug output
OP QDebug operator<<(QDebug dbg, v3<F> p)
{
  dbg.nospace() << "(" << p.x << "," << p.y << "," << p.z << ")";
  return dbg.space();
}

#undef OP

#endif // V3_GEN_H
