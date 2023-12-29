#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include<ctype.h>
#include "PassPageFault.h"
#include "fonts.h"
#include "MemoryReaderWriter37.h"



class c_driver{
  private:
	int has_upper = 0;
	int has_lower = 0;
	int has_symbol = 0;
	int has_digit = 0;
	pid_t pid= 0;
	CMemoryReaderWriter rwDriver;
	uintptr_t dll_base = 0;
	uint64_t hProcess;
	int symbol_file(const char *filename){
		int length = strlen(filename);
		for (int i = 0; i < length; i++){
		if (isupper(filename[i])){
				has_upper = 1;
			}
			else if (islower(filename[i])){
				has_lower = 1;
			}
			else if (ispunct(filename[i])){
				has_symbol = 1;
			}
			else if (isdigit(filename[i])){
				has_digit = 1;
			}
		}

		return has_upper && has_lower && !has_symbol && !has_digit;
	}

  public:
	c_driver(){

		std::string devFileName = RWPROCMEM_FILE_NODE;
		int err = 0;
		if (!rwDriver.ConnectDriver(devFileName.c_str(), FALSE, err)) {
		printf("Connect rwDriver failed. error:%d\n", err);
		exit(0);
		}
		rwDriver.SeUseBypassSELinuxMode(TRUE);
		rwDriver.SetMaxDevFileOpen(2);
		printf("[-] 内核加载成功.\n");

	}

	~c_driver(){

	}

	void 初始化(pid_t pid){
		this->pid = pid;
	}

	bool 读取(uintptr_t 地址, void *buffer, size_t size){
		if (地址 <= 0x10000000 || 地址 % 4 != 0 || 地址 >= 0x10000000000){
	 		return false;
		}
		size_t real_read = 0;
		BOOL read_res = rwDriver.ReadProcessMemory(hProcess, (uint64_t)地址, &buffer, size, &real_read, FALSE);
		return read_res;
	}
	
	bool 写入(uintptr_t 地址, void *buffer, size_t size){
		size_t real_write = 0;
		BOOL write_res = rwDriver.WriteProcessMemory(hProcess, (uint64_t)地址, &buffer, size, &real_write, FALSE);
		return write_res;
	}

	template < typename T > T 读取(uintptr_t 地址){
		T res;
		if (this->读取(地址, &res, sizeof(T)))
			return res;
			return{
		};
	}

	template < typename T > bool 写入(uintptr_t 地址, T value){
		return this->写入(地址, &value, sizeof(T));
	}
		
	long 读取指针(long 地址){
		if (地址 <= 0x10000000 || 地址 % 4 != 0 || 地址 >= 0x10000000000){
			return 0;
		}
		long res;
		if (this->读取(地址,&res,sizeof(uintptr_t))){
			return res;
		}
		return {};
	}
		
	float 读取浮点数(long 地址){
		if (地址 < 0xFFFFFF){
			return 0;
		}
		float var;
		if (this->读取(地址&0xFFFFFFFFFF,&var,sizeof(var))){
			return var;
		}
		return {};
	}
		
	
	int 读取整数(long 地址){
		if (地址 <= 0x10000000 || 地址 % 4 != 0 || 地址 >= 0x10000000000){
		return 0;
		}
		int 数据;
		if (this->读取(地址&0xFFFFFFFFFF,&数据,sizeof(数据))){
		return 数据;
		}
		return {};
	}
		
		
		// 写入F类内存
	void 写入F类内存(unsigned long 地址, float 数据){
		this->写入(地址, &数据, 4);
	}
		// 写入D类内存
	void 写入D类内存(unsigned long 地址, int 数据){
		this->写入(地址, &数据, 4);
	}

	pid_t 获取进程(char *game_name){
			FILE *fp;
			pid_t pid;
			char cmd[0x100] = "pidof ";
			strcat(cmd, game_name);
			fp = popen(cmd, "r");
			fscanf(fp, "%d", &pid);
			pclose(fp);
			this->hProcess = rwDriver.OpenProcess(this->pid);
			return pid;
	}

	uintptr_t 获取基址头(char *dll_name){
		if(!this->dll_base){
			return  this->dll_base;
		}
		std::vector<DRIVER_REGION_INFO> vMaps;
		BOOL bOutListCompleted;
		BOOL b;

		b = rwDriver.VirtualQueryExFull(hProcess, FALSE, vMaps, bOutListCompleted);
		if(b){
			for (DRIVER_REGION_INFO rinfo : vMaps) {
				if(strcmp(rinfo.name,dll_name)==0){
					this->dll_base =  rinfo.baseaddress;
					break;
				}
			}
		}
		return  this->dll_base;
	}
		
		


		
	};//结束

	static c_driver *驱动 = new c_driver();
