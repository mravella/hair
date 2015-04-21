#ifndef SIMULATION_H
#define SIMULATION_H

#include "hairCommon.h"
#include "integrator.h"
#include "objmesh.h"
#include <QMap>
#include <tuple>
#include <iostream>

#include <pthread.h>

class HairObject;
class Hair;
class GLWidget;

#define HAIRS_PER_THREAD 20

struct HairSimulationThreadInfo {
    Hair *hairs[HAIRS_PER_THREAD];
    
    QMap<std::tuple<double, double, double>, double> *densityGrid;
    QMap<std::tuple<double, double, double>, glm::vec3> *velocityGrid;
    
};


class Simulation
{
public:
    Simulation(GLWidget *widget, ObjMesh *mesh);
    
    virtual ~Simulation();
    
    void update(float _time);
    void simulate(HairObject *_object);
    
    glm::mat4 m_xform;
    
    
    
private:
    void moveObjects(HairObject *_object);
    
    void calculateExternalForces(HairObject *_object);
    
    void calculateFluidGrid(HairObject *_object);
    
    void calculateFrictionAndRepulsion(HairObject *_object);
    static void* calculateFrictionAndRepulsionThread(void *untypedInfoStruct);
    
    static glm::vec3 gradient(QMap<std::tuple<double, double, double>, double> &grid, glm::vec3 pt);
    static glm::vec3 getVelocity(QMap<std::tuple<double, double, double>, glm::vec3> &velocityGrid, QMap<std::tuple<double, double, double>, double> &densityGrid, glm::vec3 pt);
    
    void integrate(HairObject *_object);
    void integrate2(HairObject *_object);
    void integrate3(HairObject *_object);
    void integrate4(HairObject *_object);
    
    void particleSimulation(HairObject *obj);
    
    static void addToTable(QMap<std::tuple<double, double, double>, double> &grid, std::tuple<double, double, double> key, double value);
    static void addToTable(QMap<std::tuple<double, double, double>, glm::vec3> &grid, std::tuple<double, double, double> key, glm::vec3 value);
    
    
    
public:
    QList<glm::vec3> m_externalForces;
    QMap<std::tuple<double, double, double>, double> m_densityGrid;
    QMap<std::tuple<double, double, double>, glm::vec3> m_velocityGrid;
    
    
    
private:
    float m_time;
    GLWidget *m_widget;
    ObjMesh *m_mesh;
        
    pthread_t m_threads[200];
    HairSimulationThreadInfo m_threadData[200];
    
};

#endif // SIMULATION_H

