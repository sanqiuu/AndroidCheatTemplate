#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include "MemoryReaderWriter37.h"
using namespace std;
static CMemoryReaderWriter* pDriver = new CMemoryReaderWriter();
class CDriver {
private:
    int fd;
    pid_t pid;
    uint64_t hprocess;
    typedef struct _COPY_MEMORY {
        pid_t pid;
        uintptr_t addr;
        void* buffer;
        size_t size;
    } COPY_MEMORY, *PCOPY_MEMORY;

    typedef struct _MODULE_BASE {
        pid_t pid;
        char* name;
        uintptr_t base;
    } MODULE_BASE, *PMODULE_BASE;

    enum OPERATIONS {
        OP_INIT_KEY = 0x800,
        OP_READ_MEM = 0x801,
        OP_WRITE_MEM = 0x802,
        OP_MODULE_BASE = 0x803,
    };

    char *driver_path()
    {
        // 打开/dev目录
        const char *dev_path = "/dev";
        DIR *dir = opendir(dev_path);
        if (dir == NULL)
        {
            printf("无法打开/dev目录\n");
            return NULL;
        }
        char *searchFiles[] = { "wanbai", "CheckMe", "Ckanri", "lanran","rwProcMem37"};
        int typeSize = 5;
        struct dirent *entry;
        char *file_path = NULL;
        while ((entry = readdir(dir)) != NULL)
        {
            // 跳过当前目录和上级目录
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }
            size_t path_length = strlen(dev_path) + strlen(entry->d_name) + 2;
            file_path = (char *)malloc(path_length);
            snprintf(file_path, path_length, "%s/%s", dev_path, entry->d_name);
     
            // 获取文件stat结构
            struct stat file_info;
            if (stat(file_path, &file_info) < 0)
            {
                free(file_path);
                file_path = NULL;
                continue;
            }

            // 判断文件名是否包含搜索数组中的设备名
            bool is_desired_driver = false;
            for (int i = 0; i < typeSize; i++)
            {
                if (strstr(entry->d_name, searchFiles[i]) != NULL)
                {
                    is_desired_driver = true;
                    break;
                }
            }
            if (!is_desired_driver)
            {
                free(file_path);
                file_path = NULL;
                continue;
            }

            // 排除不符合条件的文件
            if ((S_ISCHR(file_info.st_mode) || S_ISBLK(file_info.st_mode)) && strchr(entry->d_name, '_') == NULL)
            {
                // 排除标准输入输出和gpio接口
                if (strcmp(entry->d_name, "stdin") == 0 || strcmp(entry->d_name, "stdout") == 0 || strcmp(entry->d_name, "stderr") == 0)
                {
                    free(file_path);
                    file_path = NULL;
                    continue;
                }
                if (strstr(entry->d_name, "gpiochip") != NULL)
                {
                    free(file_path);
                    file_path = NULL;
                    continue;
                }
                closedir(dir);
                return file_path;
            }
            free(file_path);
            file_path = NULL;
        }
        closedir(dir);
        return NULL;
    }
	
    char* getDriverPath() {	
        ifstream procMiscFile("/proc/misc");
        if (!procMiscFile.is_open()) {
            //cout << "Failed to open /proc/misc file." << endl;
            return nullptr;
        }

        string line;
        getline(procMiscFile, line); // 读取第一行
        procMiscFile.close();

        istringstream iss(line);
        string deviceNumber, deviceName;
        iss >> deviceNumber >> deviceName;

		if (deviceName == "usb_accessory") {
			//cout << "Failed to open /proc/misc file." << endl;
            return nullptr;
		}
		
        string DevName = "/dev/";
        DevName += deviceName;

        return strdup(DevName.c_str());
    }

public:
    CDriver() : fd(-1), pid(0) {
      /*  char* deviceName = getDriverPath();
        fd = open(deviceName, O_RDWR);
		
        if (fd == -1) {
			char* deviceName2 = driver_path();
			fd = open(deviceName2, O_RDWR);
			
			if (fd == -1) {
            	cout << "[-] Failed to open driver." << endl;
				free(deviceName);
            	exit(0);
			}
			
			printf("\n内核驱动:%s\n", deviceName2);
        } else {
			printf("\n内核驱动:%s\n", deviceName);
		}       
		free(deviceName);*/
        int err = 0;
        if (!pDriver->ConnectDriver(RWPROCMEM_FILE_NODE, FALSE, err)) {
            printf("[-] 驱动连接失败,错误码:%d.\n", err);
            exit(0);
        }
        printf("[+] 驱动连接成功.\n");
    }

    ~CDriver() {
        if (fd > 0)
            close(fd);
    }

    bool initialize(char* packageName) {
        pid_t pid  = get_name_pid(packageName);
        if (pid) {
            this->pid = pid;
            this->hprocess = pDriver->OpenProcess(pid);
        }
        return pid != 0;
    }

    bool initKey(const char* key) {
        return true;
    }

    // 获取进程是否运行
    bool isapkrunning(char* packageName)
    {
        pid_t pid = get_name_pid(packageName);
        return pid != 0;
    }
    pid_t get_name_pid(char* packageName) {
        pid_t nTargetPid = 0;
        DIR* dir = NULL;
        struct dirent* ptr = NULL;

        dir = opendir("/proc");
        if (dir) {
            while ((ptr = readdir(dir)) != NULL) { // 循环读取路径下的每一个文件/文件夹
                // 如果读取到的是"."或者".."则跳过，读取到的不是文件夹名字也跳过
                if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)) {
                    continue;
                }
                else if (ptr->d_type != DT_DIR) {
                    continue;
                }
                else if (strspn(ptr->d_name, "1234567890") != strlen(ptr->d_name)) {
                    continue;
                }

                pid_t pid = (pid_t)atoi(ptr->d_name);

                uint64_t hProcess = pDriver->OpenProcess(pid);
                if (!hProcess) { continue; }

                char cmdline[200] = { 0 };
                pDriver->GetProcessCmdline(hProcess, cmdline, sizeof(cmdline));

                pDriver->CloseHandle(hProcess);
                if (strcmp(packageName, cmdline) == 0) {
                    nTargetPid = pid;
                    break;
                }
            }
            closedir(dir);
        }
        return nTargetPid;
    }
    bool read(uintptr_t addr, void* buffer, size_t size) {
        size_t real_read;
        return pDriver->ReadProcessMemory(this->hprocess, addr, buffer, size, &real_read, false);
    }

    bool write(uintptr_t addr, void* buffer, size_t size) {
        size_t vm_writev;
        return pDriver->WriteProcessMemory(this->hprocess, addr, buffer, size, &vm_writev, false);
    }

    template <typename T>
    T read(uintptr_t addr) {
        T res;
        if (this->read(addr, &res, sizeof(T)))
            return res;
        return {};
    }

    template <typename T>
    bool write(uintptr_t addr, T value) {
        return this->write(addr, &value, sizeof(T));
    }

    uintptr_t getModuleBase(const char* dll_name) {
        std::vector<DRIVER_REGION_INFO> vMaps;
        BOOL bOutListCompleted;
        BOOL b = pDriver->VirtualQueryExFull(this->hprocess, FALSE, vMaps, bOutListCompleted);
        if (!vMaps.size()) {
            return 0;
        }
        uintptr_t  dll_base = 0;
        //显示进程内存块地址列表
        for (DRIVER_REGION_INFO rinfo : vMaps) {
            if (strstr(rinfo.name, dll_name) != 0) {
                dll_base = rinfo.baseaddress;
                break;
            }
        }
        return dll_base;
    }
};



static CDriver* driver = new CDriver();