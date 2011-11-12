#ifndef HITACCUM_H
#define HITACCUM_H

#include "float3.h"

class HitAccum
{
  public:
    HitAccum()
      : m_isEmpty(true) {}

    bool isEmpty() const { return m_isEmpty; }

    float z() const { return m_z; }
    float3 normal() const { return m_normal; }
    int steps() const { return m_steps; }

    void add(float z, const float3 &normal, int steps)
    {
      if (m_isEmpty || z < m_z)
      {
        m_z = z;
        m_normal = normal;
        m_steps = steps;
        m_isEmpty = false;
      }
    }
  private:
    bool m_isEmpty;

    float m_z;
    float3 m_normal;
    int m_steps;
};

#endif // HITACCUM_H
