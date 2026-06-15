#pragma once
#include "box2d.h"
#include "raylib.h"
#include "Constants.h"

class Ball {
private:
    b2Body* body;
    float radius;
    Color color;
    b2Vec2 startPosition;
    bool isActive;
    bool canBoost; //impulso


public:
    Ball(b2World* world, float x, float y, float radius, Color color);
    ~Ball();

    void Draw() const;
    void Reset();
    void ApplyForce(float forceX, float forceY);
    b2Body* GetBody() const { return body; }
    bool IsActive() const { return isActive; }
    void SetActive(bool active);

    //impulso
    void SetCanBoost(bool can);
    bool GetCanBoost() const { return canBoost; }
    void ApplyBoost(float forceY);
    
};