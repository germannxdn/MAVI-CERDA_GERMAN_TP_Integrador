#include "Ball.h"

Ball::Ball(b2World* world, float x, float y, float radius, Color color)
    : radius(radius), color(color), startPosition(x, y), isActive(true) {

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y);
    bodyDef.angle = 0.0f;

    body = world->CreateBody(&bodyDef);

    b2CircleShape shape;
    shape.m_radius = radius;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 0.6f;
    fixtureDef.friction = 0.2f;
    fixtureDef.restitution = 2.0f;  // Mucho rebote

    body->CreateFixture(&fixtureDef);

    body->SetSleepingAllowed(false);  // Nunca se duerme

    //impulso
    canBoost = true;
    
}

Ball::~Ball() {}

void Ball::ApplyForce(float forceX, float forceY) {
    if (!isActive) return;
    b2Vec2 force(forceX, forceY);
    body->ApplyForce(force, body->GetWorldCenter(), true);
}

void Ball::Reset() {
    body->SetLinearVelocity(b2Vec2(0, 0));
    body->SetAngularVelocity(0);
    body->SetTransform(startPosition, 0);
    isActive = true;
}

void Ball::Draw() const {
    if (!isActive) return;

    b2Vec2 pos = body->GetPosition();
    float pixelX = Constants::ToPixels(pos.x);
    float pixelY = Constants::ToPixels(pos.y);
    float pixelRadius = Constants::ToPixels(radius);

    // Sombra
    DrawCircle(pixelX + 3, pixelY + 3, pixelRadius, Fade(BLACK, 0.3f));
    // Cuerpo
    DrawCircle(pixelX, pixelY, pixelRadius, color);
    // Brillo
    DrawCircle(pixelX - pixelRadius / 3, pixelY - pixelRadius / 3, pixelRadius / 3, Fade(WHITE, 0.5f));
    // Borde
    DrawCircleLines(pixelX, pixelY, pixelRadius, DARKBROWN);
}

//En derrota
void Ball::SetActive(bool active) {
    isActive = active;
    if (!isActive) {
        // Congelar la bola
        body->SetLinearVelocity(b2Vec2(0, 0));
        body->SetAngularVelocity(0);
        body->SetAwake(false);  // Hacer que Box2D no la procese
    }
    else {
        body->SetAwake(true);
    }
}

//metodo para impulso
void Ball::SetCanBoost(bool can) {
    canBoost = can;
}

void Ball::ApplyBoost(float forceY) {
    if (!isActive) return;
    if (!canBoost) return;

    b2Vec2 impulse(0, -forceY);  // Hacia arriba (negativo)
    body->ApplyLinearImpulse(impulse, body->GetWorldCenter(), true);
    canBoost = false;  // Solo un impulso hasta que toque el puente otra vez
}