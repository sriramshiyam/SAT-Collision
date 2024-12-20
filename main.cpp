#include "raylib.h"
#include "math.h"
#include <iostream>
#include <array>

std::array<std::array<Vector2, 2>, 4> box1Points;
std::array<std::array<Vector2, 2>, 4> box2Points;

void LoadBox1Points(float rotation, Rectangle &box);
void LoadBox2Points(float rotation, Rectangle &box);
void DrawNormalsAndProjections(Rectangle &box1, Rectangle &box2, int normalIndex);
void DrawBoxInfo(Rectangle &box1, Rectangle &box2, float rotation1, float rotation2, bool drawNormalProjLines);
bool IsColliding(Rectangle &box1, Rectangle &box2);
void DrawCollidingState(bool isColliding);

const int screenWidth = 1200;
const int screenHeight = 800;
bool drawNormalProjLines = false;

int main(void)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "SAT Collision");

    Rectangle box1;
    box1.x = 300;
    box1.y = 200;
    box1.height = 100;
    box1.width = 100;

    Rectangle box2;
    box2.x = 400;
    box2.y = 400;
    box2.height = 100;
    box2.width = 100;

    float speed = 400;

    float rotation1 = 0;
    float rotation2 = 0;

    int normalIndex = 0;

    while (!WindowShouldClose())
    {
        box2.x += (IsKeyDown(KEY_RIGHT) ? speed : IsKeyDown(KEY_LEFT) ? -speed
                                                                      : 0) *
                  GetFrameTime();

        box2.y += (IsKeyDown(KEY_DOWN) ? speed : IsKeyDown(KEY_UP) ? -speed
                                                                   : 0) *
                  GetFrameTime();

        box1.x += (IsKeyDown(KEY_D) ? speed : IsKeyDown(KEY_A) ? -speed
                                                               : 0) *
                  GetFrameTime();

        box1.y += (IsKeyDown(KEY_S) ? speed : IsKeyDown(KEY_W) ? -speed
                                                               : 0) *
                  GetFrameTime();

        rotation1 += (IsKeyDown(KEY_X) ? speed / 2 : IsKeyDown(KEY_Z) ? -speed / 2
                                                                      : 0) *
                     GetFrameTime();

        rotation2 += (IsKeyDown(KEY_V) ? speed / 2 : IsKeyDown(KEY_C) ? -speed / 2
                                                                      : 0) *
                     GetFrameTime();

        if (IsKeyPressed(KEY_T))
        {
            drawNormalProjLines = !drawNormalProjLines;
        }

        if (drawNormalProjLines && IsKeyPressed(KEY_E))
        {
            normalIndex++;
            if (normalIndex > 7)
            {
                normalIndex = 0;
            }
        }

        LoadBox1Points(rotation1, box1);
        LoadBox2Points(rotation2, box2);

        BeginDrawing();

        ClearBackground(BLACK);

        DrawRectanglePro(box1, {box1.width / 2, box1.height / 2}, rotation1, RED);
        DrawRectanglePro(box2, {box2.width / 2, box2.height / 2}, rotation2, {255, 255, 0, 225});

        if (drawNormalProjLines)
        {
            DrawNormalsAndProjections(box1, box2, normalIndex);
        }

        DrawBoxInfo(box1, box2, rotation1, rotation2, drawNormalProjLines);
        DrawCollidingState(IsColliding(box1, box2));

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void DrawNormalsAndProjections(Rectangle &box1, Rectangle &box2, int normalIndex)
{
    std::array<std::array<Vector2, 2>, 4> boxPoints;
    Rectangle box;

    if (normalIndex > 3)
    {
        normalIndex -= 4;
        boxPoints = box2Points;
        box = box2;
    }
    else
    {
        boxPoints = box1Points;
        box = box1;
    }

    float normalizeRadian = -90 * PI / 180;

    float normalX = boxPoints[normalIndex][1].x - boxPoints[normalIndex][0].x;
    float normalY = boxPoints[normalIndex][1].y - boxPoints[normalIndex][0].y;

    Vector2 normal;
    Vector2 startPos;

    normal.x = (normalX * std::cos(normalizeRadian) - normalY * std::sin(normalizeRadian)) + box.x;
    normal.y = (normalY * std::cos(normalizeRadian) + normalX * std::sin(normalizeRadian)) + box.y;

    startPos = {(boxPoints[normalIndex][1].x + boxPoints[normalIndex][0].x) / 2 + box.x, (boxPoints[normalIndex][1].y + boxPoints[normalIndex][0].y) / 2 + box.y};

    DrawLineV(startPos, normal, YELLOW);

    normal.x = (normalX * std::cos(normalizeRadian) - normalY * std::sin(normalizeRadian)) * 20 + box.x;
    normal.y = (normalY * std::cos(normalizeRadian) + normalX * std::sin(normalizeRadian)) * 20 + box.y;

    startPos = {(boxPoints[normalIndex][1].x + boxPoints[normalIndex][0].x) / 2 * -40 + box.x, (boxPoints[normalIndex][1].y + boxPoints[normalIndex][0].y) / 2 * -40 + box.y};

    startPos.x += normalX * 2;
    startPos.y += normalY * 2;

    normal.x += normalX * 2;
    normal.y += normalY * 2;

    DrawLineV(startPos, normal, YELLOW);

    Vector2 minPoint1;
    Vector2 maxPoint1;
    Vector2 minPoint2;
    Vector2 maxPoint2;

    Vector2 minProj1;
    Vector2 maxProj1;
    Vector2 minProj2;
    Vector2 maxProj2;

    float minLen = MAXFLOAT;
    float maxLen = 0.0f;

    for (int i = 0; i < box1Points.size(); i++)
    {
        Vector2 point = {box1Points[i][0].x + box1.x, box1Points[i][0].y + box1.y};

        Vector2 vector1 = {point.x - startPos.x, point.y - startPos.y};
        Vector2 vector2 = {normal.x - startPos.x, normal.y - startPos.y};

        float proj = (vector1.x * vector2.x + vector1.y * vector2.y) / std::pow(std::sqrt(std::pow(vector2.x, 2) + std::pow(vector2.y, 2)), 2);

        Vector2 projectedVector = {vector2.x * proj, vector2.y * proj};

        float length = std::sqrt(std::pow(projectedVector.x, 2) + std::pow(projectedVector.y, 2));

        if (length < minLen)
        {
            minLen = length;
            minProj1 = {projectedVector.x + startPos.x, projectedVector.y + startPos.y};
            minPoint1 = point;
        }

        if (length > maxLen)
        {
            maxLen = length;
            maxProj1 = {projectedVector.x + startPos.x, projectedVector.y + startPos.y};
            maxPoint1 = point;
        }
    }

    minLen = MAXFLOAT;
    maxLen = 0.0f;

    for (int i = 0; i < box2Points.size(); i++)
    {
        Vector2 point = {box2Points[i][0].x + box2.x, box2Points[i][0].y + box2.y};

        Vector2 vector1 = {point.x - startPos.x, point.y - startPos.y};
        Vector2 vector2 = {normal.x - startPos.x, normal.y - startPos.y};

        float proj = (vector1.x * vector2.x + vector1.y * vector2.y) / std::pow(std::sqrt(std::pow(vector2.x, 2) + std::pow(vector2.y, 2)), 2);

        Vector2 projectedVector = {vector2.x * proj, vector2.y * proj};

        float length = std::sqrt(std::pow(projectedVector.x, 2) + std::pow(projectedVector.y, 2));

        if (length < minLen)
        {
            minLen = length;
            minProj2 = {projectedVector.x + startPos.x, projectedVector.y + startPos.y};
            minPoint2 = point;
        }

        if (length > maxLen)
        {
            maxLen = length;
            maxProj2 = {projectedVector.x + startPos.x, projectedVector.y + startPos.y};
            maxPoint2 = point;
        }
    }

    DrawLineV(minPoint1, minProj1, BLUE);
    DrawLineV(maxPoint1, maxProj1, BLUE);

    DrawLineV(minPoint2, minProj2, BLUE);
    DrawLineV(maxPoint2, maxProj2, BLUE);

    DrawLineV(minProj1, maxProj1, GREEN);
    DrawLineV(minProj2, maxProj2, {255, 0, 0, 150});

    DrawCircleV(minPoint1, 3.0f, GREEN);
    DrawCircleV(maxPoint1, 3.0f, GREEN);
    DrawCircleV(minPoint2, 3.0f, GREEN);
    DrawCircleV(maxPoint2, 3.0f, GREEN);

    DrawCircleV(minProj1, 3.0f, GREEN);
    DrawCircleV(maxProj1, 3.0f, GREEN);
    DrawCircleV(minProj2, 3.0f, GREEN);
    DrawCircleV(maxProj2, 3.0f, GREEN);
}

void DrawBoxInfo(Rectangle &box1, Rectangle &box2, float rotation1, float rotation2, bool drawNormalProjLines)
{
    DrawText("Box1:", 10, 0, 20, WHITE);
    DrawText("WASD for movement", 10, 30, 20, WHITE);
    DrawText("ZX for rotation", 10, 60, 20, WHITE);

    std::array<std::string, 3> box2Infos = {"Box2:", "ARROWS for movement", "CV for rotation"};

    for (int i = 0; i < box2Infos.size(); i++)
    {
        int textWidth = MeasureText(box2Infos[i].c_str(), 20);
        DrawText(box2Infos[i].c_str(), screenWidth - textWidth - 10, 30 * i, 20, WHITE);
    }

    int textWidth = MeasureText("T for toggle drawing of normals and projections", 20);
    DrawText("T for toggle drawing of normals and projections", screenWidth / 2 - textWidth / 2, 0, 20, WHITE);

    if (drawNormalProjLines)
    {
        textWidth = MeasureText("E for next normal axis", 20);
        DrawText("E for next normal axis", screenWidth / 2 - textWidth / 2, 30, 20, WHITE);
    }
}

bool IsColliding(Rectangle &box1, Rectangle &box2)
{
    float normalizeRadian = -90 * PI / 180;
    std::array<std::array<float, 2>, 4> projLen1;
    std::array<std::array<float, 2>, 4> projLen2;

    for (int i = 0; i < box1Points.size(); i++)
    {
        float normalX = box1Points[i][1].x - box1Points[i][0].x;
        float normalY = box1Points[i][1].y - box1Points[i][0].y;

        Vector2 normal;
        Vector2 startPos;

        normal.x = (normalX * std::cos(normalizeRadian) - normalY * std::sin(normalizeRadian)) * 20 + box1.x;
        normal.y = (normalY * std::cos(normalizeRadian) + normalX * std::sin(normalizeRadian)) * 20 + box1.y;

        startPos = {(box1Points[i][1].x + box1Points[i][0].x) / 2 * -40 + box1.x, (box1Points[i][1].y + box1Points[i][0].y) / 2 * -40 + box1.y};

        startPos.x += normalX * 2;
        startPos.y += normalY * 2;

        normal.x += normalX * 2;
        normal.y += normalY * 2;

        float minLen = MAXFLOAT;
        float maxLen = 0.0f;

        for (int j = 0; j < box1Points.size(); j++)
        {
            Vector2 point = {box1Points[j][0].x + box1.x, box1Points[j][0].y + box1.y};

            Vector2 vector1 = {point.x - startPos.x, point.y - startPos.y};
            Vector2 vector2 = {normal.x - startPos.x, normal.y - startPos.y};

            float proj = (vector1.x * vector2.x + vector1.y * vector2.y) / std::pow(std::sqrt(std::pow(vector2.x, 2) + std::pow(vector2.y, 2)), 2);

            Vector2 projectedVector = {vector2.x * proj, vector2.y * proj};

            float length = std::sqrt(std::pow(projectedVector.x, 2) + std::pow(projectedVector.y, 2));

            if (length < minLen)
            {
                minLen = length;
            }

            if (length > maxLen)
            {
                maxLen = length;
            }
        }

        projLen1[i][0] = minLen;
        projLen1[i][1] = maxLen;

        minLen = MAXFLOAT;
        maxLen = 0.0f;

        for (int j = 0; j < box2Points.size(); j++)
        {
            Vector2 point = {box2Points[j][0].x + box2.x, box2Points[j][0].y + box2.y};

            Vector2 vector1 = {point.x - startPos.x, point.y - startPos.y};
            Vector2 vector2 = {normal.x - startPos.x, normal.y - startPos.y};

            float proj = (vector1.x * vector2.x + vector1.y * vector2.y) / std::pow(std::sqrt(std::pow(vector2.x, 2) + std::pow(vector2.y, 2)), 2);

            Vector2 projectedVector = {vector2.x * proj, vector2.y * proj};

            float length = std::sqrt(std::pow(projectedVector.x, 2) + std::pow(projectedVector.y, 2));

            if (length < minLen)
            {
                minLen = length;
            }

            if (length > maxLen)
            {
                maxLen = length;
            }
        }

        projLen2[i][0] = minLen;
        projLen2[i][1] = maxLen;
    }

    for (int i = 0; i < projLen1.size(); i++)
    {
        float min1 = projLen1[i][0];
        float max1 = projLen1[i][1];
        float min2 = projLen2[i][0];
        float max2 = projLen2[i][1];

        if (!((min2 < max1 && max2 > max1) || (min2 < min1 && max2 > min1) || (min2 > min1 && max2 < max1) || (min2 < min1 && max2 > max1)))
        {
            return false;
        }
    }

    for (int i = 0; i < box2Points.size(); i++)
    {
        float normalX = box2Points[i][1].x - box2Points[i][0].x;
        float normalY = box2Points[i][1].y - box2Points[i][0].y;

        Vector2 normal;
        Vector2 startPos;

        normal.x = (normalX * std::cos(normalizeRadian) - normalY * std::sin(normalizeRadian)) * 20 + box2.x;
        normal.y = (normalY * std::cos(normalizeRadian) + normalX * std::sin(normalizeRadian)) * 20 + box2.y;

        startPos = {(box2Points[i][1].x + box2Points[i][0].x) / 2 * -40 + box2.x, (box2Points[i][1].y + box2Points[i][0].y) / 2 * -40 + box2.y};

        startPos.x += normalX * 2;
        startPos.y += normalY * 2;

        normal.x += normalX * 2;
        normal.y += normalY * 2;

        float minLen = MAXFLOAT;
        float maxLen = 0.0f;

        for (int j = 0; j < box1Points.size(); j++)
        {
            Vector2 point = {box1Points[j][0].x + box1.x, box1Points[j][0].y + box1.y};

            Vector2 vector1 = {point.x - startPos.x, point.y - startPos.y};
            Vector2 vector2 = {normal.x - startPos.x, normal.y - startPos.y};

            float proj = (vector1.x * vector2.x + vector1.y * vector2.y) / std::pow(std::sqrt(std::pow(vector2.x, 2) + std::pow(vector2.y, 2)), 2);

            Vector2 projectedVector = {vector2.x * proj, vector2.y * proj};

            float length = std::sqrt(std::pow(projectedVector.x, 2) + std::pow(projectedVector.y, 2));

            if (length < minLen)
            {
                minLen = length;
            }

            if (length > maxLen)
            {
                maxLen = length;
            }
        }

        projLen1[i][0] = minLen;
        projLen1[i][1] = maxLen;

        minLen = MAXFLOAT;
        maxLen = 0.0f;

        for (int j = 0; j < box2Points.size(); j++)
        {
            Vector2 point = {box2Points[j][0].x + box2.x, box2Points[j][0].y + box2.y};

            Vector2 vector1 = {point.x - startPos.x, point.y - startPos.y};
            Vector2 vector2 = {normal.x - startPos.x, normal.y - startPos.y};

            float proj = (vector1.x * vector2.x + vector1.y * vector2.y) / std::pow(std::sqrt(std::pow(vector2.x, 2) + std::pow(vector2.y, 2)), 2);

            Vector2 projectedVector = {vector2.x * proj, vector2.y * proj};

            float length = std::sqrt(std::pow(projectedVector.x, 2) + std::pow(projectedVector.y, 2));

            if (length < minLen)
            {
                minLen = length;
            }

            if (length > maxLen)
            {
                maxLen = length;
            }
        }

        projLen2[i][0] = minLen;
        projLen2[i][1] = maxLen;
    }

    for (int i = 0; i < projLen1.size(); i++)
    {
        float min1 = projLen1[i][0];
        float max1 = projLen1[i][1];
        float min2 = projLen2[i][0];
        float max2 = projLen2[i][1];

        if (!((min2 < max1 && max2 > max1) || (min2 < min1 && max2 > min1) || (min2 > min1 && max2 < max1) || (min2 < min1 && max2 > max1)))
        {
            return false;
        }
    }

    return true;
}

void DrawCollidingState(bool isColliding)
{
    std::string text = isColliding ? "Colliding" : "Not Colliding";
    int textWidth = MeasureText(text.c_str(), 20);
    DrawText(text.c_str(), screenWidth / 2 - textWidth / 2, 60, 20, isColliding ? RED : GREEN);
}

void LoadBox1Points(float rotation, Rectangle &box)
{
    float rotInRad = rotation * PI / 180;

    box1Points[0][0].x = (-box.width / 2) * std::cos(rotInRad) - (-box.height / 2) * std::sin(rotInRad);
    box1Points[0][0].y = (-box.height / 2) * std::cos(rotInRad) + (-box.width / 2) * std::sin(rotInRad);

    box1Points[0][1].x = (box.width / 2) * std::cos(rotInRad) - (-box.height / 2) * std::sin(rotInRad);
    box1Points[0][1].y = (-box.height / 2) * std::cos(rotInRad) + (box.width / 2) * std::sin(rotInRad);

    box1Points[1][0].x = (box.width / 2) * std::cos(rotInRad) - (-box.height / 2) * std::sin(rotInRad);
    box1Points[1][0].y = (-box.height / 2) * std::cos(rotInRad) + (box.width / 2) * std::sin(rotInRad);

    box1Points[1][1].x = (box.width / 2) * std::cos(rotInRad) - (box.height / 2) * std::sin(rotInRad);
    box1Points[1][1].y = (box.height / 2) * std::cos(rotInRad) + (box.width / 2) * std::sin(rotInRad);

    box1Points[2][0].x = (box.width / 2) * std::cos(rotInRad) - (box.height / 2) * std::sin(rotInRad);
    box1Points[2][0].y = (box.height / 2) * std::cos(rotInRad) + (box.width / 2) * std::sin(rotInRad);

    box1Points[2][1].x = (-box.width / 2) * std::cos(rotInRad) - (box.height / 2) * std::sin(rotInRad);
    box1Points[2][1].y = (box.height / 2) * std::cos(rotInRad) + (-box.width / 2) * std::sin(rotInRad);

    box1Points[3][0].x = (-box.width / 2) * std::cos(rotInRad) - (box.height / 2) * std::sin(rotInRad);
    box1Points[3][0].y = (box.height / 2) * std::cos(rotInRad) + (-box.width / 2) * std::sin(rotInRad);

    box1Points[3][1].x = (-box.width / 2) * std::cos(rotInRad) - (-box.height / 2) * std::sin(rotInRad);
    box1Points[3][1].y = (-box.height / 2) * std::cos(rotInRad) + (-box.width / 2) * std::sin(rotInRad);
}

void LoadBox2Points(float rotation, Rectangle &box)
{
    float rotInRad = rotation * PI / 180;

    box2Points[0][0].x = (-box.width / 2) * std::cos(rotInRad) - (-box.height / 2) * std::sin(rotInRad);
    box2Points[0][0].y = (-box.height / 2) * std::cos(rotInRad) + (-box.width / 2) * std::sin(rotInRad);

    box2Points[0][1].x = (box.width / 2) * std::cos(rotInRad) - (-box.height / 2) * std::sin(rotInRad);
    box2Points[0][1].y = (-box.height / 2) * std::cos(rotInRad) + (box.width / 2) * std::sin(rotInRad);

    box2Points[1][0].x = (box.width / 2) * std::cos(rotInRad) - (-box.height / 2) * std::sin(rotInRad);
    box2Points[1][0].y = (-box.height / 2) * std::cos(rotInRad) + (box.width / 2) * std::sin(rotInRad);

    box2Points[1][1].x = (box.width / 2) * std::cos(rotInRad) - (box.height / 2) * std::sin(rotInRad);
    box2Points[1][1].y = (box.height / 2) * std::cos(rotInRad) + (box.width / 2) * std::sin(rotInRad);

    box2Points[2][0].x = (box.width / 2) * std::cos(rotInRad) - (box.height / 2) * std::sin(rotInRad);
    box2Points[2][0].y = (box.height / 2) * std::cos(rotInRad) + (box.width / 2) * std::sin(rotInRad);

    box2Points[2][1].x = (-box.width / 2) * std::cos(rotInRad) - (box.height / 2) * std::sin(rotInRad);
    box2Points[2][1].y = (box.height / 2) * std::cos(rotInRad) + (-box.width / 2) * std::sin(rotInRad);

    box2Points[3][0].x = (-box.width / 2) * std::cos(rotInRad) - (box.height / 2) * std::sin(rotInRad);
    box2Points[3][0].y = (box.height / 2) * std::cos(rotInRad) + (-box.width / 2) * std::sin(rotInRad);

    box2Points[3][1].x = (-box.width / 2) * std::cos(rotInRad) - (-box.height / 2) * std::sin(rotInRad);
    box2Points[3][1].y = (-box.height / 2) * std::cos(rotInRad) + (-box.width / 2) * std::sin(rotInRad);
}