// 2022/02/11 17:44:51 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct Point3D
{
    float x;
    float y;
    float z;
};

struct Edge
{
    int numPoint0;
    int numPoint1;
};

struct Mesh
{
    int numEdges;
    Edge* edges;
    int numPoints;
    Point3D* points;
};

struct Mat4x4
{
    float n[4][4];
};


void FillMatriceRotation(Mat4x4 *mat, float angleX, float angleY, float angleZ);
void FillMatriceScale(Mat4x4 *mat, float scaleX, float scaleY, float scaleZ);
void FillMatriceMov(Mat4x4 *mat, float movX, float movY, float movZ);
void SetMatriceTransform(Mat4x4 *matTransform, Mat4x4 *matScale, Mat4x4 *matRotate, Mat4x4 *matMove);
void TransformPoint(const Mat4x4 *mat, const Point3D *pointIn, Point3D *pointOut);
void PresentPointToScreen(const Point3D *point, float *x, float *y);
void MatrixMul(const Mat4x4 *mat0, const Mat4x4 *mat1, Mat4x4 *matOut);
