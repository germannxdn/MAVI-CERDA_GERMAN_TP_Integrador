#include "PhysicsWorld.h"
#include <cmath>
#include <random>
#include <chrono>

PhysicsWorld::PhysicsWorld()
    : world(b2Vec2(0.0f, 9.8f)), groundBody(nullptr), leftPillar(nullptr), rightPillar(nullptr),
    spawnTimer(0.0f), rng(std::chrono::steady_clock::now().time_since_epoch().count()) {

    world.SetContactListener(&contactListener);
    // estado de simulacion
    simulationActive = true;

    // -------------------------
    // 1. SUELO ESTÁTICO
    // -------------------------
    b2BodyDef groundDef;
    groundDef.type = b2_staticBody;
    groundDef.position.Set(Constants::GROUND_WIDTH / 2.0f, Constants::GROUND_Y);
    groundBody = world.CreateBody(&groundDef);

    b2PolygonShape groundShape;
    groundShape.SetAsBox(Constants::GROUND_WIDTH / 2.0f, 0.2f);
    groundBody->CreateFixture(&groundShape, 0.0f);

    // ============================================
    // TECHO (estático, límite superior)
    // ============================================
    b2BodyDef ceilingDef;
    ceilingDef.type = b2_staticBody;
    ceilingDef.position.Set(Constants::GROUND_WIDTH / 2.0f, Constants::CEILING_Y);
    b2Body* ceilingBody = world.CreateBody(&ceilingDef);

    b2PolygonShape ceilingShape;
    ceilingShape.SetAsBox(Constants::GROUND_WIDTH / 2.0f, 0.2f);
    ceilingBody->CreateFixture(&ceilingShape, 0.0f);

    // -------------------------
    // 2. PILARES (estáticos)
    // -------------------------
    b2BodyDef pillarDef;
    pillarDef.type = b2_staticBody;

    // Pilar izquierdo
    pillarDef.position.Set(Constants::LEFT_PILLAR_X, Constants::PILLAR_Y);
    leftPillar = world.CreateBody(&pillarDef);
    b2PolygonShape pillarShape;
    pillarShape.SetAsBox(Constants::PILLAR_WIDTH / 2.0f, Constants::PILLAR_HEIGHT / 2.0f);
    leftPillar->CreateFixture(&pillarShape, 0.0f);

    // Pilar derecho
    pillarDef.position.Set(Constants::RIGHT_PILLAR_X, Constants::PILLAR_Y);
    rightPillar = world.CreateBody(&pillarDef);
    rightPillar->CreateFixture(&pillarShape, 0.0f);

    // -------------------------
    // 3. SEGMENTOS DEL PUENTE
    // -------------------------
    float startX = Constants::BRIDGE_START_X;
    float segmentSpacing = Constants::SEGMENT_WIDTH;

    for (int i = 0; i < Constants::NUM_SEGMENTS; i++) {
        float x = startX + i * segmentSpacing;
        float y = Constants::BRIDGE_Y;

        auto segment = std::make_unique<BridgeSegment>(
            &world, x, y,
            Constants::SEGMENT_WIDTH,
            Constants::SEGMENT_HEIGHT,
            Constants::SEGMENT_COLOR
        );
        segments.push_back(std::move(segment));
    }

    // -------------------------
    // 4. JOINTS ENTRE SEGMENTOS (Revolute Joint)
    // -------------------------
    for (int i = 0; i < Constants::NUM_SEGMENTS - 1; i++) {
        b2RevoluteJointDef revJointDef;
        revJointDef.Initialize(
            segments[i]->GetBody(),
            segments[i + 1]->GetBody(),
            segments[i]->GetBody()->GetWorldCenter()
        );
        revJointDef.collideConnected = false;  // Los segmentos no chocan entre sí

        b2RevoluteJoint* joint = (b2RevoluteJoint*)world.CreateJoint(&revJointDef);
        revoluteJoints.push_back(joint);
    }

    // -------------------------
    // 5. CABLES (Distance Joints) de los pilares a los segmentos
    // -------------------------
    // Cable del pilar izquierdo al primer segmento
    b2DistanceJointDef cableLeft;
    cableLeft.Initialize(
        leftPillar,
        segments[0]->GetBody(),
        b2Vec2(Constants::LEFT_PILLAR_X, Constants::CABLE_ATTACH_Y),
        segments[0]->GetBody()->GetWorldCenter()
    );
    
    cableJoints.push_back((b2DistanceJoint*)world.CreateJoint(&cableLeft));

    // Cable del pilar derecho al último segmento
    b2DistanceJointDef cableRight;
    cableRight.Initialize(
        rightPillar,
        segments[Constants::NUM_SEGMENTS - 1]->GetBody(),
        b2Vec2(Constants::RIGHT_PILLAR_X, Constants::CABLE_ATTACH_Y),
        segments[Constants::NUM_SEGMENTS - 1]->GetBody()->GetWorldCenter()
    );
    
    cableJoints.push_back((b2DistanceJoint*)world.CreateJoint(&cableRight));

    // ============================================
    // 6. CREAR LA ESFERA DEL JUGADOR
    // ============================================
    ball = std::make_unique<Ball>(&world,
        Constants::BRIDGE_START_X + 3.0f,
        Constants::BRIDGE_Y - 4.0f,
        0.4f, ORANGE);

    // ============================================
   // 7. CREAR PROYECTILES ENEMIGOS
   // ============================================
   
    // Catapulta
    leftCatapult = std::make_unique<Catapult>(&world, 3.0f, 10.0f, 0.4f, RED);
    rightCatapult = std::make_unique<Catapult>(&world, 34.0f, 10.0f, 0.4f, RED);
    spawnTimer = 0.0f;
}


PhysicsWorld::~PhysicsWorld() {
    // Los joints y cuerpos se destruyen con el mundo
}

void PhysicsWorld::Update() {
    if (!simulationActive) return; // Si el juego termino no actualizar nada
    world.Step(1.0f / 60.0f, 8, 3);

    if (leftCatapult) leftCatapult->Update(1.0f / 60.0f);
    if (rightCatapult) rightCatapult->Update(1.0f / 60.0f);
        
    // Spawn de proyectiles desde catapultas
    spawnTimer -= 1.0f / 60.0f;
    if (spawnTimer <= 0.0f) {
        if (leftCatapult && rightCatapult) {
            std::uniform_int_distribution<int> catDist(0, 1);
            if (catDist(rng) == 0) {
                SpawnProjectileFromCatapult(leftCatapult->GetPosition());
            }
            else {
                SpawnProjectileFromCatapult(rightCatapult->GetPosition());
            }
        }
        spawnTimer = 1.0f;  // Cada 1 segundo
    }

    // Actualizar proyectiles activos (los que vuelan)
    for (auto& p : activeProjectiles) {
        p->Update(1.0f / 60.0f);
    }
    RemoveExpiredProjectiles();
    CheckBallBounds();
    CheckEnemyCollision(); // Detectar colisiones
}

void PhysicsWorld::Draw() {
    // Dibujar suelo
    float groundYPixels = Constants::ToPixels(Constants::GROUND_Y);
    DrawRectangle(0, (int)groundYPixels, Constants::SCREEN_WIDTH, 20, DARKGREEN);

    // Dibujar pilares
    float leftX = Constants::ToPixels(Constants::LEFT_PILLAR_X);
    float leftY = Constants::ToPixels(Constants::PILLAR_Y);
    float pillarWPixels = Constants::ToPixels(Constants::PILLAR_WIDTH);
    float pillarHPixels = Constants::ToPixels(Constants::PILLAR_HEIGHT);

    DrawRectangle(leftX - pillarWPixels / 2, leftY - pillarHPixels / 2,
        pillarWPixels, pillarHPixels, Constants::PILLAR_COLOR);

    float rightX = Constants::ToPixels(Constants::RIGHT_PILLAR_X);
    DrawRectangle(rightX - pillarWPixels / 2, leftY - pillarHPixels / 2,
        pillarWPixels, pillarHPixels, Constants::PILLAR_COLOR);

    // Dibujar segmentos del puente
    for (const auto& segment : segments) {
        segment->Draw();
    }
    // Dibujar catapultas (puntos rojos)
    if (leftCatapult) leftCatapult->Draw();
    if (rightCatapult) rightCatapult->Draw();
    // Dibujar esfera
    if (ball) {
        ball->Draw();
    }
    
    // Dibujar proyectiles activos
    for (const auto& proj : activeProjectiles) {
        proj->Draw();
    }

    // Dibujar cables (líneas para visualizar)
    DrawJoints();
}

void PhysicsWorld::DrawJoints() {
    // Dibujar cables como líneas blancas
    for (auto* joint : cableJoints) {
        b2Vec2 anchorA = joint->GetAnchorA();
        b2Vec2 anchorB = joint->GetAnchorB();

        float x1 = Constants::ToPixels(anchorA.x);
        float y1 = Constants::ToPixels(anchorA.y);
        float x2 = Constants::ToPixels(anchorB.x);
        float y2 = Constants::ToPixels(anchorB.y);

        DrawLine((int)x1, (int)y1, (int)x2, (int)y2, LIGHTGRAY);

        // Dibujar círculos en los puntos de anclaje
        DrawCircle((int)x1, (int)y1, 4, RED);
        DrawCircle((int)x2, (int)y2, 4, RED);
    }
}

void PhysicsWorld::ApplyForceToBall(float forceX, float forceY) {
    if (ball) {
        ball->ApplyForce(forceX, forceY);
    }
}

//limite de pantalla
void PhysicsWorld::CheckBallBounds() {
    if (!ball) return;

    b2Vec2 pos = ball->GetBody()->GetPosition();
    float radius = 0.4f;

    // Límite izquierdo
    if (pos.x - radius < 0.0f) {
        ball->GetBody()->SetTransform(b2Vec2(radius, pos.y), 0);
    }
    // Límite derecho
    if (pos.x + radius > Constants::GROUND_WIDTH) {
        ball->GetBody()->SetTransform(b2Vec2(Constants::GROUND_WIDTH - radius, pos.y), 0);
    }
    // Límite superior (techo)
    if (pos.y - radius < Constants::CEILING_Y) {
        ball->GetBody()->SetTransform(b2Vec2(pos.x, Constants::CEILING_Y + radius), 0);
        // Pequeño impulso hacia abajo para que no se pegue
        ball->GetBody()->SetLinearVelocity(b2Vec2(ball->GetBody()->GetLinearVelocity().x, 2.0f));
    }
    
}

//Para derrota
void PhysicsWorld::SetBallActive(bool active) {
    if (ball) {
        ball->SetActive(active);
    }
}

//Impulso esfera
void PhysicsWorld::ApplyBoostToBall(float forceY) {
    if (ball) {
        ball->ApplyBoost(forceY);
    }
}

void PhysicsWorld::SetBallCanBoost(bool can) {
    if (ball) {
        ball->SetCanBoost(can);
    }
}

void PhysicsWorld::CheckEnemyCollision() {
    if (!ball) return;

    b2Vec2 ballPos = ball->GetBody()->GetPosition();
    float ballRadius = 0.4f;
    // 1 . Colisión con catapultas
    if (leftCatapult) {
        b2Vec2 catPos = leftCatapult->GetPosition();
        float dx = ballPos.x - catPos.x;
        float dy = ballPos.y - catPos.y;
        if (sqrt(dx * dx + dy * dy) < ballRadius + 0.4f) {
            contactListener.ballHitGround = true;
            TraceLog(LOG_INFO, "GAME OVER: Choque con catapulta izquierda");       
        }
        
    }
        if (rightCatapult) {
            b2Vec2 catPos = rightCatapult->GetPosition();
            float dx = ballPos.x - catPos.x;
            float dy = ballPos.y - catPos.y;
            if (sqrt(dx * dx + dy * dy) < ballRadius + 0.4f) {
                contactListener.ballHitGround = true;
                TraceLog(LOG_INFO, "GAME OVER: Choque con catapulta derecha");
            }


        }


        // 2. Colisión con los proyectiles disparados
        for (auto& proj : activeProjectiles) {
            if (!proj->IsActive()) continue;

            b2Vec2 projPos = proj->GetBody()->GetPosition();
            float dx = ballPos.x - projPos.x;
            float dy = ballPos.y - projPos.y;

            if (sqrt(dx * dx + dy * dy) < ballRadius + 0.25f) {
                contactListener.ballHitGround = true;
                TraceLog(LOG_INFO, "GAME OVER: Choque con proyectil enemigo");
            }
        }
    }


// Catapulta
void PhysicsWorld::SpawnProjectileFromCatapult(const b2Vec2& origin) {
    // Dirección: desde catapulta hacia el centro del puente
    float targetX = 20.0f;  // Centro del puente
    float dx = targetX - origin.x;
    float dy = 0.0f;

    // Ángulo aleatorio entre -30° y 30°
    std::uniform_real_distribution<float> angleDist(-30.0f, 30.0f);
    float angleDeg = angleDist(rng);
    float angleRad = angleDeg * 3.14159f / 180.0f;

    // Fuerza aleatoria entre 0.5 y 2
    std::uniform_real_distribution<float> forceDist(0.5f, 2.0f);
    float forceMagnitude = forceDist(rng);

    // Aplicar ángulo a la dirección
    float directionX = (targetX - origin.x > 0) ? 1.0f : -1.0f;
    float forceX = directionX * forceMagnitude * cos(angleRad);
    float forceY = forceMagnitude * sin(angleRad);

    auto projectile = std::make_unique<EnemyProjectile>(&world, origin.x, origin.y, 0.25f, RED);
    projectile->Launch(forceX, forceY);
    activeProjectiles.push_back(std::move(projectile));
}

// Remover proyectiles
void PhysicsWorld::RemoveExpiredProjectiles() {
    activeProjectiles.erase(
        std::remove_if(activeProjectiles.begin(), activeProjectiles.end(),
            [](const std::unique_ptr<EnemyProjectile>& p) {
                return !p->IsActive() || p->IsExpired();
            }),
        activeProjectiles.end());
}


void PhysicsWorld::UpdateProjectiles(float deltaTime) {
    for (auto& p : activeProjectiles) {
        p->Update(deltaTime);
    }
    RemoveExpiredProjectiles();
}

//metodo para estado de la simulacion
void PhysicsWorld::SetSimulationActive(bool active) {
    simulationActive = active;
}