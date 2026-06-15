#pragma once
#include "box2d.h"
#include "raylib.h"
#include "Constants.h"

class EnemyProjectile {
private:
    b2Body* body;
    float radius;
    Color color;
    bool isActive;
    float lifeTime;      // tiempo de vida
    b2Vec2 velocity;     // para seguimiento

public:
    EnemyProjectile(b2World* world, float x, float y, float radius, Color color);
    ~EnemyProjectile();

    void Draw() const;
    void SetActive(bool active);
    bool IsActive() const { return isActive; }
    b2Body* GetBody() const { return body; }
    void SetPosition(float x, float y);

    // MÉTODOS PARA PROYECTILES DISPARADOS
    void Launch(float forceX, float forceY);
    void Update(float deltaTime);
    bool IsExpired() const { return lifeTime <= 0; }
};