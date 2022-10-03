#pragma once
#define _USE_MATH_DEFINES

#include <cmath>
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
    // TODO: Implementare spatial hashing (basato su k o meno)
    vector<aabb> _registeredAABBs;

    void _processCollision(CollisionResult& collisionResult, const Camera& camera, const aabb& staticAABB, const float& maxDistance = 5.0f) const;

public:
    inline void registerAABB(aabb staticAABB);

    inline vector<aabb> registeredAABBs() const;

    CollisionResult checkCollision(const Camera& camera, aabb& staticAABB, const float& maxDistance = 5.0f) const;

    CollisionResult checkCollisionWithRegisteredAABBs(const Camera& camera, const float& maxDistance = 5.0f) const;
};

inline void CollisionSolver::registerAABB(aabb staticAABB) {
    _registeredAABBs.push_back(staticAABB);
}

inline vector<aabb> CollisionSolver::registeredAABBs() const {
    return _registeredAABBs;
}

CollisionResult CollisionSolver::checkCollision(const Camera& camera, aabb& staticAABB, const float& maxDistance) const {
    CollisionResult result;
    _processCollision(result, camera, staticAABB, maxDistance);
    return result;
}

CollisionResult CollisionSolver::checkCollisionWithRegisteredAABBs(const Camera& camera, const float& maxDistance) const {
    CollisionResult result;
    for (const auto& staticAABB : _registeredAABBs) {
        staticAABB._resetCurrentIntersection();
        _processCollision(result, camera, staticAABB, maxDistance);
    }
    return result;
}

void CollisionSolver::_processCollision(CollisionResult& collisionResult, const Camera& camera, const aabb& staticAABB, const float& maxDistance) const {
    auto cameraPosition = camera.Position;

    ray frontRay(cameraPosition, camera.Front);
    frontRay.direction.y = 0;
    collisionResult.n = collisionResult.n || staticAABB.intersectRay2D(frontRay, maxDistance);

    //TODO: Rendere il vettore UP costante
    ray backRay = frontRay.rotate(M_PI, glm::vec3(0, 1, 0));
    backRay.direction.y = 0;
    collisionResult.s = collisionResult.s || staticAABB.intersectRay2D(backRay, maxDistance);

    //TODO: Utilizzare rotazioni per ottenere tutti gli otto vettori
    //TODO: Verificare stato front all'inizializzazione della camera
    //cout << frontRay.rotate(2 * M_PI, glm::vec3(0, 1, 0)).direction.x << " " << frontRay.direction.x << endl;

    ray rightRay(cameraPosition, camera.Right);
    collisionResult.e = collisionResult.e || staticAABB.intersectRay2D(rightRay, maxDistance);

    ray leftRay = rightRay.rotate(M_PI, glm::vec3(0, 1, 0));
    collisionResult.w = collisionResult.w || staticAABB.intersectRay2D(leftRay, maxDistance);
}