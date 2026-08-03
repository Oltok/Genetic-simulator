#include <raylib.h>
#include <raymath.h>
#include <string>

#include "ui_util.h"


void DrawSpeedButton(float x, float y, std::string text, std::string selected)
{
    float rec_size = 30;
    Rectangle rec {x, y, rec_size, rec_size};

    Color rec_color;
    Color txt_color;
    Color line_color = DARKGRAY;

    if (text == selected) 
    {rec_color = GRAY; txt_color = LIGHTGRAY;}
    else 
    {rec_color = LIGHTGRAY; txt_color = DARKGRAY;};

    DrawRectangleRec(rec, rec_color);
    DrawRectangleLinesEx(rec, 1.0, line_color);

    //Centering text
    Font font = GetFontDefault();
    float fontSize = 20.0;
    float spacing = fontSize*.1f;

    Vector2 txtSize = MeasureTextEx(font, text.c_str(), fontSize, spacing);
    Vector2 txt_pos = (Vector2){
    x + (rec_size - txtSize.x) * 0.5f,
    y + (rec_size - txtSize.y) * 0.5f
    };

    DrawTextEx(font, text.c_str(), txt_pos, fontSize, spacing, txt_color);
}