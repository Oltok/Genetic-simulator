
#include "raylib.h"
#include <raymath.h>
#include <sys/types.h>

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - rotating cube");

    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 6.0f, 6.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Load image to create texture for the cube
    Model model = LoadModel("/home/sebas/Proyectos/Genetic-simulator/resources/models/gubi/modelo.obj");

    //Target vectors
    Vector3 tiltTarget = (Vector3){0.4f, 1.0f, 0.12f};
    Vector3 forwardTarget = (Vector3){-0.8f, 0.0f, 0.5f};

    Quaternion q = QuaternionFromMatrix(model.transform);
    
    float rotation = 0.0f;

    DisableCursor();
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        Quaternion qTiltTarget = QuaternionFromVector3ToVector3((Vector3){0,1,0}, tiltTarget);
        Quaternion qForwardTarget = QuaternionFromVector3ToVector3((Vector3){0,0,1}, forwardTarget);
        Quaternion qTarget = QuaternionMultiply(qTiltTarget, qForwardTarget);
        q = QuaternionNlerp(q, qTarget, 0.2f);
        model.transform = QuaternionToMatrix(q);
        //rotation += 1.0f;

        UpdateCamera(&camera, CAMERA_FREE);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                // Draw model defining: position, size, rotation-axis, rotation (degrees), size, and tint-color
                DrawModelEx(
                    model, 
                    (Vector3){ 0.0f, 0.0f, 0.0f }, 
                    (Vector3){ 0.5f, 1.0f, 0.0f },
                    rotation, 
                    (Vector3){ 1.0f, 1.0f, 1.0f }, 
                    BLUE
                );
                DrawModelWiresEx(
                    model, 
                    (Vector3){ 0.0f, 0.0f, 0.0f }, 
                    (Vector3){ 0.5f, 1.0f, 0.0f },
                    rotation, 
                    (Vector3){ 1.0f, 1.0f, 1.0f }, 
                    BLACK
                );

                DrawCylinderEx(Vector3Zero(), tiltTarget*3, 0.05f, 0.05f, 14, RED);
                DrawCylinderEx(Vector3Zero(), forwardTarget*3, 0.05f, 0.05f, 14, GREEN);

                DrawText("Tilt", 10, 10, 12, RED);
                DrawText("Forward", 10, 20, 12, GREEN);
                DrawGrid(10, 1.0f);

            EndMode3D();
            DrawText("Tilt", 10, 10, 30, RED);
            DrawText("Forward", 10, 40, 30, GREEN);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadModel(model);     // Unload model

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
