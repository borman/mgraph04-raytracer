#ifndef FLATCAMERA_H
#define FLATCAMERA_H

#include "float3.h"

class FlatCamera
{
public:
  FlatCamera() {}
  FlatCamera(float fov, float aspect,
             const float3 &cam, const float3 &lookat, const float3 &up);

  // map (x, y) to a ray; x and y vary from 0.0 to 1.0
  float3 project(float x, float y) const { return normalize(m_origin + x*m_x + y*m_y); }
  float3 pos() const { return m_pos; }
private:
  float3 m_pos;
  float3 m_origin, m_x, m_y;
};

#endif // FLATCAMERA_H
