#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>
#include <fstream>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include <iostream>
#include <fstream>
#include <sys/system_properties.h>
#include <main.h>
#include <sys/utsname.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char **argv){
	screen_config();
    init_screen_x = screen_x + screen_y;
    init_screen_y = screen_y + screen_x;
	if(!init_egl(init_screen_x,init_screen_y)) {
        exit(0);
    }
    ImGui_init(); 
	DrawInit();
	new std::thread(GetTouch);
    new std::thread(AimBotAuto);
    while (1){     
	  tick();
    }
    shutdown();
    return 0;
}
