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

        integrate2(_object);

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

}

// TODO:
// Calculate forces for maintaining position constraints
void Simulation::calculateConstraintForces(HairObject *_object)
{

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

            if (!EULER)
            {
                auto omegaDot = [rodLength, omega, I](double theta, double omega) {

                    return (-G / rodLength) * sin(theta) - B * omega / I;

                };

                auto thetaDot = [](double theta, double omega) {

                    return omega;

                };

                omega = omega + Integrator::rk4(omegaDot, theta, omega, timeStep);
                theta = theta + Integrator::rk4(thetaDot, theta, omega, timeStep);

            }
            else
            {
                float thetaPrimePrime = -G / rodLength * sin(theta);
                cout << "Theta prime prime " << thetaPrimePrime + 0.0 << endl;

                float thetaPrime = vert->omega + thetaPrimePrime * timeStep;
                vert->omega = thetaPrime;
                cout << "Theta prime " << thetaPrime + 0.0 << endl;

                theta = theta + thetaPrime * timeStep;
                cout << "Theta " << theta + 0.0 << endl;

            }

            vert->theta = theta;
            vert->omega = omega;
            vert->position.x = pivotVert->position.x + rodLength * sin(theta);
            vert->position.y = pivotVert->position.y + rodLength * cos(theta);

        }
    }
}

void Simulation::integrate2(HairObject *_object)
{
    for (int i = 0; i < _object->m_guideHairs.size(); i++)
    {
        float numVerts = _object->m_guideHairs.at(i)->m_vertices.size();
        for (int j = 2; j < numVerts; j++){

            double timeStep = .01;

            HairVertex *pivot = _object->m_guideHairs.at(i)->m_vertices.at(j - 2);
            HairVertex *v1    = _object->m_guideHairs.at(i)->m_vertices.at(j - 1);
            HairVertex *v2    = _object->m_guideHairs.at(i)->m_vertices.at(j);

            glm::vec3 rodVector1 = v1->position - pivot->position;
            glm::vec3 rodVector2 = v2->position - v1->position;

            float l1 = glm::length(rodVector1);
            float l2 = glm::length(rodVector2);

            float theta1 = v1->theta;
            float theta2 = v2->theta;
            float omega1 = v1->omega;
            float omega2 = v2->omega;

            float m1 = 10.0;
            float m2 = 1.0;

            auto omega1Dot = [l1, l2, theta2, omega2, m1, m2](double theta1, double omega1)
            {

                return ((-G * (2.0 * m1 + m2) * sin(theta1))
                        - (m2 * G * sin(theta1 - 2.0 * theta2))
                        - (2.0 * sin(theta1 - theta2) * m2 * (omega2 * omega2 * l2 + omega1 * omega1 * l1 * cos(theta1 - theta2))))
                        /
                        (l1 * (2.0 * m1 + m2 - m2 * cos(2.0 * theta1 - 2.0 * theta2))) - 0.05 * omega1 / (m1 * l1 * l1);

            };

            auto omega2Dot = [l1, l2, theta1, omega1, m1, m2](double theta2, double omega2)
            {

                return ((2.0 * sin(theta1 - theta2) * (omega1 * omega1 * l1 * (m1 + m2)))
                        + (G * (m1 + m2) * cos(theta1))
                        + (omega2 * omega2 * l2 * m2 * cos(theta1 - theta2)))
                        /
                        (l2 * (2.0 * m1 + m2 - m2 * cos(2.0 * theta1 - 2.0 * theta2))) - 0.05 * omega2 / (m2 * l2 * l2);

            };


            auto thetaDot = [](double theta, double omega) {

                return omega;

            };

            v1->omega = omega1 + Integrator::rk4(omega1Dot, theta1, omega1, timeStep);
            v2->omega = omega2 + Integrator::rk4(omega2Dot, theta2, omega2, timeStep);
            v1->theta = theta1 + Integrator::rk4(thetaDot,  theta1, omega1, timeStep);
            v2->theta = theta2 + Integrator::rk4(thetaDot,  theta2, omega2, timeStep);

            v1->position.x = pivot->position.x + l1 * sin(v1->theta);
            v1->position.y = pivot->position.y + l1 * cos(v1->theta);
            v2->position.x = v1->position.x    + l2 * sin(v2->theta);
            v2->position.y = v1->position.y    + l2 * cos(v2->theta);

        }
    }
}

