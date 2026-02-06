#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <string>

// Estados Cinematográficos (Storytelling)
typedef enum { ABERTURA, O_CONFRONT_INICIAL, O_DRIBLE_DA_VITORIA, O_SALTO_DA_FE, O_ARREMESSO_FINAL, CELEBRACAO_EPICA } Cena;

struct Ator {
    Vector3 pos;
    Color cor;
    std::string nome;
    std::string papel;
    std::string fala;
};

int main() {
    const int sW = 1280, sH = 720;
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(sW, sH, "BUCCHI STUDIOS - O LEGADO");
    SetTargetFPS(60);

    // --- PERSONAGENS COM CARISMA ---
    Ator bucchi = {{-25, 0, 0}, GOLD, "BUCCHI", "O LÍDER", "Este é o nosso momento. Não olhem para trás."};
    Ator neto = {{-22, 0, 8}, ORANGE, "NETO", "O VETERANO", "Confio em você, garoto. Faz o que sabe!"};
    Ator rival = {{12, 0, 0}, RED, "KANE", "O ANTAGONISTA", "Seu tempo acabou, Bucchi. A quadra é minha."};

    // --- PLATEIA COLORIDA E VIVA ---
    std::vector<Color> paletaCrowd = {MAROON, DARKBLUE, DARKGREEN, PURPLE, DARKPURPLE, ORANGE};
    struct Torcedor { Vector3 pos; Color cor; float delay; };
    std::vector<Torcedor> plateia;
    for(int x = -60; x < 60; x += 4) {
        for(int z = 30; z < 45; z += 3) {
            plateia.push_back({{(float)x, 1.5f, (float)z}, paletaCrowd[GetRandomValue(0, 5)], (float)GetRandomValue(0, 100)/10.0f});
        }
    }

    Camera3D camera = { { -40, 15, 30 }, { 0, 0, 0 }, { 0, 1, 0 }, 25.0f, CAMERA_PERSPECTIVE };
    Vector3 bolaPos = bucchi.pos;
    Cena cenaAtual = ABERTURA;
    float timerCena = 0.0f;
    std::string legendaAtual = "";

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        timerCena += dt;

        // --- ROTEIRO DIRIGIDO (MOVIMENTOS SUAVES) ---
        switch (cenaAtual) {
            case ABERTURA:
                legendaAtual = bucchi.nome + ": " + bucchi.fala;
                camera.position = Vector3Lerp(camera.position, (Vector3){-30, 6, 12}, 0.02f);
                camera.target = Vector3Lerp(camera.target, bucchi.pos, 0.03f);
                if (timerCena > 6) { cenaAtual = O_CONFRONT_INICIAL; timerCena = 0; }
                break;

            case O_CONFRONT_INICIAL:
                legendaAtual = rival.nome + ": " + rival.fala;
                camera.position = Vector3Lerp(camera.position, (Vector3){5, 5, 15}, 0.02f);
                camera.target = Vector3Lerp(camera.target, rival.pos, 0.03f);
                if (timerCena > 6) { cenaAtual = O_DRIBLE_DA_VITORIA; timerCena = 0; }
                break;

            case O_DRIBLE_DA_VITORIA:
                legendaAtual = neto.nome + ": " + neto.fala;
                bucchi.pos.x += 4.0f * dt;
                bolaPos = Vector3Lerp(bolaPos, (Vector3){bucchi.pos.x + 1.2f, 1.1f + (float)abs(sin(GetTime()*10))*0.8f, 0}, 0.2f);
                camera.position = Vector3Lerp(camera.position, (Vector3){bucchi.pos.x - 15, 7, 12}, 0.04f);
                camera.target = bucchi.pos;
                if (bucchi.pos.x > 2) { cenaAtual = O_SALTO_DA_FE; timerCena = 0; }
                break;

            case O_SALTO_DA_FE:
                legendaAtual = "O GINÁSIO PRENDE O FÔLEGO...";
                bucchi.pos.y = Lerp(bucchi.pos.y, 5.0f, 0.03f);
                bolaPos = Vector3Lerp(bolaPos, Vector3Add(bucchi.pos, {0.5f, 1.2f, 0}), 0.1f);
                // Efeito Dolly Zoom Manual
                camera.fovy = Lerp(camera.fovy, 15.0f, 0.02f);
                camera.position = Vector3Lerp(camera.position, (Vector3){bucchi.pos.x + 10, 4, 10}, 0.02f);
                camera.target = bucchi.pos;
                if (timerCena > 5) { cenaAtual = O_ARREMESSO_FINAL; timerCena = 0; }
                break;

            case O_ARREMESSO_FINAL:
                camera.fovy = 25.0f;
                bolaPos = Vector3Lerp(bolaPos, (Vector3){14.5f, 5.5f, 0}, 0.03f);
                camera.position = Vector3Lerp(camera.position, (Vector3){18, 10, 8}, 0.02f);
                camera.target = bolaPos;
                if (Vector3Distance(bolaPos, (Vector3){14.5f, 5.5f, 0}) < 0.2f) { cenaAtual = CELEBRACAO_EPICA; timerCena = 0; }
                break;

            case CELEBRACAO_EPICA:
                legendaAtual = "VITÓRIA HISTÓRICA! O LEGADO CONTINUA.";
                camera.position = (Vector3){15 + cos(GetTime())*15, 8, sin(GetTime())*15};
                camera.target = (Vector3){14.5f, 5, 0};
                break;
        }

        BeginDrawing();
            ClearBackground(GetColor(0x080810FF));
            BeginMode3D(camera);
                
                // Quadra com textura de cor "Quente"
                DrawPlane({0, 0, 0}, {160, 60}, GetColor(0x1a1a25FF));
                for(int i = -80; i < 80; i += 4) DrawLine3D({(float)i, 0.01f, -30}, {(float)i, 0.01f, 30}, GetColor(0x252535FF));

                // Desenhar Plateia Colorida
                for (const auto& t : plateia) {
                    float pulo = (cenaAtual == CELEBRACAO_EPICA) ? sin(GetTime() * 8 + t.delay) * 0.6f : 0;
                    DrawCube({t.pos.x, t.pos.y + pulo, t.pos.z}, 1.2f, 3.0f, 1.2f, t.cor);
                }

                // Atores (Visual Profissional)
                DrawCapsule(bucchi.pos, Vector3Add(bucchi.pos, {0, 2.2f, 0}), 0.6f, 16, 16, bucchi.cor);
                DrawCapsule(rival.pos, Vector3Add(rival.pos, {0, 2.2f, 0}), 0.6f, 16, 16, rival.cor);
                DrawCapsule(neto.pos, Vector3Add(neto.pos, {0, 2.2f, 0}), 0.6f, 16, 16, neto.cor);

                // Cesta
                DrawCube({16, 4, 0}, 0.5f, 8.5f, 0.5f, DARKGRAY);
                DrawCylinderWires({14.5f, 5.5f, 0}, 1.3f, 1.3f, 0.2f, 32, ORANGE);
                
                DrawSphere(bolaPos, 0.45f, ORANGE);

            EndMode3D();

            // --- ESTÉTICA DE CINEMA (LETTERBOX) ---
            DrawRectangle(0, 0, sW, 110, BLACK);
            DrawRectangle(0, sH - 110, sW, 110, BLACK);

            // Legendas Centralizadas
            DrawText(legendaAtual.c_str(), sW/2 - MeasureText(legendaAtual.c_str(), 22)/2, sH - 75, 22, (cenaAtual == CELEBRACAO_EPICA) ? GOLD : RAYWHITE);
            
            // Créditos Iniciais
            if (cenaAtual == ABERTURA) {
                DrawText("BUCCHI STUDIOS", 50, 40, 30, GOLD);
                DrawText("APRESENTA", 50, 75, 15, RAYWHITE);
            }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
