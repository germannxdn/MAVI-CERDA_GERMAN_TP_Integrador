#include "BridgeSegment.h"

BridgeSegment::BridgeSegment(b2World* world, float x, float y, float width, float height, Color color)
    : width(width), height(height), color(color), position(x, y) {

    // Configurar el cuerpo (dinámico, porque el puente se mueve)
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y);
    bodyDef.angle = 0.0f;

    body = world->CreateBody(&bodyDef);

    // Forma del segmento
    b2PolygonShape shape;
    shape.SetAsBox(width / 2.0f, height / 2.0f);

    // Propiedades físicas
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.5f;   // liviano para más rebote
    fixtureDef.friction = 0.2f;  // un poco de rozamiento
    fixtureDef.restitution = 2.0f; // ALTO REBOTE 

    body->CreateFixture(&fixtureDef);
}

BridgeSegment::~BridgeSegment() {
    // El cuerpo se destruye con el mundo
}

void BridgeSegment::Draw() const {
    b2Vec2 pos = body->GetPosition();
    float angleDeg = body->GetAngle() * RAD2DEG;

    float pixelX = Constants::ToPixels(pos.x);
    float pixelY = Constants::ToPixels(pos.y);
    float pixelW = Constants::ToPixels(width);
    float pixelH = Constants::ToPixels(height);

    Rectangle rect = { pixelX - pixelW / 2, pixelY - pixelH / 2, pixelW, pixelH };
    Vector2 origin = { pixelW / 2, pixelH / 2 };

    DrawRectanglePro(rect, origin, angleDeg, color);
    DrawRectangleLinesEx(rect, 2, DARKBROWN);
}