#pragma once
#include "box2d.h"
#include "raylib.h"
#include "BridgeSegment.h"
#include "Constants.h"
#include <vector>
#include <memory>
#include <random>
#include "Ball.h"
#include "ContactListener.h"
#include "EnemyProjectile.h"
#include "Catapult.h"

class PhysicsWorld {
private:
    b2World world;
    b2Body* groundBody;

    // Pilares (estáticos)
    b2Body* leftPillar;
    b2Body* rightPillar;

    // Segmentos del puente
    std::vector<std::unique_ptr<BridgeSegment>> segments;

    // Almacenar joints para poder dibujarlos
    std::vector<b2RevoluteJoint*> revoluteJoints;
    std::vector<b2DistanceJoint*> cableJoints;

    std::unique_ptr<Ball> ball;

    MyContactListener contactListener;

    // ========== CATAPULTAS (puntos rojos que suben y bajan) ==========
    std::unique_ptr<Catapult> leftCatapult;
    std::unique_ptr<Catapult> rightCatapult;

    // ========== PROYECTILES DISPARADOS ==========
    std::vector<std::unique_ptr<EnemyProjectile>> activeProjectiles;
    float spawnTimer;
    std::mt19937 rng;

    //estado de simulacion
    bool simulationActive;
 

public:
    PhysicsWorld();
    ~PhysicsWorld();

    void Update();
    void Draw();
    void DrawJoints();  // Para visualizar las conexiones

    Ball* GetBall() const { return ball.get(); }
    void ApplyForceToBall(float forceX, float forceY);
    void ResetBall() {
        if (ball) {
            ball->Reset();
            ResetBallHitGround();  // Resetear la bandera
        }
    }

    // Getters
    b2World* GetWorld() { return &world; }
    const std::vector<std::unique_ptr<BridgeSegment>>& GetSegments() const { return segments; }

    bool HasBallHitGround() const { return contactListener.ballHitGround; }
    void ResetBallHitGround() { contactListener.ballHitGround = false; }
    
    //limite de pantalla
    void CheckBallBounds();

    void SetBallActive(bool active);

    //impulso
    void ApplyBoostToBall(float forceY);
    void SetBallCanBoost(bool can);

    bool HasBallHitBridge() const { return contactListener.ballHitBridge; }
    void ResetBallHitBridge() { contactListener.ballHitBridge = false; }

    // Proyectiles
    void SpawnProjectileFromCatapult(const b2Vec2& origin);
    void UpdateProjectiles(float deltaTime);
    void RemoveExpiredProjectiles();
    void CheckEnemyCollision();

    //estado de simulacion
    void SetSimulationActive(bool active);
    bool IsSimulationActive() const { return simulationActive; }
    
};