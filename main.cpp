#include <raylib.h>
#include <vector>

typedef struct {
    Vector3 position;
    Vector3 size;
    Color color;
} Gubi;

typedef struct {

    Vector3 position;
    Vector3 size;
    Color color;
} Arbol;

typedef struct {

    Vector3 position;
    Vector2 size;
    Color color;

} Charco;

int main(void){

    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    Camera3D camera = { 0 };
    camera.position = (Vector3){0.0f, 10.0f, 20.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Gubi Juanubi;
    Juanubi.size = (Vector3){2.0f, 2.0f, 2.0f};
    Juanubi.position = (Vector3){0.0f, 1.0f, 0.0f};
    Juanubi.color = RED;

    std::vector<Charco> charcos;

    for (int i = 0; i< 10; i++) {

        Charco agua;
        agua.size = (Vector2){(float)GetRandomValue(20, 150), (float)GetRandomValue(20, 150)};
        agua.position = (Vector3){(float)GetRandomValue(-350, 350), 1.0f,  (float)GetRandomValue(-350, 350)};
        agua.color = BLUE;
        
        charcos.push_back(agua);
    }


    std::vector<Arbol> arboles;

    for (int i = 0; i < 250; i++) {
                    
        Arbol arbo;
        arbo.size = (Vector3){5.0f, 15.0f, 5.0f};
        arbo.color = BLACK;
        
        bool posicionwena = false;

        while (!posicionwena) {

            arbo.position = (Vector3){(float)GetRandomValue(-450, 450), arbo.size.y / 2.0f, (float)GetRandomValue(-450, 450)};

            BoundingBox cajaarbo = {

                (Vector3){arbo.position.x - arbo.size.x/2.0f, arbo.position.y - arbo.size.y/2.0f, arbo.position.z - arbo.size.z/2.0f},
                (Vector3){arbo.position.x + arbo.size.x/2.0f, arbo.position.y + arbo.size.y/2.0f, arbo.position.z + arbo.size.z/2.0f}
            };

            bool colisionagua = false;

            for (const auto&agua : charcos) {

                BoundingBox cajacharco = {

                    (Vector3){ agua.position.x - agua.size.x/2.0f, 0.0f, agua.position.z - agua.size.y/2.0f },
                    (Vector3){ agua.position.x + agua.size.x/2.0f, 1.0f, agua.position.z + agua.size.y/2.0f }
                };

                if (CheckCollisionBoxes(cajaarbo, cajacharco)) {
                    colisionagua = true;
                    break;
                }
            }

            if (!colisionagua) {
                posicionwena = true;
            }
        }
        arboles.push_back(arbo);
    }

    
    
    DisableCursor();
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            for (int i = 0; i < 10; i++) UpdateCamera(&camera, CAMERA_FREE);
        } else {
            UpdateCamera(&camera, CAMERA_FREE);
        }

        if (IsKeyPressed(KEY_Z)) camera.target = (Vector3){ 0.0f, 0.0f, 0.0f};

        BeginDrawing();
            ClearBackground(SKYBLUE);
            BeginMode3D(camera);
            
                DrawPlane((Vector3){0.0f, 0.0f, 0.0f}, (Vector2){1000.0f, 1000.0f}, GREEN);
                
                for (const auto &agua : charcos) {

                    DrawPlane(agua.position, agua.size, agua.color);

                }

                DrawCubeV(Juanubi.position, Juanubi.size, Juanubi.color);
                DrawCubeWiresV(Juanubi.position, Juanubi.size, BLACK);

                for (const auto &arbo : arboles) {

                    DrawCubeV(arbo.position, arbo.size, arbo.color);
                    DrawCubeWiresV(arbo.position, arbo.size, DARKGRAY);
                }

            EndMode3D();

        
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
