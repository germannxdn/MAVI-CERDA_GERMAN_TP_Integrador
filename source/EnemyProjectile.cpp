#include "EnemyProjectile.h"

EnemyProjectile::EnemyProjectile(b2World* world, float x, float y, float radius, Color color)
    : radius(radius), color(color), isActive(true), lifeTime(3.0f) {

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y);
    bodyDef.angle = 0.0f;

    body = world->CreateBody(&bodyDef);

    b2CircleShape shape;
    shape.m_radius = radius;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 0.5f;
    fixtureDef.friction = 0.2f;
    fixtureDef.restitution = 0.8f;
    fixtureDef.isSensor = false;  // Detecta colisiones

    body->CreateFixture(&fixtureDef);
}

EnemyProjectile::~EnemyProjectile() {}

// Lanzar con impulso
void EnemyProjectile::Launch(float forceX, float forceY) {
    b2Vec2 impulse(forceX, forceY);
    body->ApplyLinearImpulse(impulse, body->GetWorldCenter(), true);
    isActive = true;
    lifeTime = 3.0f;
}

// Actualizar tiempo de vida
void EnemyProjectile::Update(float deltaTime) {
    if (!isActive) return;

    lifeTime -= deltaTime;
    if (lifeTime <= 0) {
        isActive = false;
        body->SetLinearVelocity(b2Vec2(0, 0));
        body->SetAwake(false);
    }
}

void EnemyProjectile::Draw() const {
    if (!isActive) return;

    b2Vec2 pos = body->GetPosition();
    float pixelX = Constants::ToPixels(pos.x);
    float pixelY = Constants::ToPixels(pos.y);
    float pixelRadius = Constants::ToPixels(radius);

    // Sombra
    DrawCircle(pixelX + 2, pixelY + 2, pixelRadius, Fade(BLACK, 0.3f));
    // Cuerpo rojo
    DrawCircle(pixelX, pixelY, pixelRadius, color);
    // Ojo
    DrawCircle(pixelX - pixelRadius / 3, pixelY - pixelRadius / 3, pixelRadius / 4, WHITE);
    DrawCircle(pixelX - pixelRadius / 3, pixelY - pixelRadius / 3, pixelRadius / 6, BLACK);
}

void EnemyProjectile::SetActive(bool active) {
    isActive = active;
    if (!active) {
        body->SetLinearVelocity(b2Vec2(0, 0));
        body->SetAwake(false);
    }
    else {
        body->SetAwake(true);
    }
}

void EnemyProjectile::SetPosition(float x, float y) {
    body->SetTransform(b2Vec2(x, y), 0);
}