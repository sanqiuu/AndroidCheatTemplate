#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/system_properties.h>
#include <dlfcn.h>
#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <cassert>
#include <string>
#include <iostream>
#include <sys/mman.h>
#include <dlfcn.h>
#include <math.h>
#include <array>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <sys/inotify.h>
#include <wanbai.hpp>
#include <VecTool.h>
#include <timer.h>
#include <UeTools.h>
#include <socket.h>
#include <hook.h>
#include <draw.h>
#define UNITY
char* g_game_name = "com.sofunny.Sausage";

#ifdef UNITY
char* g_game_lib = "libunity.so";
#else
char* g_game_lib = "libUE4.so";
#endif // !UNITY


void getUTF8(UTF8 * buf, long namepy)
{
    UTF16 buf16[16] = { 0 };
    driver->read(namepy, buf16, 28);
    UTF16 *pTempUTF16 = buf16;
    UTF8 *pTempUTF8 = buf;
    UTF8 *pUTF8End = pTempUTF8 + 32;
    while (pTempUTF16 < pTempUTF16 + 28)
    {
        if (*pTempUTF16 <= 0x007F && pTempUTF8 + 1 < pUTF8End)
        {
            *pTempUTF8++ = (UTF8) * pTempUTF16;
        }
        else if (*pTempUTF16 >= 0x0080 && *pTempUTF16 <= 0x07FF && pTempUTF8 + 2 < pUTF8End)
        {
            *pTempUTF8++ = (*pTempUTF16 >> 6) | 0xC0;
            *pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
        }
        else if (*pTempUTF16 >= 0x0800 && *pTempUTF16 <= 0xFFFF && pTempUTF8 + 3 < pUTF8End)
        {
            *pTempUTF8++ = (*pTempUTF16 >> 12) | 0xE0;
            *pTempUTF8++ = ((*pTempUTF16 >> 6) & 0x3F) | 0x80;
            *pTempUTF8++ = (*pTempUTF16 & 0x3F) | 0x80;
        }
        else
        {
            break;
        }
        pTempUTF16++;
    }
}

FTransform getBone(uintptr_t addr)
{
    FTransform transform;  
    driver->read(addr, reinterpret_cast<void*>(&transform), 4 * 11);
    return transform;
}

bool GetWorld;
float camera;
uintptr_t libUE4;
TempData TempRead;
TempData TempUes;
TempData *tempRead;

uintptr_t Uworld, Uleve, Gname, Objaddr, Arrayaddr, Matrix, oneself, Bone, Human, Mesh, g_lineOfSightTo;
int Count, AddrCount, MyTeam;



using namespace std;

void hack_thread() 
{
    bool ls_suc = driver->initialize((char*)g_game_name);
	if (!ls_suc) {
		cout << "[-] Failed to Game." << endl;
		exit(0);
	}
	
	
	
    memset(matrix, 0, 16);
	
	libUE4 = driver->getModuleBase(g_game_lib);
	
	while (!libUE4)
	{
		libUE4 = driver->getModuleBase(g_game_lib);
		sleep(1);
	}
	
    tempRead = &TempUes;

	if (screen_x > screen_y) {
		Higtih = screen_y / 2;
		Widtih = screen_x / 2;
	} else {
		Higtih = screen_x / 2;
		Widtih = screen_y / 2;
	}
	
	for (;;) {	    	
		TempRead.mPlayerArray.Count = AddrCount;
		*tempRead = TempRead;
		
		AddrCount = 0;
#ifdef UNITY
        Arrayaddr;
        Count;
#else
        Uworld = driver->read<uintptr_t>(libUE4 + 0xCD69ED0);
        Uleve = driver->read<uintptr_t>(Uworld + 0x90) + 0xa0;

        Arrayaddr = driver->read<uintptr_t>(Uleve);

        Count = driver->read<int>(Uleve + 0x8);
#endif // !UNITY
		
		
		if (Count <= 0 || Count > 2000) {	
        	continue;           // 防止数组获取错误	
		}
		
		Matrix = driver->read<uintptr_t>(driver->read<uintptr_t>(libUE4 + 0xCD3DBB8) + 0x20) + 0x270;	
		
		oneself = driver->read<uintptr_t>(driver->read<uintptr_t>(driver->read<uintptr_t>(driver->read<uintptr_t>(Uworld + 0x98) + 0x88) + 0x30) + 0x2ae8);
		MyTeam = driver->read<int>(oneself + 0xa48);		
		
	    TempRead.MyWeapon = driver->read<int>(driver->read<uintptr_t>(oneself + 0x29c8) + 0xaf8);
		driver->read(driver->read<uintptr_t>(oneself + 0x268) + 0x1b0, &TempRead.MyPos, 12);				
		
		driver->read(Matrix, &matrix, 16 * 4);	
		driver->read(Matrix, &TempRead.matrix, 16 * 4);
		
		TempRead.Fov = driver->read<float>(driver->read<uintptr_t>(driver->read<uintptr_t>(oneself + 0x3e40) + 0x5c0) + 0x5d4);  // 自身FOV   
		TempRead.IsAiming = driver->read<int>(oneself + 0x13f8);
        TempRead.IsFiring = driver->read<int>(oneself + 0x1c48);	
				
		for (int i = 0; i < Count; i ++)
		{		
			Objaddr = driver->read<uintptr_t>(Arrayaddr + 8 * i);  // 遍历数量次数
					
			if (Objaddr == 0 || Objaddr <= 0x10000000 || Objaddr % 4 != 0 || Objaddr >= 0x10000000000)
            	continue;
			
			if (driver->read<float>(Objaddr + 0x2ad0) != 479.5) {
				continue;
			}
			
			uintptr_t object = driver->read<uintptr_t>(Objaddr + 0x268);
			
			if (object <= 0xffff || object == 0 || object <= 0x10000000 || object % 4 != 0 || object >= 0x10000000000)
            	continue;
				
			auto *Players = &TempRead.mPlayerArray.mPlayer[AddrCount];
			
			driver->read(object + 0x1b0, &Players->Pos, 12);
			if (Players->Pos.x == 0 || Players->Pos.y == 0 || Players->Pos.x == 0) {
         		continue;
     		}
			
			int State = driver->read<int>(Objaddr + 0x12c8);
        	if (State == 1048576 || State == 1048592)
           		continue;
        
        	Players->TeamID = driver->read<int>(Objaddr + 0xa48);
        	if (Players->TeamID == MyTeam || Players->TeamID < 1)
            	continue;
            
        	float MinHealth = driver->read<float>(Objaddr + 0xd60);
        	float MaxHealth = driver->read<float>(Objaddr + 0xd68);
        	Players->Health = (MinHealth / MaxHealth) * 100;
        	if (Players->Health > 100)
           		continue;
				
			if (driver->read<uintptr_t>(Objaddr + 0xf60)) {
            	driver->read(driver->read<uintptr_t>(Objaddr + 0xf60) + 0x170, &Players->Predict, sizeof(Players->Predict)); // 载具向量
      	 	} else {
            	driver->read(object + 0x200, &Players->Predict, sizeof(Players->Predict)); // 敌人向量
        	}              		
			
			GetDistance(Players->Pos, TempRead.MyPos, &Players->Distance);
			WorldToScreen(&Players->ScreenPos, &camera, &Players->w, Players->Pos);			
						
			Players->camera = camera;
			
			Players->IsBot = driver->read<int>(Objaddr + 0xa64);
			
			getUTF8(Players->PlayerName, driver->read<uintptr_t>(Objaddr + 0x9d0));
			
			Mesh = driver->read<uintptr_t>(Objaddr + 0x5b8);		
			
			if (Mesh <= 0xffff) 
				continue;
        	
        	Human = Mesh + 0x1a0;	
			
        	Bone = driver->read<uintptr_t>(Mesh + 0x6d0);		
			
			if (Bone <= 0xffff) 
				continue;
			
			FTransform meshtrans = getBone(Human);
            FMatrix c2wMatrix = TransformToMatrix(meshtrans);		
			
            /* 头部 */
            FTransform headtrans = getBone(Bone + 6 * 48);
            FMatrix boneMatrix = TransformToMatrix(headtrans);
            Players->Head.Pos = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
            Players->Head.Pos.z += 7; /* 脖子长度 */   			
            Players->Head.ScreenPos = WorldToScreen(Players->Head.Pos, matrix, camera);
			
            /* 胸部 */
            FTransform chesttrans = getBone(Bone + 5 * 48);
            FMatrix boneMatrix1 = TransformToMatrix(chesttrans);
            Players->Chest.Pos = MarixToVector(MatrixMulti(boneMatrix1, c2wMatrix));
            Players->Chest.ScreenPos = WorldToScreen(Players->Chest.Pos, matrix, camera);
            
			/* 盆骨 */
            FTransform pelvistrans = getBone(Bone + 1 * 48);
            FMatrix boneMatrix2 = TransformToMatrix(pelvistrans);
            Players->Pelvis.Pos = MarixToVector(MatrixMulti(boneMatrix2, c2wMatrix));
            Players->Pelvis.ScreenPos = WorldToScreen(Players->Pelvis.Pos, matrix, camera);
			
            /* 左肩膀 */
            FTransform lshtrans = getBone(Bone + 12 * 48);
            FMatrix boneMatrix3 = TransformToMatrix(lshtrans);
            Players->Left_Shoulder.Pos = MarixToVector(MatrixMulti(boneMatrix3, c2wMatrix));
            Players->Left_Shoulder.ScreenPos = WorldToScreen(Players->Left_Shoulder.Pos, matrix, camera);
			
            /* 右肩膀 */
            FTransform rshtrans = getBone(Bone + 33 * 48);
            FMatrix boneMatrix4 = TransformToMatrix(rshtrans);
            Players->Right_Shoulder.Pos = MarixToVector(MatrixMulti(boneMatrix4, c2wMatrix));
            Players->Right_Shoulder.ScreenPos = WorldToScreen(Players->Left_Shoulder.Pos, matrix, camera);
			
            /* 左手肘 */
            FTransform lelbtrans = getBone(Bone + 13 * 48);
            FMatrix boneMatrix5 = TransformToMatrix(lelbtrans);
            Players->Left_Elbow.Pos = MarixToVector(MatrixMulti(boneMatrix5, c2wMatrix));
            Players->Left_Elbow.ScreenPos = WorldToScreen(Players->Left_Elbow.Pos, matrix, camera);
			
            /* 右手肘 */
            FTransform relbtrans = getBone(Bone + 34 * 48);
            FMatrix boneMatrix6 = TransformToMatrix(relbtrans);
            Players->Right_Elbow.Pos = MarixToVector(MatrixMulti(boneMatrix6, c2wMatrix));
            Players->Right_Elbow.ScreenPos = WorldToScreen(Players->Right_Elbow.Pos, matrix, camera);
			
            /* 左手腕 */
            FTransform lwtrans = getBone(Bone + 64 * 48);
            FMatrix boneMatrix7 = TransformToMatrix(lwtrans);
            Players->Left_Wrist.Pos = MarixToVector(MatrixMulti(boneMatrix7, c2wMatrix));
            Players->Left_Wrist.ScreenPos = WorldToScreen(Players->Left_Wrist.Pos, matrix, camera);
			
            /* 右手腕 */
            FTransform rwtrans = getBone(Bone + 63 * 48);
            FMatrix boneMatrix8 = TransformToMatrix(rwtrans);
            Players->Right_Wrist.Pos = MarixToVector(MatrixMulti(boneMatrix8, c2wMatrix));
            Players->Right_Wrist.ScreenPos = WorldToScreen(Players->Right_Wrist.Pos, matrix, camera);
			
            /* 左大腿 */
            FTransform Llshtrans = getBone(Bone + 53 * 48);
            FMatrix boneMatrix9 = TransformToMatrix(Llshtrans);
            Players->Left_Thigh.Pos = MarixToVector(MatrixMulti(boneMatrix9, c2wMatrix));
            Players->Left_Thigh.ScreenPos = WorldToScreen(Players->Left_Thigh.Pos, matrix, camera);
			
            /* 右大腿 */
            FTransform Lrshtrans = getBone(Bone + 57 * 48);
            FMatrix boneMatrix10 = TransformToMatrix(Lrshtrans);
            Players->Right_Thigh.Pos = MarixToVector(MatrixMulti(boneMatrix10, c2wMatrix));
            Players->Right_Thigh.ScreenPos = WorldToScreen(Players->Right_Thigh.Pos, matrix, camera);
			
            /* 左膝盖 */
            FTransform Llelbtrans = getBone(Bone + 54 * 48);
            FMatrix boneMatrix11 = TransformToMatrix(Llelbtrans);
            Players->Left_Knee.Pos = MarixToVector(MatrixMulti(boneMatrix11, c2wMatrix));
            Players->Left_Knee.ScreenPos = WorldToScreen(Players->Left_Knee.Pos, matrix, camera);
			
            /* 右膝盖 */
            FTransform Lrelbtrans = getBone(Bone + 58 * 48);
            FMatrix boneMatrix12 = TransformToMatrix(Lrelbtrans);
            Players->Right_Knee.Pos = MarixToVector(MatrixMulti(boneMatrix12, c2wMatrix));
            Players->Right_Knee.ScreenPos = WorldToScreen(Players->Right_Knee.Pos, matrix, camera);
			
            /* 左脚腕 */
            FTransform Llwtrans = getBone(Bone + 55 * 48);
            FMatrix boneMatrix13 = TransformToMatrix(Llwtrans);
            Players->Left_Ankle.Pos = MarixToVector(MatrixMulti(boneMatrix13, c2wMatrix));
            Players->Left_Ankle.ScreenPos = WorldToScreen(Players->Left_Ankle.Pos, matrix, camera);
			
            /* 右脚腕 */
            FTransform Lrwtrans = getBone(Bone + 59 * 48);
            FMatrix boneMatrix14 = TransformToMatrix(Lrwtrans);
            Players->Right_Ankle.Pos = MarixToVector(MatrixMulti(boneMatrix14, c2wMatrix));
            Players->Right_Ankle.ScreenPos = WorldToScreen(Players->Right_Ankle.Pos, matrix, camera);
			
			
			
			AddrCount ++; // 遍历人物数量
		}
		
		usleep(1000);
	}
	
}
