#include "math.h"
#include <touch.h>

char* g_game_name = "com.sofunny.Sausage";
char* g_game_dll = "libUE4.so";

int style_idx2 = 2;
int aimCount = 0;
int aimbotmode = 2;//瞄准位置
float NumIo[50];
bool DrawIo[50];
float inttouchx = 300;
float inttouchy = 500;
string SaveDir;
FILE *numSave = nullptr;




long int 自身结构体;
// 杂项
int PlayerCount;
int BotCount;
float DropM;
float zm_x,zm_y;
void getAimAddr();
void getFov();
void AimbotCalcu();
void Aim_exec();
void logw();
void log(int num);
int AimObjCount = 0;
int flag = 50;
float MaxDistance = 300;
int fovAddr = 0,fov2;
float FOV;
int FingerMax = 2;
int jll;
int zxx,zxy;
int Bar = 89;
int WorldDistance;
int AimCount = 0;
float diff[3];
bool Aimbot;
float r_x1, r_y1, r_w1;
/* 自瞄临时换算坐标 */
float BulletSpeed = 650.0f;
/* 子弹速度(M4:880(720.0 m/s),98K:760(600 m/s)) */
int MaxPlayerCount = 0;
/* 人物数量 */
int ToReticleDistance;
/* 到准星距离 */
float AimRadius = 300.0f;
/* 自瞄范围 */
float SlideRadius = 100.0f;
/* 滑动范围 */
int AimMode = 1;
/* 自瞄模式 */
float SlideX = 400.0f;
float SlideY = 1200.0f;
/* 滑动位置 */
int AimPosition = 1;
/* 自瞄位置 */
int AutoPressure = 1;
/* 自动压枪 1:开启 2:关闭 */
bool AimDrawDD;
/* 倒地不喵开关 */
float camear_z;
/* 自瞄相机 */
float PushX;
float PushY;
/* 压缩Y临时 */
double speedx = 55.0f;          // 150.0f;
// 速度值，此数值越大，速度越慢，抖动更稳定
int AimSleep = 10;              // min0 max100 自瞄延迟
//SETINFO aimx = {0}, aimy = {0};
float LastCoor[3] = {0};
//上一个坐标
float NewCoor[3] = {0};
//下一个坐标
int Gmin=-1;
float SlideRanges = 200.0f;
//划屏范围
#define TIME 0.1
float obj_x, obj_y, obj_z;
/* 自瞄定义结束 */
int countddr;
// 计算旋转坐标

Vector2A Head; Vector2A Chest; Vector2A Pelvis; Vector2A Left_Shoulder; Vector2A Right_Shoulder; Vector2A Left_Elbow; Vector2A Right_Elbow;  Vector2A Left_Wrist; Vector2A Right_Wrist; Vector2A Left_Thigh; Vector2A Right_Thigh;Vector2A Left_Knee;Vector2A Right_Knee;Vector2A Left_Ankle;Vector2A Right_Ankle;

long int libbase, Gname, XJZZ, Uworld, Matrix, MySelf, MySelf1,Arrayaddr,  Count, Objaddr, ye, xu, Object, weapon,
    handheld, Mesh, human,  Bone, Uleve ,Uleves ,Meshself ,Humanself ,Boneself;








