#include "VecTool.h"
#include "Aim.h"
#include <main.h>
#include <cstdio>
#include <cstdlib>

using namespace std;

int AimCount = 0;
/* 自瞄对象数量 */
float zm_x, zm_y;
/* 自瞄目标 */
int MaxPlayerCount = 0;
/* 人物数量 */
int Gmin = -1;

float GetPitch(float Fov) 
{
    if (Fov > 75 && Fov <= 130)          // 不开镜
    {
        return 0.6f * NumIo[14];
    }
    else if (Fov == 70 || Fov == 75)    // 机瞄 ，侧瞄
    {     
        return 2.5f * NumIo[14];       
    }
    else if (Fov == 55 || Fov == 60)    // 红点 ，全息
    {
        return 2.5f * NumIo[14];
    }   
    else if ((int)Fov == 44)    // 2倍
    {
        return 5.1f * NumIo[14];
    }
    else if ((int)Fov == 26)    // 3倍
    {
        return 7.1f * NumIo[14];
    }
    else if ((int)Fov == 20)    // 4倍
    {
        return 8.4f * NumIo[14];
    }
    else if ((int)Fov == 13)    // 6倍
    {       
        return 13.9f * NumIo[14];
    }
	return 2.5f * NumIo[14];   // 8倍
}

float GetWeaponId(int WeaponId)
{
	switch (WeaponId)
	{
		//突击步枪
		case 101008:
			return 1.2; // M762
			break;
		case 101001:
			return 1.1; // AKM
			break;
		case 101004:
			return 0.8; // M416
			break;
		case 101003:
			return 0.8; // SCAR-L
			break;
		case 101002:
			return 0.85; // M16A4
			break;
		case 101009:
			return 1.05; // Mk47
			break;
		case 101006:
			return 0.75; // AUG
			break;
		case 101005:
			return 1.05; // Groza
			break;
		case 101010:
			return 1.15; // G36C
			break;
		case 101007:
			return 0.95; // QBZ
			break;
		case 101011:
			return 1.0; // AC-VAL
			break;
		case 101012:
			return 0.90; // 蜜獾突击步枪
			break;
			//机关枪
		case 105001:
			return 1.3; // M249
			break;
		case 105002:
			return 1.3; // DP-28
		case 105010:
			return 1.05; // MG3
			break;
			//冲锋枪
		case 102001:
			return 0.6; // UZI
			break;
		case 102003:
			return 0.6; // Vector
			break;
		case 100103:
			return 0.65; // PP-19
			break;
		case 102007:
			return 0.6; // MP5K
			break;
		case 102002:
			return 0.6; // UMP 5
			break;
		case 102004:
			return 0.6; // 汤姆逊
			break;
		case 102105:
			return 0.5; // P90
			break;
		case 102005:
			return 0.6; // 野牛
			break;
		default:
			return 0.95; // 未收录
			break;
	}
	return 1.0; // 未获取
}

float getScopeAcc(int Scope){
    switch (Scope) {
        case 0:
            return 1.0f;
            break;
        case 1:
            return .98f;
            break;
        case 2:
            return .95f;
            break;
        case 3:
            return .94f;
            break;
        case 4:
            return .92f;
            break;
        case 5:
            return .9f;
            break;
        case 6:
            return .88f;
            break;
        case 7:
            return .86f;
            break;
        case 8:
            return .85f;
            break;
        default:
            return .9f;
            break;
    }
}
