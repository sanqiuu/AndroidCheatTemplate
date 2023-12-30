#pragma once
#ifndef USTOOL_H
#define USTOOL_H

#define PI 3.141592653589793238

float matrix[16] = {0};
double Widtih, Higtih;

typedef char UTF8;
typedef unsigned short UTF16;

void GetDistance(Vec3 Object, Vec3 Self, float *Distance)
{
	float DistanceX = pow(Object.x - Self.x, 2);
    float DistanceY = pow(Object.y - Self.y, 2);
    float DistanceZ = pow(Object.z - Self.z, 2);
    *Distance = sqrt(DistanceX + DistanceY + DistanceZ) * 0.01f;
}

void WorldToScreen(float *bscreen, Vec3 *obj)
{
    float camear = matrix[3] * obj->x + matrix[7] * obj->y + matrix[11] * obj->z + matrix[15];
    *bscreen = Higtih - (matrix[1] * obj->x + matrix[5] * obj->y + matrix[9] * obj->z + matrix[13]) / camear * Higtih;
}

void WorldToScreen(Vec2 *bscreen, Vec3 *obj)
{
    float camear = matrix[3] * obj->x + matrix[7] * obj->y + matrix[11] * obj->z + matrix[15];
    bscreen->x = Widtih + (matrix[0] * obj->x + matrix[4] * obj->y + matrix[8] * obj->z + matrix[12]) / camear * Widtih;
    bscreen->y = Higtih - (matrix[1] * obj->x + matrix[5] * obj->y + matrix[9] * obj->z + matrix[13]) / camear * Higtih;
}

void WorldToScreen(Vec2 *bscreen, Vec3 obj)
{
    float camear = matrix[3] * obj.x + matrix[7] * obj.y + matrix[11] * obj.z + matrix[15];
    bscreen->x = Widtih + (matrix[0] * obj.x + matrix[4] * obj.y + matrix[8] * obj.z + matrix[12]) / camear * Widtih;
    bscreen->y = Higtih - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * obj.z + matrix[13]) / camear * Higtih;
}

Vec2 WorldToScreen(Vec3 obj, float a[16], float b)	
{
	Vec2 bscreen;
    float camear = matrix[3] * obj.x + matrix[7] * obj.y + matrix[11] * obj.z + matrix[15];
    bscreen.x = Widtih + (matrix[0] * obj.x + matrix[4] * obj.y + matrix[8] * obj.z + matrix[12]) / camear * Widtih;
    bscreen.y = Higtih - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * obj.z + matrix[13]) / camear * Higtih;
	return bscreen;
}

void WorldToScreen(Vec2 *bscreen, float *camea, Vec3 obj)
{
    float camear = matrix[3] * obj.x + matrix[7] * obj.y + matrix[11] * obj.z + matrix[15];
    *camea = camear;
    bscreen->x = Widtih + (matrix[0] * obj.x + matrix[4] * obj.y + matrix[8] * obj.z + matrix[12]) / camear * Widtih;
    bscreen->y = Higtih - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * obj.z + matrix[13]) / camear * Higtih;
}

void WorldToScreen(Vec2 *bscreen, float *camea, float *w, Vec3 obj)
{
    float camear = matrix[3] * obj.x + matrix[7] * obj.y + matrix[11] * obj.z + matrix[15];
    *camea = camear;
    bscreen->x = Widtih + (matrix[0] * obj.x + matrix[4] * obj.y + matrix[8] * obj.z + matrix[12]) / camear * Widtih;
    bscreen->y = Higtih - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * obj.z + matrix[13]) / camear * Higtih;
	float bscreenZ = Higtih - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * (obj.z + 165) + matrix[13]) / camear * Higtih;
    float bscreenz = bscreen->y - bscreenZ;
    *w = (bscreen->y - bscreenZ) / 2;
}

void WorldToScreen(Vec4 *bscreen, float *camea, Vec3 obj)
{
    float camear = matrix[3] * obj.x + matrix[7] * obj.y + matrix[11] * obj.z + matrix[15];
    *camea = camear;
    bscreen->x = Widtih + (matrix[0] * obj.x + matrix[4] * obj.y + matrix[8] * obj.z + matrix[12]) / camear * Widtih;
    bscreen->y = Higtih - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * obj.z + matrix[13]) / camear * Higtih;
    float bscreenZ = Higtih - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * (obj.z + 165) + matrix[13]) / camear * Higtih;
    bscreen->z = bscreen->y - bscreenZ;
    bscreen->w = (bscreen->y - bscreenZ) / 2;
}

Vec2 rotateCoord(float angle, float objRadar_x, float objRadar_y)
{
    Vec2 radarCoordinate;
    float s = sin(angle * PI / 180);
    float c = cos(angle * PI / 180);
    radarCoordinate.x = objRadar_x * c + objRadar_y * s;
    radarCoordinate.y = -objRadar_x * s + objRadar_y * c;
    return radarCoordinate;
}

Vec3 MarixToVector(FMatrix matrix)
{
    return Vec3(matrix.M[3][0], matrix.M[3][1], matrix.M[3][2]);
}

FMatrix MatrixMulti(FMatrix m1, FMatrix m2)
{
    FMatrix matrix = FMatrix();
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                matrix.M[i][j] += m1.M[i][k] * m2.M[k][j];
            }
        }
    }
    return matrix;
}

FMatrix TransformToMatrix(FTransform transform)
{
    FMatrix matrix;
    matrix.M[3][0] = transform.Translation.x;
    matrix.M[3][1] = transform.Translation.y;
    matrix.M[3][2] = transform.Translation.z;
    float x2 = transform.Rotation.x + transform.Rotation.x;
    float y2 = transform.Rotation.y + transform.Rotation.y;
    float z2 = transform.Rotation.z + transform.Rotation.z;
    float xx2 = transform.Rotation.x * x2;
    float yy2 = transform.Rotation.y * y2;
    float zz2 = transform.Rotation.z * z2;
    matrix.M[0][0] = (1 - (yy2 + zz2)) * transform.Scale3D.x;
    matrix.M[1][1] = (1 - (xx2 + zz2)) * transform.Scale3D.y;
    matrix.M[2][2] = (1 - (xx2 + yy2)) * transform.Scale3D.z;
    float yz2 = transform.Rotation.y * z2;
    float wx2 = transform.Rotation.w * x2;
    matrix.M[2][1] = (yz2 - wx2) * transform.Scale3D.z;
    matrix.M[1][2] = (yz2 + wx2) * transform.Scale3D.y;
    float xy2 = transform.Rotation.x * y2;
    float wz2 = transform.Rotation.w * z2;
    matrix.M[1][0] = (xy2 - wz2) * transform.Scale3D.y;
    matrix.M[0][1] = (xy2 + wz2) * transform.Scale3D.x;
    float xz2 = transform.Rotation.x * z2;
    float wy2 = transform.Rotation.w * y2;
    matrix.M[2][0] = (xz2 + wy2) * transform.Scale3D.z;
    matrix.M[0][2] = (xz2 - wy2) * transform.Scale3D.x;
    matrix.M[0][3] = 0;
    matrix.M[1][3] = 0;
    matrix.M[2][3] = 0;
    matrix.M[3][3] = 1;
    return matrix;
}

bool GetWether(char *ClassName) {
	if (strstr(ClassName, "DragonBoat") != nullptr)
		return true;
	if (strstr(ClassName, "CoupeRB") != nullptr)
		return true;
	if (strstr(ClassName, "AquaRail") != nullptr)
		return true;
	if (strstr(ClassName, "PG117") != nullptr)
		return true;	
	if (strstr(ClassName, "MiniBus") != nullptr)
		return true;
	if (strstr(ClassName, "Mirado") != nullptr) 
		return true;
	if (strstr(ClassName, "Rony") != nullptr)
		return true;
	if (strstr(ClassName, "_PickUp") != nullptr) 
		return true;
	if (strstr(ClassName, "UAZ") != nullptr)
		return true;
    if (strstr(ClassName, "Dacia") != nullptr)
		return true;
	if (strstr(ClassName, "Buggy") != nullptr)
		return true;
    if (strcmp(ClassName, "BP_VH_Tuk_C") == 0)
		return true;
	if (strcmp(ClassName, "BP_VH_Tuk_1_C") == 0)
		return true;
	if (strcmp(ClassName, "VH_Snowmobile_C") == 0)
		return true;
	if (strcmp(ClassName, "VH_MotorcycleCart_1_C") == 0)
		return true;
	if (strcmp(ClassName, "VH_MotorcycleCart_C") == 0)
		return true;
	if (strcmp(ClassName, "VH_Motorcycle_C") == 0)
		return true;
    if (strcmp(ClassName, "VH_Motorcycle_1_C") == 0)
		return true;
	if (strcmp(ClassName, "VH_Scooter_C") == 0)
		return true;
	if (strcmp(ClassName, "VH_BRDM_C") == 0)
		return true;
	return false;
}

#endif
