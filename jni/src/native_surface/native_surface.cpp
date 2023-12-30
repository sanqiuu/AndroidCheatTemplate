//
// Created by 泓清 on 2022/8/26.
//
#include "native_surface/utils.h"
#include "native_surface/native_surface.h"

#include "aosp/dev.h"
#include "aosp/native_surface_9.h"
#include "aosp/native_surface_10.h"
#include "aosp/native_surface_11.h"
#include "aosp/native_surface_12.h"
#include "aosp/native_surface_12_1.h"
#include "aosp/native_surface_13.h"


void* get_createNativeWindow() {
    void *handle = nullptr;// 动态库方案
    if (!handle) {
//        handle = dlblob(&native_surface_test, sizeof(native_surface_test)); // 测试
        printf("android api level:%d\n", get_android_api_level());
        if (get_android_api_level() == 33) { // 安卓13支持
            exec_native_surface("settings put global block_untrusted_touches 0");
#ifdef __aarch64__
            handle = dlblob(&native_surface_13_64, sizeof(native_surface_13_64)); // 64位支持
#else
            handle = dlblob(&native_surface_13_32, sizeof(native_surface_13_32)); // 32位支持 <<-- 其实很没必要 未测试
#endif
        } else if (get_android_api_level() == /*__ANDROID_API_S__*/ 32) { // 安卓12.1支持
            exec_native_surface("settings put global block_untrusted_touches 0");
#ifdef __aarch64__
            handle = dlblob(&native_surface_12_1_64, sizeof(native_surface_12_1_64)); // 64位支持
#else
            handle = dlblob(&native_surface_12_1_32, sizeof(native_surface_12_1_32)); // 32位支持 <<-- 其实很没必要 未测试
#endif
        } else if (get_android_api_level() == /*__ANDROID_API_S__*/ 31) { // 安卓12支持
            exec_native_surface("settings put global block_untrusted_touches 0");
#ifdef __aarch64__
            handle = dlblob(&native_surface_12_64, sizeof(native_surface_12_64)); // 64位支持
#else
            handle = dlblob(&native_surface_12_32, sizeof(native_surface_12_32)); // 32位支持 <<-- 其实很没必要 未测试
#endif
        } else if (get_android_api_level() == /*__ANDROID_API_R__*/ 30) { // 安卓11支持
#ifdef __aarch64__
            handle = dlblob(&native_surface_11_64, sizeof(native_surface_11_64)); // 64位支持
#else
            handle = dlblob(&native_surface_11_32, sizeof(native_surface_11_32)); // 32位支持 <<-- 其实很没必要 未测试
#endif
        } else if (get_android_api_level() == /*__ANDROID_API_Q__*/ 29) { // 安卓10支持
#ifdef __aarch64__
            handle = dlblob(&native_surface_10_64, sizeof(native_surface_10_64)); // 64位支持
#else
            handle = dlblob(&native_surface_10_32, sizeof(native_surface_10_32)); // 32位支持 <<-- 其实很没必要 未测试
#endif
        } else if (get_android_api_level() == /*__ANDROID_API_P__*/28) { // 安卓9支持
#ifdef __aarch64__
            handle = dlblob(&native_surface_9_64, sizeof(native_surface_9_64)); // 64位支持
#else
            handle = dlblob(&native_surface_9_32, sizeof(native_surface_9_32)); // 32位支持 <<-- 其实很没必要 未测试
#endif
        } else {
            printf("Sorry, level:%d Don't Support~\n", get_android_api_level());
            exit(0);
        }
        
        void *sy = dlsym(handle, "_Z18createNativeWindowPKcjj");
    if (sy != nullptr) {
        return sy;
    }else{
		sy = dlsym(handle, "_Z18createNativeWindowPKcjjb");      
		if (sy != nullptr) {
      		return sy;
   		}else{
       		sy = dlsym(handle, "_Z14getDisplayInfov");	
			if (sy != nullptr) {
      	  		return sy;
    		}else{
        		sy = dlsym(handle, "_Z12setSurfaceWHjj");
				if (sy != nullptr) {
        			return sy;
    			}else{
        			sy = dlsym(handle, "_Z10initRecordPKcfjj");
					if (sy != nullptr) {
        				return sy;
   	 				}else{
        				sy = dlsym(handle, "_Z9runRecordPbPFvPhmE");
						if (sy != nullptr) {
        					return sy;
    					}else{
        					sy = dlsym(handle, "_Z10stopRecordv");
							if (sy != nullptr) {
        						return sy;
    						}else{
        						sy = dlsym(handle, "_Z21getRecordNativeWindowv");						
								if (sy != nullptr) {
        							return sy;
								}else{
									printf("createNativeWindow _ null~\n");           
    								exit(0);
								}
							}
						}
					}
				}
			}
		}       
	}
	}
}

/**
 * 创建 native surface
 * @param surface_name 创建名称
 * @param screen_width 创建宽度
 * @param screen_height 创建高度
 * @param author 是否打印作者信息
 * @return
ANativeWindow *ExternFunction::createNativeWindow(const char *surface_name, uint32_t screen_width, uint32_t screen_height, bool author) {
    return ((ANativeWindow *(*)(const char *, uint32_t, uint32_t, bool))(funcPointer.func_createNativeWindow))(surface_name, screen_width, screen_height, author);
}
 */


