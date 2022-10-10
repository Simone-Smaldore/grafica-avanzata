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

    inline glm::ivec2 _indices(const glm::vec3& vector) const;

    inline std::vector<glm::ivec2> _indices(const aabb& staticAABB) const;

    void _processCollision(CollisionResult& collisionResult, const Camera& camera, aabb* staticAABB, const float& maxDistance = 5.0f) const;

public:
    void registerAABB(aabb* staticAABB);

    void registerAABBs(const std::vector<aabb*>& staticAABBs);

    std::vector<aabb*> registeredAABBNear(const glm::vec3& vector) const;

    CollisionResult checkCollision(const Camera& camera, aabb* staticAABB, const float& maxDistance = 5.0f) const;

    CollisionResult checkCollisionWithRegisteredAABBs(const Camera& camera, const float& maxDistance = 5.0f) const;

    void clearRegisteredAABBs();
};

inline std::pair<int, int> CollisionSolver::_hash(int i, int j) const {
    return { i, j };
}

inline glm::ivec2 CollisionSolver::_indices(const glm::vec3& vector) const {
    return glm::ivec2(static_cast<int>(ceil(vector.x / kCells)), static_cast<int>(ceil(vector.z / kCells)));
}

inline std::vector<glm::ivec2> CollisionSolver::_indices(const aabb& staticAABB) const {
    return std::vector<glm::ivec2>({ _indices(staticAABB.min), _indices(staticAABB.max) });
}

void CollisionSolver::registerAABB(aabb* staticAABB) {
    auto indices = _indices(*staticAABB);
    for (int i = indices[0].x; i < indices[1].x + 1; i++) {
        for (int j = indices[0].y; j < indices[1].y + 1; j++) {
            pair<int, int> hash = _hash(i, j);
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

CollisionResult CollisionSolver::checkCollision(const Camera& camera, aabb* staticAABB, const float& maxDistance) const {
    CollisionResult result;
    _processCollision(result, camera, staticAABB, maxDistance);
    return result;
}

CollisionResult CollisionSolver::checkCollisionWithRegisteredAABBs(const Camera& camera, const float& maxDistance) const {
    CollisionResult result;
    auto currentAABBs = registeredAABBNear(camera.Position);
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