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
#define B   0.3f
#define MASS 1.0f

#define EULER false
#define __BMONTELL_MODE__ false
#define TIMESTEP 0.01

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

            _vert->position.x = 0.5 * sin(2 * m_time + 6 * (float)j / _numVertices);
            _vert->position.y = 1 - 2.f * j / (_numVertices-1);

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

            // Get relevant vertices
            HairVertex *vert = _object->m_guideHairs.at(i)->m_vertices.at(j);
            HairVertex *pivotVert = _object->m_guideHairs.at(i)->m_vertices.at(j-1);

            // Treat previous vertex at pendulum pivot, so rod length is length between two vertices
            glm::vec3 rodVector = vert->position - pivotVert->position;

            // Rod length is useful for moment of inertia
            float rodLength = glm::length(rodVector);

            // Moment of inertia for point mass is mR^2
            float I = MASS * rodLength * rodLength;

            // Variables to update
            float theta = vert->theta;
            float omega = vert->omega;

            if (!EULER)
            {
                // Computes angular acceleration of a vertex
                auto omegaDot = [rodLength, omega, I](double theta, double omega) {

                    return (-G / rodLength) * sin(theta) - B * omega / I;

                };

                // Computes angular velocity of a vertex
                auto thetaDot = [](double theta, double omega) {

                    return omega;

                };

                // Integrate forward
                omega = omega + Integrator::rk4(omegaDot, theta, omega, TIMESTEP);
                theta = theta + Integrator::rk4(thetaDot, theta, omega, TIMESTEP);

            }
            else
            {
                float thetaPrimePrime = -G / rodLength * sin(theta);

                float thetaPrime = vert->omega + thetaPrimePrime * TIMESTEP;
                vert->omega = thetaPrime;

                theta = theta + thetaPrime * TIMESTEP;

            }

            // Store values and update positions
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

            // Relevant vertices
            HairVertex *pivot = _object->m_guideHairs.at(i)->m_vertices.at(j - 2);
            HairVertex *v1    = _object->m_guideHairs.at(i)->m_vertices.at(j - 1);
            HairVertex *v2    = _object->m_guideHairs.at(i)->m_vertices.at(j);

            // Vector from one vertex to the vertex above it
            glm::vec3 rodVector1 = v1->position - pivot->position;
            glm::vec3 rodVector2 = v2->position - v1->position;

            // Pendula lengths
            float l1 = glm::length(rodVector1);
            float l2 = glm::length(rodVector2);

            // Store theta and omega so we can use them later
            float theta1 = v1->theta;
            float theta2 = v2->theta;
            float omega1 = v1->omega;
            float omega2 = v2->omega;

            // TODO: These will be stored on vertices eventually
            float m1 = .5;
            float m2 = .2;

            // Calculates angular acceleration for v1
            auto omega1Dot = [l1, l2, theta2, omega2, m1, m2](double theta1, double omega1)
            {
                //TODO: refactor this
                double deltaTheta = theta1 - theta2;
                double capitalM = m2 / (m1 + m2);
                double littleL = l2 / l1;
                double wSquared = G / l1;

                return (wSquared * littleL * (-sin(theta1) + capitalM * cos(deltaTheta) * sin(theta2))
                        - capitalM * littleL * (omega1 * omega1 * cos(deltaTheta) + littleL * omega2 * omega2) * sin(deltaTheta))
                        /
                        (littleL - capitalM * littleL * cos(deltaTheta) * cos(deltaTheta)) - B * omega1 / (m1 * l1 * l1);

            };

            // Calculates angular acceleration for v2
            auto omega2Dot = [l1, l2, theta1, omega1, m1, m2](double theta2, double omega2)
            {
                // TODO: refactor this
                double deltaTheta = theta1 - theta2;
                double capitalM = m2 / (m1 + m2);
                double littleL = l2 / l1;
                double wSquared = G / l1;

                return (wSquared * cos(deltaTheta) * sin(theta1) - wSquared* sin(theta2)
                        + (omega1 * omega1 + capitalM * littleL * omega2 * omega2 * cos(deltaTheta)) * sin(deltaTheta))
                        /
                        (littleL - capitalM * littleL * cos(deltaTheta) * cos(deltaTheta)) - B * omega2 / (m2 * l2 * l2);

            };


            // Calculates angular velocity of v1 and v2
            auto thetaDot = [](double theta, double omega) {

                return omega;

            };

            // Integrate forward
            v1->omega = omega1 + Integrator::rk4(omega1Dot, theta1, omega1, TIMESTEP);
            v2->omega = omega2 + Integrator::rk4(omega2Dot, theta2, omega2, TIMESTEP);
            v1->theta = theta1 + Integrator::rk4(thetaDot,  theta1, omega1, TIMESTEP);
            v2->theta = theta2 + Integrator::rk4(thetaDot,  theta2, omega2, TIMESTEP);

            // Update grid position based on angular position
            v1->position.x = pivot->position.x + l1 * sin(v1->theta);
            v1->position.y = pivot->position.y + l1 * cos(v1->theta);
            v2->position.x = v1->position.x    + l2 * sin(v2->theta);
            v2->position.y = v1->position.y    + l2 * cos(v2->theta);

        }
    }
}

