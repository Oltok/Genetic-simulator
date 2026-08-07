//#include <cstdio>
#include <string>
#include <raylib.h>
#include <raymath.h> //mate y maticas
#include <vector>
#include<rlgl.h>

#include "rcamera.h" //con comillas para incluir el que tenemos en el directorio

#include "ui_util.h"
#include "bioma.h"

//func declaration para poder usarlas donde sea -- Despues podriamos hacer un file que contenga todas las declaraciones globales
float find_height(Mesh*, float, float);
Vector3 GetAvgNormal(Vector3, Vector3);


//const and variables -- Despues podriamos hacer un file que contenga todas las declaraciones globales
Model gubi_model;
Model tree_model;
Mesh* pTerrainMesh;
const int ADN_LENGTH = 10;



//CLASS IMPLEMENTATION
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
        float pos_x = (float)GetRandomValue(0, terrainWidth - terrainBarrier);
        float pos_z = (float)GetRandomValue(0, terrainDepth - terrainBarrier);
        
        return (Vector3){pos_x, find_height(pTerrainMesh, pos_x, pos_z), pos_z};                          
    }
};


class Gubi : public Entity // lo que tenga la clase Gubi tmb tiene la clase Entity. Por si queremos mas criaturas despues
{
public:

    Model model;
    Brain brain;

    Vector3 targetPosition;
    bool isMoving;
    float speed;

    Quaternion q;

    Gubi() {    //inicializar las variables

        model = gubi_model;

        BoundingBox bbox = GetModelBoundingBox(model);
        size = bbox.max - bbox.min;

        float pos_x = (float)GetRandomValue(0, terrainWidth-terrainBarrier);
        float pos_z = (float)GetRandomValue(0, terrainDepth-terrainBarrier);
        position = (Vector3){pos_x, find_height(pTerrainMesh, pos_x, pos_z)+size.y/2, pos_z};

        targetPosition = position;
        isMoving = false;
        speed = 0.1f;

        q = QuaternionFromMatrix(model.transform);
    }

    void Draw() {

        DrawModel(model, position, 1, RED);
        DrawModelWires(model, position, 1, BLACK);
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
        Move(direction);
        ApplyRotation(direction);
    }

    //He separado las funciones para tener pathfinding y moverse como tal mas organizado
    void Move(Vector3 direction){
        position.x += direction.x * speed; 
        position.z += direction.z * speed; 
        position.y = find_height(pTerrainMesh, position.x, position.z) + size.y/2; 
    }

    void ApplyRotation(Vector3 direction){
        Quaternion qForwardTarget = QuaternionFromVector3ToVector3((Vector3){0,0,1}, direction);
        Quaternion qTiltTarget = QuaternionFromVector3ToVector3((Vector3){0,1,0}, GetAvgNormal(position, size)); //Para las cuestas
        Quaternion qTarget = QuaternionMultiply(qTiltTarget, qForwardTarget);
        q = QuaternionNlerp(q, qTarget, 0.2f);
        model.transform = QuaternionToMatrix(q);
    }

    Vector3 GetAvgNormal(Vector3 pos, Vector3 size){
    
        float x;
        float z;
        float y = 3.0f;

        //top left
        x = -size.x/2;
        z = -size.z/2;
        Ray tl = (Ray){
        pos+(Vector3){x, -y, z},
        pos+(Vector3){x, y, z}, 
        };

        //top right
        x = size.x/2;
        z = -size.z/2;
        Ray tr = (Ray){
        pos+(Vector3){x, -y, z},
        pos+(Vector3){x, y, z}, 
        };

        //bottom left
        x = -size.x/2;
        z = size.z/2;
        Ray bl = (Ray){
        pos+(Vector3){x, -y, z},
        pos+(Vector3){x, y, z}, 
        };

        //bottom right
        x = size.x/2;
        z = size.z/2;
        Ray br = (Ray){
        pos+(Vector3){x, -y, z},
        pos+(Vector3){x, y, z}, 
        };

        Vector3 tlCollision = GetRayCollisionMesh(tl, *pTerrainMesh, MatrixIdentity()).normal;
        Vector3 trCollision = GetRayCollisionMesh(tr, *pTerrainMesh, MatrixIdentity()).normal;
        Vector3 blCollision = GetRayCollisionMesh(bl, *pTerrainMesh, MatrixIdentity()).normal;
        Vector3 brCollision = GetRayCollisionMesh(br, *pTerrainMesh, MatrixIdentity()).normal;

        Vector3 avg = Vector3Add(tlCollision, trCollision);
        avg = Vector3Add(avg, blCollision);
        avg = Vector3Add(avg, brCollision);
        avg = Vector3Normalize(avg);

        return avg;
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

//FUNCTION IMPLEMENTATION
float find_height(Mesh* pMesh, float x, float z){
    Ray ray = {
        {x, 0, z},
        {0, 1, 0}
    };

    
    RayCollision info = GetRayCollisionMesh(ray, *pMesh, MatrixIdentity());
    return info.distance;
};

Mesh GenTerrainAssignColor()
{
    Image terrainHeightMap = GenImagePerlinNoise(terrainImgWidth, terrainImgHeight, 0, 0, 1.2f);
    Image temperatureMap = GenImagePerlinNoise(terrainImgWidth, terrainImgHeight, 1000, 500, 1.2f); //meto offsets random a mano en cada mapa par que no coincidan
    Image humidityMap = GenImagePerlinNoise(terrainImgWidth, terrainImgHeight, -500, 2000, 1.2f);
    Image qualityMap = GenImagePerlinNoise(terrainImgWidth, terrainImgHeight, 3000, -1000, 1.2f);
    Image quantityMap = GenImagePerlinNoise(terrainImgWidth, terrainImgHeight, 50, 8000, 1.2f);

    //pillar la informacion
    Color* heightColors = LoadImageColors(terrainHeightMap);
    Color* temperatureColors = LoadImageColors(temperatureMap);
    Color* humidityColors = LoadImageColors(humidityMap);
    Color* qualityColors = LoadImageColors(qualityMap);
    Color* quantityColors = LoadImageColors(quantityMap);

    World::mapWidth = terrainImgWidth;
    World::mapHeight = terrainImgHeight;
    World::worldWidth = terrainWidth;
    World::worldDepth = terrainDepth; 

    World::mapData.resize(terrainImgWidth * terrainImgHeight);

    for (int y = 0; y < terrainImgHeight; y++) {
        for (int x = 0; x < terrainImgWidth; x++) {

            int index = y * terrainImgWidth + x;

            //normalizar valores 0.0 - 1.0
            World::mapData[index].height = heightColors[index].r / 255.0f;
            World::mapData[index].temperature = temperatureColors[index].r / 255.0f;
            World::mapData[index].humidity = humidityColors[index].r / 255.0f;
            World::mapData[index].quality = qualityColors[index].r / 255.0f;
            World::mapData[index].quantity = quantityColors[index].r / 255.0f;
        }
    }

    Mesh terrainMesh = GenMeshHeightmap(terrainHeightMap, {terrainWidth, terrainMaxHeight, terrainDepth});

    UnloadImageColors(heightColors);
    UnloadImageColors(temperatureColors);
    UnloadImageColors(humidityColors);
    UnloadImageColors(qualityColors);
    UnloadImageColors(quantityColors);

    UnloadImage(terrainHeightMap); //Como la malla ya esta generada podemos liberar de la memoria la imagen
    UnloadImage(temperatureMap);
    UnloadImage(humidityMap);
    UnloadImage(qualityMap);
    UnloadImage(quantityMap);

    return terrainMesh;
}



int main(void){

    const int screenWidth = 800;
    const int screenHeight = 450;
    

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");


    //Terrain generation
    Mesh terrainMesh= GenTerrainAssignColor();
    pTerrainMesh = &terrainMesh;

    // generar biomas y hacer 3 iteraciones de smooth
    World::GenerateBiomes();
    World::SmoothBiomes(3);

    Model terrainModel = LoadModelFromMesh(terrainMesh);
    
    //VISUALIZACION DE BIOMAS (TEXTURA 2D TEMPORAL)
    Image colormapImage = GenImageColor(World::mapWidth, World::mapHeight, BLANK);

    // pintar cada coordenada dependiendo del bioma
    for (int y = 0; y < World::mapHeight; y++) {
        for (int x = 0; x < World::mapWidth; x++) {

            float worldX = ((float) x / World::mapWidth) * World::worldWidth;
            float worldZ = ((float) y / World::mapHeight) * World::worldDepth;

            //No podriamos hacer GetBaseBiome antes de pasarlo a coordenadas de mundo?
            BaseBiome biome = World::GetBaseBiome(worldX, worldZ);   
            Color biomeColor = BLACK;

            switch (biome) {
            case BaseBiome::FIELD: biomeColor = YELLOW; break;
            case BaseBiome::JUNGLE: biomeColor = GREEN; break;
            case BaseBiome::DESERT: biomeColor = BROWN; break;
            case BaseBiome::ICE: biomeColor = WHITE; break;
            case BaseBiome::TUNDRA: biomeColor = PURPLE; break;
            case BaseBiome::ACQUATIC: biomeColor = BLUE; break;            
            default: biomeColor = RED; break;
            }

            ImageDrawPixel(&colormapImage, x, y, biomeColor);
        }
    }


    Texture2D terrainTexture = LoadTextureFromImage(colormapImage);
    UnloadImage(colormapImage);

    terrainModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = terrainTexture;
    terrainModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;


    //model loading - se ve que debe ir despues del InitWindow
    //Arbol
    Model tree_model = LoadModel("resources/models/arbol/modelo.obj");
    BoundingBox base_tree_bbox = GetModelBoundingBox(tree_model);

    //Gubi - Placeholder
    gubi_model = LoadModel("resources/models/gubi/modelo.obj");


    // camera init    
    Camera3D camera = { 0 };
    camera.position = (Vector3){
        terrainWidth/2, 
        find_height(pTerrainMesh, terrainWidth/2, terrainDepth/2)+50, 
        terrainDepth/2
    };
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    //cosas en el mundo
    int n_arboles = 0; //500
    int n_charcos = 0; //10
    
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
        arbo.model = tree_model; //TODO moverlo a su funcion de inizializacion, como la de gubi
        
        bool posicionwena = false;
        while (!posicionwena) {

            float pos_x = (float)GetRandomValue(0, terrainWidth);
            float pos_z = (float)GetRandomValue(0, terrainDepth);
        
            //encontramos la altura en esa posicion y la variamos un poco para tener arboles con diferentes alturas
            arbo.position = (Vector3){pos_x, find_height(pTerrainMesh, pos_x, pos_z)+GetRandomValue(-1, 0), pos_z};

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
                
                DrawModel(terrainModel, Vector3Zero(), 1, WHITE);
                DrawModelWires(terrainModel, Vector3Zero(), 1, LIGHTGRAY);
                
                for (const auto &agua : charcos) {

                    DrawPlane(agua.position, agua.size, agua.color);

                }

                
                // para ver donde va (me ayuda al debugging)
                if (Juanubi.isMoving) {
                    DrawLine3D(Juanubi.position, Juanubi.targetPosition, PURPLE);
                    DrawSphere(Juanubi.targetPosition, 0.5f, PURPLE);
                }

                Juanubi.Draw();

                for (const auto &arbo : arboles) {

                    arbo.Draw();
                }

            EndMode3D();

            //Simulation speed buttons
            DrawSpeedButton(10.0, 10.0, "x1", selected_speed);
            DrawSpeedButton(50.0, 10.0, "x2", selected_speed);
            DrawSpeedButton(90.0, 10.0, "x4", selected_speed);

        EndDrawing();
    }

    UnloadModel(tree_model); //Pongo el unload fuera del loop porque seguiremos spawneando
    UnloadTexture(terrainTexture);

    CloseWindow();

    return 0;
}
