#include <check.h>
#include <Encrypt.h>
#include <cJSON.h>
#include <cJSON.c>
#include <http.h>

using namespace std;

typedef long int ADDRESS;
typedef char PACKAGENAME;
char *ztm;
char *jhm;
char *jqm;
char *appmy = "doGavbQbIi7D7Dbl";//APP密钥
char *rc4key = "MopVhRJMqxtABwrOto";//选择rc4-2加密
char *host = "wy.llua.cn";//官网
char *dmdl = "api/?id=kmlogon&app=14491";    // 9999改你的应用id

int android_check()
{
    char imei[50] = "vgg";
	char km[50] = "hhj";
    
    cout << endl << "正在链接中......." << endl;
    
    // 获取时间
    time_t t;
    t = time(NULL);
    int ii = time(&t);

    // 合并微验提交数据
    char value[256];
    char sign[256];
    char data[256];
    sprintf(value, "%d", ii);
    sprintf(sign, "kami=%s&markcode=%s&t=%d&%s", km, imei, ii, appmy);

    // md5验证签名
    char *aaa = sign;
    unsigned char *bbb = (unsigned char *)aaa;
    MD5_CTX md5c;
    MD5Init(&md5c);
    int i;
    unsigned char decrypt[16];
    MD5Update(&md5c, bbb, strlen((char *)bbb));
    MD5Final(&md5c, decrypt);
    char lkey[32] = { 0 };
    for (i = 0; i < 16; i++)
    {
        sprintf(&lkey[i * 2], "%02x", decrypt[i]);
    }
    char weiyan[256];
    sprintf(weiyan, "%d%s%s", ii, appmy, value);

    sprintf(data, "kami=%s&markcode=%s&t=%d&sign=%s&value=%s", km, imei, ii, lkey, value);
    char* adga = Encrypt(data, rc4key);
    
    char url[128];
    sprintf(url, "&data=%s", adga);
    ztm = httppost(host, dmdl, url);
    char* abcdstr = Decrypt(ztm, rc4key);
    
    cJSON *cjson = cJSON_Parse(abcdstr);
    int code = cJSON_GetObjectItem(cjson, "code")->valueint;
    int time = cJSON_GetObjectItem(cjson, "time")->valueint;
    char *msg = cJSON_GetObjectItem(cjson, "msg")->valuestring;
    char *check = cJSON_GetObjectItem(cjson, "check")->valuestring;
    if (code == 975)
    {                           // 200是你的状态码即是code
        if (time - ii > 30)
        {
            printf("error");
            exit(0);
        }
        if (time - ii < -30)
        {
            printf("error");
            exit(0);
        }
        // md5验证签名
        char *aaaa = weiyan;
        unsigned char *bbbb = (unsigned char *)aaaa;
        MD5_CTX md5c;
        MD5Init(&md5c);
        int i;
        unsigned char decrypt[16];
        MD5Update(&md5c, bbbb, strlen((char *)bbbb));
        MD5Final(&md5c, decrypt);
        char ykey[32] = { 0 };
        for (i = 0; i < 16; i++)
        {
            sprintf(&ykey[i * 2], "%02x", decrypt[i]);
        }
        // ---------------------------------------------------------
        if (check = ykey)
        {
            printf("登录成功\n"); // 这里放可执行代码
            return 0;
        } else {
            exit(0);
        }
    }
    else
    {
        printf("Error:%s", msg);// 这里是错误提示，正式环境可删掉    
    }
	exit(0);
}

