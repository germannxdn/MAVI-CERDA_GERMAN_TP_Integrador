#pragma once
#include "raylib.h"

namespace Constants {
    // Pantalla
    constexpr int SCREEN_WIDTH = 1200;  
    constexpr int SCREEN_HEIGHT = 600;

    //Fondo
    constexpr const char* BACKGROUND_IMAGE = "assets/background.png";

    // Escala
    constexpr float PIXELS_PER_METER = 30.0f;

    inline float ToPixels(float meters) { return meters * PIXELS_PER_METER; }
    inline float ToMeters(float pixels) { return pixels / PIXELS_PER_METER; }

    // Posiciones del mundo
    constexpr float GROUND_Y = 19.0f;
    // Techo (límite superior)
    constexpr float CEILING_Y = 1.0f;  // Altura del techo en metros
    // Ancho del mundo (para el suelo)
    constexpr float GROUND_WIDTH = 40.0f;  // 40 metros de ancho

    // Configuración del puente
    constexpr int NUM_SEGMENTS = 10;           // Cantidad de segmentos
    constexpr float SEGMENT_WIDTH = 1.2f;     // Ancho de cada segmento (metros)
    constexpr float SEGMENT_HEIGHT = 0.25f;    // Alto (metros)
    constexpr float BRIDGE_START_X = 14.0f;    // Inicio del puente (X)
    constexpr float BRIDGE_Y = 12.0f;         // Altura del puente (Y)

    // Pilares
    constexpr float PILLAR_WIDTH = 0.8f;
    constexpr float PILLAR_HEIGHT = 7.0f;
    constexpr float LEFT_PILLAR_X = 12.5f;
    constexpr float RIGHT_PILLAR_X = 25.5f;
    constexpr float PILLAR_Y = GROUND_Y - PILLAR_HEIGHT / 2.0f;

    // Cables (Distance Joints)
    constexpr float CABLE_LENGTH = 1.0f;       // Largo de cada cable
    constexpr float CABLE_ATTACH_Y = BRIDGE_Y + 1.5f;  // altura 1.5 metros arriba del puente

    // Colores
    constexpr Color BACKGROUND = { 135, 206, 235, 255 };  // Celeste claro
    constexpr Color SEGMENT_COLOR = { 160, 82, 45, 255 };  // Marrón madera
    constexpr Color PILLAR_COLOR = { 139, 69, 19, 255 };   // Marrón oscuro
    constexpr Color CABLE_COLOR = { 200, 200, 200, 255 };  // Gris
    constexpr Color JOINT_COLOR = { 255, 0, 0, 255 };      // Rojo (debug)

    
}