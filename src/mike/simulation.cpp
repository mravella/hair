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

#define GRID_WIDTH 0.1f
#define FRICTION 0.07f
#define REPULSION 0.000f

#define DAMPENING 0.95f

#define EULER false
#define __BMONTELL_MODE__ false
#define TIMESTEP 0.01f


Simulation::Simulation(ObjMesh *mesh)
{
    m_time = 0;
    m_mesh = mesh;
    m_xform = glm::mat4(1.0);
    m_densityGrid = QMap<std::tuple<double, double, double>, double>(); 
    m_velocityGrid = QMap<std::tuple<double, double, double>, glm::vec3>();
    
}

Simulation::~Simulation()
{
}

void Simulation::update(float _time){
    m_time = _time;
}

void Simulation::simulate(HairObject *_object)
{
    
    moveObjects(_object);
    
    calculateExternalForces(_object);
    
//    QTime t;
//    t.start();
    
    calculateFluidGrid(_object);
//    cout << "1: " << t.restart() << " ms"<< endl;
    
    calculateFrictionAndRepulsion(_object);
//    cout << "2: " << t.restart() << " ms"<< endl;
    
    particleSimulation(_object);
    
}

void Simulation::moveObjects(HairObject *_object)
{
    for (int i = 0; i < _object->m_guideHairs.size(); ++i)
    {
        for (int j = 0; j < _object->m_guideHairs.at(j)->m_vertices.size(); ++j)
        {
            _object->m_guideHairs.at(i)->m_vertices.at(j)->prevPos = glm::vec3(m_xform * glm::vec4(_object->m_guideHairs.at(i)->m_vertices.at(j)->startPosition, 1.0));
        }
    }
    m_xform = glm::rotate((float) sin(m_time), glm::vec3(0, 1, 0));
    //    m_xform = glm::translate(m_xform, glm::vec3(sin(m_time), 0.0 , sin(m_time)));
    //    float x = CLAMP(fabs(sin(m_time)), 0.5, 1.0); m_xform = glm::scale(glm::mat4(1.0), glm::vec3(x, x, x));
    
    
}

// Calculate forces for each joint, for each external force included in the simulation
void Simulation::calculateExternalForces(HairObject *_object)
{
    for (int i = 0; i < _object->m_guideHairs.size(); i++)
    {
        float numVerts = _object->m_guideHairs.at(i)->m_vertices.size();
        
        for (int j = 0; j < numVerts; j++)
        {
            HairVertex *currVert = _object->m_guideHairs.at(i)->m_vertices.at(j);
            
            glm::vec3 force = glm::vec3(0.0);
            force += glm::vec3(0.0, -9.8, 0.0);
            glm::vec4 curr = m_xform * glm::vec4(currVert->startPosition, 1.0);
            glm::vec3 acceleration = (glm::vec3(currVert->prevPos - glm::vec3(curr)) - currVert->velocity * TIMESTEP) / (TIMESTEP * TIMESTEP);
            force += acceleration * currVert->mass * 0.1f;
            glm::vec3 normal;
            if (m_mesh->contains(normal, currVert->position)) force = 5.0f * normal;
            currVert->forces = force;
            
        }
    }
}

// Convert the hair to a fluid
void Simulation::calculateFluidGrid(HairObject *_object){
    
    m_densityGrid = QMap<std::tuple<double, double, double>, double>();
    m_velocityGrid = QMap<std::tuple<double, double, double>, glm::vec3>();
    
    QMap<std::tuple<double, double, double>, double> *densityGrid = &m_densityGrid;
    QMap<std::tuple<double, double, double>, glm::vec3> *velocityGrid = &m_velocityGrid;
    
        
    for (int i = 0; i < _object->m_guideHairs.size(); ++i)
    {
        Hair *currHair = _object->m_guideHairs.at(i);
        //
        for (int j = 0; j < currHair->m_vertices.size(); ++j)
        {
            HairVertex *currVert = currHair->m_vertices.at(j);
            
            float x = currVert->position.x;
            float y = currVert->position.y;
            float z = currVert->position.z;
            
            float scaleFactor = (1.0f / GRID_WIDTH);
            
            float xFloor = floor(x * scaleFactor) / scaleFactor;
            float yFloor = floor(y * scaleFactor) / scaleFactor;
            float zFloor = floor(z * scaleFactor) / scaleFactor;
            
            float xCeil = ceil(x * scaleFactor) / scaleFactor;
            float yCeil = ceil(y * scaleFactor) / scaleFactor;
            float zCeil = ceil(z * scaleFactor) / scaleFactor;
            
            float xPercentage = x - xFloor;
            float yPercentage = y - yFloor;
            float zPercentage = z - zFloor;
            
            float XYZ = (1.0 - xPercentage) * (1.0 - yPercentage) * (1.0 - zPercentage);
            float XYz = (1.0 - xPercentage) * (1.0 - yPercentage) * (zPercentage);
            float XyZ = (1.0 - xPercentage) * (yPercentage) * (1.0 - zPercentage);
            float Xyz = (1.0 - xPercentage) * (yPercentage) * (zPercentage);
            float xYZ = (xPercentage) * (1.0 - yPercentage) * (1.0 - zPercentage);
            float xYz = (xPercentage) * (1.0 - yPercentage) * (zPercentage);
            float xyZ = (xPercentage) * (yPercentage) * (1.0 - zPercentage);
            float xyz = (xPercentage) * (yPercentage) * (zPercentage);
            
            //            cout << "xOercentage: " << xPercentage << endl;
            //            cout << "yPercentage: " << yPercentage << endl;
            //            cout << "zPercentage: " << zPercentage << endl;
            
            addToTable(*densityGrid, std::make_tuple(xCeil, yCeil, zCeil), XYZ);
            addToTable(*densityGrid, std::make_tuple(xCeil, yCeil, zFloor), XYz);
            addToTable(*densityGrid, std::make_tuple(xCeil, yFloor, zCeil), XyZ);
            addToTable(*densityGrid, std::make_tuple(xCeil, yFloor, zFloor), Xyz);
            addToTable(*densityGrid, std::make_tuple(xFloor, yCeil, zCeil), xYZ);
            addToTable(*densityGrid, std::make_tuple(xFloor, yCeil, zFloor), xYz);
            addToTable(*densityGrid, std::make_tuple(xFloor, yFloor, zCeil), xyZ);
            addToTable(*densityGrid, std::make_tuple(xFloor, yFloor, zFloor), xyz);
            
            addToTable(*velocityGrid, std::make_tuple(xCeil, yCeil, zCeil), XYZ * currVert->velocity);
            addToTable(*velocityGrid, std::make_tuple(xCeil, yCeil, zFloor), XYz * currVert->velocity);
            addToTable(*velocityGrid, std::make_tuple(xCeil, yFloor, zCeil), XyZ * currVert->velocity);
            addToTable(*velocityGrid, std::make_tuple(xCeil, yFloor, zFloor), Xyz * currVert->velocity);
            addToTable(*velocityGrid, std::make_tuple(xFloor, yCeil, zCeil), xYZ * currVert->velocity);
            addToTable(*velocityGrid, std::make_tuple(xFloor, yCeil, zFloor), xYz * currVert->velocity);
            addToTable(*velocityGrid, std::make_tuple(xFloor, yFloor, zCeil), xyZ * currVert->velocity);
            addToTable(*velocityGrid, std::make_tuple(xFloor, yFloor, zFloor), xyz * currVert->velocity);
        }
    }
    
}



void Simulation::calculateFrictionAndRepulsion(HairObject *_object)
{
    // end normal function
    
    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    int _numHairs = _object->m_guideHairs.size();
    int _numThreads = _numHairs/HAIRS_PER_THREAD;
    
    int _index;
    for (int i = 0; i < _numThreads; i++)
    {
        m_threadData[i].densityGrid = &m_densityGrid;
        m_threadData[i].velocityGrid = &m_velocityGrid;
        
        _index = HAIRS_PER_THREAD*i;
        for (int k = 0; k < HAIRS_PER_THREAD; k++){
            if (_index > _numHairs) m_threadData[i].hairs[k] = NULL;
            m_threadData[i].hairs[k] = _object->m_guideHairs.at(_index);
            _index++;
        }
        
        if (pthread_create(&m_threads[i], &attr, calculateFrictionAndRepulsionThread, (void *)&(m_threadData[i])))
            cerr << "threadfail 1 " << i << endl;
    }
    
    for (int i = 0; i < _numThreads; ++i)
    {
        if (pthread_join(m_threads[i], NULL))
            cerr << "threadfail 2 " << i << endl;
    }
    
    pthread_attr_destroy(&attr);
}


void* Simulation::calculateFrictionAndRepulsionThread(void *untypedInfoStruct){
    
    
    HairSimulationThreadInfo *infoStruct = (HairSimulationThreadInfo *) untypedInfoStruct;
    
    QMap<std::tuple<double, double, double>, double> *densityGrid = infoStruct->densityGrid;
    QMap<std::tuple<double, double, double>, glm::vec3> *velocityGrid = infoStruct->velocityGrid;
    
    // begin normal function
    
    for (int i = 0; i < HAIRS_PER_THREAD; i++){
        
        Hair *currHair = infoStruct->hairs[i];
        if (currHair == NULL) break;
        
        for (int j = 0; j < currHair->m_vertices.size(); ++j)
        {
            HairVertex *currVert = currHair->m_vertices.at(j);
            
            float x = currVert->position.x;
            float y = currVert->position.y;
            float z = currVert->position.z;
            
            float scaleFactor = (1.0f / GRID_WIDTH);
            
            float xFloor = floor(x * scaleFactor) / scaleFactor;
            float yFloor = floor(y * scaleFactor) / scaleFactor;
            float zFloor = floor(z * scaleFactor) / scaleFactor;
            
            float xCeil = ceil(x * scaleFactor) / scaleFactor;
            float yCeil = ceil(y * scaleFactor) / scaleFactor;
            float zCeil = ceil(z * scaleFactor) / scaleFactor;
            
            float xPercentage = x - xFloor;
            float yPercentage = y - yFloor;
            float zPercentage = z - zFloor;
            
            //            float c00 = m_densityGrid.value(std::make_tuple(xFloor, yFloor, zFloor)) * (1.0 - xPercentage) + m_densityGrid.value(std::make_tuple(xCeil, yFloor, zFloor)) * xPercentage;
            //            float c10 = m_densityGrid.value(std::make_tuple(xFloor, yCeil, zFloor)) * (1.0 - xPercentage) + m_densityGrid.value(std::make_tuple(xCeil, yCeil, zFloor)) * xPercentage;
            //            float c01 = m_densityGrid.value(std::make_tuple(xFloor, yFloor, zCeil)) * (1.0 - xPercentage) + m_densityGrid.value(std::make_tuple(xCeil, yFloor, zCeil)) * xPercentage;
            //            float c11 = m_densityGrid.value(std::make_tuple(xFloor, yCeil, zCeil)) * (1.0 - xPercentage) + m_densityGrid.value(std::make_tuple(xCeil, yCeil, zCeil)) * xPercentage;
            
            //            float c0 = c00 * (1.0 - yPercentage) + c10 * (yPercentage);
            //            float c1 = c01 * (1.0 - yPercentage) + c11 * (yPercentage);
            
            //            // Density
            //            float c = c0 * (1.0 - zPercentage) + c1 * (zPercentage);
            glm::vec3 currGradient = gradient(*densityGrid, currVert->position);
            
            float XYZ = (1.0 - xPercentage) * (1.0 - yPercentage) * (1.0 - zPercentage);
            float XYz = (1.0 - xPercentage) * (1.0 - yPercentage) * (zPercentage);
            float XyZ = (1.0 - xPercentage) * (yPercentage) * (1.0 - zPercentage);
            float Xyz = (1.0 - xPercentage) * (yPercentage) * (zPercentage);
            float xYZ = (xPercentage) * (1.0 - yPercentage) * (1.0 - zPercentage);
            float xYz = (xPercentage) * (1.0 - yPercentage) * (zPercentage);
            float xyZ = (xPercentage) * (yPercentage) * (1.0 - zPercentage);
            float xyz = (xPercentage) * (yPercentage) * (zPercentage);
            
            glm::vec3 v00 = getVelocity(*velocityGrid, *densityGrid, glm::vec3(xFloor, yFloor, zFloor)) * (1.0f - xPercentage) * xyz
                    + getVelocity(*velocityGrid, *densityGrid, glm::vec3(xCeil, yFloor, zFloor)) * (xPercentage) * Xyz;
            glm::vec3 v10 = getVelocity(*velocityGrid, *densityGrid, glm::vec3(xFloor, yCeil, zFloor)) * (1.0f - xPercentage) * xYz
                    + getVelocity(*velocityGrid, *densityGrid, glm::vec3(xCeil, yCeil, zFloor)) * (xPercentage) * XYz;
            glm::vec3 v01 = getVelocity(*velocityGrid, *densityGrid, glm::vec3(xFloor, yFloor, zCeil)) * (1.0f - xPercentage) * xyZ
                    + getVelocity(*velocityGrid, *densityGrid, glm::vec3(xCeil, yFloor, zCeil)) * (xPercentage) * XyZ;
            glm::vec3 v11 = getVelocity(*velocityGrid, *densityGrid, glm::vec3(xFloor, yCeil, zCeil)) * (1.0f - xPercentage) * xYZ
                    + getVelocity(*velocityGrid, *densityGrid, glm::vec3(xCeil, yCeil, zCeil)) * (xPercentage) * XYZ;
            
            glm::vec3 v0 = v00 * (1.0f - yPercentage) + v10 * (yPercentage);
            glm::vec3 v1 = v01 * (1.0f - yPercentage) + v11 * (yPercentage);
            
            // Velocity
            glm::vec3 v = v0 * (1.0f - zPercentage) + v1 * (zPercentage);
            
            // Account for friction;
            currVert->velocity = (1.0f - FRICTION) * currVert->velocity + FRICTION * v;
            
            currVert->velocity = currVert->velocity + REPULSION * currGradient / TIMESTEP;
            
        }
    }
    
    pthread_exit(NULL);
}







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
            float m1 = 0.4;
            float m2 = 0.2;
            
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

void Simulation::particleSimulation(HairObject *obj)
{
    
    
    for (int i = 0; i < obj->m_guideHairs.size(); i++)
    {
        float numVerts = obj->m_guideHairs.at(i)->m_vertices.size();
        
        obj->m_guideHairs.at(i)->m_vertices.at(0)->tempPos = obj->m_guideHairs.at(i)->m_vertices.at(0)->position;
        
        // Update Velocities
        for (int j = 1; j < numVerts; ++j)
        {
            HairVertex *h = obj->m_guideHairs.at(i)->m_vertices.at(j);
            
            // TODO: Precompute the mass inverse
            h->velocity = h->velocity + TIMESTEP * (h->forces * (1.0f / h->mass));
            h->tempPos += (h->velocity * TIMESTEP);
            h->forces = glm::vec3(0.0);
            h->velocity *= 0.99f;
        }
        
        glm::vec3 dir;
        glm::vec3 curr_pos;
        for (int j = 1; j < numVerts; ++j)
        {
            HairVertex *prev = obj->m_guideHairs.at(i)->m_vertices.at(j - 1);
            HairVertex *curr = obj->m_guideHairs.at(i)->m_vertices.at(j);
            curr_pos = curr->tempPos;
            dir = glm::normalize(curr->tempPos - prev->tempPos);
            curr->tempPos = prev->tempPos + dir * prev->segLen;
            curr->correctionVector = curr_pos - curr->tempPos;
        }
        
        for (int j = 1; j < numVerts; ++j)
        {
            HairVertex *prev = obj->m_guideHairs.at(i)->m_vertices.at(j - 1);
            HairVertex *curr = obj->m_guideHairs.at(i)->m_vertices.at(j);
            prev->velocity = ((prev->tempPos - prev->position) / TIMESTEP) + DAMPENING * (curr->correctionVector / TIMESTEP);
            prev->position = prev->tempPos;
        }
        
        HairVertex *last = obj->m_guideHairs.at(i)->m_vertices.last();
        last->position = last->tempPos;
    }
}


void Simulation::addToTable(QMap<std::tuple<double, double, double>, double> &grid, std::tuple<double, double, double> key, double value)
{
    grid.insert(key, grid.value(key, 0.0) + value);
}

void Simulation::addToTable(QMap<std::tuple<double, double, double>, glm::vec3> &grid, std::tuple<double, double, double> key, glm::vec3 value)
{
    grid.insert(key, grid.value(key, glm::vec3(0.0)) + value);
}

glm::vec3 Simulation::getVelocity(QMap<std::tuple<double, double, double>, glm::vec3> &velocityGrid, QMap<std::tuple<double, double, double>, double> &densityGrid, glm::vec3 pt)
{
    std::tuple<double, double, double> key = std::make_tuple(pt.x, pt.y, pt.z);
    
    return velocityGrid.value(key) / (float) densityGrid.value(key);
}

glm::vec3 Simulation::gradient(QMap<std::tuple<double, double, double>, double> &grid, glm::vec3 pt)
{
    float scaleFactor = (1.0f / GRID_WIDTH);
    
    float xFloor = floor(pt.x * scaleFactor) / scaleFactor;
    float yFloor = floor(pt.y * scaleFactor) / scaleFactor;
    float zFloor = floor(pt.z * scaleFactor) / scaleFactor;
    
    float xCeil = ceil(pt.x * scaleFactor) / scaleFactor;
    float yCeil = ceil(pt.y * scaleFactor) / scaleFactor;
    float zCeil = ceil(pt.z * scaleFactor) / scaleFactor;
    
    float XYZ = grid.value(std::make_tuple(xCeil, yCeil, zCeil), 0.0);
    float XYz = grid.value(std::make_tuple(xCeil, yCeil, zFloor), 0.0);
    float XyZ = grid.value(std::make_tuple(xCeil, yFloor, zCeil), 0.0);
    float Xyz = grid.value(std::make_tuple(xCeil, yFloor, zFloor), 0.0);
    float xYZ = grid.value(std::make_tuple(xFloor, yCeil, zCeil), 0.0);
    float xYz = grid.value(std::make_tuple(xFloor, yCeil, xFloor), 0.0);
    float xyZ = grid.value(std::make_tuple(xFloor, yFloor, zCeil), 0.0);
    float xyz = grid.value(std::make_tuple(xFloor, yFloor, zFloor), 0.0);
    
    float maxX = std::max(XYZ - xYZ, std::max(XyZ - xyZ, std::max(XYz - xYz, Xyz - xyz)));
    float maxY = std::max(XYZ - XyZ, std::max(xYZ - xyZ, std::max(XYz - Xyz, xYz - xyz)));
    float maxZ = std::max(XYZ - XYz, std::max(xYZ - xYz, std::max(XyZ - Xyz, xyZ - xyz)));
    
    if (EQ(MAX3(maxX, maxY, maxZ), maxX))
        return glm::vec3(1, 0, 0) * (float) sgn(maxX);
    else if (EQ(MAX3(maxX, maxY, maxZ), maxY))
        return glm::vec3(0, 1, 0) * (float) sgn(maxY);
    else
        return glm::vec3(0, 0, 1) * (float) sgn(maxZ);
    
}

