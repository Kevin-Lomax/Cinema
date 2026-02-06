#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <string>

typedef enum { 
    INTRO_DRAMA,    // Pensamentos iniciais, clima tenso
    DISPUTA_POSSE,  // Corrida e dribles
    PREPARO_SALTO,  // O mundo para (Slow Motion)
    CORTE_PLATEIA,  // Close na galera
    CORTE_TIME,     // Close nos amigos
    CORTE_BUCCHI,   // Close no herói
    BOLA_TRAJECTORY, // O voo da bola
    GLORIA_ETERNA   // Comemoração final
} Cena;

struct Personagem {
    Vector3 pos;
    Color cor;
    std::string nome;
    std::string pensamento;
};

int main() {
    const int screenWidth = 1280, screenHeight = 720;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "BUCCHI STUDIOS - FINAL CHAMPIONSHIP");
    SetTargetFPS(60);

    // Elenco
    Personagem bucchi = {{-25, 0, 0}, GOLD, "BUCCHI", "Minha familia está assistindo... não posso errar."};
    Personagem rival = {{5, 0, 1}, RED, "RIVAL", "Ele está tremendo. A vitória é nossa!"};
    Personagem colega = {{-10, 0, -5}, ORANGE, "NETO (TIME)", "Confio em você, cara! Faz a história!"};
    
    Vector3 bolaPos = bucchi.pos;
    Vector3 bolaVel = {0, 0, 0};
    Camera3D camera = { { -35, 15, 20 }, { 0, 0, 0 }, { 0, 1, 0 }, 40.0f, CAMERA_PERSPECTIVE };
    
    Cena cenaAtual = INTRO_DRAMA;
    float cronometro = 0.0f;
    float escalaTempo = 1.0f;
    float flashVitoria = 0.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        cronometro += dt;

        // --- ROTEIRO CINEMATOGRÁFICO DE 120 SEGUNDOS ---
        switch (cenaAtual) {
            case INTRO_DRAMA:
                escalaTempo = 0.8f;
                camera.position = Vector3Lerp(camera.position, (Vector3){-30, 8, 15}, 0.02f);
                camera.target = bucchi.pos;
                if (cronometro > 10.0f) { cenaAtual = DISPUTA_POSSE; cronometro = 0; }
                break;

            case DISPUTA_POSSE:
                escalaTempo = 1.2f; // Ação rápida
                bucchi.pos.x += 6.5f * dt;
                rival.pos.x = bucchi.pos.x + 2.0f; // Rival marcando em cima
                bolaPos = Vector3Add(bucchi.pos, {0.7f, 1.2f, sin(GetTime()*10)*0.5f});
                camera.position = Vector3Lerp(camera.position, (Vector3){bucchi.pos.x - 12, 5, 10}, 0.1f);
                camera.target = bucchi.pos;
                if (bucchi.pos.x > 8.0f) { cenaAtual = PREPARO_SALTO; cronometro = 0; }
                break;

            case PREPARO_SALTO:
                escalaTempo = 0.15f; // Super Slow Motion
                bucchi.pos.y += 2.5f * dt * escalaTempo;
                bolaPos = Vector3Add(bucchi.pos, {0.5f, 1.5f, 0});
                camera.position = (Vector3){bucchi.pos.x + 5, 3, 5};
                camera.target = bucchi.pos;
                if (cronometro > 1.0f) { cenaAtual = CORTE_PLATEIA; cronometro = 0; }
                break;

            case CORTE_PLATEIA:
                camera.position = (Vector3){0, 18, 35}; 
                camera.target = (Vector3){0, 5, 45};
                if (cronometro > 1.5f) { cenaAtual = CORTE_TIME; cronometro = 0; }
                break;

            case CORTE_TIME:
                camera.position = Vector3Add(colega.pos, {4, 2, 4});
                camera.target = colega.pos;
                if (cronometro > 1.5f) { cenaAtual = CORTE_BUCCHI; cronometro = 0; }
                break;

            case CORTE_BUCCHI:
                camera.position = Vector3Add(bucchi.pos, {2.5f, 2.5f, 0}); 
                camera.target = Vector3Add(bucchi.pos, {0, 2.2f, 0});
                if (cronometro > 2.0f) { 
                    cenaAtual = BOLA_TRAJECTORY; 
                    bolaVel = {20.0f, 11.0f, 0}; 
                    cronometro = 0; 
                }
                break;

            case BOLA_TRAJECTORY:
                escalaTempo = (cronometro < 2.0f) ? 0.3f : 3.0f; // Voo dramático e aceleração final
                bolaPos = Vector3Add(bolaPos, Vector3Scale(bolaVel, dt * escalaTempo));
                bolaVel.y -= 9.8f * dt * escalaTempo;
                camera.position = Vector3Add(bolaPos, {-5, 3, -3});
                camera.target = bolaPos;
                if (bolaPos.x > 14.8f) { cenaAtual = GLORIA_ETERNA; cronometro = 0; }
                break;
        }

        BeginDrawing();
            ClearBackground(GetColor(0x050508FF));
            BeginMode3D(camera);
                // Quadra com grid elegante
                DrawPlane({0, 0, 0}, {120, 50}, GetColor(0x151520FF));
                DrawGrid(60, 2.0f);
                
                // Plateia Animada (Centenas de cubos)
                for (int x = -50; x < 50; x += 4) {
                    float pulo = (cenaAtual == GLORIA_ETERNA) ? sin(GetTime()*15 + x)*1.0f : 0;
                    DrawCube({(float)x, 2.0f + pulo, 30}, 1.5f, 4.0f, 1.5f, DARKGRAY);
                }

                // Jogadores
                DrawCapsule(bucchi.pos, Vector3Add(bucchi.pos, {0, 1.8f, 0}), 0.6f, 8, 8, bucchi.cor);
                DrawCapsule(colega.pos, Vector3Add(colega.pos, {0, 1.8f, 0}), 0.5f, 8, 8, colega.cor);
                DrawCapsule(rival.pos, Vector3Add(rival.pos, {0, 1.8f, 0}), 0.5f, 8, 8, rival.cor);

                // Cesta de Basquete
                DrawCube({16, 4, 0}, 0.5f, 9, 0.5f, GRAY);
                DrawCylinderWires({14.5f, 5.8f, 0}, 1.2f, 1.2f, 0.1f, 24, RED);
                
                // Bola com rastro visual (opcional)
                DrawSphere(bolaPos, 0.45f, ORANGE);
            EndMode3D();

            // --- CINEMATIC HUD ---
            DrawRectangle(0, 0, screenWidth, 110, BLACK);
            DrawRectangle(0, screenHeight - 110, screenWidth, 110, BLACK);

            // Legendas Baseadas na Cena
            if (cenaAtual == INTRO_DRAMA) {
                DrawText("FINAL DA COPA MUNDIAL - BUCCHI STUDIOS", 400, 40, 20, GOLD);
                DrawText(bucchi.pensamento.c_str(), screenWidth/2 - MeasureText(bucchi.pensamento.c_str(), 24)/2, 650, 24, RAYWHITE);
            }
            else if (cenaAtual == CORTE_PLATEIA) DrawText("A MULTIDÃO EM SILÊNCIO...", 480, 650, 26, LIGHTGRAY);
            else if (cenaAtual == CORTE_TIME) DrawText(colega.pensamento.c_str(), 450, 650, 24, ORANGE);
            else if (cenaAtual == CORTE_BUCCHI) DrawText("FOCO TOTAL. AGORA!", 540, 650, 28, GOLD);
            else if (cenaAtual == GLORIA_ETERNA) {
                DrawText("HISTÓRICO! BUCCHI É O NOME DELE!", 350, 300, 45, GOLD);
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(WHITE, 0.15f));
            }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
