#include <raylib.h>
#include <raymath.h> //mate y maticas
#include <vector>



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
        return (Vector3){(float)GetRandomValue(-450,450), 1.0f, (float)GetRandomValue(-450, 450)}; //aleatorio
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
        speed = 15.0f;
    }

    void MoveTo(float deltaTime) { //delta time es la constante temporal

        if (!isMoving) return;

        Vector3 direction = Vector3Subtract(targetPosition, position);
        direction.y = 0.0; //si no se le va la olla al cabrón

        //para que sea más óptimo he visto que se puede usar el cuadrado en lugar de la raíz para calcular la distancia
        float distancia = Vector3LengthSqr(direction);
        float thrLlegada = 0.01f; //para definir si ha llegado a la posición, está al cuadrado

        if (distancia <= thrLlegada) {

            position.x = targetPosition.x;
            position.z = targetPosition.z;
            isMoving = false;
            return;
        }

        direction = Vector3Normalize(direction);

        position.x += direction.x * speed * deltaTime;
        position.z += direction.z * speed * deltaTime;

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

int main(void){

    const int screenWidth = 800;
    const int screenHeight = 450;
    

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    //model loading - se ve que debe ir despues del InitWindow

    //en este caso no se carga la textura porque en el modelo.mtl ya se referencia
    Model tree_model = LoadModel("resources/models/arbol/modelo.obj");
    BoundingBox base_tree_bbox = GetModelBoundingBox(tree_model);
    
    //Para cargar textura y ponerla al modelo:
    //Texture2D tree_texture = LoadTexture("resources/models/arbol/base_color.png");
    //tree_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tree_texture;


    // camera init    
    Camera3D camera = { 0 };
    camera.position = (Vector3){0.0f, 10.0f, 20.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Gubi Juanubi;

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
        arbo.model = tree_model;
        
        bool posicionwena = false;
        while (!posicionwena) {

            arbo.position = (Vector3){(float)GetRandomValue(-450, 450), (float)GetRandomValue(-2, 1), (float)GetRandomValue(-450, 450)};

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
    
        if (!Juanubi.isMoving) {
            Juanubi.targetPosition = Juanubi.brain.Destiny();
            Juanubi.isMoving = true;
        }

        Juanubi.MoveTo(GetFrameTime());

        BeginDrawing();
            ClearBackground(SKYBLUE);
            BeginMode3D(camera);
            
                DrawPlane((Vector3){0.0f, 0.0f, 0.0f}, (Vector2){1000.0f, 1000.0f}, GREEN);
                
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

        
        EndDrawing();
    }

    UnloadModel(tree_model);

    CloseWindow();

    return 0;
}
