// Box, sphere, box, reflections, refractions

static const float3 box1Pos(0.0f, 0.0f, -0.5f);
static const float3 box1Size(5.0f, 5.0f, 0.5f);
static const int nSpheres = 5;
static const float radSpheres = 0.7f;
static float3 spherePos[nSpheres];
static float sphereRad[nSpheres];

static float sceneDist(float3 p)
{
  float3 box1 = box(p-box1Pos, box1Size);

  float3 scene = box1;
  for (int i=0; i<nSpheres; i++)
    scene = unite(scene, sphere(p-spherePos[i], sphereRad[i]));
  //scene = unite(scene, box1);
  //scene = unite(scene, sphere2);
  //scene = unite(scene, box2);
  return scene.scalar();
}

static float innerSceneDist(float3 p)
{
  return -sceneDist(p);
}

static int sceneMat(float3 p)
{
  for (int i=0; i<nSpheres; i++)
    if (vlength(p-spherePos[i]).scalar() - sphereRad[i] < 1e-4)
      return i+1;
  return 0;
}

static const float3 blueSky(0.2f, 0.4f, 1.0f);
static const float3 tintedSky(0.95f, 0.95f, 1.0f);
static const float3 sunColor(1.0f, 1.0f, 0.8f);
static const float3 sunPos(0.6f, 0.6f, 0.52f);
static float3 envColor(float3 p)
{
  float k = max(0.0f, p.z()+0.3f);
  float s = max(0.0f, vdot(p, sunPos).scalar());
  s = min(1.0f, pow(s, 10)*0.3f + pow(s, 100));
  return (k*blueSky + (1.0f-k)*tintedSky) * (1-s) + sunColor *s;
}


MyScene::MyScene()
{
  for (int i=0; i<nSpheres; i++)
  {
    //float r = sqrt(1.0f * float(i+1)/nSpheres);
    float r = 0.3f;
    float phi = 2*M_PI * float(i)/nSpheres;
    spherePos[i] = float3(radSpheres*cos(phi), radSpheres*sin(phi), r);
    sphereRad[i] = r;
  }

  sceneDist = ::sceneDist;
  innerSceneDist = ::innerSceneDist;
  matId = ::sceneMat;
  envColor = ::envColor;

  // Basement
  {
    Renderer::Material mat;
    mat.ambientColor = float3(0.2f, 0.2f, 0.2f);
    mat.diffuseColor = float3(1.0f, 1.0f, 1.0f);
    mat.specularColor = float3(1.0f, 1.0f, 1.0f);
    mat.shininess = 10;
    mat.reflect = 0.0f;
    mat.refract = 0.0f;
    mat.refractionIndex = 1.0f;
    materials.push_back(mat);

    for (int i=0; i<nSpheres; i++)
      materials.push_back(mat);
  }

  // Chromium
  materials[1].ambientColor = float3(0.0f);
  materials[1].diffuseColor = float3(0.0f);
  materials[1].shininess = 1000;
  materials[1].reflect = 1.0f;
  materials[1].refract = 0.0f;
  materials[1].refractionIndex = 8.0f;

  // Phong test
  materials[2].diffuseColor = float3(0.9f, 0.0f, 0.0f);
  materials[2].ambientColor = 0.1f * materials[2].diffuseColor;
  materials[2].specularShader = Renderer::Material::specularPhong;

  // Blinn test
  materials[3].diffuseColor = float3(0.9f, 0.0f, 0.0f);
  materials[3].ambientColor = 0.1f * materials[3].diffuseColor;

  // Something glassy
  materials[4].ambientColor = float3(0.0f, 0.0f, 0.2f);
  materials[4].diffuseColor = float3(0.0f);
  materials[4].shininess = 1000;
  materials[4].reflect = 1.0f;
  materials[4].refract = 0.5f;
  materials[4].refractionIndex = 2.3f;

  // Oren-Nayar-Blinn test
  materials[5].diffuseColor = float3(0.0f, 0.9f, 0.0f);
  materials[5].ambientColor = 0.1f * materials[5].diffuseColor;
  //materials[5].diffuseShader = Renderer::Material::diffuseOrenNayar;

  lights.push_back(
        Renderer::Light(float3(10.0f, 10.0f, 10.0f),
                        float3(1.0f, 1.0f, 1.0f),
                        1.5f, 0.0f, 0.005f));
  lights.push_back(
        Renderer::Light(float3(-5.0f, 5.0f, 4.0f),
                        float3(1.0f, 1.0f, 1.0f),
                        5.0f, 0.0f, 0.05f));
  /*
  lights.push_back(
        Renderer::Light(float3(-1.0f, -5.0f, 5.0f),
                        float3(1.0f, 1.0f, 1.0f),
                        5.0f, 0.0f, 0.01f));
                        */
}
