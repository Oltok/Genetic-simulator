//#include <cstdio>
#include <string>
#include <raylib.h>
#include <raymath.h> //mate y maticas
#include <vector>

#include "rcamera.h" //con comillas para incluir el que tenemos en el directorio
#include "ui_util.h"


const int ADN_LENGTH = 10;

class Entity 
{
public:
    Vector3 position;
    Vector3 size;
    Color color;

};


class Brain
{
public:
    char genes[ADN_LENGTH];
    
    Vector3 Destiny() {
        return (Vector3){(float)GetRandomValue(400,600), 1.0f, (float)GetRandomValue(400, 600)}; //aleatorio
    }
};


class Gubi : public Entity // lo que tenga la clase Gubi tmb tiene la clase Entity. Por si queremos mas criaturas despues
{
public:

    Brain brain;
    Vector3 targetPosition;
    bool isMoving;
    float speed;

    Gubi() {    //inicializar las variables

        position = (Vector3){0.0f, 1.0f, 0.0f};
        size = (Vector3){2.0f, 2.0f, 2.0f};
        color = RED;
        targetPosition = position;
        isMoving = false;
        speed = 0.4f;
    }

    void MoveTo() { 

        if (!isMoving) return;

        Vector3 direction = Vector3Subtract(targetPosition, position);
        direction.y = 0.0; //si no se le va la olla al cabrón

        //para que sea más óptimo he visto que se puede usar el cuadrado en lugar de la raíz para calcular la distancia
        float distancia = Vector3LengthSqr(direction);
        float thrLlegada = 0.3f; //para definir si ha llegado a la posición, está al cuadrado

        if (distancia <= thrLlegada) {
            position.x = targetPosition.x;
            position.z = targetPosition.z;
            isMoving = false;
            return;
        }

        direction = Vector3Normalize(direction);

        //Al quitar deltatime, la velocidad depende de los frames
        //entonces si subimos los FPS, corremos la simulacion a mas velocidad :D
        position.x += direction.x * speed; 
        position.z += direction.z * speed; 

    }
};


class Arbol 
{
public:

    Vector3 position;
    Model model;
    BoundingBox bbox;

    void Draw() const{
        DrawModel(model, position, 1, WHITE);
        //DrawModelWires(model, position, 1, BLACK);
    }
    
};

class Charco
{
public:

    Vector3 position;
    Vector2 size;
    Color color;

};


float find_height(Mesh mesh, float x, float z){
    Ray ray = {
        {x, 0, z},
        {0, 1, 0}
    };

    
    RayCollision info = GetRayCollisionMesh(ray, mesh, MatrixIdentity());
    return info.distance;
};


int main(void){

    const int screenWidth = 800;
    const int screenHeight = 450;
    

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");


    //Terrain generation
    const float terrainWidth = 2000.0f;
    const float terrainDepth = 2000.0f;
    const float terrainMaxHeight = 1000.0f;
    const int terrainImgWidth = 100;
    const int terrainImgHeight = 100;

    Image terrainHeightMap = GenImagePerlinNoise(terrainImgWidth, terrainImgHeight, 0, 0, 1);
    Mesh terrainMesh = GenMeshHeightmap(terrainHeightMap, {terrainWidth, terrainMaxHeight, terrainDepth});

    UnloadImage(terrainHeightMap); //Como la malla ya esta generada podemos liberar de la memoria la imagen
    Model terrainModel = LoadModelFromMesh(terrainMesh);
    terrainModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = LIME;


    //model loading - se ve que debe ir despues del InitWindow
    
    //Arbol
    Model tree_model = LoadModel("resources/models/arbol/modelo.obj");
    BoundingBox base_tree_bbox = GetModelBoundingBox(tree_model);

    //en este caso no se carga la textura porque en el modelo.mtl ya se referencia
    //Para cargar textura y ponerla al modelo:
    //Texture2D tree_texture = LoadTexture("resources/models/arbol/base_color.png");
    //tree_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tree_texture;


    // camera init    
    Camera3D camera = { 0 };
    camera.position = (Vector3){terrainWidth/2, terrainMaxHeight+10, terrainDepth/2};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;


    //cosas en el mundo
    int n_arboles = 500;
    int n_charcos = 10;

    Gubi Juanubi;

    std::vector<Charco> charcos;

    for (int i = 0; i < n_charcos; i++) {

        Charco agua;
        agua.size = (Vector2){(float)GetRandomValue(20, 150), (float)GetRandomValue(20, 150)};
        agua.position = (Vector3){(float)GetRandomValue(0, terrainWidth), 1.0f,  (float)GetRandomValue(0, terrainDepth)};
        agua.color = BLUE;
        
        charcos.push_back(agua);
    }


    std::vector<Arbol> arboles;
    

    for (int i = 0; i < n_arboles; i++) {
                    
        Arbol arbo;
        arbo.model = tree_model;
        
        bool posicionwena = false;
        while (!posicionwena) {

            float pos_x = (float)GetRandomValue(0, terrainWidth);
            float pos_z = (float)GetRandomValue(0, terrainDepth);
        
            //encontramos la altura en esa posicion y la variamos un poco para tener arboles con diferentes alturas
            arbo.position = (Vector3){pos_x, find_height(terrainMesh, pos_x, pos_z)+GetRandomValue(-1, 1), pos_z};

            //La BBox original esta en 0,0,0. Creamos una nueva y le sumamos la posicion del arbol que creamos.
            BoundingBox cajaarbo;
            cajaarbo.max = Vector3Add(base_tree_bbox.max, arbo.position);
            cajaarbo.min = Vector3Add(base_tree_bbox.min,  arbo.position);

            arbo.bbox = cajaarbo;

            bool colisionagua = false;

            for (const auto &agua : charcos){
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

    std::string selected_speed = "x1";
    
    DisableCursor();
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            for (int i = 0; i < 5; i++) UpdateCamera(&camera, CAMERA_FREE);
        } else {
            UpdateCamera(&camera, CAMERA_FREE);
        }

        if (IsKeyPressed(KEY_Z)) camera.target = (Vector3){ 0.0f, 0.0f, 0.0f};

        //simulation speed
        if (IsKeyPressed(KEY_ONE)) {SetTargetFPS(60); selected_speed = "x1";}
        if (IsKeyPressed(KEY_TWO)) {SetTargetFPS(120); selected_speed = "x2";}
        if (IsKeyPressed(KEY_THREE)) {SetTargetFPS(240); selected_speed = "x4";}
    
        if (!Juanubi.isMoving) {
            Juanubi.targetPosition = Juanubi.brain.Destiny();
            Juanubi.isMoving = true;
        }

        Juanubi.MoveTo();

        BeginDrawing();
            ClearBackground(SKYBLUE);
            BeginMode3D(camera);
            
                //DrawPlane((Vector3){0.0f, 0.0f, 0.0f}, (Vector2){1000.0f, 1000.0f}, GREEN);
                DrawModel(terrainModel, Vector3Zero(), 1, WHITE);
                DrawModelWires(terrainModel, Vector3Zero(), 1, GREEN);

                for (const auto &agua : charcos) {

                    DrawPlane(agua.position, agua.size, agua.color);

                }
                
                // para ver donde va (me ayuda al debugging)
                if (Juanubi.isMoving) {
                    DrawLine3D(Juanubi.position, Juanubi.targetPosition, PURPLE);
                    DrawSphere(Juanubi.targetPosition, 0.5f, PURPLE);
                }

                DrawCubeV(Juanubi.position, Juanubi.size, Juanubi.color);
                DrawCubeWiresV(Juanubi.position, Juanubi.size, BLACK);

                for (const auto &arbo : arboles) {

                    arbo.Draw();
                }

            EndMode3D();

            DrawSpeedButton(10.0, 10.0, "x1", selected_speed);
            DrawSpeedButton(50.0, 10.0, "x2", selected_speed);
            DrawSpeedButton(90.0, 10.0, "x4", selected_speed);

        EndDrawing();
    }

    UnloadModel(tree_model); //Pongo el unload fuera del loop porque seguiremos spawneando

    CloseWindow();

    return 0;
}
