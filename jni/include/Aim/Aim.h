//
// Created by 飞蓝 on 2023/6/30.
//
#ifndef GUI_AIM_H
#define GUI_AIM_H

struct AimStruct
{
    Vec3 ObjAim;  
    Vec3 AimMovement;  
    float ScreenDistance = 0;
    float WodDistance = 0; 
	char Name[32];
};

extern int AimCount;
/* 自瞄对象数量 */
extern float zm_x, zm_y;
/* 自瞄目标 */
extern int MaxPlayerCount;
/* 人物数量 */
extern int Gmin;
/* 遍历自瞄数量 */

float GetPitch(float Fov);
float GetWeaponId(int WeaponId);
float getScopeAcc(int Scope);

#endif //GUI_AIM_H
