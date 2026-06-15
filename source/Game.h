// Game.h
#pragma once
#include "PhysicsWorld.h"
#include "Constants.h"

class Game {
private:
    PhysicsWorld physicsWorld;
    bool running;

    //Variables para victoria
    float survivalTimer;
    bool gameWin;

    //variables para derrota
    bool gameOver;
    float gameOverTimer;

    //Textura de Fondo
    Texture2D backgroundTexture;
    bool textureLoaded;

public:
    Game();
    ~Game();

    void Run();
    void Update();
    void Draw();
    void DrawUI();

    //victoria
    bool HasWon() const { return gameWin; }

private:
    void HandleInput();
};