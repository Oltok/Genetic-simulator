#include <string>
#include <raylib.h>
#include <raymath.h> //mate y maticas
#include <vector>
#include<rlgl.h>

#include "rcamera.h" //con comillas para incluir el que tenemos en el directorio

#include "ui_util.h"
#include "bioma.h"
#include "recursos.h"

//func declaration para poder usarlas donde sea -- Despues podriamos hacer un file que contenga todas las declaraciones globales
float find_height(Mesh*, float, float);
Vector3 GetAvgNormal(Vector3, Vector3);


//const and variables -- Despues podriamos hacer un file que contenga todas las declaraciones globales
Model tree_model;
Model gubi_model;
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
        speed = 0.4f;

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
                                                                                  //0,0,1 es como el vector forward global
                                                                                  //0,0,1 es el vector up
        Quaternion qForwardTarget = QuaternionFromVector3ToVector3((Vector3){0,0,1}, direction);
        Quaternion qTiltTarget = QuaternionFromVector3ToVector3((Vector3){0,1,0}, (GetAvgNormal())); //Para las cuestas
        Quaternion qTarget = QuaternionMultiply(qTiltTarget, qForwardTarget);
        q = QuaternionNlerp(q, qTarget, 0.2f);
        model.transform = QuaternionToMatrix(q);
    }

    Vector3 GetAvgNormal(){ //supongo que seria mejor hacerla con un for loop?
        float x;
        float z;
        float y = 3.0f;

        //top left
        x = -size.x/2;
        z = -size.z/2;
        Ray tl = (Ray){
        position+(Vector3){x, -y, z},
        (Vector3){0, 1, 0}, 
        };

        //top right
        x = size.x/2;
        z = -size.z/2;
        Ray tr = (Ray){
        position+(Vector3){x, -y, z},
        (Vector3){0, 1, 0}, 
        };

        //bottom left
        x = -size.x/2;
        z = size.z/2;
        Ray bl = (Ray){
        position+(Vector3){x, -y, z},
        (Vector3){0, 1, 0}, 
        };

        //bottom right
        x = size.x/2;
        z = size.z/2;
        Ray br = (Ray){
        position+(Vector3){x, -y, z},
        (Vector3){0, 1, 0}, 
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
            CoordsData data = World::GetMapData(worldX, worldZ);
            BaseBiome biome = data.biome; 
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
    //terrainModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = LIME;


    //Gubi - Placeholder
    gubi_model = LoadModel("resources/models/gubi/modelo.obj");
    // Arbol - Placeholder para comidas y bebidas
    tree_model = LoadModel("resources/models/arbol/modelo.obj");
    BoundingBox base_tree_bbox ? GetModelBoundingBox(tree_model);

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

    
    Gubi Juanubi;
    
    std::vector<Food> comidas;
    std::vector<Drink> bebidas;

    float step = 30.0f;

    for (float x = 40; x < terrainWidth - 40; x += step) {
        for (float z = 40; z < terrainDepth - 40; z += step) {

            CoordsData data = World::GetMapData(x, z);

            if (data.biome == BaseBiome::ACQUATIC) continue;

            //probabilidad de aparicion de comida, primero pasada a porcentaje y luego * 0.4 para no saturar el mapa
            float prob = data.quantity * 40.0f;
            if (GetRandomValue(0,100) > prob) continue;

            int chanceFood = 50;
            float maxFood = 30.0f;
            float maxDrink = 30.0f;

            //reglas de aparición de alimentos para cada bioma
            if (data.biome == BaseBiome::DESERT) {
                chanceFood = 90;    // 90% comida 10% agua
                maxFood = 15.0f;
                maxDrink = 30.0f;
            }
            else if (data.biome == BaseBiome::JUNGLE) {
                chanceFood = 60;
                maxFood = 40.0f;
                maxDrink = 40.0f;
            }
            else if (data.biome == BaseBiome::FIELD) {
                chanceFood = 50;
                maxFood = 30.0f;
                maxDrink = 30.0f;
            }
            else if (data.biome == BaseBiome::ICE) {
                chanceFood = 10;
                maxFood = 30.0f;
                maxDrink = 15.0f;
            }
            else if (data.biome == BaseBiome::TUNDRA) {
                chanceFood = 40;
                maxFood = 20.0f;
                maxDrink = 40.0f;
            }
            
            bool isFood = GetRandomValue(0, 100) <= chanceFood;

            if (isFood) {
                Food f;
                f.model = tree_model;
                f.position = (Vector3){x, find_height(pTerrainMesh, x, z), z};
                f.quality = data.quality;
                f.quantity = maxFood * data.quantity;
                comidas.push_back(f);
            } else {
                Drink d;
                d.model = tree_model;
                d.position = (Vector3){x, find_height(pTerrainMesh, x, z), z};
                d.quality = data.quality;
                d.quantity = maxDrink * data.quantity;
                bebidas.push_back(d);
            }
        }
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

        float deltaTime = GetFrameTime();
        for (auto& f : comidas) {
            f.Update(deltaTime);
        }
        for (auto& d : bebidas) {
            d.Update(deltaTime);
        }


        BeginDrawing();
            ClearBackground(SKYBLUE);
            BeginMode3D(camera);
                
                DrawModel(terrainModel, Vector3Zero(), 1, WHITE);
                DrawModelWires(terrainModel, Vector3Zero(), 1, LIGHTGRAY);
                
                // para ver donde va (me ayuda al debugging)
                if (Juanubi.isMoving) {
                    DrawLine3D(Juanubi.position, Juanubi.targetPosition, PURPLE);
                    DrawSphere(Juanubi.targetPosition, 0.5f, PURPLE);
                }

                Juanubi.Draw();

                for (auto& f : comidas) {
                    f.Draw();
                }
                for (auto& d : bebidas) {
                    d.Draw();
                }


            EndMode3D();

            //Simulation speed buttons
            DrawSpeedButton(10.0, 10.0, "x1", selected_speed);
            DrawSpeedButton(50.0, 10.0, "x2", selected_speed);
            DrawSpeedButton(90.0, 10.0, "x4", selected_speed);

        EndDrawing();
    }

    UnloadTexture(terrainTexture);

    CloseWindow();

    return 0;
}
