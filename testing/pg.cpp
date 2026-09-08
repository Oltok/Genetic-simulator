
#include "raylib.h"
#include <raymath.h>
#include <sys/types.h>

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

float find_height(Mesh* pMesh, float x, float z){
    Ray ray = {
        {x, 0, z},
        {0, 1, 0}
    };
 
    RayCollision info = GetRayCollisionMesh(ray, *pMesh, MatrixIdentity());
    return info.distance;
};


int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - rotating cube");

    // Terrain Gen
    const float terrainWidth = 100.0f; //2000, 2000, 1000, 100, 100
    const float terrainDepth = 100.0f;
    const float terrainMaxHeight = 40.0f;
    const int terrainImgWidth = 10;
    const int terrainImgHeight = 10;
    const int terrainBarrier = 10;

    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ 50.0f, 45.0f, 50.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    //RenderTexture2D mainCam = LoadRenderTexture(screenWidth, screenHeight);

    Camera camera2 = { 0 };
    camera2.position = (Vector3){ 50.0f, 160.0f, 50.0f };
    camera2.target = (Vector3){ 50.0f, 50.0f, 50.0f };
    camera2.up = (Vector3){ 0.0f, 0.0f, -1.0f};
    camera2.fovy = 45.0f;
    camera2.projection = CAMERA_PERSPECTIVE;

    //Mini map
    RenderTexture2D minimap = LoadRenderTexture(120, 120);

    Rectangle minimapRect = {0.0f, 0.0f, (float)minimap.texture.width, (float)-minimap.texture.height};


    Image terrainHeightMap = GenImagePerlinNoise(terrainImgWidth, terrainImgHeight, 0, 0, 1.2f);
    Mesh terrainMesh = GenMeshHeightmap(terrainHeightMap, {terrainWidth, terrainMaxHeight, terrainDepth});
    Model terrainModel = LoadModelFromMesh(terrainMesh);
    terrainModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = LIME;


    // Load image to create texture for the cube
    Model model = LoadModel("/home/sebas/Proyectos/Genetic-simulator/resources/models/gubi/modelo.obj");

    
    DisableCursor();
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            for (int i = 0; i < 10; i++) UpdateCamera(&camera, CAMERA_FREE);
        } else {
            UpdateCamera(&camera, CAMERA_FREE);
        }

        if (IsKeyDown(KEY_Z)) {
            camera2.target = camera.target,
            camera2.position = camera.position;
        }


        //Vector2 windowStart = GetScreenToWorld3D((Vector3) {0,0, 0}, camera);

        //----------------------------------------------------------------------------------

        // Draw what the minimap sees
        //----------------------------------------------------------------------------------
        BeginTextureMode(minimap);

            ClearBackground(BLACK);

            BeginMode3D(camera2);

                DrawModel(terrainModel, Vector3Zero(), 1, WHITE);
                DrawModelWires(terrainModel, Vector3Zero(), 1, LIGHTGRAY);
                DrawCube(Vector3Zero(), 50, 50, 50, RED);


            EndMode3D();
        EndTextureMode();


        //----------------------------------------------------------------------------------

        // Draw Main Cam
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(SKYBLUE);

            BeginMode3D(camera);

                DrawModel(terrainModel, Vector3Zero(), 1, WHITE);
                DrawModelWires(terrainModel, Vector3Zero(), 1, LIGHTGRAY);

                DrawCube(Vector3Zero(), 50, 50, 50, RED);

                DrawCube(camera2.position, 4, 2, 4, PURPLE);
                DrawCapsule(camera2.position, camera2.target, 2, 4, 4, PURPLE);


            EndMode3D();

            DrawRectangle(0, 0, 120, 40, BLACK);
            DrawFPS(10, 10);

            const char *trgt = 0;
            trgt = TextFormat("%02.01f, %02.01f, %02.01f", camera.target.x, camera.target.y, camera.target.z);
            
            DrawRectangle(0, 60, 120, 40, BLACK);
            DrawText(trgt, 0, 70, 20, WHITE);


            //camera2 position
            const char *trgt2 = 0;
            trgt2 = TextFormat("%02.01f, %02.01f, %02.01f", camera2.position.x, camera2.position.y, camera2.position.z);
            
            DrawRectangle(screenWidth-180, screenHeight-220, 180, 40, BLACK);
            DrawText(trgt2, screenWidth-180, screenHeight-210, 20, WHITE);


            //camera2 target
            const char *trgt3 = 0;
            trgt3 = TextFormat("%02.01f, %02.01f, %02.01f", camera2.target.x, camera2.target.y, camera2.target.z);
            
            DrawRectangle(screenWidth-180, screenHeight-180, 180, 40, BLACK);
            DrawText(trgt3, screenWidth-180, screenHeight-170, 20, WHITE);

            DrawTextureRec(
                minimap.texture, 
                minimapRect, 
                (Vector2){(float)screenWidth - minimap.texture.width, (float)screenHeight - minimap.texture.height}, 
                WHITE);

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
