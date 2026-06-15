#pragma once
#include "box2d.h"
#include "raylib.h"
#include "Constants.h"

// ============================================
// Clase Catapulta
// Representa los puntos rojos que suben y bajan en los costados
// Desde estos puntos se disparan los proyectiles enemigos
// ============================================
class Catapult {
private:
    b2Body* body;      // Cuerpo físico de la catapulta
    float radius;      // Radio del círculo
    Color color;       // Color visual (rojo)

public:
    Catapult(b2World* world, float x, float y, float radius, Color color);
    ~Catapult();

    void Update(float deltaTime);   // Actualizar posición (sube y baja)
    void Draw() const;              // Dibujar la catapulta
    b2Vec2 GetPosition() const;     // Obtener posición actual
    b2Body* GetBody() const { return body; }
};