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

double oldTheta = 1.570;

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

            _vert->position.x = 0.05 * sin(8 * (m_time + (float)j / _numVertices));
            _vert->position.y = -1 + 2.f * j / (_numVertices-1);

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
//        for (int j = 1; j < numVerts; j++){

            double timeStep = .02;

//            HairVertex *vert = _object->m_guideHairs.at(i)->m_vertices.at(j);
            HairVertex *vert = _object->m_guideHairs.at(i)->m_vertices.at(1);
//            HairVertex *pivotVert = _object->m_guideHairs.at(i)->m_vertices.at(j-1);
            HairVertex *pivotVert = _object->m_guideHairs.at(i)->m_vertices.at(0);
            
            cout << /*"j " << j <<*/ ", "<< glm::to_string(vert->position) << ", "<< glm::to_string(pivotVert->position) << endl;

            // Treat previous vertex at pendulum pivot, so rod length is length between two vertices
            glm::vec3 rodVector = vert->position - pivotVert->position;
            cout << "Rod vector " << glm::to_string(rodVector) << endl;

            float rodLength = glm::length(rodVector);
            cout << "Rod length " << rodLength + 0.0 << endl;

            // Moment of inertia for point mass is mR^2
            float I = MASS * rodLength * rodLength;
            cout << "Moment of inertia " << I + 0.0 << endl;

            // Theta is dot of down vector and rod vector
            float theta = acos(CLAMP(glm::dot(glm::vec3(0.0, -1.0, 0.0), glm::normalize(rodVector)), -1.0, 1.0));
            theta = oldTheta;
            cout << "Theta " << theta + 0.0 << endl;

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
                auto calcOmegaDot = [this, rodLength, omega, I](double _theta, double _omega) {

                    return (-G / rodLength) * sin(_theta) - B * omega / I;

                };

                {
                    double k1 = calcOmegaDot(theta, vert->omega);
                    double k2 = calcOmegaDot(theta + .5*timeStep*k1, vert->omega + .5*timeStep*k1);
                    double k3 = calcOmegaDot(theta + .5*timeStep*k2, vert->omega + .5*timeStep*k2);
                    double k4 = calcOmegaDot(theta + timeStep*k3, vert->omega + timeStep*k3);

                    vert->omega = vert->omega + timeStep/6. * (k1 + 2*k2 + 2*k3 + k4);
                    cout << "Omega " << vert->omega << endl;
                }


                auto calcThetaDot = [this, rodLength](double _theta, double _omega) {

                    return _omega;

                };

                {
                    double k1 = calcThetaDot(theta, vert->omega);
                    double k2 = calcThetaDot(theta + .5*timeStep*k1, vert->omega + .5*timeStep*k1);
                    double k3 = calcThetaDot(theta + .5*timeStep*k2, vert->omega + .5*timeStep*k2);
                    double k4 = calcThetaDot(theta + timeStep*k3, vert->omega + timeStep*k3);

                    cout << "Delta Theta " << timeStep/6. * (k1 + 2*k2 + 2*k3 + k4) << endl;
                    theta = theta + timeStep/6. * (k1 + 2*k2 + 2*k3 + k4);
                    cout << "Theta New " << theta << endl;
                }


            } else {
                // EULER
                float thetaPrimePrime = -G / rodLength * sin(theta);
                //                                     - B * vert->omega
                //                                     / (MASS * rodLength * rodLength);

                cout << "Theta prime prime " << thetaPrimePrime + 0.0 << endl;

                // How do I get the timestep?
                float thetaPrime = vert->omega + thetaPrimePrime * timeStep;
                vert->omega = thetaPrime;
                cout << "Theta prime " << thetaPrime + 0.0 << endl;

                theta = theta + thetaPrime * timeStep;
                cout << "Theta " << theta + 0.0 << endl;

            }


            double translateX = rodLength * sin(theta) - sin(oldTheta);
            double translateY = rodLength * cos(theta) + cos(oldTheta);
            oldTheta = theta;
            _object->m_guideHairs.at(i)->m_vertices.at(1)->position.x = _object->m_guideHairs.at(i)->m_vertices.at(0)->position.x + rodLength * sin(theta);
            _object->m_guideHairs.at(i)->m_vertices.at(1)->position.y = _object->m_guideHairs.at(i)->m_vertices.at(0)->position.y + rodLength * cos(theta);


//        }
    }
}


