#ifndef FLOAT3_SSE_H
#define FLOAT3_SSE_H

#ifdef USE_SSE4
# include "smmintrin.h"
#else
# include "emmintrin.h"
#endif

class float3
{
public:
  explicit float3(float _x, float _y, float _z) { d = _mm_set_ps(0.0f, _z, _y, _x); }
  explicit float3(float w = 0.0f) { d = _mm_set1_ps(w); }
//private:
  float3(__m128 _d) : d(_d) {}

public:
  float scalar() const { return fs[0]; }
  float x() const { return fs[0]; }
  float y() const { return fs[1]; }
  float z() const { return fs[2]; }

  float3 operator+(float3 p) const { return _mm_add_ps(d, p.d); }
  float3 operator-(float3 p) const { return _mm_sub_ps(d, p.d); }
  float3 operator*(float3 p) const { return _mm_mul_ps(d, p.d); }
  float3 operator/(float3 p) const { return _mm_div_ps(d, p.d); }

  void operator+=(float3 p) { d = _mm_add_ps(d, p.d); }
  void operator-=(float3 p) { d = _mm_sub_ps(d, p.d); }
  void operator*=(float3 p) { d = _mm_mul_ps(d, p.d); }
  void operator/=(float3 p) { d = _mm_div_ps(d, p.d); }

  float3 operator*(float p) const { return (*this) * float3(p); }
  float3 operator/(float p) const { return (*this) / float3(p); }
  void operator*=(float p) { (*this) *= float3(p); }
  void operator/=(float p) { (*this) /= float3(p); }

  float3 operator-() const
  {
    return (*this)*(-1.0f);
  }

//private:
  union {
    __m128 d;
    float fs[4];
  };
};

static inline float3 operator*(float p, float3 v) { return v*p; }

static inline float3 vmin(float3 u, float3 v) { return _mm_min_ps(u.d, v.d); }
static inline float3 vmax(float3 u, float3 v) { return _mm_max_ps(u.d, v.d); }

static inline float3 vmaxcomp(float3 u)
{
  __m128 u1 = _mm_shuffle_ps(u.d, u.d, _MM_SHUFFLE(3,0,2,1));
  __m128 u2 = _mm_shuffle_ps(u.d, u.d, _MM_SHUFFLE(3,1,0,2));
  return vmax(vmax(u, u1), u2);
}

static inline float3 vabs(float3 u)
{
  static const unsigned int mask = 0x7fffffff;
  return _mm_and_ps(u.d, _mm_load1_ps((float *) &mask));
}

static inline float3 vsqrt(float3 u)
{
  return _mm_sqrt_ps(u.d);
}

static inline float3 vdot(float3 u, float3 v)
{
#ifdef USE_SSE4
  return _mm_dp_ps(u.d, v.d, 0x77); // SSE4
#else
  float3 t = u*v;
  float3 r = t + _mm_shuffle_ps(t.d, t.d, _MM_SHUFFLE(3,0,2,1));
  return r + _mm_shuffle_ps(t.d, t.d, _MM_SHUFFLE(3,1,0,2));
#endif
}

static inline float3 vcross(float3 u, float3 v)
{
  __m128 u1 = _mm_shuffle_ps(u.d, u.d, _MM_SHUFFLE(3,0,2,1));
  __m128 v2 = _mm_shuffle_ps(v.d, v.d, _MM_SHUFFLE(3,1,0,2));

  __m128 u2 = _mm_shuffle_ps(u.d, u.d, _MM_SHUFFLE(3,1,0,2));
  __m128 v1 = _mm_shuffle_ps(v.d, v.d, _MM_SHUFFLE(3,0,2,1));

  __m128 u1v2 = _mm_mul_ps(u1, v2);
  __m128 u2v1 = _mm_mul_ps(u2, v1);

  return _mm_sub_ps(u1v2, u2v1);
}

static inline float3 vlength(float3 u)
{
  return vsqrt(vdot(u, u));
}

static inline float3 vnormalize(float3 u)
{
  return u/vlength(u);
}

#endif // FLOAT3_SSE_H
