// Game.cpp
#include "Game.h"
#include "raylib.h"


//Constructor
Game::Game() : running(true), gameOver(false), gameWin(false), gameOverTimer(0.0f), survivalTimer(20.0f), textureLoaded(false) {
    InitWindow(Constants::SCREEN_WIDTH, Constants::SCREEN_HEIGHT,
        "MAVI II - Unidad 3: Trabajo Practico Integrador");
    SetTargetFPS(60);
    // Cargar fondo
    backgroundTexture = LoadTexture(Constants::BACKGROUND_IMAGE);
    textureLoaded = (backgroundTexture.id != 0);
    if (!textureLoaded) {
        TraceLog(LOG_WARNING, "No se pudo cargar el fondo: %s", Constants::BACKGROUND_IMAGE);
    }
}

// Destructor
Game::~Game() {
    if (textureLoaded) {
        UnloadTexture(backgroundTexture);
    }
    CloseWindow();
}

void Game::HandleInput() {
    // Control de la esfera con flechas
    float forceAmount = 10.0f;

    if (IsKeyDown(KEY_RIGHT)) {
        physicsWorld.ApplyForceToBall(forceAmount, 0);
    }
    if (IsKeyDown(KEY_LEFT)) {
        physicsWorld.ApplyForceToBall(-forceAmount, 0);
    }
    
    // Reiniciar con R
    if (IsKeyPressed(KEY_R)) {
        physicsWorld.ResetBall();
        physicsWorld.SetBallActive(true);  // Reactivar la esfera
        physicsWorld.SetBallCanBoost(true);
        physicsWorld.SetSimulationActive(true); // Reactivar la simulacion
        gameOver = false;
        gameWin = false;
        gameOverTimer = 0.0f;
        survivalTimer = 20.0f; //Resetear temporizador
        physicsWorld.ResetBallHitGround();
        physicsWorld.ResetBallHitBridge();
    }

    // Impulso hacia arriba con ESPACIO
    if (IsKeyPressed(KEY_SPACE)) {
        physicsWorld.ApplyBoostToBall(3.0f);  // Fuerza hacia arriba
    }
        
}

void Game::Update() {
    physicsWorld.Update();

    // ===== TEMPORIZADOR DE SUPERVIVENCIA =====
    if (!gameOver && !gameWin) {
        survivalTimer -= GetFrameTime();
        if (survivalTimer <= 0.0f) {
            gameWin = true;
            survivalTimer = 0.0f;
            physicsWorld.SetBallActive(false);  // Congelar la esfera
            physicsWorld.SetSimulationActive(false); // Detener la simulacion
        }
    }
    // Verificar si la esfera toca el suelo
    if (physicsWorld.HasBallHitGround() && !gameOver) {
        gameOver = true;
        gameOverTimer = 2.0f;  // Mostrar mensaje por 2 segundos
        physicsWorld.SetBallActive(false); //desactivar la esfera
        physicsWorld.SetSimulationActive(false); //detener la simulacion
        TraceLog(LOG_INFO, "=== GAME OVER ACTIVADO ===");
    }

    // Verificar si la bola toca el puente (para recargar el boost)
    if (physicsWorld.HasBallHitBridge()) {
        physicsWorld.SetBallCanBoost(true);
        physicsWorld.ResetBallHitBridge();
    }

    // Actualizar timer
    if (gameOverTimer > 0) {
        gameOverTimer -= GetFrameTime();
        if (gameOverTimer <= 0) {
            // physicsWorld.ResetBall();
            // gameOver = false;
        }
    }
}

void Game::DrawUI() {
    DrawText("MAVI II - Trabajo Practico Integrador", 10, 10, 25, BLACK);
    DrawText("Revolute Joints: conectan los segmentos", 10, 50, 17, BLACK);
    DrawText("Distance Joints: cables de los pilares", 10, 70, 17, BLACK);
    DrawText("Evita el suelo y las esferas rojas por 30 segundos", 10, 90, 17, BLACK);
    DrawText("Presiona SPACE para impulsarte hacia arriba", 10, 110, 17, BLACK);
    
    // Mostrar temporizador de supervivencia
    if (!gameOver && !gameWin) {
        int secondsLeft = (int)survivalTimer + 1;
        DrawText(TextFormat("Tiempo para ganar: %d", secondsLeft),
            Constants::SCREEN_WIDTH - 250, 10, 20, WHITE);
    }
    
    //Mensajes de victoria
    if (gameWin) {
        DrawText("VICTORIA! Sobreviviste 20 segundos!",
            Constants::SCREEN_WIDTH / 2 - 200,
            Constants::SCREEN_HEIGHT / 2,
            30, GREEN);
        DrawText("Presiona R para jugar de nuevo",
            Constants::SCREEN_WIDTH / 2 - 180,
            Constants::SCREEN_HEIGHT / 2 + 40,
            20, YELLOW);
    }

    //Mensajes de derrota
    if (gameOver) {
        DrawText("GAME OVER",
            Constants::SCREEN_WIDTH / 2 - 250,
            Constants::SCREEN_HEIGHT / 2,
            30, RED);
        DrawText("Presiona R para reiniciar",
            Constants::SCREEN_WIDTH / 2 - 150,
            Constants::SCREEN_HEIGHT / 2 + 40,
            20, YELLOW);

    }
}

void Game::Draw() {
    BeginDrawing();

    // Dibujar fondo 
    if (textureLoaded) {
        DrawTexturePro(backgroundTexture,
            { 0, 0, (float)backgroundTexture.width, (float)backgroundTexture.height },
            { 0, 0, (float)Constants::SCREEN_WIDTH, (float)Constants::SCREEN_HEIGHT },
            { 0, 0 }, 0, WHITE);
    }
    else {
        ClearBackground(Constants::BACKGROUND);
    }

    physicsWorld.Draw();
    DrawUI();

    EndDrawing();
}

void Game::Run() {
    while (running && !WindowShouldClose()) {
        HandleInput();
        Update();
        Draw();
    }
}

