#include "hairobject.h"

#include "hair.h"
#include "errorchecker.h"
#include "simulation.h"

HairObject::HairObject(int _numGuideHairs, Simulation *_simulation)
{
    for (int i = 0; i < _numGuideHairs; i++)
    {
        m_guideHairs.append(new Hair(2, 2, glm::vec3(i + 0.25, 1, 0), glm::vec3(1, 0, 0)));
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
