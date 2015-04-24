#ifndef SIMULATION_H
#define SIMULATION_H

#include "hairCommon.h"
#include "integrator.h"
#include "objmesh.h"
#include "md5.h"
#include <QMap>
#include <tuple>
#include <iostream>
#include <unordered_map>
#include <string>

#include <pthread.h>

class HairObject;
class Hair;
class GLWidget;

#define HAIRS_PER_THREAD 20


struct fluid
{
    glm::vec3 velocity;
    double    density;

    fluid()
    {
        velocity = glm::vec3(0.0);
        density = 0.0;
    }

    fluid(double d, glm::vec3 v)
    {
        velocity = v;
        density = d;
    }

};

struct grid_loc
{
    glm::vec3 pos;

    grid_loc()
    {
        pos = glm::vec3(0.0);
    }

    grid_loc(glm::vec3 p)
    {
        pos = p;
    }

    bool operator==(const grid_loc &other) const
    {
        return (pos == other.pos);
    }

};

typedef struct
{
    std::size_t operator() (fluid *key) const
    {
        size_t returnValue;

        glm::vec4 toHash = glm::vec4(key->velocity, key->density);
        const char *hashString = md5(glm::to_string(toHash)).c_str();
        returnValue = strtol(hashString, NULL, 16) % 64577;

        cout << "return value: " << returnValue << endl;

        return returnValue;
    }
} fluid_hash;

struct grid_loc_hash
{
    std::size_t operator() (const grid_loc &key) const
    {
        std::string keyString = glm::to_string(key.pos);
        const char *hashString = md5(keyString).c_str();
        std::string hString = std::string(hashString);
        hString.insert(0, "0x");
        const char *newHashString = hString.substr(0,10).c_str();
        size_t returnValue = (unsigned int) strtol(newHashString, NULL, 0);

//        cout << keyString << " -> " << newHashString << " -> " << returnValue << endl;

        return returnValue;
    }
};

typedef struct
{
    bool operator() (const fluid &f1, const fluid &f2) const
    {
        return ((f1.velocity == f2.velocity) && EQ(f1.density, f2.density));
    }
} fluid_eq;

typedef struct
{
    bool operator() (fluid *f1, fluid *f2) const
    {
        return ((f1->velocity == f2->velocity) && EQ(f1->density, f2->density));
    }
} grid_loc_eq;

struct HairSimulationThreadInfo {
    Hair *hairs[HAIRS_PER_THREAD];

//    QMap<std::tuple<double, double, double>, double> *densityGrid;
//    QMap<std::tuple<double, double, double>, glm::vec3> *velocityGrid;
    std::unordered_map<grid_loc, fluid, grid_loc_hash> *fluidGrid;

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
    
    static glm::vec3 gradient(std::unordered_map<grid_loc, fluid, grid_loc_hash> &map, glm::vec3 pt);
    static glm::vec3 getVelocity(QMap<std::tuple<double, double, double>, glm::vec3> &velocityGrid, QMap<std::tuple<double, double, double>, double> &densityGrid, glm::vec3 pt);
    
    void integrate(HairObject *_object);
    void integrate2(HairObject *_object);
    void integrate3(HairObject *_object);
    void integrate4(HairObject *_object);
    
    void particleSimulation(HairObject *obj);
    
    static void addToTable(QMap<std::tuple<double, double, double>, double> &grid, std::tuple<double, double, double> key, double value);
    static void addToTable(QMap<std::tuple<double, double, double>, glm::vec3> &grid, std::tuple<double, double, double> key, glm::vec3 value);
    void insertFluid(std::unordered_map<grid_loc, fluid, grid_loc_hash> &map, glm::vec3 pos, double density, glm::vec3 vel);
    static fluid getFluid(std::unordered_map<grid_loc, fluid, grid_loc_hash> &map, glm::vec3 pos);
    static glm::vec3 getFluidVelocity(std::unordered_map<grid_loc, fluid, grid_loc_hash> &map, glm::vec3 pos);
    static double getFluidDensity(std::unordered_map<grid_loc, fluid, grid_loc_hash> &map, glm::vec3 pos);

    
    
    
public:
    QList<glm::vec3> m_externalForces;
    QMap<std::tuple<double, double, double>, double> m_densityGrid;
    QMap<std::tuple<double, double, double>, glm::vec3> m_velocityGrid;
    std::unordered_map<grid_loc, fluid, grid_loc_hash> m_fluidGrid;
    
    
    
private:
    float m_time;
    GLWidget *m_widget;
    ObjMesh *m_mesh;
        
    pthread_t m_threads[200];
    HairSimulationThreadInfo m_threadData[200];
    
};

#endif // SIMULATION_H

