#pragma once
#include "box2d.h"
#include "raylib.h"
#include "Constants.h"

class BridgeSegment {
private:
    b2Body* body;
    float width;
    float height;
    Color color;
    b2Vec2 position;

public:
    BridgeSegment(b2World* world, float x, float y, float width, float height, Color color);
    ~BridgeSegment();

    void Draw() const;
    b2Body* GetBody() const { return body; }
    b2Vec2 GetPosition() const { return body->GetPosition(); }
};