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

#define G   -29.8f
#define B   0.35f
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

        integrate3(_object);

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
<<<<<<< HEAD
            float m1 = 0.4;
            float m2 = 0.2;
=======
            float m1 = .5;
            float m2 = .2;
>>>>>>> ea7c78ec9ebcb826ebac24151ae2d18c2b79ce98

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

void Simulation::integrate3(HairObject *_object)
{
    for (int i = 0; i < _object->m_guideHairs.size(); i++)
    {
        float numVerts = _object->m_guideHairs.at(i)->m_vertices.size();
        for (int j = 3; j < numVerts; j++){

            // Relevant vertices
            HairVertex *pivot = _object->m_guideHairs.at(i)->m_vertices.at(j - 3);
            HairVertex *v1    = _object->m_guideHairs.at(i)->m_vertices.at(j - 2);
            HairVertex *v2    = _object->m_guideHairs.at(i)->m_vertices.at(j - 1);
            HairVertex *v3    = _object->m_guideHairs.at(i)->m_vertices.at(j);

            // Vector from one vertex to the vertex above it
            glm::vec3 rodVector1 = v1->position - pivot->position;
            glm::vec3 rodVector2 = v2->position - v1->position;
            glm::vec3 rodVector3 = v3->position - v2->position;

            // Pendula lengths
            float l1 = glm::length(rodVector1);
            float l2 = glm::length(rodVector2);
            float l3 = glm::length(rodVector3);

            // Store theta and omega so we can use them later
            float theta1 = v1->theta;
            float theta2 = v2->theta;
            float theta3 = v3->theta;
            float omega1 = v1->omega;
            float omega2 = v2->omega;
            float omega3 = v3->omega;

            // TODO: These will be stored on vertices eventually
            float m1 = 1.0;
            float m2 = 1.0;
            float m3 = 1.0;

            // Calculates angular acceleration for v1
            auto omega1Dot = [l1, l2, l3, theta2, theta3, omega2, omega3, m1, m2, m3](double theta1, double omega1)
            {

                return (10.0 * G * sin(theta1)
                                 + 4.0 * G * sin(theta1 - 2.0 * theta2)
                                 - G * sin(theta1 + 2.0 * theta2 - 2.0 * theta3)
                                 - G * sin(theta1 - 2.0 * theta2 + 2.0 * theta3)
                                 + 4.0 * omega1 * omega1 * sin(2.0 * theta1 - 2.0 * theta2)
                                 + 8.0 * omega2 * omega2 * sin(theta1 - theta2)
                                 + 2.0 * omega3 * omega3 * sin(theta1 - theta3)
                                 + 2.0 * omega3 * omega3 * sin(theta1 - 2.0 * theta2 + theta3)
                                )
                                /
                                (-10.0 + 4.0 * cos(2.0 * theta1 - 2.0 * theta2) + 2.0 * cos(2.0 * theta2 - 2.0 * theta3))
                                - B * omega1 / (m1 * l1 * l1);
            };

            // Calculates angular acceleration for v2
            auto omega2Dot = [l1, l2, l3, theta1, theta3, omega1, omega3, m1, m2, m3](double theta2, double omega2)
            {
                return ( -7.0 * G * sin(2.0 * theta1 - theta2)
                                 + 7.0 * G * sin(theta2)
                                 + G * sin(theta2 - 2 * theta3)
                                 + G * sin(2 * theta1 + theta2 - 2 * theta3)
                                 + 2.0 * omega1 * omega1 * sin(theta1 + theta2 - 2.0 * theta3)
                                 - 14.0 * omega1 * omega1 * sin(theta1 - theta2)
                                 + 2.0 * omega2 * omega2 * sin(2.0 * theta2 - 2.0 * theta3)
                                 - 4.0 * omega2 * omega2 * sin(2.0 * theta1 - 2.0 * theta2)
                                 + 6.0 * omega3 * omega3 * sin(theta2 - theta3)
                                 - 2.0 * omega3 * omega3 * sin(2.0 * theta1 - theta2 - theta3)
                               ) /
                                    (-10.0 + 4.0 * cos(2.0 * theta1 - 2.0 * theta2) + 2.0 * cos(2.0 * theta2 - 2.0 * theta3))
                               - B * omega2 / (m2 * l2 * l2);
            };

            auto omega3Dot = [l1, l2, l3, theta1, theta2, omega1, omega2, m1, m2, m3](double theta3, double omega3)
            {
                return -2.0 * sin(theta2 - theta3) *
                                    (   G * cos(2.0 * theta1 - theta2)
                                      + G * cos(theta2)
                                      + 2.0 * omega1 * omega1 * cos(theta1 - theta2)
                                      + 2.0 * omega2 * omega2
                                      + omega3 * omega3 * cos(theta2 - theta3)
                                    ) /
                                        (-5.0 + 2.0 * cos(2.0 * theta1 - 2.0 * theta2) + cos(2.0 * theta2 - 2.0 * theta3))
                               - B * omega3 / (m3 * l3 * l3);
            };


            // Calculates angular velocity of v1 and v2
            auto thetaDot = [](double theta, double omega) {

                return omega;

            };

            // Integrate forward
            v1->omega = omega1 + Integrator::rk4(omega1Dot, theta1, omega1, TIMESTEP);
            v2->omega = omega2 + Integrator::rk4(omega2Dot, theta2, omega2, TIMESTEP);
            v3->omega = omega3 + Integrator::rk4(omega3Dot, theta3, omega3, TIMESTEP);
            v1->theta = theta1 + Integrator::rk4(thetaDot,  theta1, omega1, TIMESTEP);
            v2->theta = theta2 + Integrator::rk4(thetaDot,  theta2, omega2, TIMESTEP);
            v3->theta = theta3 + Integrator::rk4(thetaDot,  theta3, omega3, TIMESTEP);

            // Update grid position based on angular position
            v1->position.x = pivot->position.x + l1 * sin(v1->theta);
            v1->position.y = pivot->position.y + l1 * cos(v1->theta);
            v2->position.x = v1->position.x    + l2 * sin(v2->theta);
            v2->position.y = v1->position.y    + l2 * cos(v2->theta);
            v3->position.x = v2->position.x    + l3 * sin(v3->theta);
            v3->position.y = v2->position.y    + l3 * cos(v3->theta);

        }
    }
}

void Simulation::integrate4(HairObject *_object)
{
    for (int i = 0; i < _object->m_guideHairs.size(); i++)
    {
        float numVerts = _object->m_guideHairs.at(i)->m_vertices.size();
        for (int j = 4; j < numVerts; j++){

            // Relevant vertices
            HairVertex *pivot = _object->m_guideHairs.at(i)->m_vertices.at(j - 4);
            HairVertex *v1    = _object->m_guideHairs.at(i)->m_vertices.at(j - 3);
            HairVertex *v2    = _object->m_guideHairs.at(i)->m_vertices.at(j - 2);
            HairVertex *v3    = _object->m_guideHairs.at(i)->m_vertices.at(j - 1);
            HairVertex *v4    = _object->m_guideHairs.at(i)->m_vertices.at(j);

            // Vector from one vertex to the vertex above it
            glm::vec3 rodVector1 = v1->position - pivot->position;
            glm::vec3 rodVector2 = v2->position - v1->position;
            glm::vec3 rodVector3 = v3->position - v2->position;
            glm::vec3 rodVector4 = v4->position - v3->position;

            // Pendula lengths
            float l1 = glm::length(rodVector1);
            float l2 = glm::length(rodVector2);
            float l3 = glm::length(rodVector3);
            float l4 = glm::length(rodVector4);

            // Store theta and omega so we can use them later
            float theta1 = v1->theta;
            float theta2 = v2->theta;
            float theta3 = v3->theta;
            float theta4 = v4->theta;
            float omega1 = v1->omega;
            float omega2 = v2->omega;
            float omega3 = v3->omega;
            float omega4 = v4->omega;

            // TODO: These will be stored on vertices eventually
            float m1 = 1.0;
            float m2 = 1.0;
            float m3 = 1.0;
            float m4 = 1.0;

            // Compute moments of inertia
            float i1 = m1 * l1 * l1;
            float i2 = m2 * l2 * l2;
            float i3 = m3 * l3 * l3;
            float i4 = m4 * l4 * l4;

            // Calculates angular acceleration for v1
            auto omega1Dot = [l1, l2, l3, l4, theta2, theta3, theta4, omega2, omega3, omega4, m1](double theta1, double omega1)
            {

                return (3*(493*G*sin(theta1) - 2*omega2*omega2*(
                                -187. + 45*cos(2*(theta3 - theta4)))*sin(theta1 - theta2)
                                +3*omega2*omega2*(-9*sin(theta1 + theta2 - 2*theta3)
                                +sin(theta1 + theta2 - 2*theta4))
                                +3*omega1*omega1*((73. - 18*cos(2*(theta3 - theta4)))*sin(2*(theta1 - theta2))
                                -9*sin(2*(theta1 - theta3)) + sin(2*(theta1 - theta4)))
                                +omega4*omega4*(sin(theta1 - theta4)
                                +27*sin(theta1 - 2*theta2 + 2*theta3 - theta4)
                                +6*sin(theta1 - 2*theta2 + theta4) + 18*sin(theta1 - 2*theta3 + theta4))
                                +3*omega3*omega3*(21*sin(theta1 - theta3) + 36*sin(theta1 - 2*theta2 + theta3)
                                -2*sin(theta1 + theta3 - 2*theta4) - 3*sin(theta1 - 2*theta2 - theta3 + 2*theta4))
                                +3*G*(73*sin(theta1 - 2*theta2) - 9*sin(theta1 - 2*theta3)
                                -27*sin(theta1 + 2*theta2 - 2*theta3) - 27*sin(theta1 - 2*theta2 + 2*theta3)
                                -9*sin(theta1 - 2*(theta2 + theta3 - theta4)) + sin(theta1 - 2*theta4)
                                +3*(sin(theta1 + 2*theta2 - 2*theta4) - 7*sin(theta1 + 2*theta3 - 2*theta4)
                                +sin(theta1 - 2*theta2 + 2*theta4) - 7*sin(theta1 - 2*theta3 + 2*theta4)
                                -3*sin(theta1 - 2*(theta2 - theta3 + theta4)))))) /
                                (-1310. + 657*cos(2*(theta1 - theta2)) - 81*cos(2*(theta1 - theta3))
                                 +405*cos(2*(theta2 - theta3)) + 9*cos(2*(theta1 - theta4))
                                 -45*cos(2*(theta2 - theta4)) + 333*cos(2*(theta3 - theta4))
                                 -81*cos(2*(theta1 - theta2 + theta3 - theta4))
                                 -81*cos(2*(theta1 - theta2 - theta3 + theta4)))
                        - B * omega1 / (m1 * l1 * l1);
            };

            // Calculates angular acceleration for v2
            auto omega2Dot = [l1, l2, l3, l4, theta1, theta3, theta4, omega1, omega3, omega4, m2](double theta2, double omega2)
            {
                return (-3*(758*omega1*omega1*sin(theta1 - theta2)
                                -18*cos(2*(theta3 - theta4))*(11*omega1*omega1*sin(theta1 - theta2)
                                +3*omega2*omega2*sin(2*(theta1 - theta2))
                                +6*G*sin(2*theta1 - theta2) - 5*G*sin(theta2))
                                +15*omega1*omega1*(-9*sin(theta1 + theta2 - 2*theta3)
                                +sin(theta1 + theta2 - 2*theta4)) + G*(411*sin(2*theta1 - theta2)
                                -347*sin(theta2) - 54*sin(theta2 - 2*theta3)
                                -81*sin(2*theta1 + theta2 - 2*theta3) + 6*sin(theta2 - 2*theta4)
                                +9*sin(2*theta1 + theta2 - 2*theta4))
                                +3*omega3*omega3*(36*sin(2*theta1 - theta2 - theta3)
                                -3*(37*sin(theta2 - theta3) + sin(2*theta1 - theta2 + theta3 - 2*theta4))
                                +8*sin(theta2 + theta3 - 2*theta4))
                                +3*omega2*omega2*(73*sin(2*(theta1 - theta2))
                                +5*(-9*sin(2*(theta2 - theta3)) + sin(2*(theta2 - theta4))))
                                +omega4*omega4*(6*sin(2*theta1 - theta2 - theta4)
                                -31*sin(theta2 - theta4) + 27*sin(2*theta1 - theta2 - 2*theta3 + theta4)
                                -72*sin(theta2 - 2*theta3 + theta4)))) /
                                (-1310. + 657*cos(2*(theta1 - theta2))
                                 -81*cos(2*(theta1 - theta3)) + 405*cos(2*(theta2 - theta3))
                                 +9*cos(2*(theta1 - theta4)) - 45*cos(2*(theta2 - theta4))
                                 +333*cos(2*(theta3 - theta4))
                                 -81*cos(2*(theta1 - theta2 + theta3 - theta4))
                                 -81*cos(2*(theta1 - theta2 - theta3 + theta4)))
                               - B * omega2 / (m2 * l2 * l2);
            };

            auto omega3Dot = [l1, l2, l3, l4, theta1, theta2, theta4, omega1, omega2, omega4, m3](double theta3, double omega3)
            {
                return (3*(3*omega2*omega2*(18*sin(2*theta1 - theta2 - theta3)
                                -3*(49*sin(theta2 - theta3) + sin(2*theta1 - theta2 + theta3 - 2*theta4))
                                +22*sin(theta2 + theta3 - 2*theta4))
                                +omega4*omega4*(9*sin(2*theta1 - theta3 - theta4)
                                -45*sin(2*theta2 - theta3 - theta4) + 14*(
                                + 17. - 9*cos(2*(theta1 - theta2)))*sin(theta3 - theta4))
                                +3*omega3*omega3*(9*sin(2*(theta1 - theta3))
                                -45*sin(2*(theta2 - theta3)) + (
                                + 37. -18*cos(2*(theta1 - theta2)))*sin(2*(theta3 - theta4)))
                                +3*omega1*omega1*(-39*sin(theta1 - theta3)
                                +90*sin(theta1 - 2*theta2 + theta3) + 4*sin(theta1 + theta3 - 2*theta4)
                                -15*sin(theta1 - 2*theta2 - theta3 + 2*theta4))
                                +3*G*(-27*sin(2*theta1 - theta3) - 36*sin(2*theta2 - theta3)
                                +12*sin(theta3) + 54*sin(2*theta1 - 2*theta2 + theta3)
                                +sin(theta3 - 2*theta4) + 3*sin(2*theta1 + theta3 - 2*theta4)
                                +6*sin(2*theta2 + theta3 - 2*theta4)
                                -9*sin(2*theta1 - 2*theta2 - theta3 + 2*theta4)))) /
                                (-1310. + 657*cos(2*(theta1 - theta2))
                                 -81*cos(2*(theta1 - theta3)) + 405*cos(2*(theta2 - theta3))
                                 +9*cos(2*(theta1 - theta4)) - 45*cos(2*(theta2 - theta4))
                                 +333*cos(2*(theta3 - theta4))
                                 -81*cos(2*(theta1 - theta2 + theta3 - theta4))
                                 -81*cos(2*(theta1 - theta2 - theta3 + theta4)))
                               - B * omega3 / (m3 * l3 * l3);
            };


            auto omega4Dot = [l1, l2, l3, l4, theta1, theta2, theta3, omega1, omega2, omega3, m4](double theta4, double omega4)
            {
                return (-3*(omega3*omega3*(9*sin(2*theta1 - theta3 - theta4)
                                -45*sin(2*theta2 - theta3 - theta4) + 2*(
                                +251. - 117*cos(2*(theta1 - theta2)))*sin(theta3 - theta4))
                                +3*omega4*omega4*(sin(2*(theta1 - theta4)) - 5*sin(2*(theta2 - theta4))
                                +(37. - 18*cos(2*(theta1 - theta2)))*sin(2*(theta3 - theta4)))
                                +omega1*omega1*(sin(theta1 - theta4)
                                +135*sin(theta1 - 2*theta2 + 2*theta3 - theta4)
                                -60*sin(theta1 - 2*theta2 + theta4)
                                -36*sin(theta1 - 2*theta3 + theta4))
                                +omega2*omega2*(-12*sin(2*theta1 - theta2 - theta4)
                                +73*sin(theta2 - theta4) + 27*sin(2*theta1 - theta2 - 2*theta3 + theta4)
                                -198*sin(theta2 - 2*theta3 + theta4)) + G*(3*sin(2*theta1 - theta4)
                                +24*sin(2*theta2 - theta4) + 9*sin(2*theta3 - theta4)
                                +81*sin(2*(theta1 - theta2 + theta3) - theta4) + 2*sin(theta4)
                                -9*(4*sin(2*theta1 - 2*theta2 + theta4) + 3*sin(2*theta1 - 2*theta3 + theta4)
                                +6*sin(2*theta2 - 2*theta3 + theta4))))) /
                                (-1310. + 657*cos(2*(theta1 - theta2))
                                 -81*cos(2*(theta1 - theta3)) + 405*cos(2*(theta2 - theta3))
                                 +9*cos(2*(theta1 - theta4)) - 45*cos(2*(theta2 - theta4))
                                 +333*cos(2*(theta3 - theta4)) - 81*cos(2*(theta1 - theta2 + theta3 - theta4))
                                 -81*cos(2*(theta1 - theta2 - theta3 + theta4)))
                                 - B * omega4 / (m4 * l4 * l4);
            };



            // Calculates angular velocity of v1 and v2
            auto thetaDot = [](double theta, double omega) {

                return omega;

            };

            // Integrate forward
            v1->omega = omega1 + Integrator::rk4(omega1Dot, theta1, omega1, TIMESTEP);
            v2->omega = omega2 + Integrator::rk4(omega2Dot, theta2, omega2, TIMESTEP);
            v3->omega = omega3 + Integrator::rk4(omega3Dot, theta3, omega3, TIMESTEP);
            v4->omega = omega4 + Integrator::rk4(omega4Dot, theta4, omega4, TIMESTEP);
            v1->theta = theta1 + Integrator::rk4(thetaDot,  theta1, omega1, TIMESTEP);
            v2->theta = theta2 + Integrator::rk4(thetaDot,  theta2, omega2, TIMESTEP);
            v3->theta = theta3 + Integrator::rk4(thetaDot,  theta3, omega3, TIMESTEP);
            v4->theta = theta4 + Integrator::rk4(thetaDot,  theta4, omega3, TIMESTEP);

            // Update grid position based on angular position
            v1->position.x = pivot->position.x + l1 * sin(v1->theta);
            v1->position.y = pivot->position.y + l1 * cos(v1->theta);
            v2->position.x = v1->position.x    + l2 * sin(v2->theta);
            v2->position.y = v1->position.y    + l2 * cos(v2->theta);
            v3->position.x = v2->position.x    + l3 * sin(v3->theta);
            v3->position.y = v2->position.y    + l3 * cos(v3->theta);
            v4->position.x = v3->position.x    + l4 * sin(v4->theta);
            v4->position.y = v3->position.y    + l4 * cos(v4->theta);

        }
    }
}



