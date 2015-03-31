#include "simulation.h"

/*
 * @file simulation.cpp
 *
 * Performs simulation of Simulation as chain
 *
 * SHOULD THIS BE STATIC?!?!?!?!?
 */

#include "hairobject.h"
#include "mike/hair.h"

Simulation::Simulation()
{
    m_time = 0;
}

Simulation::~Simulation()
{
}

void Simulation::update(float _time){
    m_time = _time;
}

void Simulation::simulate(HairObject *_object)
{

    calculateExternalForces(_object);
    calculateConstraintForces(_object);

    integrate(_object);

}


// TODO:
// Calculate linear & angular forces for each joint, for each external force included in the simulation
void Simulation::calculateExternalForces(HairObject *_object)
{
    for (int i = 0; i < _object->m_guideHairs.size(); i++)
    {
        for (int j = 0; j < _object->m_guideHairs.at(i)->m_joints.size(); j++)
        {
            Joint *_joint = _object->m_guideHairs.at(i)->m_joints.at(j);
            // calculate linear force
            _joint->linearForce = glm::vec3(0, 0, 0);

            // calculate angular force
            _joint->angularForce = glm::vec3(0, 0, 0);
        }
    }
}

// TODO:
// Calculate forces for maintaining position constraints
void Simulation::calculateConstraintForces(HairObject *_object){
    for (int i = 0; i < _object->m_guideHairs.size(); i++)
    {
        for (int j = 0; j < _object->m_guideHairs.at(i)->m_joints.size(); j++)
        {
            Joint *_joint = _object->m_guideHairs.at(i)->m_joints.at(j);
            _joint->constraintForce = glm::vec3(0, 0, 0);
        }
    }
}


// TODO:
// Euler integration of forces, per vertex per hair
void Simulation::integrate(HairObject *_object)
{
    for (int i = 0; i < _object->m_guideHairs.size(); i++)
    {
        // calculate acceleration

        // calculate velocity

        // calculate position
        float numVerts = _object->m_guideHairs.at(i)->m_vertices.size();
        for (int j = 0; j < numVerts; j++){
            HairVertex *_vert = _object->m_guideHairs.at(i)->m_vertices.at(j);

            _vert->position.x = 0.05 * sin(8 * (m_time + (float)j / numVerts));


        }
    }
}


