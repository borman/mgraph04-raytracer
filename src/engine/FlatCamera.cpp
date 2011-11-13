#include <cmath>
#include "FlatCamera.h"

FlatCamera::FlatCamera(float fov, float aspect,
                       const float3 &cam, const float3 &lookat, const float3 &_up)
{
  m_pos = cam;

  float3 camv = normalize(lookat - cam);
  float3 right = normalize(cross(camv, _up));
  float3 up = normalize(cross(right, camv));

  m_x = tanf(fov) * right;
  m_y = (tanf(fov) / aspect) * up;

  m_origin = camv - 0.5f*m_x - 0.5f*m_y;
}
