#include <cmath>
#include "FlatCamera.h"

FlatCamera::FlatCamera(float fov, float aspect,
                       const float3 &cam, const float3 &lookat, const float3 &_up)
{
  m_pos = cam;

  float3 camv = vnormalize(lookat - cam);
  float3 right = vnormalize(vcross(camv, _up));
  float3 up = vnormalize(vcross(right, camv));

  m_x = tan(fov) * right;
  m_y = (tan(fov) / aspect) * up;

  m_origin = camv - float(0.5)*m_x - float(0.5)*m_y;
}
