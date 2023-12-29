#include <draw.h>
#include ".\Game\include.h"

void *handle;// 动态库方案
EGLDisplay display = EGL_NO_DISPLAY;
EGLConfig config;
EGLSurface surface = EGL_NO_SURFACE;
ANativeWindow *native_window;
ANativeWindow *(*createNativeWindow)(const char *surface_name, uint32_t screen_width, uint32_t screen_height, bool author);
EGLContext context = EGL_NO_CONTEXT;

Screen full_screen;
int Orientation = 0;
int screen_x = 0, screen_y = 0;
int init_screen_x = 0, init_screen_y = 0;
bool g_Initialized = false;
pid_t 进程;
uintptr_t 基址头;

string exec(string command) {
    char buffer[128];
    string result = "";
    // Open pipe to file
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "popen failed!";
    }
    // read till end of process:
    while (!feof(pipe)) {
        // use buffer to read and add to result
        if (fgets(buffer, 128, pipe) != nullptr){
            result += buffer;
        }
    }
    pclose(pipe);
    return result;
}

int init_egl(int _screen_x, int _screen_y, bool log){
    void* sy = get_createNativeWindow(); // 适配9-13安卓版本
    createNativeWindow = (ANativeWindow *(*)(const char *, uint32_t, uint32_t, bool))(sy);
    native_window = createNativeWindow("Ssage", _screen_x, _screen_y, false);
    ANativeWindow_acquire(native_window);
    
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        printf("eglGetDisplay error=%u\n", glGetError());
        return -1;
    }
    if(log){
        printf("eglGetDisplay ok\n");
    }
    if (eglInitialize(display, 0, 0) != EGL_TRUE) {
        printf("eglInitialize error=%u\n", glGetError());
        return -1;
    }
    if(log){
        printf("eglInitialize ok\n");
    }
    EGLint num_config = 0;
    const EGLint attribList[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE, 5,   //-->delete
            EGL_GREEN_SIZE, 6,  //-->delete
            EGL_RED_SIZE, 5,    //-->delete
            EGL_BUFFER_SIZE, 32,  //-->new field
            EGL_DEPTH_SIZE, 16,
            EGL_STENCIL_SIZE, 8,
            EGL_NONE
    };
    if (eglChooseConfig(display, attribList, nullptr, 0, &num_config) != EGL_TRUE) {
        printf("eglChooseConfig  error=%u\n", glGetError());
        return -1;
    }
    if(log){
        printf("num_config=%d\n", num_config);
    }
    if (!eglChooseConfig(display, attribList, &config, 1, &num_config)) {
        printf("eglChooseConfig  error=%u\n", glGetError());
        return -1;
    }
    if(log){
        printf("eglChooseConfig ok\n");
    }
    EGLint egl_format;
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &egl_format);
    ANativeWindow_setBuffersGeometry(native_window, 0, 0, egl_format);
    const EGLint attrib_list[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, attrib_list);
    if (context == EGL_NO_CONTEXT) {
        printf("eglCreateContext  error = %u\n", glGetError());
        return -1;
    }
    if(log){
        printf("eglCreateContext ok\n");
    }
    surface = eglCreateWindowSurface(display, config, native_window, nullptr);
    if (surface == EGL_NO_SURFACE) {
        printf("eglCreateWindowSurface  error = %u\n", glGetError());
        return -1;
    }
    if(log){
        printf("eglCreateWindowSurface ok\n");
    }
    if (!eglMakeCurrent(display, surface, surface, context)) {
        printf("eglMakeCurrent  error = %u\n", glGetError());
        return -1;
    }
    if(log){
        printf("eglMakeCurrent ok\n");
    }
    return 1;
}



void screen_config(){
    std::string window_size = exec("wm size");
    sscanf(window_size.c_str(),"Physical size: %dx%d",&screen_x, &screen_y);
    full_screen.ScreenX = screen_x;
    full_screen.ScreenY = screen_y;
    std::thread *orithread = new std::thread([&] {
        while(true){
            Orientation = atoi(exec("dumpsys display | grep 'mCurrentOrientation' | cut -d'=' -f2").c_str());
			if(Orientation == 0 || Orientation == 2){
                screen_x = full_screen.ScreenX;
                screen_y = full_screen.ScreenY;
            }
            if(Orientation == 1 || Orientation == 3){
                screen_x = full_screen.ScreenY;
                screen_y = full_screen.ScreenX;
            }
            std::this_thread::sleep_for(0.5s);
        }
    });
    orithread->detach();
}

void NumIoSave(const char *name){
    if (numSave == nullptr) {
        string SaveFile = "/data";
        SaveFile += "/";
        SaveFile += name;
        numSave = fopen(SaveFile.c_str(), "wb+");
    }
    fseek(numSave, 0, SEEK_SET);
    fwrite(NumIo, sizeof(float) * 50, 1, numSave);
	
    fflush(numSave);
    fsync(fileno(numSave));
}




void NumIoLoad(const char *name){
    if (numSave == nullptr) {
        string SaveFile = "/data";
        SaveFile += "/";
        SaveFile += name;
        numSave = fopen(SaveFile.c_str(), "rb+");
    }
    if (numSave != nullptr) {
        fseek(numSave, 0, SEEK_SET);
        fread(NumIo, sizeof(float) * 50, 1, numSave);
		
    } else {             
NumIo[1] = 300.0f;                                                  
    NumIo[2] = 400.0f;
 	NumIo[3] = 340.0f;
    NumIo[4] = 20.0f;
  	NumIo[6] = 1500.0f;
    NumIo[5] = 650.0f;
    NumIo[7] = 300.0f;
    NumIo[8] = 2.0f;
    NumIo[9] = 10.0f; 
	NumIo[11] = 500.0f;
	NumIo[13] = 0.0f;
	NumIo[14] = 105.0f;
	NumIo[15] = 105.0f;
    }
}
void ImGuiMenustyle(){
	NumIo[1] = 300.0f;                                                  
    NumIo[2] = 400.0f;
 	NumIo[3] = 340.0f;
    NumIo[4] = 20.0f;
  	NumIo[6] = 1500.0f;
    NumIo[5] = 650.0f;
    NumIo[7] = 300.0f;
    NumIo[8] = 2.0f;
    NumIo[9] = 10.0f; 
	NumIo[11] = 1000.0f;
	NumIo[13] = 0.0f;
	NumIo[14] = 105.0f;
	NumIo[15] = 105.0f;
}


	
void ImGui_init(){
    if (g_Initialized){
        return;
    }
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;
	ImGui::StyleColorsLight();
    ImGui_ImplAndroid_Init(native_window);
    ImGui_ImplOpenGL3_Init("#version 300 es");
	Android_LoadSystemFont();
    ImGui::GetStyle().ScaleAllSizes(3.0f);
    g_Initialized = true;
	
}

void DrawInit(){
py = screen_x / 2;
px = screen_y / 2;
//进程 = 驱动->获取进程((char*)g_game_name);
//驱动->初始化(进程);
//基址头 = 驱动->获取基址头((char*)g_game_dll);
ImGuiMenustyle();
ColorInitialization();
}




struct AimStruct{
    Vector3A ObjAim;  
	Vector3A MyObjAim;
    Vector3A AimMovement;  
    float ScreenDistance = 0;
    float WodDistance = 0; 
}Aim[100];





int findminat()
{
    float min = NumIo[3];
    int minAt = 999;
    for (int i = 0; i < MaxPlayerCount; i++)
    {
        if (Aim[i].ScreenDistance < min && Aim[i].ScreenDistance != 0)
        {
            min = Aim[i].ScreenDistance;
            minAt = i;
        }
    }
    if (minAt == 999)
    {
        Gmin = -1;
        return -1;
    }
    Gmin = minAt;   
    WorldDistance = Aim[minAt].WodDistance;
    return minAt;
}


float GetPitch(float Fov) 
{
    if (Fov > 75 && Fov <= 130)          // 不开镜
    {
        return (0.7f/30)*NumIo[4];
    }
    else if (Fov == 70 || Fov == 75)    // 机瞄 ，侧瞄
    {     
        return (1.8f/30)*NumIo[4];       
    }
    else if (Fov == 55 || Fov == 60)    // 红点 ，全息
    {
        return (1.8f/30)*NumIo[4];
    }   
    else if ((int)Fov == 44)    // 2倍
    {
        return (5.3f/30)*NumIo[4];
    }
    else if ((int)Fov == 26)    // 3倍
    {
        return (7.2f/30)*NumIo[4];
    }
    else if ((int)Fov == 20)    // 4倍
    {
        return (8.6f/30)*NumIo[4];
    }
    else if ((int)Fov == 13)    // 6倍
    {       
        return (14.1f/30)*NumIo[4];
    }
	return (2.5f/30)*NumIo[4];   // 8倍
}

void AimBotAuto(){   
    bool isDown = false;
    // 是否按下触摸
    double leenx = NumIo[14];
    // x轴速度
    double leeny = NumIo[15];
    // y轴速度     
    double de = 1.5f;
    // 顶部不可触摸区域
   
    double tx = 0, ty = 0;
    // 触摸点位置

    float SpeedMin = 2.0f;
    // 临时触摸速度

    double w = 0.0f, h = 0.0f, cmp = 0.0f;
    // 宽度 高度 正切

    double ScreenX = screen_x, ScreenY = screen_y;
    // 分辨率(竖屏)PS:滑屏用的坐标是竖屏状态下的

    double ScrXH = ScreenX / 2.0f;
    // 一半屏幕X

    double ScrYH = ScreenY / 2.0f;
    // 一半屏幕X

    static float TargetX = 0;
    static float TargetY = 0;
    // 触摸目标位置
    
    Vector3A obj;
    
    float NowCoor[3];
   
	Touch_Init(&screen_x,&screen_y);	
	
    while (1)
    {
        
        if (!DrawIo[20])
        {           
            if (isDown == true)
            {
                tx = NumIo[5], ty = NumIo[6];
                // 恢复变量 
                Touch_Up(8);
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
            if (isDown == true)
            {
                tx = NumIo[5], ty = NumIo[6];
                // 恢复变量 
                Touch_Up(8);
                // 抬起
                isDown = false;
            }
            usleep(NumIo[9] * 1000);
            continue;
        }
		
	
		
		
        float ToReticleDistance = Aim[Gmin].ScreenDistance;              
        float FlyTime = WorldDistance / NumIo[11];
        float DropM = 500.0f * FlyTime * FlyTime;
		//
		float Fov = 驱动->读取浮点数(驱动->读取指针(驱动->读取指针(自身结构体  + 0x430) + 0x490) + 0x410);
        int Firing = 驱动->读取整数(自身结构体 + 0x14a0);
        int Aiming = 驱动->读取整数(自身结构体 + 0x1dc8);
		
		float yq = GetPitch(Fov);
		
	    NowCoor[0] = Aim[Gmin].ObjAim.X;
        NowCoor[1] = Aim[Gmin].ObjAim.Y;
        NowCoor[2] = Aim[Gmin].ObjAim.Z;
        obj.X = NowCoor[0] + (FlyTime /0.2 * diff[0]);
        obj.Y = NowCoor[1] + (FlyTime /0.2 * diff[1]);
        obj.Z = NowCoor[2] + (FlyTime /0.2 * diff[2]) + DropM;
        float cameras = matrix[3] * obj.X + matrix[7] * obj.Y + matrix[11] * obj.Z + matrix[15];        
        
		
        Vector2A vpvp = WorldToScreen(obj,matrix,cameras);             
		
		float AimDs = sqrt(pow(px - vpvp.X, 2) + pow(py - vpvp.Y, 2));
        zm_y = vpvp.X;     
        zm_x = ScreenX - vpvp.Y;        
		
        if (zm_x <= 0 || zm_x >= ScreenX || zm_y <= 0 || zm_y >= ScreenY)
        {          
            if (isDown == true)
            {
                tx = NumIo[5], ty = NumIo[6];
                // 恢复变量 
                Touch_Up(8);
                // 抬起
                isDown = false;
            }
            usleep(NumIo[9] * 1000);
            continue;
        }

        if (ToReticleDistance <= NumIo[3])
        {                          
        switch ((int)NumIo[0])
            {
                case 0:
                    if (Firing != 1)
                    {
                        if (isDown == true)
                        {
                        
                            tx = NumIo[5], ty = NumIo[6];
                            // 恢复变量
                            Touch_Up(8);
                            isDown = false;
                        }                      
                        usleep(NumIo[9] * 1000);
                        continue;
                    }
                break;
                case 1:
                    if (Aiming != 256)
                    {
                        if (isDown == true)
                        {
                            tx = NumIo[5], ty = NumIo[6];
                            // 恢复变量
                            Touch_Up(8);
                            isDown = false;
                        }
                        usleep(NumIo[9] * 1000);
                        continue;
                    }
                break;
                case 2:
                    if (Firing != 1 && Aiming != 256)
                    {
                        if (isDown == true)
                        {
                            tx = NumIo[5], ty = NumIo[6];
                            // 恢复变量
                            Touch_Up(8);
                            isDown = false;
                        }
                        usleep(NumIo[9] * 1000);
                        continue;
                    }
                break;
            }         
                
                   
            if (isDown == false)
            {
				if (NumIo[10] == 0.0f)
                Touch_Down(8,(int)tx, (int)ty);
                isDown = true;
            }
			
			float Aimspeace = NumIo[4];
			if (AimDs < 1)
				Aimspeace = NumIo[4] / 0.09;		
			else if (AimDs < 2)
				Aimspeace = NumIo[4] / 0.11;		
			else if (AimDs < 3)
				Aimspeace = NumIo[4] / 0.12;	
			else if (AimDs < 5)
				Aimspeace = NumIo[4] / 0.15;		
			else if (AimDs < 10)
				Aimspeace = NumIo[4] / 0.25;		
			else if (AimDs < 15)
				Aimspeace = NumIo[4] / 0.4;		
			else if (AimDs < 20)
				Aimspeace = NumIo[4] / 0.5;
			else if (AimDs < 25)
				Aimspeace = NumIo[4] / 0.6;		
			else if (AimDs < 30)
				Aimspeace = NumIo[4] / 0.7;
	        else if (AimDs < 40)
				Aimspeace = NumIo[4] / 0.75;
			else if (AimDs < 50)
				Aimspeace = NumIo[4] / 0.8;
			else if (AimDs < 60)
				Aimspeace = NumIo[4] / 0.85;
			else if (AimDs < 70)
				Aimspeace = NumIo[4] / 0.9;
			else if (AimDs < 80)
				Aimspeace = NumIo[4] / 0.95;	
			else if (AimDs < 90)
				Aimspeace = NumIo[4] / 1.0;		
			else if (AimDs < 100)
				Aimspeace = NumIo[4] / 1.05;
			else if (AimDs < 150)
				Aimspeace = NumIo[4] / 1.25;
			else if (AimDs < 200)
				Aimspeace = NumIo[4] / 1.5;
			else
				Aimspeace = NumIo[4] / 1.55;					

            if (zm_x > ScrXH)
            {
                TargetX = -(ScrXH - zm_x);
                TargetX /= Aimspeace;                                       
                if (TargetX + ScrXH > ScrXH * 2)
                    TargetX = 0;
            }
            if (zm_x < ScrXH)
            {
                TargetX = zm_x - ScrXH;
                TargetX /= Aimspeace;             
                if (TargetX + ScrXH < 0)
                    TargetX = 0;
            }
            if (zm_y > ScrYH)
            {
                TargetY = -(ScrYH - zm_y);
                TargetY /= Aimspeace;              
                if (TargetY + ScrYH > ScrYH * 2)
                    TargetY = 0;
            }
            if (zm_y < ScrYH)
            {
                TargetY = zm_y - ScrYH;
                TargetY /= Aimspeace;              
                if (TargetY + ScrYH < 0)
                    TargetY = 0;
            }
            
            if (TargetY >= 35 || TargetX >= 35 || TargetY <= -35 || TargetX <= -35)
            {
                if (isDown == true)
                {
                    tx = NumIo[5], ty = NumIo[6];
                    // 恢复变量
                    Touch_Up(8);
                    isDown = false;
                }
                usleep(NumIo[9] * 1000);
                continue;
            }           		
            tx += TargetX;
            ty += TargetY;		
			if(Firing == 1)
				tx -= yq;
            if (tx >= NumIo[5] + NumIo[7] || tx <= NumIo[5] - NumIo[7]
                || ty >= NumIo[6] + NumIo[7] || ty <= NumIo[6] - NumIo[7])
            {
                // 只要滑屏达到了边界，直接还原至中心
                tx = NumIo[5], ty = NumIo[6];
                // 恢复变量
                Touch_Up(8);
                // 抬起            
				if (NumIo[10] == 0.0f)
                	Touch_Down(8,(int)tx, (int)ty);
            }         		           
			if (NumIo[10] == 0.0f)
           		Touch_Move(8,(int)tx, (int)ty);
        }
        else
        {          
            if (isDown == true)
            {
                tx = NumIo[5], ty = NumIo[6];
                // 恢复变量 
                Touch_Up(8);
                // 抬起
                isDown = false;
            }
        }
        usleep(NumIo[9] * 1000);
    }
}



ImColor health_Color = ImColor(ImVec4(255/255.f, 255/255.f, 258/255.f, 0.85f));
ImColor halflackColor = ImColor(ImVec4(0/255.f, 0/255.f, 0/255.f, 0.3f));
ImColor whiteColor = ImColor(ImVec4(255/255.f, 255/255.f, 258/255.f, 1.0f));
ImColor bdzrColor = ImColor(225, 0, 0, 100);





			
static ImVec4 Colbox1= ImVec4(255/255.f,255/255.f,255/255.f,255.f/255.f);
static ImVec4 Colbox2 = ImVec4(255/255.f,255/255.f,255/255.f,255.f/255.f);
static ImVec4 Colbox3 = ImVec4(255/255.f,255/255.f,255/255.f,255.f/255.f);
static ImVec4 Colbox4 = ImVec4(1.0f, 1.0f, 0.0f,1.0f);//Yellow
static bool show_draw_Rect = false;
static bool show_draw_Skeleton = false;
static bool show_draw_Health = false;
static bool show_draw_Line = false;
static bool show_draw_distance = false;
static bool show_draw_human = false;
static bool show_draw_Camp = false;
static bool show_draw_name = false;
static bool show_draw_zz = false;
char 输出帧率字[32];
char 国家[32];


float top1;
float top;
void DrawPlayer(){
   	const ImU32 方框 = ImColor(Colbox1);
    const ImU32 射线 = ImColor(Colbox2);
    const ImU32 骨骼 = ImColor(Colbox3);
    const ImU32 zmcolor = ImColor(Colbox4);//自瞄圈
    // 自瞄圈圈
    /*
	if (DrawIo[20]){         
	   ImGui::GetForegroundDrawList()->AddRect({px - NumIo[3], py - NumIo[3]}, {px + NumIo[3], py + NumIo[3]}, zmcolor, 0, 1.3f);
   
	   }
   */
   long int	zmAddr = 驱动->读取浮点数(驱动->读取指针(自身结构体  + 0xE8) + 0x388); //FOV
		long int CameraAddrx = zmAddr + 0x1ebc;//本人相机视角X
		long int CameraAddry = zmAddr + 0x1ec0;//本人相机视角Y
		long int CameraAddrz = zmAddr + 0x1ec4;//本人相机视角Z
	


	    long int Temporary = 0; // 临时对象
	    D3DVector aimatPlace, zszb;
	/*
		if (DrawIo[60] && !人机判断 || !DrawIo[60])
			if (DrawIo[61] && !Health || !DrawIo[61])
		*/
		  {
	   	if (aimbotmode == 1) {
			aimatPlace = getBoneXYZ(human, Bone, 4);//胸部
		}
		else if (aimbotmode == 2) {
			aimatPlace = getBoneXYZ(human, Bone, 5.5);//头部
		}
		else if (aimbotmode == 3) {
			aimatPlace = getBoneXYZ(human, Bone, 1);//腰部
		}
		}
   
   if (DrawIo[20])
       {//下面就是范围圈
	//Draw->AddCircle({px, py}, NumIo[3],ImColor(255,255,0,255),0,2.5f);
	ImGui::GetForegroundDrawList()->AddRect({px - NumIo[3], py - NumIo[3]}, {px + NumIo[3], py + NumIo[3]}, zmcolor, 0, 1.3f);
   
	float d2d = getQuasicentered(px, py, r_x, r_y);//屏幕座标
			if (d2d < NumIo[3]) {//过滤范围外的敌人
				//continue;
	if(d2d <= NumIo[3] )//判断准心最近的敌人
			{
				
				
				Temporary = Objaddr;
			}		
		

		        int Firing = 驱动->读取整数(驱动->读取指针(驱动->读取指针(自身结构体 + 0x208) + 0x58) + 0x748);//开火
		if(Firing == 1)
	    {
            float hp = 驱动->读取浮点数(Temporary + 0x13d8);
			if(hp >= 0 && Temporary != 0) {				
				驱动->写入F类内存(CameraAddrx, aimatPlace.X);
				驱动->写入F类内存(CameraAddry, aimatPlace.Y);
				驱动->写入F类内存(CameraAddrz, aimatPlace.Z);
	    }
	    }
	    }
	}
       
    if (DrawIo[21]){
       ImGui::GetForegroundDrawList()->AddRectFilled({0,0}, {px*2, py*2},ImColor(0,0,0,110));
       std::string ssf;  
       ssf += "触摸位置";
       auto textSize = ImGui::CalcTextSize(ssf.c_str(), 0, 25);
       ImGui::GetForegroundDrawList()->AddRectFilled({NumIo[6] - NumIo[7]/2, py*2 - NumIo[5] + NumIo[7]/2}, {NumIo[6] + NumIo[7]/2, py*2 - NumIo[5] - NumIo[7]/2},ImColor(255,0,0,120)); 
	   ImGui::GetForegroundDrawList()->AddText(NULL,32,{NumIo[6] - (textSize.x / 2),py*2 - NumIo[5]+50},ImColor(255,255,255),ssf.c_str());                                                   
    }
	/*	Uworld = getZZ(libbase + 0xD83E758);//游戏世界
	Uleve = getZZ(Uworld + 0x90);
    Arrayaddr = getZZ(Uleve + 0xA0); // 对象阵列
   Count = getDword(Uleve + 0xA8);
	
	MySelf = getZZ(getZZ(getZZ(getZZ(getZZ(libbase + 0xD83E758) + 0x98) + 0x88)+0x30)+0x2d50);
	
	Myteam = getDword(MySelf + 0xA60); // 自身队伍编号
*/
	long int Matrix = 驱动->读取指针(驱动->读取指针(基址头 +0xD80FB08) + 0x20) + 0x270; //矩阵
	long int Uworld = 驱动->读取指针(基址头 + 0xD83E758);  // 读取游戏世界
	long int Uleve = 驱动->读取指针(Uworld + 0x90);
	long int 列阵 = 驱动->读取指针(Uleve + 0xA0);
	int 数量 = 驱动->读取整数(Uleve + 0xA8);
	//0xD83E758) + 0x98)+0x88)+0x30)+0x2d50);
	自身结构体 = 驱动->读取指针(驱动->读取指针(驱动->读取指针(驱动->读取指针(驱动->读取指针(基址头 + 0xD83E758)+0x98)+0x88)+0x30)+0x2d50);
	int 自己阵营 = 驱动->读取整数(自身结构体 + 0xA60);//自身队编
    驱动->读取(Matrix, matrix, 16 * 4);
    PlayerCount = 0; 
	BotCount = 0;
    AimCount = 0;
    AimObjCount = 0;
    for (int i = 0; i < 数量; i++){
        long int 结构体 = 驱动->读取指针(列阵 + 8 * i);  // 遍历数量次数
		
		if (结构体 <= 0x10000000 || 结构体 % 4 != 0 || 结构体 >= 0x10000000000){
		 continue;
		 }
		 
		if (!isMemoryTrap(结构体)){//缺页地址停止循环，正常地址往下继续执行
          continue;
		 }

		//玩家国家
        getUTF8(国家, 驱动->读取指针(结构体 + 0x8c0));

	    //玩家名称
        getUTF8(输出帧率字, 驱动->读取指针(结构体 + 0x9E0));
			
		//人机判断
		 int 人机判断 = 驱动->读取整数(结构体 + 0xA7c);
		 
		//去除自身及队友
	    int 敌人阵营 = 驱动->读取整数(结构体 + 0xA60);
		if (敌人阵营 == 自己阵营){
		continue;
		}
		
		//血量
	   float Health = (驱动->读取浮点数(结构体 + 0xdc0) / 驱动->读取浮点数(结构体 + 0xdc8)) *100;;

		Vector3A 自身坐标;
		long int 自身坐标指针 = 驱动->读取指针(自身结构体 + 0x268);
        驱动->读取(自身坐标指针 + 0x1B0, &自身坐标, sizeof(自身坐标)); // 对象坐标

		
		Vector3A 敌人坐标;
		long int 敌人坐标指针 = 驱动->读取指针(结构体 + 0x268);
        驱动->读取(敌人坐标指针 + 0x1B0, &敌人坐标, sizeof(敌人坐标)); // 对象坐标
		
		
		float 去掉物资 = 驱动->读取浮点数(结构体 + 0x2D20);
		if (去掉物资 != 479.5){
		continue;
		}
			
			
        float 距离 = sqrt(pow(敌人坐标.X - 自身坐标.X, 2) + pow(敌人坐标.Y - 自身坐标.Y, 2) + pow(敌人坐标.Z - 自身坐标.Z, 2)) * 0.01;
         // 计算人物矩阵
        float camera = matrix[3] * 敌人坐标.X + matrix[7] * 敌人坐标.Y + matrix[11] * 敌人坐标.Z + matrix[15];

        float r_x = px + (matrix[0] * 敌人坐标.X + matrix[4] * 敌人坐标.Y + matrix[8] * 敌人坐标.Z + matrix[12]) / camera * px;
	     
	    float r_y = py - (matrix[1] * 敌人坐标.X + matrix[5] * 敌人坐标.Y + matrix[9] * (敌人坐标.Z - 5) + matrix[13]) / camera * py;
	
        float r_w = py - (matrix[1] * 敌人坐标.X + matrix[5] * 敌人坐标.Y + matrix[9] * (敌人坐标.Z + 205) + matrix[13]) / camera * py;
	
		//坐标转换
        float X = r_x - (r_y - r_w) / 4;
	    float Y = r_y;
        float W = (r_y - r_w) / 2;
	    float MIDDLE = X + W / 2;
        float BOTTOM = Y + W;
		float TOP = Y - W;
	  
		
	  //人物骨骼
	  Vector3A relLocation;Vector3A relLocation1;Vector3A LrelLocation1;
	  // 阵列偏移
      long int Mesh = 驱动->读取指针(结构体 + 0x5d0);
      // 骨骼阵列
      long int human = Mesh + 0x1a0;
      // 骨骼指针0x6c8+0x30
      long int Bone = 驱动->读取指针(Mesh + 0x6c8) + 0x30;
      // 计算骨节
      FTransform meshtrans = getBone(human);
      FMatrix c2wMatrix = TransformToMatrix(meshtrans);
      // 头部
      // 头部
      FTransform headtrans = getBone(Bone + 5 * 48);
      FMatrix boneMatrix = TransformToMatrix(headtrans);
       relLocation = MarixToVector(MatrixMulti(boneMatrix, c2wMatrix));
      relLocation.Z += 7; // 脖子长度
      Head = WorldToScreen(relLocation, matrix, camera);
      // 胸部
      FTransform chesttrans = getBone(Bone + 4 * 48);
      FMatrix boneMatrix1 = TransformToMatrix(chesttrans);
       relLocation1 = MarixToVector(MatrixMulti(boneMatrix1, c2wMatrix));
      Chest = WorldToScreen(relLocation1, matrix, camera);
      // 盆骨
      FTransform pelvistrans = getBone(Bone + 0 * 48);
      FMatrix boneMatrix2 = TransformToMatrix(pelvistrans);
       LrelLocation1 = MarixToVector(MatrixMulti(boneMatrix2, c2wMatrix));
      Pelvis = WorldToScreen(LrelLocation1, matrix, camera);
      // 左肩膀
      FTransform lshtrans = getBone(Bone + 11 * 48);
      FMatrix boneMatrix3 = TransformToMatrix(lshtrans);
      Vector3A relLocation2 = MarixToVector(MatrixMulti(boneMatrix3, c2wMatrix));
      Left_Shoulder = WorldToScreen(relLocation2, matrix, camera);
      // 右肩膀
      FTransform rshtrans = getBone(Bone + 33 * 48);
      FMatrix boneMatrix4 = TransformToMatrix(rshtrans);
      Vector3A relLocation3 = MarixToVector(MatrixMulti(boneMatrix4, c2wMatrix));
      Right_Shoulder = WorldToScreen(relLocation3, matrix, camera);
      // 左手肘
      FTransform lelbtrans = getBone(Bone + 12 * 48);
      FMatrix boneMatrix5 = TransformToMatrix(lelbtrans);
      Vector3A relLocation4 = MarixToVector(MatrixMulti(boneMatrix5, c2wMatrix));
      Left_Elbow = WorldToScreen(relLocation4, matrix, camera);
      // 右手肘
      FTransform relbtrans = getBone(Bone + 34 * 48);
      FMatrix boneMatrix6 = TransformToMatrix(relbtrans);
      Vector3A relLocation5 = MarixToVector(MatrixMulti(boneMatrix6, c2wMatrix));
      Right_Elbow = WorldToScreen(relLocation5, matrix, camera);
      // 左手腕
      FTransform lwtrans = getBone(Bone + 13 * 48);
      FMatrix boneMatrix7 = TransformToMatrix(lwtrans);
      Vector3A relLocation6 = MarixToVector(MatrixMulti(boneMatrix7, c2wMatrix));
      Left_Wrist = WorldToScreen(relLocation6, matrix, camera);
      // 右手腕
      FTransform rwtrans = getBone(Bone + 35 * 48);
      FMatrix boneMatrix8 = TransformToMatrix(rwtrans);
      Vector3A relLocation7 = MarixToVector(MatrixMulti(boneMatrix8, c2wMatrix));
      Right_Wrist = WorldToScreen(relLocation7, matrix, camera);
      // 左大腿
      FTransform Llshtrans = getBone(Bone + 55 * 48);
      FMatrix boneMatrix9 = TransformToMatrix(Llshtrans);
      Vector3A LrelLocation2 = MarixToVector(MatrixMulti(boneMatrix9, c2wMatrix));
      Left_Thigh = WorldToScreen(LrelLocation2, matrix, camera);
      // 右大腿
      FTransform Lrshtrans = getBone(Bone + 59 * 48);
      FMatrix boneMatrix10 = TransformToMatrix(Lrshtrans);
      Vector3A LrelLocation3 = MarixToVector(MatrixMulti(boneMatrix10, c2wMatrix));
      Right_Thigh = WorldToScreen(LrelLocation3, matrix, camera);
      // 左膝盖
      FTransform Llelbtrans = getBone(Bone + 56 * 48);
      FMatrix boneMatrix11 = TransformToMatrix(Llelbtrans);
      Vector3A LrelLocation4 = MarixToVector(MatrixMulti(boneMatrix11, c2wMatrix));
      Left_Knee = WorldToScreen(LrelLocation4, matrix, camera);
      // 右膝盖
      FTransform Lrelbtrans = getBone(Bone + 60 * 48);
      FMatrix boneMatrix12 = TransformToMatrix(Lrelbtrans);
      Vector3A LrelLocation5 = MarixToVector(MatrixMulti(boneMatrix12, c2wMatrix));
      Right_Knee = WorldToScreen(LrelLocation5, matrix, camera);
      // 左脚腕
      FTransform Llwtrans = getBone(Bone + 57 * 48);
      FMatrix boneMatrix13 = TransformToMatrix(Llwtrans);
      Vector3A LrelLocation6 = MarixToVector(MatrixMulti(boneMatrix13, c2wMatrix));
      Left_Ankle = WorldToScreen(LrelLocation6, matrix, camera);
      // 右脚腕
      FTransform Lrwtrans = getBone(Bone + 61 * 48);
      FMatrix boneMatrix14 = TransformToMatrix(Lrwtrans);
      Vector3A LrelLocation7 = MarixToVector(MatrixMulti(boneMatrix14, c2wMatrix));
      Right_Ankle = WorldToScreen(LrelLocation7, matrix, camera);
      // Max
      FTransform Maxs = getBone(Bone + 6 * 48);
      FMatrix boneMatrix15 = TransformToMatrix(Maxs);
      Vector3A Max = MarixToVector(MatrixMulti(boneMatrix15, c2wMatrix));
      // Min
      FTransform Mins = getBone(Bone + 58 * 48);
      FMatrix boneMatrix16 = TransformToMatrix(Mins);
      Vector3A Min = MarixToVector(MatrixMulti(boneMatrix16, c2wMatrix));
	   top1 = Pelvis.Y - Head.Y;
       top = Pelvis.Y - top1 - W / 5;    
	   if (LrelLocation6.Z < LrelLocation7.Z){
		 LrelLocation7.Z = LrelLocation6.Z;
		}
	  
	  //敌人阵营颜色
      ImColor 颜色 = ImColor(TeamColor(敌人阵营));
	  
	  //大于400米不显示
		if (距离 > 400){
		 continue;
}




              //敌人预警
			    std::string ssd;
                ssd += std::to_string((int) 距离);    
                ssd += " m";
                auto textSize = ImGui::CalcTextSize(ssd.c_str(), 0, 25);
                if (X+W/2<0){
                    ImGui::GetForegroundDrawList()->AddCircleFilled({0,Head.Y},60,bdzrColor);    
                    ImGui::GetForegroundDrawList()->AddText(NULL,35,{5,Head.Y-20},whiteColor,ssd.c_str());                                      
                }else if(W>0&&X>px*2){
                    ImGui::GetForegroundDrawList()->AddCircleFilled({px*2,Head.Y},60,bdzrColor);                       
                    ImGui::GetForegroundDrawList()->AddText(NULL,35,{px*2 - 50,Head.Y-20},whiteColor,ssd.c_str());                                      
                }else if(W>0&&Y+W<0){               
                    ImGui::GetForegroundDrawList()->AddCircleFilled({Head.X,0},60,bdzrColor);     
                    ImGui::GetForegroundDrawList()->AddText(NULL,35,{Head.X - 30,10},whiteColor,ssd.c_str());                                      
                }else if(W<0){
                    ImGui::GetForegroundDrawList()->AddCircleFilled({px*2 - Head.X,py*2},60,bdzrColor);   
                    ImGui::GetForegroundDrawList()->AddText(NULL,35,{px*2 - Head.X-30,py*2 - 30},whiteColor,ssd.c_str());                                      
                }
				
				
            Vector3A 敌人向量;   
            驱动->读取(驱动->读取指针(结构体 + 0x1A78)+0x124, &敌人向量, sizeof(敌人向量)); // 敌人向量           
            
            if (W > 0){
	 	    Aim[AimCount].WodDistance = 距离;
			Aim[AimCount].AimMovement = 敌人向量;
			if (NumIo[8] == 1.0){
                Aim[AimCount].ObjAim = relLocation;                            
                Aim[AimCount].ScreenDistance = sqrt(pow(px - Head.X, 2) + pow(py - Head.Y, 2));
            }else if (NumIo[8] == 2.0 || NumIo[8] == 0.0){                                 
                Aim[AimCount].ObjAim = relLocation1;                            
                Aim[AimCount].ScreenDistance = sqrt(pow(px - Chest.X, 2) + pow(py - Chest.Y, 2));
            }else if (NumIo[8] == 3.0){                                 
                Aim[AimCount].ObjAim = LrelLocation1;                                                 
                Aim[AimCount].ScreenDistance = sqrt(pow(px - Pelvis.X, 2) + pow(py - Pelvis.Y, 2));
            }else{
                Aim[AimCount].ObjAim = relLocation1;                            
                Aim[AimCount].ScreenDistance = sqrt(pow(px - Chest.X, 2) + pow(py - Chest.Y, 2));
            }
			AimCount++;
		}
         
            	 			
            // 开始绘制           
             if(W > 0) {    		
              if (show_draw_Rect) {
				  /*
				 long int 枪械管理 = 驱动->读取指针(驱动->读取指针(自身结构体 + 0x2158) + 0x4e8);
				 驱动->写入F类内存(枪械管理 + 0xab8,0);
				 驱动->写入F类内存(枪械管理 + 0xabc,0);*/
				 
				 
					 
					 
                 ImGui::GetForegroundDrawList()->AddText(NULL, 29, {MIDDLE + 118, top - 50.5f}, whiteColor,国家);
				
					
			    //方框
			   ImGui::GetForegroundDrawList()->AddRect({X, TOP}, {X+W,Y+W},方框, {1}, 0,{1.3f});
		     }
	          if (show_draw_Skeleton) {
				//骨骼
			   ImGui::GetForegroundDrawList()->AddCircle({Head.X , Head.Y}, W/14, 骨骼, 0);
               ImGui::GetForegroundDrawList()->AddLine({Head.X, Head.Y}, {Chest.X, Chest.Y}, 骨骼,{1.5f});
               ImGui::GetForegroundDrawList()->AddLine({Chest.X, Chest.Y}, {Pelvis.X, Pelvis.Y},骨骼,{1.5f});
               ImGui::GetForegroundDrawList()->AddLine({Chest.X, Chest.Y}, {Left_Shoulder.X,Left_Shoulder.Y},骨骼,{1.5f});
               ImGui::GetForegroundDrawList()->AddLine({Chest.X, Chest.Y}, {Right_Shoulder.X,Right_Shoulder.Y}, 骨骼,{1.5f});
               ImGui::GetForegroundDrawList()->AddLine({Left_Shoulder.X, Left_Shoulder.Y}, {Left_Elbow.X,Left_Elbow.Y},骨骼,{1.5f});
               ImGui::GetForegroundDrawList()->AddLine({Right_Shoulder.X, Right_Shoulder.Y},{Right_Elbow.X, Right_Elbow.Y},骨骼,{1.5f});
               ImGui::GetForegroundDrawList()->AddLine({Left_Elbow.X, Left_Elbow.Y}, {Left_Wrist.X,Left_Wrist.Y}, 骨骼,{1.5f});
               ImGui::GetForegroundDrawList()->AddLine({Right_Elbow.X, Right_Elbow.Y}, {Right_Wrist.X,Right_Wrist.Y},骨骼,{1.5f});
               ImGui::GetForegroundDrawList()->AddLine({Pelvis.X, Pelvis.Y}, {Left_Thigh.X, Left_Thigh.Y},骨骼,{1.5f});
               ImGui::GetForegroundDrawList()->AddLine({Pelvis.X, Pelvis.Y}, {Right_Thigh.X,Right_Thigh.Y}, 骨骼,{1.5f});
               ImGui::GetForegroundDrawList()->AddLine({Left_Thigh.X, Left_Thigh.Y}, {Left_Knee.X,Left_Knee.Y}, 骨骼,{1.5f});
               ImGui::GetForegroundDrawList()->AddLine({Right_Thigh.X, Right_Thigh.Y}, {Right_Knee.X,Right_Knee.Y}, 骨骼,{1.5f});
               ImGui::GetForegroundDrawList()->AddLine({Left_Knee.X, Left_Knee.Y}, {Left_Ankle.X,Left_Ankle.Y}, 骨骼,{1.5f});
               ImGui::GetForegroundDrawList()->AddLine({Right_Knee.X, Right_Knee.Y}, {Right_Ankle.X,Right_Ankle.Y}, 骨骼,{1.5f});
		       }
		 
		       if (show_draw_Health){
                    // 血量
                    ImGui::GetForegroundDrawList()->AddRectFilled({MIDDLE - 105, top - 76}, {MIDDLE - 65, top - 43}, halflackColor, 1, 0);
                    ImGui::GetForegroundDrawList()->AddRectFilled({MIDDLE - 65, top - 76}, {MIDDLE + 105, top - 43}, 颜色, 1, 0);
                    ImGui::GetForegroundDrawList()->AddRectFilled({MIDDLE - 105, top - 43}, {MIDDLE - 105 + (2.1f * Health), top - 38},health_Color, 1, 0);
				}
				
                if (show_draw_Line){
                  // 射线
				ImGui::GetForegroundDrawList()->AddLine({px,0}, { MIDDLE, TOP }, 射线, {1.3f});
				}
				
				if (show_draw_distance){
				   //距离
				   std::string s;
                    s += std::to_string((int)距离);
                    s += "M";
                    auto textSize = ImGui::CalcTextSize2(s.c_str(), 0, 29);
                    ImGui::GetForegroundDrawList()->AddText(NULL, 29, {MIDDLE + 118, top - 73.5f}, whiteColor, s.c_str());
				}
				  
				if (show_draw_Camp){
					 //敌人阵营
				    std::string s;
                    s += std::to_string((int)敌人阵营);
                    auto textSize = ImGui::CalcTextSize2(s.c_str(), 0, 30);
                    ImGui::GetForegroundDrawList()->AddText(NULL, 30, {MIDDLE - 84 - (textSize.x / 2), top - 73.5f}, whiteColor, s.c_str());
					}
					
					
					if (show_draw_name){
				     //敌人名字
					if (人机判断==1) {
		              ImGui::GetForegroundDrawList()->AddText(NULL, 30,{MIDDLE - 60, top - 73.5f}, whiteColor, "人机");
                    }else{
                      ImGui::GetForegroundDrawList()->AddText(NULL, 30,{MIDDLE - 60, top - 73.5f}, whiteColor, 输出帧率字);
					 }
				 }
					
				
				
				
            }  
			MaxPlayerCount = AimCount;
			
			if (人机判断==1) {
			BotCount ++;
		    } else {
			PlayerCount ++;
			}
		
	
	
  }          

 if (show_draw_human){
 if (PlayerCount + BotCount > 0){
	string str = "玩家";
    str += to_string(PlayerCount);
    str += "] & Robot[";
    str += to_string(BotCount);
    str += "]";
	auto textSizes = ImGui::CalcTextSize(str.c_str(), 0, 35);
    ImGui::GetForegroundDrawList()->AddRectFilled({px - 170, 80}, {px - 120, 120}, ImColor(35, 35, 35), 5, 0);
	ImGui::GetForegroundDrawList()->AddRectFilled({px - 120, 80}, {px - 50, 120}, ImColor(175, 175, 175), 5, 0);
	ImGui::GetForegroundDrawList()->AddRectFilled({px + 50, 80}, {px + 100, 120}, ImColor(35, 35, 35), 5, 0);
	ImGui::GetForegroundDrawList()->AddRectFilled({px + 100, 80}, {px + 170, 120}, ImColor(175, 175, 175), 5, 0);
  	ImGui::GetForegroundDrawList()->AddText(NULL, 35, {screen_x / 2 - 145 - (ImGui::CalcTextSize(to_string(PlayerCount).c_str(), 0, 35).x / 2), 82}, ImColor(175, 175, 175), to_string(PlayerCount).c_str());
	ImGui::GetForegroundDrawList()->AddText(NULL, 35, {screen_x / 2 - 85 - (ImGui::CalcTextSize("玩家", 0, 35).x / 2), 82}, ImColor(35, 35, 35), "玩家");
	ImGui::GetForegroundDrawList()->AddText(NULL, 35, {screen_x / 2 + 75 - (ImGui::CalcTextSize(to_string(BotCount).c_str(), 0, 35).x / 2), 82}, ImColor(175, 175, 175), to_string(BotCount).c_str());
	ImGui::GetForegroundDrawList()->AddText(NULL, 35, {screen_x / 2 + 135 - (ImGui::CalcTextSize("人机", 0, 35).x / 2), 82}, ImColor(35, 35, 35), "人机");
	}else{
    sprintf(输出帧率字,"[ 安全 ]");
	绘制字体描边(33,px-30,40,ImColor(248,248,255),输出帧率字);
	}
}
    ImGuiIO &io = ImGui::GetIO();
	sprintf(输出帧率字,"帧率:%.1f",ImGui::GetIO().Framerate);
    绘制字体描边(25,100,160,ImColor(248,248,255),输出帧率字);
	
	sprintf(输出帧率字,"数量:%d",数量);
	绘制字体描边(25,100,110,ImColor(248,248,255),输出帧率字);

//	sprintf(输出帧率字," :%p",基址头);
	//绘制字体描边(25,100,90,ImColor(248,248,255),输出帧率字);
}


void GetTouch(){
	for(;;){
		usleep(2000);	
		ImGuiIO& ioooi = ImGui::GetIO();
		if (DrawIo[21] && ioooi.MouseDown[0] && point2.x <= NumIo[6] + NumIo[7] && point2.y <= py*2 - NumIo[5] + NumIo[7] && point2.x >= NumIo[6] - NumIo[7] && point2.y >= py*2 - NumIo[5] - NumIo[7]){			
			usleep(55000);		
			if (ioooi.MouseDown[0] && point2.x <= NumIo[6] + NumIo[7] && point2.y <= py*2 - NumIo[5] + NumIo[7] && point2.x >= NumIo[6] - NumIo[7] && point2.y >= py*2 - NumIo[5] - NumIo[7]){
				while (ioooi.MouseDown[0]){
					NumIo[6] = point2.x;
					NumIo[5] = py*2 - point2.y;
					usleep(500);		
				}
			}		
		}
	}
}




void tick(){
    static bool show_ChildMenu1 = true;
	static bool show_ChildMenu2 = false;
	static bool show_ChildMenu3 = false;
	ImGuiIO& io = ImGui::GetIO();
        if (display == EGL_NO_DISPLAY)
        return;
        static ImVec4 clear_color = ImVec4(0, 0, 0, 0);
        ImGui_ImplOpenGL3_NewFrame();    
        ImGui_ImplAndroid_NewFrame(init_screen_x, init_screen_y);
        ImGui::NewFrame();
		ImGuiStyle&style = ImGui::GetStyle();	  	
        style.ScrollbarRounding = 5; // 设置滚动条圆角
        style.ScrollbarSize = 30; // 设置滚动条宽度
        style.WindowRounding = 3; // 设置边框圆角
        style.GrabRounding = 5; // 设置滑块圆角
        style.GrabMinSize = 20; // 设置滑块宽度
        style.FrameBorderSize = 2; // 设置控件描边宽度
        style.WindowBorderSize = 2; // 设置框架描边宽度
	    style.WindowTitleAlign = ImVec2(0.5, 0.5); // 设置标题居中
        style.WindowRounding = 5.0f;
        style.FrameRounding = 5.0f;
        style.ScrollbarRounding = 5.0f;
		ImGui::SetNextWindowBgAlpha(0.8);
		ImGui::SetNextWindowSize(ImVec2(760,650), ImGuiCond_FirstUseEver);
	   ImGui::Begin("默gei[稳定版]"); 
		  if(ImGui::BeginChild("##右侧",ImVec2(250,0),false,ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NavFlattened));   {
            if(ImGui::Button("绘制",ImVec2(245,85))) {
             show_ChildMenu1 = true;
             show_ChildMenu2 = false;
             show_ChildMenu3 = false;
				
            }
            if(ImGui::Button("功能",ImVec2(245,85))) {
                show_ChildMenu1 = false;
                show_ChildMenu2 = true;
                show_ChildMenu3 = false;
            }
			
			if(ImGui::Button("设置",ImVec2(245,85))) {
                show_ChildMenu1 = false;
                show_ChildMenu2 = false;
                show_ChildMenu3 = true;
            }
            if(ImGui::Button("退出",ImVec2(245,85))){
                exit(0);
            }
            ImGui::EndChild();// 需要和ImGui::BeginChild成对出现。
        }
		
	    ImGui::SameLine();
        if(show_ChildMenu1){
		if(ImGui::BeginChild("##绘制",ImVec2(0,0),false,ImGuiWindowFlags_NavFlattened)){
     	if (ImGui::Button("一键全开")){   
            show_draw_Rect = true;
			show_draw_Line = true;
			show_draw_distance = true;
			show_draw_human = true;
			show_draw_Camp = true;
			show_draw_name = true;
			show_draw_Skeleton = true;
			show_draw_Health = true;
        }
		ImGui::SameLine();
		if(ImGui::Button("一键全关")){
			show_draw_Rect = false;
			show_draw_Line = false;
			show_draw_distance = false;
			show_draw_human = false;
			show_draw_Camp = false;
			show_draw_name = false;
			show_draw_Skeleton = false;
			show_draw_Health = false;
		}
	
        ImGui::Checkbox("绘制方框", &show_draw_Rect);
		ImGui::SameLine();
		ImGui::Checkbox("绘制骨骼", &show_draw_Skeleton);
	
		ImGui::Checkbox("绘制血量", &show_draw_Health);
		ImGui::SameLine();
        ImGui::Checkbox("绘制射线", &show_draw_Line);
		
        ImGui::Checkbox("绘制距离", &show_draw_distance);
		ImGui::SameLine();
		ImGui::Checkbox("绘制人数", &show_draw_human);
		
        ImGui::Checkbox("绘制阵营", &show_draw_Camp);
		ImGui::SameLine();
		ImGui::Checkbox("绘制名字", &show_draw_name);
		ImGui::EndTabBar();  	
		}
	}
		
if(show_ChildMenu2){
		if(ImGui::BeginChild("功能",ImVec2(0,0),false,ImGuiWindowFlags_NavFlattened)){
		//ImGui::SwitchButton("初始化子追", &DrawIo[20]);//这个不用
		ImGui::SwitchButton("视角追踪", &DrawIo[20]);//这里面有一个自瞄圈
	 ImGui::SliderFloat("追踪范围", &NumIo[3],10.0f,500.0f,"%.0f",1);//这是调节范围的
    if (ImGui::Combo("子追部位", &style_idx2, "智能\0头部\0胸部\0臀部\0")) {
      ImGui::Text("默认开火");	
           switch (style_idx2) 
           {
             case 0:NumIo[8]=0.0; 
             break;
             case 1:NumIo[8]=1.0; 
             break;
             case 2:NumIo[8]=2.0; 
             break;
             case 3:NumIo[8]=3.0;
             break;	
           }          
        }
	 }
	}
	
		if(show_ChildMenu3){
		if(ImGui::BeginChild("##设置",ImVec2(0,0),false,ImGuiWindowFlags_NavFlattened)){
       	ImGui::Text("颜色调节");
        ImGui::ColorEdit4("方框颜色", (float*)&Colbox1);                
		
	    ImGui::ColorEdit4("射线颜色", (float*)&Colbox2);
		
        ImGui::ColorEdit4("骨骼颜色", (float*)&Colbox3);
		
		ImGui::ColorEdit4("圈圈颜色", (float*)&Colbox4); 
	}
}
	ImGui::End();
   	//DrawPlayer();
    ImGui::Render();  
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    eglSwapBuffers(display, surface);
}

void shutdown()
{
    if (!g_Initialized) {
        return;
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();
    if (display != EGL_NO_DISPLAY){
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context != EGL_NO_CONTEXT){
            eglDestroyContext(display, context);
        }
        if (surface != EGL_NO_SURFACE){
            eglDestroySurface(display, surface);
        }
        eglTerminate(display);
    }
    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    surface = EGL_NO_SURFACE;
    ANativeWindow_release(native_window);
}


	
