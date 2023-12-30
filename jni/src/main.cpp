#include <main.h>
#include <check.h>
#include <GLES3/gl3.h>
#include <ctgmath>
#include <imgui_internal.h>
#include <dirent.h>
#include <stb_image.h>
#include <base64.h>
#include "VecTool.h"
#include "DrawTools.h"
#include "Aim.h"
#include "hook.h"

CanSeeData CanSee;

Vec2 WorldIsScreen(Vec3 obj, float matrix[16], float ViewW)
{
	float x =
		(screen_x / 2) + (matrix[0] * obj.x + matrix[4] * obj.y + matrix[8] * obj.z + matrix[12]) / ViewW * (screen_x / 2);
	float y =
		(screen_y / 2) - (matrix[1] * obj.x + matrix[5] * obj.y + matrix[9] * obj.z + matrix[13]) / ViewW * (screen_y / 2);

	return Vec2(x, y);
}

#define PI 3.14159265358979323846

timer RenderingFPS;
AimStruct Aim[100]; // 自瞄结构
struct TempData Temp; // 对象结构
struct TempData *temp; // 结构地址

ImVec2 suspensionPos;			// NOLINT
ImVec2 aimsuspensionPos;			// NOLINT
ImVec2 SaveWindowPos = {-520,-520}; //NOLINT
bool my_window_focused = false;
bool isSetWindowPos = false;
bool isImageDown = false;
bool isAimDown = false;
bool isSetAim = false;
bool isSetSize = false;

struct TextureInfo {
    ImTextureID textureId;
    int width;
    int height;
};

TextureInfo imageButton;
TextureInfo offButton;
TextureInfo onButton;
TextureInfo robotpng;
TextureInfo playerpng;
static TextureInfo textureInfo;

TextureInfo createTexture(const string &ImagePath) {
    int w, h, n;
    stbi_uc *data = stbi_load(ImagePath.c_str(), &w, &h, &n, 0);
    GLuint texture;
    glGenTextures(1, &texture);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    stbi_image_free(data);
	
    textureInfo.textureId = texture;
    textureInfo.width = w;
    textureInfo.height = h;
    return textureInfo;
}

int style_idx = 0;

/* 定义自定义颜色 */
ImColor TouchingColor = ImColor(255, 0, 0, 150);
ImColor BoxColor = {1.0f,0.0f,0.0f,1.0f};
ImColor BotBoxColor = ImColor(255,255,255,255);
ImColor BoxblackColor = ImColor(255,0,0,25);
ImColor BotBoxblackColor = ImColor(255,0,0,25);
ImColor LineColor = ImColor(255,0,0,255);
ImColor BotLineColor = ImColor(255,255,255,255);
ImColor BoneColor = ImColor(255,0,0,255);
ImColor BotBoneColor = ImColor(255,255,255,255);
ImColor RightColor = ImColor(255,200,0,255);
ImColor BotRightColor = ImColor(255,255,255,255);    
ImColor WarningColor = ImColor(255,0,0,255);
ImColor BotWarningColor = ImColor(255,255,255,255);    

/* 定义自定义大小 */
float BoxSize = 1.5f;
float BotBoxSize = 1.5f;
float LineSize = 1.5f;
float BotLineSize = 1.5f;
float BoneSize = 2.5f;
float BotBoneSize = 2.5f;

int show_ChildMenu = 0;
bool UE4Get = false;
bool IsBall = false;
bool DrawIo[50];
float NumIo[50];

FILE *numSave = nullptr;
FILE *numSave2 = nullptr;
long addr_src = 0;

void WriteDrawSwitchData(const char *name)
{
    string game_name = g_game_name;
    string SaveFile = "/storage/emulated/0/Android/data/"+ game_name +"/ files";
    SaveFile += "/";
    SaveFile += name;
    numSave = fopen(SaveFile.c_str(), "wb+");   
	
    fseek(numSave, 0, SEEK_SET);
    fwrite(&Temp, sizeof(TempData), 1, numSave);
    fflush(numSave);
    fsync(fileno(numSave));
}

void NumIoSave(const char *name)
{
    if (numSave2 == nullptr) {
        string SaveFile = "/data";
        SaveFile += "/";
        SaveFile += name;
        numSave2 = fopen(SaveFile.c_str(), "wb+");
    }
    fseek(numSave2, 0, SEEK_SET);
	fwrite(DrawIo, sizeof(bool) * 50, 1, numSave2);
    fwrite(NumIo, sizeof(float) * 50, 1, numSave2);
	fwrite(&BoxColor, sizeof(ImColor), 1, numSave2);
    fwrite(&BotBoxColor, sizeof(ImColor), 1, numSave2);
    fwrite(&LineColor, sizeof(ImColor), 1, numSave2);
    fwrite(&BotLineColor, sizeof(ImColor), 1, numSave2);
    fwrite(&BoneColor, sizeof(ImColor), 1, numSave2);
    fwrite(&BotBoneColor, sizeof(ImColor), 1, numSave2);
    fwrite(&RightColor, sizeof(ImColor), 1, numSave2);
    fwrite(&BotRightColor, sizeof(ImColor), 1, numSave2);    
	fwrite(&WarningColor, sizeof(ImColor), 1, numSave2);    
	fwrite(&BotWarningColor, sizeof(ImColor), 1, numSave2); 
	fwrite(&BoxSize, sizeof(float), 1, numSave2);
    fwrite(&BotBoxSize, sizeof(float), 1, numSave2);
    fwrite(&LineSize, sizeof(float), 1, numSave2);
    fwrite(&BotLineSize, sizeof(float), 1, numSave2);
    fwrite(&BoneSize, sizeof(float), 1, numSave2);
    fwrite(&BotBoneSize, sizeof(float), 1, numSave2);
	fwrite(&BoxblackColor, sizeof(ImColor), 1, numSave2);
    fwrite(&BotBoxblackColor, sizeof(ImColor), 1, numSave2);
    fflush(numSave2);
    fsync(fileno(numSave2));
}

void NumIoLoad(const char *name)
{
    if (numSave2 == nullptr) {
        string SaveFile = "/data";
        SaveFile += "/";
        SaveFile += name;
        numSave2 = fopen(SaveFile.c_str(), "rb+");
    }
    if (numSave2 != nullptr) {
        fseek(numSave2, 0, SEEK_SET);
		fread(DrawIo, sizeof(bool) * 50, 1, numSave2);
        fread(NumIo, sizeof(float) * 50, 1, numSave2);
		fread(&BoxColor, sizeof(ImColor), 1, numSave2);
        fread(&BotBoxColor, sizeof(ImColor), 1, numSave2);
        fread(&LineColor, sizeof(ImColor), 1, numSave2);
        fread(&BotLineColor, sizeof(ImColor), 1, numSave2);
        fread(&BoneColor, sizeof(ImColor), 1, numSave2);
        fread(&BotBoneColor, sizeof(ImColor), 1, numSave2);
        fread(&RightColor, sizeof(ImColor), 1, numSave2);
        fread(&BotRightColor, sizeof(ImColor), 1, numSave2);    
		fread(&WarningColor, sizeof(ImColor), 1, numSave2);    
		fread(&BotWarningColor, sizeof(ImColor), 1, numSave2);    		
		fread(&BoxSize, sizeof(float), 1, numSave2);
    	fread(&BotBoxSize, sizeof(float), 1, numSave2);
    	fread(&LineSize, sizeof(float), 1, numSave2);
    	fread(&BotLineSize, sizeof(float), 1, numSave2);
    	fread(&BoneSize, sizeof(float), 1, numSave2);
    	fread(&BotBoneSize, sizeof(float), 1, numSave2);
		fread(&BoxblackColor, sizeof(ImColor), 1, numSave2);
    	fread(&BotBoxblackColor, sizeof(ImColor), 1, numSave2);
		if (NumIo[17] < 100.0f)
			NumIo[17] = 400.0f;
    } else {             
		NumIo[1] = 300.0f;                                                  
		NumIo[2] = 400.0f;
 	    NumIo[3] = 150.0f;
 	    NumIo[4] = 15.0f;
  	    NumIo[6] = 1400.0f;
    	NumIo[5] = 650.0f;
    	NumIo[7] = 300.0f;  
    	NumIo[8] = 0.0f;  
    	NumIo[9] = 3.5f; 
		NumIo[10] = 0.0f;  
		NumIo[11] = 600.0f;
		NumIo[12] = 90;
		NumIo[13] = 0.0f;
		NumIo[14] = 1.9f;
		NumIo[15] = 0.0f;
		NumIo[16] = 100.0f;
		NumIo[17] = 400.0f;
		NumIo[20] = 0.0f;
		NumIo[21] = 0.0f;
    }
}

void CleanData() {
	/* 定义自定义颜色 */
    TouchingColor = ImColor(255, 0, 0, 150);
    BoxColor = {1.0f,0.0f,0.0f,1.0f};
    BotBoxColor = ImColor(255,255,255,255);
    BoxblackColor = ImColor(255,0,0,25);
    BotBoxblackColor = ImColor(255,0,0,25);
    LineColor = ImColor(255,0,0,255);
    BotLineColor = ImColor(255,255,255,255);
    BoneColor = ImColor(255,0,0,255);
    BotBoneColor = ImColor(255,255,255,255);
    RightColor = ImColor(255,200,0,255);
    BotRightColor = ImColor(255,255,255,255);    
    WarningColor = ImColor(255,0,0,255);
    BotWarningColor = ImColor(255,255,255,255);    

	/* 定义自定义大小 */
	BoxSize = 1.5f;
	BotBoxSize = 1.5f;
	LineSize = 1.5f;
	BotLineSize = 1.5f;
	BoneSize = 2.5f;
	BotBoneSize = 2.5f;
	
	NumIo[1] = 300.0f;                       
	NumIo[2] = 400.0f;
    NumIo[3] = 150.0f;
    NumIo[4] = 15.0f;
    NumIo[6] = 1400.0f;
	NumIo[5] = 650.0f;
	NumIo[7] = 300.0f;  
	NumIo[8] = 0.0f;  
	NumIo[9] = 3.5f; 
	NumIo[10] = 0.0f;  
	NumIo[11] = 600.0f;
	NumIo[12] = 90;
	NumIo[13] = 0.0f;
	NumIo[14] = 1.9f;
	NumIo[15] = 0.0f;
	NumIo[16] = 100.0f;
	NumIo[17] = 400.0f;
	NumIo[20] = 0.0f;
	NumIo[21] = 0.0f;
}

string getCmdResult(const string &strCmd) {
    char buf[10240] = {0};
    FILE *pf = NULL;
    if ((pf = popen(strCmd.c_str(), "r")) == NULL) {
        return "";
    }
    string strResult;
    while (fgets(buf, sizeof buf, pf)) {
        strResult += buf;
    }
    pclose(pf);
    unsigned int iSize = strResult.size();
    if (iSize > 0 && strResult[iSize - 1] == '\n') {
        strResult = strResult.substr(0, iSize - 1);
    }
    return strResult;
}

bool IsAimLongAim = false;
char AimName[32];
int Aimchoose = (int)NumIo[21];

// 遍历自瞄对象
int findminat()
{
    float min = NumIo[3];
	float DistanceMin = NumIo[17];
    int minAt = 999;
    for (int i = 0; i < MaxPlayerCount; i++)
    {
		switch ((int)NumIo[21])
        {
            case 0:
                if (IsAimLongAim) {
					if (strcmp(Aim[i].Name, AimName) == 0)
    				{           	
        				minAt = i;
    				}
				} else {
    				if (Aim[i].ScreenDistance < min)
    				{
						if (DrawIo[30]) {
							strcpy(AimName, Aim[i].Name);
						}
        				min = Aim[i].ScreenDistance;
        				minAt = i;
    				}
				}
			break;
			case 1:
                if (IsAimLongAim) {
					if (strcmp(Aim[i].Name, AimName) == 0)
    				{           	
        				minAt = i;
    				}
				} else {
    				if (Aim[i].WodDistance < DistanceMin)
    				{
						if (DrawIo[30]) {
							strcpy(AimName, Aim[i].Name);
						}
        				DistanceMin = Aim[i].WodDistance;
        				minAt = i;
					}
    			}
        	break;
		}
    }
    if (minAt == 999)
    {
        Gmin = -1;
		IsAimLongAim = false;
        return -1;
    }
    Gmin = minAt;   
    Aim[minAt].WodDistance;
	if (DrawIo[30]) {
		IsAimLongAim = true;
	}
    return minAt;
}

Vec2 vpvp;

// 自瞄线程
void AimBotAuto()
{   
    bool isDown = false;
    // 是否按下触摸
	
    double tx = NumIo[5], ty = NumIo[6];
    // 触摸点位置

    double ScreenX = screen_x, ScreenY = screen_y;
    // 分辨率(竖屏)PS:滑屏用的坐标是竖屏状态下的

    double ScrXH = ScreenX / 2.0f;
    // 一半屏幕X

    double ScrYH = ScreenY / 2.0f;
    // 一半屏幕X

    static float TargetX = 0;
    static float TargetY = 0;
    // 触摸目标位置
	
    Vec3 obj;
    
    float NowCoor[3];
   
    while (1)
    {		
		usleep(1000000 / 120);
		
		ImGuiIO& iooi = ImGui::GetIO();
		
		if (DrawIo[21] && iooi.MouseDown[0] && iooi.MousePos.x <= NumIo[6] + NumIo[7] && iooi.MousePos.y <= screen_y - NumIo[5] + NumIo[7] && iooi.MousePos.x >= NumIo[6] - NumIo[7] && iooi.MousePos.y >= screen_y - NumIo[5] - NumIo[7])
        {           
        	usleep(30000);      
            if (DrawIo[21] && iooi.MouseDown[0] && iooi.MousePos.x <= NumIo[6] + NumIo[7] && iooi.MousePos.y <= screen_y - NumIo[5] + NumIo[7] && iooi.MousePos.x >= NumIo[6] - NumIo[7] && iooi.MousePos.y >= screen_y - NumIo[5] - NumIo[7])
        	{
           		while (DrawIo[21] && iooi.MouseDown[0] && iooi.MousePos.x <= NumIo[6] + NumIo[7] && iooi.MousePos.y <= screen_y - NumIo[5] + NumIo[7] && iooi.MousePos.x >= NumIo[6] - NumIo[7] && iooi.MousePos.y >= screen_y - NumIo[5] - NumIo[7])
                {			
    				NumIo[6] = iooi.MousePos.x;
                    NumIo[5] = screen_y - iooi.MousePos.y;
					TouchingColor = ImColor(0, 220, 0, 150);
    				usleep(500);           				
                }		
				TouchingColor = ImColor(255, 0, 0, 150);
            }
        }
		
        if (!DrawIo[20])
        {           
			IsAimLongAim = false;
	        if (isDown == true)
            {
				usleep(1000);
                tx = NumIo[5], ty = NumIo[6];
                // 恢复变量 
                Touch_Up(6);
                // 抬起
                isDown = false;
            }
            usleep(NumIo[9] * 1000);
            continue;
        }
        
		if (!DrawIo[40])
        {           
			IsAimLongAim = false;
			
            if (isDown == true)
            {
				usleep(1000);
                tx = NumIo[5], ty = NumIo[6];
                // 恢复变量 
                Touch_Up(6);
                // 抬起
                isDown = false;
            }
            usleep(NumIo[9] * 1000);
            continue;
        }
		
        findminat();
        // 获取目标

        if (Gmin == -1)
        {          
			IsAimLongAim = false;
            if (isDown == true)
            {
				usleep(1000);
                tx = NumIo[5], ty = NumIo[6];
                // 恢复变量 
                Touch_Up(6);
                // 抬起
                isDown = false;
            }
            usleep(NumIo[9] * 1000);
            continue;
        }
		
        float ToReticleDistance = Aim[Gmin].ScreenDistance;              
        float FlyTime = Aim[Gmin].WodDistance / NumIo[11];
        float DropM = 540.0f * FlyTime * FlyTime;
        // 下坠
		
        bool Firing = temp->IsFiring;
        bool Aiming = temp->Fov <= 75;     
                  
	    NowCoor[0] = Aim[Gmin].ObjAim.x;
        NowCoor[1] = Aim[Gmin].ObjAim.y;
        NowCoor[2] = Aim[Gmin].ObjAim.z;
        obj.x = NowCoor[0] + (Aim[Gmin].AimMovement.x * FlyTime);
        obj.y = NowCoor[1] + (Aim[Gmin].AimMovement.y * FlyTime);
        obj.z = NowCoor[2] + (Aim[Gmin].AimMovement.z * FlyTime) + DropM;
		
		if (Firing)
			obj.z -= Aim[Gmin].WodDistance * NumIo[14] * GetWeaponId(temp->MyWeapon);
		
		float cameras = temp->matrix[3] * obj.x + temp->matrix[7] * obj.y + temp->matrix[11] * obj.z + temp->matrix[15]; 
        
        vpvp = WorldIsScreen(obj, temp->matrix, cameras);      
		
        zm_y = vpvp.x;     
        zm_x = ScreenX - vpvp.y;        
		float AimDs = sqrt(pow(screen_x / 2 - vpvp.x, 2) + pow(screen_y / 2 - vpvp.y, 2));
		
		float Aimspeace = NumIo[4];
						
        if (zm_x <= 0 || zm_x >= ScreenX || zm_y <= 0 || zm_y >= ScreenY)
        {          
			IsAimLongAim = false;
            if (isDown == true)
            {
				usleep(1000);
                tx = NumIo[5], ty = NumIo[6];
                // 恢复变量 
                Touch_Up(6);
                // 抬起
                isDown = false;
            }
            usleep(NumIo[9] * 1000);
            continue;
        }

        if (ToReticleDistance <= NumIo[3] || AimDs <= NumIo[3])
        {                                    
            switch ((int)NumIo[0])
            {
                case 0:
                    if (!Firing)
                    {
						IsAimLongAim = false;
                        if (isDown == true)
                        {
							usleep(1000);
                            tx = NumIo[5], ty = NumIo[6];
                            // 恢复变量
                            Touch_Up(6);
                            isDown = false;
                        }                      
                        usleep(NumIo[9] * 1000);
                        continue;
                    }
                break;
                case 1:
                    if (!Aiming)
                    {
						IsAimLongAim = false;
                        if (isDown == true)
                        {
							usleep(1000);
                            tx = NumIo[5], ty = NumIo[6];
                            // 恢复变量
                            Touch_Up(6);
                            isDown = false;
                        }
                        usleep(NumIo[9] * 1000);
                        continue;
                    }
                break;
                case 2:
                    if (!Firing && !Aiming)
                    {
						IsAimLongAim = false;
                        if (isDown == true)
                        {
							usleep(1000);
                            tx = NumIo[5], ty = NumIo[6];
                            // 恢复变量
                            Touch_Up(6);
                            isDown = false;
                        }
                        usleep(NumIo[9] * 1000);
                        continue;
                    }
                break;
				case 3:
                    
                break;
            }
			
			float Acc = getScopeAcc((int)(90 / temp->Fov));
			
            if (isDown == false)
            {
				usleep(1000);
				if (NumIo[10] == 0.0f)
                	Touch_Down(6, (int)tx, (int)ty);
				else
					Touch_Down(6, screen_y - (int)tx, screen_x-(int)ty);
                isDown = true;
				usleep(1000);
            }

            if (zm_x > ScrXH) {
                TargetX = -(ScrXH - zm_x) / NumIo[4] * Acc;
                if (TargetX + ScrXH > ScrXH * 2) {
                    TargetX = 0;
                }
            }
            else if (zm_x < ScrXH) {
                TargetX = (zm_x - ScrXH) / NumIo[4] * Acc;             
                if (TargetX + ScrXH < 0) {
                    TargetX = 0;
                }
            }
            
            if (zm_y > ScrYH) {
                TargetY = -(ScrYH - zm_y) / NumIo[4] * Acc;              
                if (TargetY + ScrYH > ScrYH * 2) {
                    TargetY = 0;
                }
            }
            else if (zm_y < ScrYH) {
                TargetY = (zm_y - ScrYH) / NumIo[4] * Acc;              
                if (TargetY + ScrYH < 0) {
                    TargetY = 0;
                }
            }

            if (TargetY >= 35 || TargetX >= 35 || TargetY <= -35 || TargetX <= -35)
            {
                if (isDown)
                {
					usleep(1000);
                    tx = NumIo[5], ty = NumIo[6];
                    // 恢复变量
                    Touch_Up(6);
                    isDown = false;
                }
                usleep(NumIo[9] * 1000);
                continue;
            }           		
			
            tx += TargetX;
            ty += TargetY;		
			
            if (tx >= NumIo[5] + NumIo[7] || tx <= NumIo[5] - NumIo[7]
                || ty >= NumIo[6] + NumIo[7] || ty <= NumIo[6] - NumIo[7])
            {
				usleep(1000);
                // 只要滑屏达到了边界，直接还原至中心
                tx = NumIo[5], ty = NumIo[6];
                // 恢复变量
                Touch_Up(6);
                // 抬起          
				usleep(3000);  
				// 延迟
				if (!NumIo[10])
                	Touch_Down(6, (int)tx, (int)ty);
			    else
					Touch_Down(6, screen_y - (int)tx, screen_x - (int)ty);
                // 按下           
				isDown = true;
				
				tx += TargetX;
            	ty += TargetY;		
				
				usleep(1000);
            }         		     
			
			if (!NumIo[10])
           		Touch_Move(6, (int)tx, (int)ty);
		    else
		    	Touch_Move(6, screen_y - (int)tx, screen_x - (int)ty);
			
			isDown = true;
			
			usleep(NumIo[9] * 1000);
        } else {          
			IsAimLongAim = false;
            if (isDown)
            {
                tx = NumIo[5], ty = NumIo[6];
                // 恢复变量 
                Touch_Up(6);
                // 抬起
                isDown = false;
				// 延时
				usleep(NumIo[9] * 1000);
            }
        }      
    }
}

ImTextureID createTexturePNGFro(const unsigned char *buf,int len) {
    int w, h, n;
    stbi_uc *data = stbi_png_load_from_memory(buf, len, &w, &h, &n, 0);
    GLuint texture;
    glGenTextures(1, &texture);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    if (n == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    stbi_image_free(data);
    ImTextureID textureId = texture;
    return textureId;
}

int main()
{
	//android_check();//验证
    screen_config();
    init_screen_x = screen_x + screen_y;
    init_screen_y = screen_y + screen_x;
	new std::thread(hack_thread);
    if (!init_egl(init_screen_x, init_screen_y)) {
        exit(0);
    }
    ImGui_init();
	
	usleep(5000);
    int x, y;
	Touch_Init(&x,&y);  // 监听
	sleep(1);
	
	temp = tempRead;
	
	usleep(1000);
	
	/*puts("\n\n # GUI MAX 使用说明\n 如果没效果请注意以下几点!\n 1.恢复原始面具版本 不要用一些花里胡哨面具!\n 2.配置排除列表和平全选 但是遵守排除列表开关要关闭!\n ▶ 没有百分百稳定的辅助，适当演戏才能安全使用\n ▶ 本辅助为免费公益辅助，传播比较广泛，稳定性一般，大号的自行决定是否使用");
	
	puts("\n\033[1;34;40m 欢迎使用 GUI MAX 和平内核版本\033[0m\n");
	puts("\033[1;37;40m 当前版本：3.1.3\033[0m\n");

	puts("\033[33;31m**************************************\033[0m");
    puts("\033[1;33;40mQQ交流群 609690497\033[0m\n");
    puts("\033[1;33;40mTG @guiprofl\033[0m");
    puts("\033[33;31m**************************************\033[0m");   	*/
	new std::thread(AimBotAuto);
	NumIoLoad("FlyBlueSaveNum");
	RenderingFPS.SetFps(NumIo[12]);
	RenderingFPS.AotuFPS_init();
	RenderingFPS.setAffinity();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplOpenGL3_NewFrame();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    bool flag = true;
	size_t length = (logoDataBase64.length() + 1) / 4 * 3;
	unsigned char *data = base64_decode((unsigned char *)logoDataBase64.c_str());
	imageButton.textureId = createTexturePNGFro(data, length);
	
	length = (aim_off_Base64.length() + 1) / 4 * 3;
	data = base64_decode((unsigned char *)aim_off_Base64.c_str());
	offButton.textureId = createTexturePNGFro(data, length);
	
	length = (aim_on_Base64.length() + 1) / 4 * 3;
	data = base64_decode((unsigned char *)aim_on_Base64.c_str());
	onButton.textureId = createTexturePNGFro(data, length);
	
	length = (robot_Base64.length() + 1) / 4 * 3;
	data = base64_decode((unsigned char *)robot_Base64.c_str());
	robotpng.textureId = createTexturePNGFro(data, length);
	
	length = (player_Base64.length() + 1) / 4 * 3;
	data = base64_decode((unsigned char *)player_Base64.c_str());
	playerpng.textureId = createTexturePNGFro(data, length);
	

	sleep(1);
    while (flag) {
  
        Rendering(&flag);    
		RenderingFPS.SetFps(NumIo[12]);
		RenderingFPS.AotuFPS();
    }
	
    shutdown();
	
    return 0;
}

// 信息绘制函数
void DrawHealth(Vec2 box, float w, float entityHealth, uint32_t TeamID, char* name)
{
    float x = box.x - (140 - w) / 2;
    float y = box.y;
    char TeamText[50];
    sprintf(TeamText,"%d",TeamID);
    ImVec4 HPColor = entityHealth < 80 ? entityHealth < 60 ? entityHealth < 30 ? ImVec4{ 0.5f,0.0f,0.0f,127/255.f } : ImVec4{ 1, 0, 0, 127/255.f } : ImVec4{ 1, 1, 0, 127/255.f } : ImVec4{ 255/255.f, 255/255.f, 255/255.f, 127/255.f };
	
	if (DrawIo[4]) {
    	tm = 100.f/255.f;
		if (name == "RoBot") {
    		DrawTf.DrawRectFilled(x-strlen(name), y - 41, 120 + strlen(name)*2, 20,ImVec4(1, 1, 1, 100.f/255.f));//玩家名称背景
		} else {
			DrawTf.DrawRectFilled(x-strlen(name), y - 41, 120 + strlen(name)*2, 20,arr[TeamID%length]);//玩家名称背景
		}
		
    	tm = 255.f/255.f;
		if (name == "RoBot") {
    		DrawTf.DrawRectFilled(x-strlen(name), box.y - 41, 25, 20, ImVec4(1, 1, 1, 255.f/255.f));
		} else {
    		DrawTf.DrawRectFilled(x-strlen(name), box.y - 41, 25, 20, arr[TeamID%length]);
		}
		
		if (name == "RoBot") {
			ImGui::GetBackgroundDrawList()->AddImage(robotpng.textureId, ImVec2{x + 100 + strlen(name), y - 20}, ImVec2{x + 125 + strlen(name), y - 42});
		} else {
			ImGui::GetBackgroundDrawList()->AddImage(playerpng.textureId, ImVec2{x + 100 + strlen(name), y - 20}, ImVec2{x + 125 + strlen(name), y - 42});
		}
		
    	if (strlen(TeamText) < 2) {
        	DrawTf.DrawStrokeText(x+6-strlen(name), box.y - 42, ImVec4{ 1.0f,1.0f ,1.0f ,1.0f }, TeamText);//队伍ID
    	} else {
        	DrawTf.DrawStrokeText(x-strlen(name), box.y - 42, ImVec4{ 1.0f,1.0f ,1.0f ,1.0f }, TeamText);//队伍ID
    	}
    	DrawTf.DrawStrokeText(x + 28-strlen(name), y - 43, ImVec4{ 255.f/255.f,255.f/255.f,255.f/255.f,255.f/255.f }, name);//玩家名称
	}
	
    if (DrawIo[6]) {
        DrawTf.DrawRectFilled(x-strlen(name), y - 18, entityHealth * (strlen(name)*2 + 120) / 100, 8, HPColor);//血量线条
	}
	
    DrawTf.DrawTriangle(box.x + w / 2 - 10,y - 8, box.x + w / 2 + 5,y -8,box.x+w/2-2,y,ColorY.White_,1);//第一个左边 第二个右边 第三个底边
}

ImColor BoneDrawColor;

// 绘制骨骼函数
void DrawBone(ImVec2 start, ImVec2 end, bool Cansee)
{
	if (Cansee) {
		ImGui::GetForegroundDrawList()->AddLine(start, end, BoneDrawColor, {BoneSize});
	} else if (!Cansee){
		ImGui::GetForegroundDrawList()->AddLine(start, end, ImColor(0, 255, 0), {BotBoneSize});
	}
}

/**
 * 绘制立体方框
 * @param object_position 人物底部坐标
 * @param object_hand 人物头部坐标
 * @param object_Angle 人物欧拉角
 * @param box_width 方框宽度
 * @param BoxColor 方框颜色
 * @param BoxSize 方框粗细
 * @return
 */
void Draw3DBox(Vec3 object_position,Vec3 object_pass, Vec3 object_hand, float object_Angle, float box_width, ImColor BoxColor, float BoxSize, bool IsAi) {
    Vec3 rotate_position[8];     // 存储旋转变换8个点坐标
    Vec2 object_point[8];          // 对象屏幕坐标                                        

	/* 下_左上 */
    rotate_position[0].x = object_position.x + box_width * (float)sin((object_Angle + 45) * M_PI / 180);
    rotate_position[0].y = object_position.y + box_width * (float)cos((object_Angle + 45) * M_PI / 180);
    rotate_position[0].z = object_pass.z;
	
	/* 下_左下 */
	rotate_position[1].x = object_position.x + box_width * (float)sin((object_Angle + 135) * M_PI / 180);
    rotate_position[1].y = object_position.y + box_width * (float)cos((object_Angle + 135) * M_PI / 180);
    rotate_position[1].z = object_pass.z;
	
	/* 下_右下 */
	rotate_position[2].x = object_position.x + box_width * (float)sin((object_Angle + 225) * M_PI / 180);
    rotate_position[2].y = object_position.y + box_width * (float)cos((object_Angle + 225) * M_PI / 180);
    rotate_position[2].z = object_pass.z;
	
	/* 下_右上 */
	rotate_position[3].x = object_position.x + box_width * (float)sin((object_Angle + 315) * M_PI / 180);
    rotate_position[3].y = object_position.y + box_width * (float)cos((object_Angle + 315) * M_PI / 180);
    rotate_position[3].z = object_pass.z;
	
	
	/* 上_左上 */
    rotate_position[4].x = object_position.x + box_width * (float)sin((object_Angle + 45) * M_PI / 180);
    rotate_position[4].y = object_position.y + box_width * (float)cos((object_Angle + 45) * M_PI / 180);
    rotate_position[4].z = object_hand.z + 10;
	
	/* 上_左下 */
	rotate_position[5].x = object_position.x + box_width * (float)sin((object_Angle + 135) * M_PI / 180);
    rotate_position[5].y = object_position.y + box_width * (float)cos((object_Angle + 135) * M_PI / 180);
    rotate_position[5].z = object_hand.z + 10;
	
	/* 上_右下 */
	rotate_position[6].x = object_position.x + box_width * (float)sin((object_Angle + 225) * M_PI / 180);
    rotate_position[6].y = object_position.y + box_width * (float)cos((object_Angle + 225) * M_PI / 180);
    rotate_position[6].z = object_hand.z + 10;
	
	/* 上_右上 */
	rotate_position[7].x = object_position.x + box_width * (float)sin((object_Angle + 315) * M_PI / 180);
    rotate_position[7].y = object_position.y + box_width * (float)cos((object_Angle + 315) * M_PI / 180);
    rotate_position[7].z = object_hand.z + 10;
	
	/* 遍历坐标数组转换屏幕坐标 */
	for (int i = 0 ; i < 8 ; i ++) {
		float cameras = temp->matrix[3] * rotate_position[i].x + temp->matrix[7] * rotate_position[i].y + temp->matrix[11] * rotate_position[i].z + temp->matrix[15]; 
		object_point[i] = WorldIsScreen(rotate_position[i], temp->matrix, cameras);
	}
	
	ImGui::GetBackgroundDrawList()->AddLine({object_point[0].x, object_point[0].y}, {object_point[3].x, object_point[3].y}, BoxColor, {BoxSize});
	ImGui::GetBackgroundDrawList()->AddLine({object_point[1].x, object_point[1].y}, {object_point[2].x, object_point[2].y}, BoxColor, {BoxSize});
	ImGui::GetBackgroundDrawList()->AddLine({object_point[0].x, object_point[0].y}, {object_point[1].x, object_point[1].y}, BoxColor, {BoxSize});
	ImGui::GetBackgroundDrawList()->AddLine({object_point[3].x, object_point[3].y}, {object_point[2].x, object_point[2].y}, BoxColor, {BoxSize});
	
	ImGui::GetBackgroundDrawList()->AddLine({object_point[4].x, object_point[4].y}, {object_point[7].x, object_point[7].y}, BoxColor, {BoxSize});
	ImGui::GetBackgroundDrawList()->AddLine({object_point[5].x, object_point[5].y}, {object_point[6].x, object_point[6].y}, BoxColor, {BoxSize});
	ImGui::GetBackgroundDrawList()->AddLine({object_point[4].x, object_point[4].y}, {object_point[5].x, object_point[5].y}, BoxColor, {BoxSize});
	ImGui::GetBackgroundDrawList()->AddLine({object_point[7].x, object_point[7].y}, {object_point[6].x, object_point[6].y}, BoxColor, {BoxSize});
	
	ImGui::GetBackgroundDrawList()->AddLine({object_point[0].x, object_point[0].y}, {object_point[4].x, object_point[4].y}, BoxColor, {BoxSize});
	ImGui::GetBackgroundDrawList()->AddLine({object_point[1].x, object_point[1].y}, {object_point[5].x, object_point[5].y}, BoxColor, {BoxSize});
	ImGui::GetBackgroundDrawList()->AddLine({object_point[2].x, object_point[2].y}, {object_point[6].x, object_point[6].y}, BoxColor, {BoxSize});
	ImGui::GetBackgroundDrawList()->AddLine({object_point[3].x, object_point[3].y}, {object_point[7].x, object_point[7].y}, BoxColor, {BoxSize});
	
	if (IsAi) {
		ImGui::GetBackgroundDrawList()->AddQuadFilled({object_point[0].x, object_point[0].y}, {object_point[4].x, object_point[4].y}, {object_point[5].x, object_point[5].y}, {object_point[1].x, object_point[1].y}, BotBoxblackColor);
		ImGui::GetBackgroundDrawList()->AddQuadFilled({object_point[3].x, object_point[3].y}, {object_point[7].x, object_point[7].y}, {object_point[2].x, object_point[2].y}, {object_point[6].x, object_point[6].y}, BotBoxblackColor);
	
		ImGui::GetBackgroundDrawList()->AddQuadFilled({object_point[0].x, object_point[0].y}, {object_point[4].x, object_point[4].y}, {object_point[7].x, object_point[7].y}, {object_point[3].x, object_point[3].y}, BotBoxblackColor);
		ImGui::GetBackgroundDrawList()->AddQuadFilled({object_point[1].x, object_point[1].y}, {object_point[5].x, object_point[5].y}, {object_point[6].x, object_point[6].y}, {object_point[2].x, object_point[2].y}, BotBoxblackColor);
	
		ImGui::GetBackgroundDrawList()->AddQuadFilled({object_point[0].x, object_point[0].y}, {object_point[1].x, object_point[1].y}, {object_point[2].x, object_point[2].y}, {object_point[3].x, object_point[3].y}, BotBoxblackColor);
		ImGui::GetBackgroundDrawList()->AddQuadFilled({object_point[4].x, object_point[4].y}, {object_point[5].x, object_point[5].y}, {object_point[6].x, object_point[6].y}, {object_point[7].x, object_point[7].y}, BotBoxblackColor);
	} else {
		ImGui::GetBackgroundDrawList()->AddQuadFilled({object_point[0].x, object_point[0].y}, {object_point[4].x, object_point[4].y}, {object_point[5].x, object_point[5].y}, {object_point[1].x, object_point[1].y}, BotBoxblackColor);
		ImGui::GetBackgroundDrawList()->AddQuadFilled({object_point[3].x, object_point[3].y}, {object_point[7].x, object_point[7].y}, {object_point[2].x, object_point[2].y}, {object_point[6].x, object_point[6].y}, BotBoxblackColor);
	
		ImGui::GetBackgroundDrawList()->AddQuadFilled({object_point[0].x, object_point[0].y}, {object_point[4].x, object_point[4].y}, {object_point[7].x, object_point[7].y}, {object_point[3].x, object_point[3].y}, BotBoxblackColor);
		ImGui::GetBackgroundDrawList()->AddQuadFilled({object_point[1].x, object_point[1].y}, {object_point[5].x, object_point[5].y}, {object_point[6].x, object_point[6].y}, {object_point[2].x, object_point[2].y}, BotBoxblackColor);
	
		ImGui::GetBackgroundDrawList()->AddQuadFilled({object_point[0].x, object_point[0].y}, {object_point[1].x, object_point[1].y}, {object_point[2].x, object_point[2].y}, {object_point[3].x, object_point[3].y}, BotBoxblackColor);
		ImGui::GetBackgroundDrawList()->AddQuadFilled({object_point[4].x, object_point[4].y}, {object_point[5].x, object_point[5].y}, {object_point[6].x, object_point[6].y}, {object_point[7].x, object_point[7].y}, BotBoxblackColor);
	}
}

// 绘图主函数
void Draw_Main(ImDrawList *Draw)
{
	float top, right, left, bottom, top1; 
	int PlayerCount = 0, BotCount = 0;
	
	AimCount = 0;
	
	// 绘制水印
	/*string guistr = "GUI MAX 免费公益辅助 \n官方频道: @guiprofl\n官方QQ群：609690497";
    Draw->AddText(NULL, 32, {150, 85}, ImColor(255, 0, 0), guistr.c_str());*/       
	
	// 自瞄圈圈
    if (DrawIo[20])       
    {         
        Draw->AddCircle({screen_x / 2, screen_y / 2}, NumIo[3], ImColor(255, 0, 0), 0, 2.5f);     
    }
	
	if (DrawIo[44] && Gmin != -1)
    {   
		Draw->AddLine({screen_x / 2, screen_y / 2}, {vpvp.x, vpvp.y}, ImColor(255, 135, 0), 2.5f);
	}
		
	// 触摸区域
	if (DrawIo[21])
    {	
        std::string ssf;  
        ssf += "勿放控件，长按拖动";
        auto textSize = ImGui::CalcTextSize(ssf.c_str(), 0, 32);
		Draw->AddRectFilled({0,0}, {screen_x, screen_y},ImColor(0,0,0,110));    
        Draw->AddRectFilled({NumIo[6] - NumIo[7] / 2, screen_y - NumIo[5] + NumIo[7] / 2}, {NumIo[6] + NumIo[7] / 2, screen_y - NumIo[5] - NumIo[7] / 2}, TouchingColor); 
		Draw->AddText(NULL, 32, {NumIo[6] - (textSize.x / 2), screen_y - NumIo[5]}, ImColor(255, 255, 255), ssf.c_str());                                                   
    }
	
	// 雷达背景
    if (DrawIo[7]){
        Draw->AddRectFilled({NumIo[1] - 100, NumIo[2] - 100}, {NumIo[1] + 100, NumIo[2] + 100}, ImColor(0, 0, 0, 55));
        Draw->AddRect({NumIo[1] - 100, NumIo[2] - 100}, {NumIo[1] + 100, NumIo[2] + 100}, ImColor(ImVec4(255/255.f, 255/255.f, 258/255.f, 1.0f)));
        Draw->AddLine({NumIo[1] + 100, NumIo[2]}, {NumIo[1] - 100, NumIo[2]}, ImColor(ImVec4(255/255.f, 255/255.f, 258/255.f, 0.5f)), 2);
        Draw->AddLine({NumIo[1], NumIo[2] + 100}, {NumIo[1], NumIo[2] - 100}, ImColor(ImVec4(255/255.f, 255/255.f, 258/255.f, 0.5f)), 2);
    }
	
    if (temp) {
        for (int i = 0; i < temp->mPlayerArray.Count; i++)
        {
            if (temp->mPlayerArray.mPlayer[i].Distance > NumIo[17])
                continue;

            Vec2 Radar = { (temp->MyPos.x - temp->mPlayerArray.mPlayer[i].Pos.x) / NumIo[16], (temp->MyPos.y - temp->mPlayerArray.mPlayer[i].Pos.y) / NumIo[16] };

            if (DrawIo[7]) {
                // 雷达
                if (NumIo[1] + Radar.x >= NumIo[1] - 100 && NumIo[1] + Radar.x <= NumIo[1] + 100 && NumIo[2] + Radar.y >= NumIo[2] - 100 && NumIo[2] + Radar.y <= NumIo[2] + 100) {
                    if (temp->mPlayerArray.mPlayer[i].IsBot) {
                        Draw->AddCircleFilled({ NumIo[1] + Radar.x, NumIo[2] + Radar.y }, { 20 }, ImColor(255, 255, 255));
                        string sdt = "AI";
                        auto textSize = ImGui::CalcTextSize(sdt.c_str(), 0, 25);
                        Draw->AddText(NULL, 25, { NumIo[1] + Radar.x - textSize.x / 2, NumIo[2] + Radar.y - textSize.y * 0.45 }, ImColor(50, 50, 50), sdt.c_str());
                    }
                    else {
                        tm = 150.f / 255.f;
                        Draw->AddCircleFilled({ NumIo[1] + Radar.x, NumIo[2] + Radar.y }, { 20 }, ImColor(arr[temp->mPlayerArray.mPlayer[i].TeamID % length]));
                        string sdt = to_string((int)temp->mPlayerArray.mPlayer[i].TeamID);
                        auto textSize = ImGui::CalcTextSize(sdt.c_str(), 0, 25);
                        Draw->AddText(NULL, 25, { NumIo[1] + Radar.x - textSize.x / 2, NumIo[2] + Radar.y - textSize.y * 0.45 }, ImColor(255, 255, 255), sdt.c_str());
                    }
                }
            }

            if (temp->mPlayerArray.mPlayer[i].w > 0 && temp->mPlayerArray.mPlayer[i].Head.ScreenPos.x > 0 && temp->mPlayerArray.mPlayer[i].Head.ScreenPos.x < screen_x && temp->mPlayerArray.mPlayer[i].Head.ScreenPos.y > 0 && temp->mPlayerArray.mPlayer[i].Head.ScreenPos.y < screen_y)
            {
                if (DrawIo[20])
                {
                    strcpy(Aim[AimCount].Name, temp->mPlayerArray.mPlayer[i].PlayerName);
                    Aim[AimCount].WodDistance = temp->mPlayerArray.mPlayer[i].Distance;
                    Aim[AimCount].AimMovement = temp->mPlayerArray.mPlayer[i].Predict;
                    if (NumIo[8] == 1.0) {
                        Aim[AimCount].ObjAim = temp->mPlayerArray.mPlayer[i].Head.Pos;
                        Aim[AimCount].ScreenDistance = sqrt(pow(screen_x / 2 - temp->mPlayerArray.mPlayer[i].Head.ScreenPos.x, 2) + pow(screen_y / 2 - temp->mPlayerArray.mPlayer[i].Head.ScreenPos.y, 2));
                    }
                    else if (NumIo[8] == 2.0) {
                        Aim[AimCount].ObjAim = temp->mPlayerArray.mPlayer[i].Chest.Pos;
                        Aim[AimCount].ScreenDistance = sqrt(pow(screen_x / 2 - temp->mPlayerArray.mPlayer[i].Chest.ScreenPos.x, 2) + pow(screen_y / 2 - temp->mPlayerArray.mPlayer[i].Chest.ScreenPos.y, 2));
                    }
                    else if (NumIo[8] == 3.0) {
                        Aim[AimCount].ObjAim = temp->mPlayerArray.mPlayer[i].Pelvis.Pos;
                        Aim[AimCount].ScreenDistance = sqrt(pow(screen_x / 2 - temp->mPlayerArray.mPlayer[i].Pelvis.ScreenPos.x, 2) + pow(screen_y / 2 - temp->mPlayerArray.mPlayer[i].Pelvis.ScreenPos.y, 2));
                    }
                    else {
                        Aim[AimCount].ObjAim = temp->mPlayerArray.mPlayer[i].Head.Pos;
                        Aim[AimCount].ScreenDistance = sqrt(pow(screen_x / 2 - temp->mPlayerArray.mPlayer[i].Head.ScreenPos.x, 2) + pow(screen_y / 2 - temp->mPlayerArray.mPlayer[i].Head.ScreenPos.y, 2));
                    }
                    AimCount++;
                }

                left = temp->mPlayerArray.mPlayer[i].Head.ScreenPos.x - temp->mPlayerArray.mPlayer[i].w * 0.6;
                right = temp->mPlayerArray.mPlayer[i].Head.ScreenPos.x + temp->mPlayerArray.mPlayer[i].w * 0.6;

                if (!temp->mPlayerArray.mPlayer[i].Head.Pos.x) {
                    top1 = temp->mPlayerArray.mPlayer[i].Pelvis.ScreenPos.y - temp->mPlayerArray.mPlayer[i].Chest.ScreenPos.y;
                }
                else {
                    top1 = temp->mPlayerArray.mPlayer[i].Pelvis.ScreenPos.y - temp->mPlayerArray.mPlayer[i].Head.ScreenPos.y;
                }

                top = temp->mPlayerArray.mPlayer[i].Pelvis.ScreenPos.y - top1 - temp->mPlayerArray.mPlayer[i].w / 5;

                if (temp->mPlayerArray.mPlayer[i].Left_Ankle.ScreenPos.y < temp->mPlayerArray.mPlayer[i].Right_Ankle.ScreenPos.y) {
                    bottom = temp->mPlayerArray.mPlayer[i].Right_Ankle.ScreenPos.y + temp->mPlayerArray.mPlayer[i].w / 10;
                }
                else {
                    bottom = temp->mPlayerArray.mPlayer[i].Left_Ankle.ScreenPos.y + temp->mPlayerArray.mPlayer[i].w / 10;
                }

                if (DrawIo[1]) {
                    // 方框
                    float box_width = 0;
                    float box_width_1 = fabs(temp->mPlayerArray.mPlayer[i].Left_Shoulder.Pos.x - temp->mPlayerArray.mPlayer[i].Right_Shoulder.Pos.x) + fabs(temp->mPlayerArray.mPlayer[i].Left_Shoulder.Pos.y - temp->mPlayerArray.mPlayer[i].Right_Shoulder.Pos.y);
                    float box_width_2 = fabs(temp->mPlayerArray.mPlayer[i].Left_Ankle.Pos.x - temp->mPlayerArray.mPlayer[i].Head.Pos.x) + fabs(temp->mPlayerArray.mPlayer[i].Left_Ankle.Pos.y - temp->mPlayerArray.mPlayer[i].Head.Pos.y);
                    float box_width_3 = fabs(temp->mPlayerArray.mPlayer[i].Pelvis.Pos.x - temp->mPlayerArray.mPlayer[i].Head.Pos.x) + fabs(temp->mPlayerArray.mPlayer[i].Pelvis.Pos.y - temp->mPlayerArray.mPlayer[i].Head.Pos.y);
                    Vec3 Pass = Vec3();
                    if (temp->mPlayerArray.mPlayer[i].Left_Ankle.Pos.z > temp->mPlayerArray.mPlayer[i].Right_Ankle.Pos.z) {
                        Pass = temp->mPlayerArray.mPlayer[i].Left_Ankle.Pos;
                    }
                    else {
                        Pass = temp->mPlayerArray.mPlayer[i].Right_Ankle.Pos;
                    }
                    if (box_width_3 > box_width_1) {
                        box_width = box_width_2;
                    }
                    else {
                        box_width = box_width_1;
                    }
                    if (NumIo[15]) {
                        if (temp->mPlayerArray.mPlayer[i].IsBot) {
                            Draw3DBox(temp->mPlayerArray.mPlayer[i].Pos, Pass, temp->mPlayerArray.mPlayer[i].Head.Pos, 0, 10 + box_width, BotBoxColor, BotBoxSize, true);
                        }
                        else {
                            Draw3DBox(temp->mPlayerArray.mPlayer[i].Pos, Pass, temp->mPlayerArray.mPlayer[i].Head.Pos, 0, 10 + box_width, BoxColor, BoxSize, false);
                        }
                    }
                    else {
                        if (temp->mPlayerArray.mPlayer[i].IsBot) {
                            Draw->AddRect({ left, top }, { right, bottom }, BotBoxColor, { 0 }, 0, { BotBoxSize });
                            Draw->AddRectFilled({ left, top }, { right, bottom }, BotBoxblackColor);
                        }
                        else {
                            Draw->AddRect({ left, top }, { right, bottom }, BoxColor, { 0 }, 0, { BoxSize });
                            Draw->AddRectFilled({ left, top }, { right, bottom }, BoxblackColor);
                        }
                    }
                }

                if (DrawIo[2]) {
                    // 射线
                    if (temp->mPlayerArray.mPlayer[i].IsBot)
                    {
                        Draw->AddLine({ screen_x / 2 , 0 }, { temp->mPlayerArray.mPlayer[i].Head.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Head.ScreenPos.y }, BotLineColor, { BotLineSize });
                    }
                    else {
                        Draw->AddLine({ screen_x / 2 , 0 }, { temp->mPlayerArray.mPlayer[i].Head.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Head.ScreenPos.y }, LineColor, { LineSize });
                    }
                }

                if (DrawIo[3]) {
                    if (temp->mPlayerArray.mPlayer[i].IsBot) {
                        BoneDrawColor = BotBoneColor;
                    }
                    else {
                        BoneDrawColor = BoneColor;
                    }
                    // 骨骼
                    if (temp->mPlayerArray.mPlayer[i].Head.CanSee) {
                        Draw->AddCircle({ temp->mPlayerArray.mPlayer[i].Head.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Head.ScreenPos.y }, temp->mPlayerArray.mPlayer[i].w / 14, BoneColor, BoneSize);
                    }
                    else {
                        Draw->AddCircle({ temp->mPlayerArray.mPlayer[i].Head.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Head.ScreenPos.y }, temp->mPlayerArray.mPlayer[i].w / 14, ImColor(0, 255, 0), BotBoneSize);
                    }
                    DrawBone({ temp->mPlayerArray.mPlayer[i].Head.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Head.ScreenPos.y }, { temp->mPlayerArray.mPlayer[i].Chest.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Chest.ScreenPos.y }, temp->mPlayerArray.mPlayer[i].Chest.CanSee);
                    DrawBone({ temp->mPlayerArray.mPlayer[i].Chest.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Chest.ScreenPos.y }, { temp->mPlayerArray.mPlayer[i].Pelvis.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Pelvis.ScreenPos.y }, temp->mPlayerArray.mPlayer[i].Pelvis.CanSee);
                    DrawBone({ temp->mPlayerArray.mPlayer[i].Chest.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Chest.ScreenPos.y }, { temp->mPlayerArray.mPlayer[i].Left_Shoulder.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Left_Shoulder.ScreenPos.y }, temp->mPlayerArray.mPlayer[i].Left_Shoulder.CanSee);
                    DrawBone({ temp->mPlayerArray.mPlayer[i].Chest.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Chest.ScreenPos.y }, { temp->mPlayerArray.mPlayer[i].Right_Shoulder.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Right_Shoulder.ScreenPos.y }, temp->mPlayerArray.mPlayer[i].Right_Shoulder.CanSee);
                    DrawBone({ temp->mPlayerArray.mPlayer[i].Left_Shoulder.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Left_Shoulder.ScreenPos.y }, { temp->mPlayerArray.mPlayer[i].Left_Elbow.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Left_Elbow.ScreenPos.y }, temp->mPlayerArray.mPlayer[i].Left_Elbow.CanSee);
                    DrawBone({ temp->mPlayerArray.mPlayer[i].Right_Shoulder.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Right_Shoulder.ScreenPos.y }, { temp->mPlayerArray.mPlayer[i].Right_Elbow.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Right_Elbow.ScreenPos.y }, temp->mPlayerArray.mPlayer[i].Right_Elbow.CanSee);
                    DrawBone({ temp->mPlayerArray.mPlayer[i].Left_Elbow.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Left_Elbow.ScreenPos.y }, { temp->mPlayerArray.mPlayer[i].Left_Wrist.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Left_Wrist.ScreenPos.y }, temp->mPlayerArray.mPlayer[i].Left_Wrist.CanSee);
                    DrawBone({ temp->mPlayerArray.mPlayer[i].Right_Elbow.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Right_Elbow.ScreenPos.y }, { temp->mPlayerArray.mPlayer[i].Right_Wrist.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Right_Wrist.ScreenPos.y }, temp->mPlayerArray.mPlayer[i].Right_Wrist.CanSee);
                    DrawBone({ temp->mPlayerArray.mPlayer[i].Pelvis.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Pelvis.ScreenPos.y }, { temp->mPlayerArray.mPlayer[i].Left_Thigh.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Left_Thigh.ScreenPos.y }, temp->mPlayerArray.mPlayer[i].Left_Thigh.CanSee);
                    DrawBone({ temp->mPlayerArray.mPlayer[i].Pelvis.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Pelvis.ScreenPos.y }, { temp->mPlayerArray.mPlayer[i].Right_Thigh.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Right_Thigh.ScreenPos.y }, temp->mPlayerArray.mPlayer[i].Right_Thigh.CanSee);
                    DrawBone({ temp->mPlayerArray.mPlayer[i].Left_Thigh.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Left_Thigh.ScreenPos.y }, { temp->mPlayerArray.mPlayer[i].Left_Knee.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Left_Knee.ScreenPos.y }, temp->mPlayerArray.mPlayer[i].Left_Knee.CanSee);
                    DrawBone({ temp->mPlayerArray.mPlayer[i].Right_Thigh.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Right_Thigh.ScreenPos.y }, { temp->mPlayerArray.mPlayer[i].Right_Knee.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Right_Knee.ScreenPos.y }, temp->mPlayerArray.mPlayer[i].Right_Knee.CanSee);
                    DrawBone({ temp->mPlayerArray.mPlayer[i].Left_Knee.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Left_Knee.ScreenPos.y }, { temp->mPlayerArray.mPlayer[i].Left_Ankle.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Left_Ankle.ScreenPos.y }, temp->mPlayerArray.mPlayer[i].Left_Ankle.CanSee);
                    DrawBone({ temp->mPlayerArray.mPlayer[i].Right_Knee.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Right_Knee.ScreenPos.y }, { temp->mPlayerArray.mPlayer[i].Right_Ankle.ScreenPos.x, temp->mPlayerArray.mPlayer[i].Right_Ankle.ScreenPos.y }, temp->mPlayerArray.mPlayer[i].Right_Ankle.CanSee);
                }

                if (DrawIo[4] || DrawIo[6]) {
                    // 信息
                    if (!temp->mPlayerArray.mPlayer[i].IsBot) {
                        DrawHealth({ temp->mPlayerArray.mPlayer[i].Head.ScreenPos.x - temp->mPlayerArray.mPlayer[i].w, top - 10 }, temp->mPlayerArray.mPlayer[i].w * 2, temp->mPlayerArray.mPlayer[i].Health, temp->mPlayerArray.mPlayer[i].TeamID, temp->mPlayerArray.mPlayer[i].PlayerName);
                    }
                    else {
                        DrawHealth({ temp->mPlayerArray.mPlayer[i].Head.ScreenPos.x - temp->mPlayerArray.mPlayer[i].w, top - 10 }, temp->mPlayerArray.mPlayer[i].w * 2, temp->mPlayerArray.mPlayer[i].Health, temp->mPlayerArray.mPlayer[i].TeamID, "RoBot");
                    }
                }

                if (DrawIo[5]) {
                    // 距离
                    string str = to_string((int)temp->mPlayerArray.mPlayer[i].Distance);
                    str += " m";
                    const char* s = str.c_str();
                    DrawTf.DrawStrokeText(temp->mPlayerArray.mPlayer[i].Head.ScreenPos.x - strlen(s) - temp->mPlayerArray.mPlayer[i].w / 2, bottom + 15, ImVec4{ 255.f / 255.f,200.f / 255.f,0.f / 255.f,255.f / 255.f }, s);
                }
            }
            else {
                if (DrawIo[8]) {
                    tm = 120.f / 255.f;
                    float cameras = temp->matrix[3] * temp->mPlayerArray.mPlayer[i].Pos.x + temp->matrix[7] * temp->mPlayerArray.mPlayer[i].Pos.y + temp->matrix[11] * temp->mPlayerArray.mPlayer[i].Pos.z + temp->matrix[15];
                    if (!temp->mPlayerArray.mPlayer[i].IsBot) {
                        OffScreen(temp->mPlayerArray.mPlayer[i].ScreenPos, cameras, ImColor(arr[temp->mPlayerArray.mPlayer[i].TeamID % length]), NumIo[3] + 20 + temp->mPlayerArray.mPlayer[i].Distance * 0.3);
                    }
                    else {
                        OffScreen(temp->mPlayerArray.mPlayer[i].ScreenPos, cameras, ImColor(255, 255, 255, 255), NumIo[3] + 20 + temp->mPlayerArray.mPlayer[i].Distance * 0.3);
                    }
                }
            }

            if (temp->mPlayerArray.mPlayer[i].IsBot) {
                BotCount++;
            }
            else {
                PlayerCount++;
            }
        }
    }
	
	MaxPlayerCount = AimCount;
	
	// 绘制人数
	if (PlayerCount + BotCount == 0) {
		string str = "SAFE"; 	
		auto textSizes = ImGui::CalcTextSize(str.c_str(), 0, 35);
    	Draw->AddRectFilled({screen_x / 2 - 50, 80}, {screen_x / 2 + 50, 120}, ImColor(35, 35, 35), 5, 0);
		Draw->AddText(NULL, 35, {screen_x / 2 - (textSizes.x / 2), 82}, ImColor(175, 175, 175), str.c_str());
	} else {
		string str = "玩家";
    	str += to_string(PlayerCount);
    	str += "] & Robot[";
    	str += to_string(BotCount);
    	str += "]";
		auto textSizes = ImGui::CalcTextSize(str.c_str(), 0, 35);
    	Draw->AddRectFilled({screen_x / 2 - 170, 80}, {screen_x / 2 - 120, 120}, ImColor(35, 35, 35), 5, 0);
		Draw->AddRectFilled({screen_x / 2 - 120, 80}, {screen_x / 2 - 50, 120}, ImColor(175, 175, 175), 5, 0);
		Draw->AddRectFilled({screen_x / 2 + 50, 80}, {screen_x / 2 + 100, 120}, ImColor(35, 35, 35), 5, 0);
		Draw->AddRectFilled({screen_x / 2 + 100, 80}, {screen_x / 2 + 170, 120}, ImColor(175, 175, 175), 5, 0);
  		Draw->AddText(NULL, 35, {screen_x / 2 - 145 - (ImGui::CalcTextSize(to_string(PlayerCount).c_str(), 0, 35).x / 2), 82}, ImColor(175, 175, 175), to_string(PlayerCount).c_str());
		Draw->AddText(NULL, 35, {screen_x / 2 - 85 - (ImGui::CalcTextSize("玩家", 0, 35).x / 2), 82}, ImColor(35, 35, 35), "玩家");
		Draw->AddText(NULL, 35, {screen_x / 2 + 75 - (ImGui::CalcTextSize(to_string(BotCount).c_str(), 0, 35).x / 2), 82}, ImColor(175, 175, 175), to_string(BotCount).c_str());
		Draw->AddText(NULL, 35, {screen_x / 2 + 135 - (ImGui::CalcTextSize("人机", 0, 35).x / 2), 82}, ImColor(35, 35, 35), "人机");
	}
}

// 渲染主函数
void Rendering(bool *flag) 
{
    ImGuiIO& io = ImGui::GetIO();

    if (display == EGL_NO_DISPLAY) {
        return;
    }
    ImGui_ImplAndroid_NewFrame(init_screen_x, init_screen_y);
    ImGui::NewFrame();
	Draw_Main(ImGui::GetForegroundDrawList());
	if (IsBall) {
    	if (ImGui::Begin("cheat[内核测试]", &IsBall, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse))
    	{
			if (!isSetSize) {
    			ImGui::SetWindowSize({1150, 700});
				isSetSize = true;
			}
			my_window_focused = ImGui::IsWindowFocused();
			SaveWindowPos = ImGui::GetWindowPos();
    		if (ImGui::BeginChild("##左侧菜单标题", ImVec2(220, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NavFlattened));
    		{
    			if (ImGui::Button("主页", ImVec2(200, 90)))
    			{
    				show_ChildMenu = 0;					
    			}
    			if (ImGui::Button("绘图", ImVec2(200, 90)))
    			{
    				show_ChildMenu = 1;					
    			}
    			if (ImGui::Button("自瞄", ImVec2(200, 90)))
    			{
    				show_ChildMenu = 2;				
    			} 
    			if (ImGui::Button("设置", ImVec2(200, 90)))
    			{
    				show_ChildMenu = 3;					
    			}			
    			ImGui::Text("by sanqiu");             
    			ImGui::EndChild();
    		}
    		ImGui::SameLine();
    		if (!show_ChildMenu) {
    			if (ImGui::BeginChild("##绘图", ImVec2(0, 0), false,  ImGuiWindowFlags_NavFlattened)) 
    			{
    				ImGui::Text("- 耗时 %.3fms/真实帧率 (%.1fFPS)", 1000.0f / io.Framerate, io.Framerate);
    				
    				ImGui::TextColored(ImGui::GetStyle().Colors[ImGuiCol_Text], "- 前设备分辨率为: %dx%d\n- 贡献者名单:  \n- 开发者名单: ", screen_x, screen_y);			
    				
    				ImGui::Text("##FPS调整");
    				
    				ImGui::RadioButton("60FPS", &NumIo[12], 60.0f);      
                    ImGui::SameLine();
                    ImGui::RadioButton("90FPS", &NumIo[12], 90.0f);
                    ImGui::SameLine();
                    ImGui::RadioButton("120FPS", &NumIo[12], 120.0f);
    				ImGui::SameLine();
    				ImGui::RadioButton("144FPS", &NumIo[12], 144.0f);
    			    //测试
                    ImGui::Text("so:%p\narray:%p\ncount:%d\nmatrix:%p\n",libUE4, Arrayaddr, Count, Matrix);
    				ImGui::EndChild();
    			}
    		} else if (show_ChildMenu == 1) {
    			if (ImGui::BeginChild("##绘图", ImVec2(0, 0), false,  ImGuiWindowFlags_NavFlattened)) 
    			{
    				
    				ImGui::Text("人物显示");
    							
    				if (ImGui::Button("显示全部",{-1,75}))   
                    {
                   		for (int i = 1; i <= 8; i ++)
    					{
    						DrawIo[i] = true;
    					}
                    }						
    				
                    if (ImGui::Button("隐藏全部",{-1,75}))   
                    {
                    	for (int i = 1; i <= 8; i ++)
    					{
    						DrawIo[i] = false;
    					}
                    }
    										
                    ImGui::Checkbox("人物方框", &DrawIo[1]);
                    ImGui::SameLine();
                    ImGui::Checkbox("人物射线", &DrawIo[2]);
                    ImGui::SameLine();
                    ImGui::Checkbox("人物骨骼", &DrawIo[3]);
                    
                    ImGui::Checkbox("人物信息", &DrawIo[4]);
    				ImGui::SameLine();
                    ImGui::Checkbox("人物距离", &DrawIo[5]);
                    ImGui::SameLine();
                    ImGui::Checkbox("人物血量", &DrawIo[6]);
                    
                    ImGui::Checkbox("绘制雷达", &DrawIo[7]);
                    ImGui::SameLine();
                    ImGui::Checkbox("背敌预警", &DrawIo[8]);           
    				
					ImGui::SliderFloat("·绘制距离调节", &NumIo[17], 100.0f, 400.0f, "%.0f", 1);               
					
    				ImGui::SliderFloat("·雷达X位置调节", &NumIo[1], 0.0f, 2400.0f, "%.1f", 2);                                       
    					
            		ImGui::SliderFloat("·雷达Y位置调节", &NumIo[2], 0.0f, 1080.0f, "%.1f", 3);
    					   						
					ImGui::SliderFloat("·雷达缩放调节", &NumIo[16], 10.0f, 400.0f, "%.0f", 4);
					
    				ImGui::EndChild();
    			}
    		} else if (show_ChildMenu == 2) {
    			if (ImGui::BeginChild("##自瞄", ImVec2(0, 0), false,  ImGuiWindowFlags_NavFlattened)) 
    			{
    				    					
    				ImGui::Text("自瞄");
    					
                    ImGui::Checkbox("自瞄开关", &DrawIo[20]);      
    				ImGui::SameLine();
    				ImGui::Checkbox("调整触摸位置", &DrawIo[21]);
					ImGui::SameLine();
					ImGui::Checkbox("准心射线", &DrawIo[44]);
					
    				ImGui::Checkbox("持续锁定", &DrawIo[30]);				
					ImGui::SameLine();
    				ImGui::Checkbox("人机不瞄", &DrawIo[31]);				
                                          
                    ImGui::RadioButton("开火自瞄", &NumIo[0], 0.0f);      
                    ImGui::SameLine();
                    ImGui::RadioButton("开镜自瞄", &NumIo[0], 1.0f);
                    ImGui::SameLine();
                    ImGui::RadioButton("开火||开镜", &NumIo[0], 2.0f);
					ImGui::SameLine();
                    ImGui::RadioButton("始终自瞄", &NumIo[0], 3.0f);
         
					ImGui::RadioButton("准心优先", &NumIo[21], 0.0f);
					ImGui::SameLine();
                    ImGui::RadioButton("距离优先", &NumIo[21], 1.0f);				
					
    				ImGui::RadioButton("充电口右", &NumIo[10], 0.0f);      
                    ImGui::SameLine();
                    ImGui::RadioButton("充电口左", &NumIo[10], 1.0f);
                   
                    if (ImGui::Combo("自瞄部位", &style_idx, "自动\0头部\0胸部\0臀部\0")) {
                        switch (style_idx) 
                        {
                            case 0:NumIo[8] = 0.0; 
                                break;
                            case 1:NumIo[8] = 1.0; 
                                 break;
                            case 2:NumIo[8] = 2.0; 
                                break;
                            case 3:NumIo[8] = 3.0; 
                                break;
                        }          
                    }
            
                    if (ImGui::CollapsingHeader("基础设置"))
                    {
                        ImGui::SliderFloat("自瞄范围", &NumIo[3], 10.0f, 500.0f, "%.0f", 1);                             
                        ImGui::SliderFloat("自瞄速度", &NumIo[4], 0.1f, 30.0f, "%.2f", 2);         
                        ImGui::SliderFloat("平滑力度", &NumIo[9], 0.1f, 10.0f, "%.1f", 3);                
    					ImGui::SliderFloat("子弹速度", &NumIo[11], 100.0f, 1000.0f, "%.0f", 4);              
    					ImGui::SliderFloat("压枪力度", &NumIo[14], 0.1f, 20.0f, "%.1f", 5);                
                    }
    				
                    if (ImGui::CollapsingHeader("触摸设置"))
                    {
                        ImGui::SliderFloat("触摸范围", &NumIo[7],5.0f,600.0f,"%.0f",2);                                              
                    }					
    				
    				ImGui::EndChild();
    			}			
    		} else {
    			if (ImGui::BeginChild("##设置", ImVec2(0, 0), false,  ImGuiWindowFlags_NavFlattened)) 
    			{
    				if (ImGui::CollapsingHeader("绘制样式设置"))
                    {	
    					ImGui::RadioButton("普通方框", &NumIo[15], 0.0f);      
                    	ImGui::SameLine();
                    	ImGui::RadioButton("立体方框", &NumIo[15], 1.0f);						
    				}
    				
    				if (ImGui::CollapsingHeader("绘制粗细设置"))
                    {						
    					ImGui::SliderFloat("玩家方框", &BoxSize, 0.4f, 10.0f, "%.1f", 1);                             
                        ImGui::SliderFloat("BOT方框", &BotBoxSize, 0.1f, 10.0f, "%.1f", 2);         
                        ImGui::SliderFloat("玩家射线", &LineSize, 0.1f, 10.0f, "%.1f", 3);                
    					ImGui::SliderFloat("BOT射线", &BotLineSize, 0.1f, 10.0f, "%.1f", 4);              
    					ImGui::SliderFloat("玩家骨骼", &BoneSize, 0.1f, 10.0f, "%.1f", 5);   
    					ImGui::SliderFloat("BOT骨骼", &BotBoneSize, 0.1f, 10.0f, "%.1f", 6);                
    				}
        				
    				if (ImGui::CollapsingHeader("绘制颜色设置"))
                    {
        				ImGui::ColorEdit4("玩家方框", (float*)&BoxColor,ImGuiColorEditFlags_NoInputs);	
        				ImGui::SameLine();
        				ImGui::ColorEdit4("BOT方框", (float*)&BotBoxColor,ImGuiColorEditFlags_NoInputs);
						
						ImGui::ColorEdit4("玩家方框背景", (float*)&BoxblackColor,ImGuiColorEditFlags_NoInputs);	
        				ImGui::SameLine();
        				ImGui::ColorEdit4("BOT方框背景", (float*)&BotBoxblackColor,ImGuiColorEditFlags_NoInputs);
        			
        				ImGui::ColorEdit4("玩家射线", (float*)&LineColor,ImGuiColorEditFlags_NoInputs);
        				ImGui::SameLine();
        				ImGui::ColorEdit4("BOT射线", (float*)&BotLineColor,ImGuiColorEditFlags_NoInputs);
        			
        				ImGui::ColorEdit4("玩家骨骼", (float*)&BoneColor,ImGuiColorEditFlags_NoInputs);
        				ImGui::SameLine();
        				ImGui::ColorEdit4("BOT骨骼", (float*)&BotBoneColor,ImGuiColorEditFlags_NoInputs); 
    				}				
        			
        			if (ImGui::Button("保存设置",{-1,75}))   
                    {
                   		NumIoSave("FlyBlueSaveNum");
                    }	
					
        			if (ImGui::Button("重置设置",{-1,75}))   
                    {
                   		CleanData();
                    }	
					
                    if (ImGui::Button("退出",{-1,75}))   
                    {
                    	//*flag = false;
                        exit(0);
                    }	
        					
        			ImGui::EndChild();
    			}
    		}		
    	}
    	ImGui::End();
		
		if (!IsBall) {
			if (ImGui::Begin("##悬浮球", nullptr,ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar |ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
			{
				ImGui::SetWindowPos(SaveWindowPos);
			}
			ImGui::End();
		}
	} else {
		if (ImGui::Begin("##悬浮球", nullptr,ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar |ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
		{
			if (!isSetWindowPos)
			{
				if (SaveWindowPos.x != -520 && SaveWindowPos.y != -520) 
				ImGui::SetWindowPos(SaveWindowPos);
				isSetWindowPos = true;
			}

			// 是否按住编辑等
			my_window_focused = ImGui::IsWindowFocused();
			if (ImGui::IsItemActive())
			{
				if (!isImageDown)
				{
					// 按下
					isImageDown = true;
					suspensionPos = ImGui::GetWindowPos();
				}
			} else if (isImageDown) {
				// 未按下
				isImageDown = false;
				if (suspensionPos.x == ImGui::GetWindowPos().x && ImGui::GetWindowPos().y == suspensionPos.y)
					IsBall = !IsBall;
			}

			static float LogoSize = 300;

			ImGui::Image(imageButton.textureId, ImVec2{LogoSize, LogoSize}, ImVec2{0, 0}, ImVec2{1, 1});

			if (isSetWindowPos)
				SaveWindowPos = ImGui::GetWindowPos();
		}
		ImGui::End();
		
		if (IsBall) {
			if (ImGui::Begin("心动人测试", &IsBall, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse))
    		{
				ImGui::SetWindowPos(SaveWindowPos);
			}
			ImGui::End();
		}
	}
	
	if (DrawIo[20]) {
		if (ImGui::Begin("##自瞄", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
			
			if (!isSetAim) {
				ImGui::SetWindowPos({100, 450});
				isSetAim = true;
			}
			
			if (ImGui::IsItemActive())
			{
				if (!isAimDown)
				{
					// 按下
					isAimDown = true;
					aimsuspensionPos = ImGui::GetWindowPos();
				}
			} else if (isAimDown) {
				// 未按下
				isAimDown = false;
				if (aimsuspensionPos.x == ImGui::GetWindowPos().x && ImGui::GetWindowPos().y == aimsuspensionPos.y)
					DrawIo[40] = !DrawIo[40];
			}
			
			if (DrawIo[40]) {
				ImGui::Image(onButton.textureId, ImVec2{100, 100}, ImVec2{0, 0}, ImVec2{1, 1});
			} else {
				ImGui::Image(offButton.textureId, ImVec2{100, 100}, ImVec2{0, 0}, ImVec2{1, 1});
			}
		}
		ImGui::End();
	}
	
    ImGui::Render();  
	
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    eglSwapBuffers(display, surface);
}
