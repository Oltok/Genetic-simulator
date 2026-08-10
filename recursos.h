#pragma once
#include <raylib.h>
#include <raymath.h>


class Recurso 
{
    public:
        Vector3 position;
        Model model;
        Color tinte; //temporal para distinguir comida y bebida
        float quantity;
        float quality;
        bool isEmpty;
        float regenerationTime;
        float actualTime;

        Recurso() {
            isEmpty = false;
            actualTime = 0.0f;
            regenerationTime = 10.0f;
        }

        void Update(float deltaTime) {
            if (isEmpty) {
                actualTime += deltaTime;
                if (actualTime >= regenerationTime) {

                    isEmpty = false;
                    actualTime = 0.0f;
                }
            }
        }

        void Draw() {
            if (!isEmpty) {
                DrawModel(model, position, 1.0f, tinte);
            } else {
                DrawModelWires(model, position, 1.0f, GRAY);
            }
        }

};

class Food: public Recurso
{
    public: 

        Food() : Recurso() {
            tinte = GREEN;

        }

};

class Drink: public Recurso
{
    public:

        Drink() : Recurso() {
            tinte = BLUE;
        }
};