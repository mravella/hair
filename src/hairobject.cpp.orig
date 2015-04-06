#include "hairobject.h"

#include "hair.h"
#include "errorchecker.h"
#include "simulation.h"

HairObject::HairObject(int _numGuideHairs, Simulation *_simulation)
{
    for (int i = 0; i < _numGuideHairs; i++)
    {
<<<<<<< HEAD
        m_guideHairs.append(new Hair(3, 1, glm::vec3(i + 0.25, 1, 0), glm::vec3(1, 0, 0)));
=======
        m_guideHairs.append(new Hair(2, 2, glm::vec3(i + 0.25, 1, 0), glm::vec3(1, 0, 0)));
>>>>>>> ea7c78ec9ebcb826ebac24151ae2d18c2b79ce98
    }

    m_simulation = _simulation;
}

HairObject::HairObject(ObjMesh *_mesh, const char * _hairGrowthMap, Simulation *_simulation)
{
    QImage image(_hairGrowthMap);
    std::cout << QColor(image.pixel(0,0)).value() << std::endl;
    for (int i = 0; i < _mesh->triangles.size(); i++)
    {
        Triangle t = _mesh->triangles[i];

        // If hair growth map is black, skip this triangle.
        glm::vec2 uv = (t.uv1 + t.uv2 + t.uv3) / 3.f;
        QColor hairGrowth = QColor(image.pixel(uv.x * image.width(), (1 - uv.y) * image.height()));
        if (hairGrowth.value() == 0) continue;

        glm::vec3 pos = (t.v1 + t.v2 + t.v3) / 3.f;
        glm::vec3 normal = (t.n1 + t.n2 + t.n3) / 3.f;

        // TODO: Don't set the z component to 0 when the sim works in 3D.
        normal.z = 0.f; normal = glm::normalize(normal);
        m_guideHairs.append(new Hair(2, 2, pos, normal));
    }
    m_simulation = _simulation;
}

void HairObject::update(float _time){

    if (m_simulation != NULL)
    {
        m_simulation->simulate(this);
    }

    for (int i = 0; i < m_guideHairs.size(); i++)
    {
        m_guideHairs.at(i)->update(_time);
    }
}

void HairObject::paint(ShaderProgram &program){
    program.uniforms.color = glm::vec3(.6f, .4f, .3f);
    program.uniforms.numGroupHairs = 15;
    program.uniforms.hairGroupWidth = 0.2;
    program.uniforms.hairRadius = 0.005f;
    program.uniforms.noiseAmplitude = 0.03f;
    program.uniforms.numSplineVertices = 20;
    program.setPerHairObjectUniforms();

    for (int i = 0; i < m_guideHairs.size(); i++)
    {
        m_guideHairs.at(i)->paint(program);
    }
}
