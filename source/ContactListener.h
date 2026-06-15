#pragma once
#include "box2d.h"

class MyContactListener : public b2ContactListener {
public:
    bool ballHitGround = false;
    bool ballHitBridge = false; //bandera para el impulso de la esfera

    void BeginContact(b2Contact* contact) override {
        b2Fixture* fixtureA = contact->GetFixtureA();
        b2Fixture* fixtureB = contact->GetFixtureB();

        b2Body* bodyA = fixtureA->GetBody();
        b2Body* bodyB = fixtureB->GetBody();

        // Verificar si es la bola (cuerpo dinámico pequeño) contra el suelo (estático)
        bool isDynamicA = (bodyA->GetType() == b2_dynamicBody);
        bool isDynamicB = (bodyB->GetType() == b2_dynamicBody);
        bool isStaticA = (bodyA->GetType() == b2_staticBody);
        bool isStaticB = (bodyB->GetType() == b2_staticBody);

        // Obtener radios para distinguir la esfera del jugador (0.4f) de los proyectiles (0.25f)
        float radiusA = 0.0f;
        float radiusB = 0.0f;

        if (fixtureA->GetShape()->GetType() == b2Shape::e_circle) {
            b2CircleShape* circle = (b2CircleShape*)fixtureA->GetShape();
            radiusA = circle->m_radius;
        }
        if (fixtureB->GetShape()->GetType() == b2Shape::e_circle) {
            b2CircleShape* circle = (b2CircleShape*)fixtureB->GetShape();
            radiusB = circle->m_radius;
        }
        // La ESFERA DEL JUGADOR es el círculo dinámico con RADIO 0.4f
        bool isPlayerBallA = isDynamicA && (fixtureA->GetShape()->GetType() == b2Shape::e_circle) && (radiusA > 0.35f);
        bool isPlayerBallB = isDynamicB && (fixtureB->GetShape()->GetType() == b2Shape::e_circle) && (radiusB > 0.35f);

        // Para mantener compatibilidad, isBallA/isBallB apuntan a la esfera del jugador
        bool isBallA = isPlayerBallA;
        bool isBallB = isPlayerBallB;

    
        // Identificar segmento del puente (polígono dinámico)
        bool isSegmentA = isDynamicA && (fixtureA->GetShape()->GetType() == b2Shape::e_polygon);
        bool isSegmentB = isDynamicB && (fixtureB->GetShape()->GetType() == b2Shape::e_polygon);

        //Identificar suelo (estático en la parte baja)
        bool isGroundA = isStaticA && (bodyA->GetPosition().y > 15.0f);  // Suelo está abajo
        bool isGroundB = isStaticB && (bodyB->GetPosition().y > 15.0f);

        //esfera contra suelo es game over
        if ((isBallA && isGroundB) || (isBallB && isGroundA)) {
            ballHitGround = true;
        }

        //esfera contra segmento del puente recarga impulso
        if ((isBallA && isSegmentB) || (isBallB && isSegmentA)) {
            ballHitBridge = true;
        }

        // Esfera contra enemigo (círculo dinámico rojo)
        bool isEnemyA = isDynamicA && (fixtureA->GetShape()->GetType() == b2Shape::e_circle) && (bodyA->GetPosition().x < 5.0f || bodyA->GetPosition().x > 30.0f);
        bool isEnemyB = isDynamicB && (fixtureB->GetShape()->GetType() == b2Shape::e_circle) && (bodyB->GetPosition().x < 5.0f || bodyB->GetPosition().x > 30.0f);

        if ((isBallA && isEnemyB) || (isBallB && isEnemyA)) {
            ballHitGround = true;  // Game Over
        }
    }

    void EndContact(b2Contact* contact) override {
        // No necesario
    }
};