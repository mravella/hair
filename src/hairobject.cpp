#include "hairobject.h"

#include "hair.h"
#include "simulation.h"

HairObject::HairObject(int _numGuideHairs, Simulation *_simulation)
{
    for (int i = 0; i < _numGuideHairs; i++)
    {
        m_guideHairs.append(new Hair(1, 1, glm::vec3(i, 0, 0), glm::vec3(1, 0, 0)));
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
    for (int i = 0; i < m_guideHairs.size(); i++)
    {
        m_guideHairs.at(i)->paint(program);
    }
}
