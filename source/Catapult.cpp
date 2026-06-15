#include "Catapult.h"

// ============================================
// Constructor: crea la catapulta como cuerpo cinemático
// Uso b2_kinematicBody porque quiero controlar su movimiento manualmente
// con SetTransform(), sin que la gravedad lo afecte.
// ============================================
Catapult::Catapult(b2World* world, float x, float y, float radius, Color color)
    : radius(radius), color(color) {

    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;  // No reacciona a fuerzas ni gravedad
    bodyDef.position.Set(x, y);
    body = world->CreateBody(&bodyDef);

    // Forma circular
    b2CircleShape shape;
    shape.m_radius = radius;

    // Propiedades físicas
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.isSensor = true;   // Solo detecta colisiones, no afecta físicamente
    body->CreateFixture(&fixtureDef);
}

Catapult::~Catapult() {}

// ============================================
// Actualizar posición de la catapulta
// Sube y baja entre los límites Y = 6m y Y = 14m
// Uso una variable static para recordar la dirección de movimiento
// ============================================
void Catapult::Update(float deltaTime) {
    float speed = 2.0f;           // Velocidad de movimiento (metros por segundo)
    float yPos = body->GetPosition().y;
    static float moveDirection = 1.0f;   // 1 = subiendo, -1 = bajando

    // Actualizar posición
    yPos += moveDirection * speed * deltaTime;

    // Límites de movimiento
    if (yPos >= 14.0f) {
        yPos = 14.0f;
        moveDirection = -1.0f;   // Cambia a bajar
    }
    if (yPos <= 6.0f) {
        yPos = 6.0f;
        moveDirection = 1.0f;    // Cambia a subir
    }

    // Aplicar nueva posición
    body->SetTransform(b2Vec2(body->GetPosition().x, yPos), 0);
}

// ============================================
// Dibujar la catapulta en pantalla
// ============================================
void Catapult::Draw() const {
    b2Vec2 pos = body->GetPosition();
    float pixelX = Constants::ToPixels(pos.x);
    float pixelY = Constants::ToPixels(pos.y);
    float pixelRadius = Constants::ToPixels(radius);

    // Cuerpo principal (rojo)
    DrawCircle(pixelX, pixelY, pixelRadius, color);
    // Borde rojo oscuro para que se vea mejor
    DrawCircleLines(pixelX, pixelY, pixelRadius, RED);
    // Anillo exterior rojo
    DrawCircleLines(pixelX, pixelY, pixelRadius + 3, RED);
}

// ============================================
// Devuelve la posición actual de la catapulta
// ============================================
b2Vec2 Catapult::GetPosition() const {
    return body->GetPosition();
}