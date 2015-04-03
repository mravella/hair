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

#define G   -9.8f
#define B   0.1f
#define MASS 1.0f

#define EULER false
#define __BMONTELL_MODE__ false

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

    if (__BMONTELL_MODE__){

        sinDerp(_object);

    } else {

        calculateExternalForces(_object);
        calculateConstraintForces(_object);

        integrate(_object);

    }

}

void Simulation::sinDerp(HairObject *_object){
    for (int i = 0; i < _object->m_guideHairs.size(); i++){
        int _numVertices = _object->m_guideHairs.at(i)->m_vertices.size();

        for (int j = 0; j < _numVertices; j++){
            HairVertex *_vert = _object->m_guideHairs.at(i)->m_vertices.at(j);

            _vert->position.x = 0.5 * sin(2 * m_time + 6 * (float)j / _numVertices);
            _vert->position.y = 1 - 2.f * j / (_numVertices-1);

        }
    }
}


// TODO:
// Calculate linear & angular forces for each joint, for each external force included in the simulation
void Simulation::calculateExternalForces(HairObject *_object)
{
    for (int i = 0; i < _object->m_guideHairs.size(); i++)
    {
//        for (int j = 0; j < _object->m_guideHairs.at(i)->m_joints.size(); j++)
//        {
//            Joint *_joint = _object->m_guideHairs.at(i)->m_joints.at(j);
            // calculate linear force
//            _joint->linearForce = glm::vec3(0, 0, 0);

            // calculate angular force
//            _joint->angularForce = glm::vec3(0, 0, 0);
//        }
    }
}

// TODO:
// Calculate forces for maintaining position constraints
void Simulation::calculateConstraintForces(HairObject *_object){
    for (int i = 0; i < _object->m_guideHairs.size(); i++)
    {
//        for (int j = 0; j < _object->m_guideHairs.at(i)->m_joints.size(); j++)
//        {
//            Joint *_joint = _object->m_guideHairs.at(i)->m_joints.at(j);
//            _joint->constraintForce = glm::vec3(0, 0, 0);
//        }
    }
}


// TODO:
// Euler integration of forces, per vertex per hair
void Simulation::integrate(HairObject *_object)
{
    for (int i = 0; i < _object->m_guideHairs.size(); i++)
    {
        float numVerts = _object->m_guideHairs.at(i)->m_vertices.size();
        for (int j = 1; j < numVerts; j++){

            double timeStep = .02;

            HairVertex *vert = _object->m_guideHairs.at(i)->m_vertices.at(j);
            HairVertex *pivotVert = _object->m_guideHairs.at(i)->m_vertices.at(j-1);

            cout << /*"j " << j <<*/ ", "<< glm::to_string(vert->position) << ", "<< glm::to_string(pivotVert->position) << endl;

            // Treat previous vertex at pendulum pivot, so rod length is length between two vertices
            glm::vec3 rodVector = vert->position - pivotVert->position;
            cout << "Rod vector " << glm::to_string(rodVector) << endl;

            float rodLength = glm::length(rodVector);
            cout << "Rod length " << rodLength + 0.0 << endl;

            // Moment of inertia for point mass is mR^2
            float I = MASS * rodLength * rodLength;
            cout << "Moment of inertia " << I + 0.0 << endl;

            // Variables to update
            float theta = vert->theta;
            float omega = vert->omega;

            /**
             * I * a = sum of torques
             * I * a = gravity torque + friction torque
             * gravity = -R * m * g * sin(theta)
             * friction = -b * w (where b is damping constant and w is angular velocity)
             * I * a = -R * m * g * sin(theta) - b * w
             * m * R^2 * a = -R * m * g * sin(theta) - b * w
             * a = -g / R * sin(theta) - b * w / (m * R^2)
             **/

            if (!EULER){
                auto omegaDot = [this, rodLength, omega, I](double theta, double omega) {

                    return (-G / rodLength) * sin(theta) - B * omega / I;

                };
                vert->omega = vert->omega + Integrator::rk4(omegaDot, theta, vert->omega, timeStep);

                auto thetaDot = [this, rodLength](double theta, double omega) {

                    return omega;

                };
                theta = theta + Integrator::rk4(thetaDot, theta, vert->omega, timeStep);


            } else {
                // EULER
                float thetaPrimePrime = -G / rodLength * sin(theta);

                cout << "Theta prime prime " << thetaPrimePrime + 0.0 << endl;

                // How do I get the timestep?
                float thetaPrime = vert->omega + thetaPrimePrime * timeStep;
                vert->omega = thetaPrime;
                cout << "Theta prime " << thetaPrime + 0.0 << endl;

                theta = theta + thetaPrime * timeStep;
                cout << "Theta " << theta + 0.0 << endl;

            }

            vert->theta = theta;
            _object->m_guideHairs.at(i)->m_vertices.at(j)->position.x = _object->m_guideHairs.at(i)->m_vertices.at(j-1)->position.x + rodLength * sin(theta);
            _object->m_guideHairs.at(i)->m_vertices.at(j)->position.y = _object->m_guideHairs.at(i)->m_vertices.at(j-1)->position.y + rodLength * cos(theta);

        }
    }
}


