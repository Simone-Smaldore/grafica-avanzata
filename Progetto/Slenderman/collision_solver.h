#pragma once
#define _USE_MATH_DEFINES

#include <cmath>

#include "aabb.h"
#include "camera.h"
#include "ray.h"

struct CollisionResult {
    bool n, s, e, w;
    bool ne, nw, se, sw;

    inline bool isColliding() const {
        return n || s || e || w;
    }
};

class CollisionSolver {
public:
    static CollisionResult checkCollision(const Camera& camera, const aabb& staticAABB, const float& maxDistance = 5.0f);
};

CollisionResult CollisionSolver::checkCollision(const Camera& camera, const aabb& staticAABB, const float& maxDistance) {
    CollisionResult result;
    auto cameraPosition = camera.Position;

    ray frontRay(cameraPosition, camera.Front);
    frontRay.direction.y = 0;
    result.n = staticAABB.intersectRay2D(frontRay, maxDistance);

    //TODO: Rendere il vettore UP costante
    ray backRay = frontRay.rotate(M_PI, glm::vec3(0, 1, 0));
    backRay.direction.y = 0;
    result.s = staticAABB.intersectRay2D(backRay, maxDistance);

    //TODO: Utilizzare rotazioni per ottenere tutti gli otto vettori
    //TODO: Verificare stato front all'inizializzazione della camera
    //cout << frontRay.rotate(2 * M_PI, glm::vec3(0, 1, 0)).direction.x << " " << frontRay.direction.x << endl;

    ray rightRay(cameraPosition, camera.Right);
    result.e = staticAABB.intersectRay2D(rightRay, maxDistance);

    ray leftRay = rightRay.rotate(M_PI, glm::vec3(0, 1, 0));
    result.w = staticAABB.intersectRay2D(leftRay, maxDistance);

    return result;
}