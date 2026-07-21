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
    Juanubi.position = (Vector3){0.0f, Juanubi.size.y / 2.0f, 0.0f};
    Juanubi.color = RED;

    std::vector<Arbol> arboles;

    for (int i = 0; i < 250; i++) {
                    
        Arbol arbo;
        arbo.size = (Vector3){5.0f, 15.0f, 5.0f};
        arbo.position = (Vector3){ (float)GetRandomValue(-500, 500), arbo.size.y / 2.0f, (float)GetRandomValue(-500, 500)};        
        arbo.color = BLACK;
        
        arboles.push_back(arbo);
    }
    
    DisableCursor();
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        UpdateCamera(&camera, CAMERA_FREE);
        if (IsKeyPressed(KEY_Z)) camera.target = (Vector3){ 0.0f, 0.0f, 0.0f};

        BeginDrawing();
            ClearBackground(SKYBLUE);
            BeginMode3D(camera);
            
                DrawPlane((Vector3){0.0f, 0.0f, 0.0f}, (Vector2){1000.0f, 1000.0f}, GREEN);
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
