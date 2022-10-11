#pragma once
#define _USE_MATH_DEFINES

#include <math.h>
#include <map>
#include <set>
#include <vector>

#include "aabb.h"
#include "camera.h"
#include "ray.h"

struct CollisionResult {
    bool n = false, s = false, e = false, w = false;
    bool ne, nw, se, sw;

    inline bool isColliding() const {
        return n || s || e || w;
    }
};

class CollisionSolver {
private:
    static const int kCells = 250;

    std::map<std::pair<int, int>, std::vector<aabb*>> _registeredAABBs;

    inline std::pair<int, int> _hash(int x, int y) const;

    glm::ivec2 _indices(const glm::vec3& vector) const;

    inline std::vector<glm::ivec2> _indices(const aabb& staticAABB) const;

    void _processCollision(CollisionResult& collisionResult, const Camera& camera, aabb* staticAABB, const float& maxDistance = 5.0f) const;

public:
    void registerAABB(aabb* staticAABB);

    void registerAABBs(const std::vector<aabb*>& staticAABBs);

    std::vector<aabb*> registeredAABBNear(const glm::vec3& vector) const;

    std::vector<aabb*> registeredAABBNear(const aabb& boundigBox) const;

    CollisionResult checkCollision(const Camera& camera, aabb* staticAABB, const float& maxDistance = 5.0f) const;

    CollisionResult checkCollisionWithRegisteredAABBs(const Camera& camera, const float& maxDistance = 5.0f, const float& cameraMargin = 5.0f) const;

    void clearRegisteredAABBs();
};

inline std::pair<int, int> CollisionSolver::_hash(int x, int z) const {
    return { x, z };
}

glm::ivec2 CollisionSolver::_indices(const glm::vec3& vector) const {
    int xIndex = vector.x >= 0 ? ceil(vector.x / kCells) : floor(vector.x / kCells);
    int zIndex = vector.z >= 0 ? ceil(vector.z / kCells) : floor(vector.z / kCells);;
    return glm::ivec2(xIndex, zIndex);
}

inline std::vector<glm::ivec2> CollisionSolver::_indices(const aabb& staticAABB) const {
    std::vector<glm::ivec2> indices = { _indices(staticAABB.min), _indices(staticAABB.max) };

    int& xMin = indices[0].x;
    int& xMax = indices[1].x;
    if (xMin > xMax)
        swap(xMin, xMax);

    int& zMin = indices[0].y;
    int& zMax = indices[1].y;
    if (zMin > zMax)
        swap(zMin, zMax);

    return indices;
}

void CollisionSolver::registerAABB(aabb* staticAABB) {
    auto indices = _indices(*staticAABB);

    int xMin = indices[0].x;
    int xMax = indices[1].x;
    int zMin = indices[0].y;
    int zMax = indices[1].y;

    for (int x = xMin; x < xMax + 1; x++) {
        for (int z = zMin; z < zMax + 1; z++) {
            pair<int, int> hash = _hash(x, z);
            _registeredAABBs[hash].push_back(staticAABB);
        }
    }
}

void CollisionSolver::registerAABBs(const std::vector<aabb*>& staticAABBs) {
    for (auto staticAABB : staticAABBs)
        registerAABB(staticAABB);
}

std::vector<aabb*> CollisionSolver::registeredAABBNear(const glm::vec3& vector) const {
    auto indices = _indices(vector);
    auto hash = _hash(indices.x, indices.y);
    auto registeredAABBs = _registeredAABBs.find(hash);
    return registeredAABBs == _registeredAABBs.end() ? std::vector<aabb*>() : registeredAABBs->second;
}

std::vector<aabb*> CollisionSolver::registeredAABBNear(const aabb& boundigBox) const {
    std::vector<aabb*> result;
    auto indices = _indices(boundigBox);

    int xMin = indices[0].x;
    int xMax = indices[1].x;
    int zMin = indices[0].y;
    int zMax = indices[1].y;

    for (int x = xMin; x < xMax + 1; x++) {
        for (int z = zMin; z < zMax + 1; z++) {
            pair<int, int> hash = _hash(x, z);
            auto registeredAABBs = _registeredAABBs.find(hash);
            if (registeredAABBs == _registeredAABBs.end())
                continue;
            result.insert(result.end(), registeredAABBs->second.begin(), registeredAABBs->second.end());
        }
    }

    return result;
}

CollisionResult CollisionSolver::checkCollision(const Camera& camera, aabb* staticAABB, const float& maxDistance) const {
    CollisionResult result;
    _processCollision(result, camera, staticAABB, maxDistance);
    return result;
}

CollisionResult CollisionSolver::checkCollisionWithRegisteredAABBs(const Camera& camera, const float& maxDistance, const float& cameraMargin) const {
    CollisionResult result;
    aabb cameraAABB = aabb(
        glm::vec3(camera.Position.x - cameraMargin, 0, camera.Position.z + cameraMargin),
        glm::vec3(camera.Position.x + cameraMargin, 0, camera.Position.z - cameraMargin)
    );
    auto currentAABBs = registeredAABBNear(cameraAABB);
    for (auto staticAABB : currentAABBs) {
        staticAABB->_resetCurrentIntersection();
        _processCollision(result, camera, staticAABB, maxDistance);
    }
    return result;
}

void CollisionSolver::_processCollision(CollisionResult& collisionResult, const Camera& camera, aabb* staticAABB, const float& maxDistance) const {
    auto cameraPosition = camera.Position;

    ray frontRay(cameraPosition, camera.Front);
    frontRay.direction.y = 0;
    collisionResult.n = collisionResult.n || staticAABB->intersectRay2D(frontRay, maxDistance);

    //TODO: Rendere il vettore UP costante
    ray backRay = frontRay.rotate(M_PI, glm::vec3(0, 1, 0));
    backRay.direction.y = 0;
    collisionResult.s = collisionResult.s || staticAABB->intersectRay2D(backRay, maxDistance);

    //TODO: Utilizzare rotazioni per ottenere tutti gli otto vettori
    //TODO: Verificare stato front all'inizializzazione della camera
    //cout << frontRay.rotate(2 * M_PI, glm::vec3(0, 1, 0)).direction.x << " " << frontRay.direction.x << endl;

    ray rightRay(cameraPosition, camera.Right);
    collisionResult.e = collisionResult.e || staticAABB->intersectRay2D(rightRay, maxDistance);

    ray leftRay = rightRay.rotate(M_PI, glm::vec3(0, 1, 0));
    collisionResult.w = collisionResult.w || staticAABB->intersectRay2D(leftRay, maxDistance);
}

void CollisionSolver::clearRegisteredAABBs() {
    std::set<aabb*> aabbs;

    for (auto cell : _registeredAABBs)
        for (auto staticAABB : cell.second)
            aabbs.insert(staticAABB);

    for (auto staticAABB : aabbs)
        delete staticAABB;

    _registeredAABBs.clear();
}