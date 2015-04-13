#ifndef MESHOCTTREE_H
#define MESHOCTTREE_H

#include "QList"

#include "hairCommon.h"

class ObjMesh;

struct OctTreeNode {

    int distFromMaxDepth;

    OctTreeNode **children;
    QList<Triangle> triangles;

    glm::vec3 min;
    glm::vec3 max;

    OctTreeNode(glm::vec3 _min, glm::vec3 _max, int _distFromMaxDepth){
        min = _min;
        max = _max;

        distFromMaxDepth = _distFromMaxDepth;

        children = NULL;
    }

    ~OctTreeNode(){
        delete[] children;
    }

    bool containsPoint(glm::vec3 pt){ // should change to actual containsTriangle, as this won't catch triangles with all points outside but part of the face inside a cube
        if (pt.x < min.x) return false;
        if (pt.y < min.y) return false;
        if (pt.z < min.z) return false;
        if (pt.x > max.x) return false;
        if (pt.y > max.y) return false;
        if (pt.z > max.z) return false;
        return true;
    }


    void split(){ // after reaching minimum requisite triangles, split node
        if (distFromMaxDepth <= 0){
            cerr << "mesh oct tree node should not be splitting, at lowest depth" << endl;
            return;
        }
        if (children != NULL){
            cerr << "attempting to split node that already has child nodes" << endl;
            return;
        }

        children = new OctTreeNode*[8];

        glm::vec3 half = min + max;
        half /= 2.0;

        int d = distFromMaxDepth-1;
        children[0] = new OctTreeNode(glm::vec3(min.x, min.y, min.z), glm::vec3(half.x, half.y, half.z), d);
        children[1] = new OctTreeNode(glm::vec3(min.x, min.y, half.z), glm::vec3(half.x, half.y, max.z), d);
        children[2] = new OctTreeNode(glm::vec3(min.x, half.y, min.z), glm::vec3(half.x, max.y, half.z), d);
        children[3] = new OctTreeNode(glm::vec3(half.x, min.y, min.z), glm::vec3(max.x, half.y, half.z), d);
        children[4] = new OctTreeNode(glm::vec3(half.x, half.y, min.z), glm::vec3(max.x, max.y, half.z), d);
        children[5] = new OctTreeNode(glm::vec3(min.x, half.y, half.z), glm::vec3(half.x, max.y, max.z), d);
        children[6] = new OctTreeNode(glm::vec3(half.x, min.y, half.z), glm::vec3(max.x, half.y, max.z), d);
        children[7] = new OctTreeNode(glm::vec3(half.x, half.y, half.z), glm::vec3(max.x, max.y, max.z), d);

        for (int i = 0; i < triangles.size(); i++){
            for (int j = 0; j < 8; j++){
                Triangle tri = triangles.at(i);
                if (children[j]->containsPoint(tri.v1) || children[j]->containsPoint(tri.v2) || children[j]->containsPoint(tri.v3)){
                    children[j]->triangles.append(tri);
                }
            }
        }

    }

};

class MeshOctTree
{
public:
    MeshOctTree(ObjMesh *mesh);
    ~MeshOctTree();

    void getRelevantTriangles(QList<Triangle> &list, glm::vec3 pt, glm::vec3 dir);
    void print();


private:
    void calculateBoundingBox();
    bool rayIntersectsCube(glm::vec3 pt, glm::vec3 dir, glm::vec3 minCorner, glm::vec3 maxCorner);
    void insertTriangle(Triangle tri);

    ObjMesh *m_mesh;
    OctTreeNode *m_root;

    glm::vec3 boundingMin;
    glm::vec3 boundingMax;

    int maxDepth;
    int minTrianglesForSplitting;


};

#endif // MESHOCTTREE_H
