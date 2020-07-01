#include "App.h"
#include "SimpleTest.h"

 /*
可以以命令行传入PkpmInfo.ini文件的绝对/相对路径。
若命令行找不到，程序找当前exe目录下的PkpmInfo.ini
程序依次尝试读取配置文件的Path项和PriorityVersion，并尝试打开
[AppInfo]
Path=D:\PKPMV52\PKPM2010V52.exe1
PriorityVersion=CFGV51
若没有找到配置文件，或者以上配置无效，程序将尝试读取注册表里最新的版本
若注册表里没有CFGV**的字样，程序将转到结构软件下载网页
*/

int main(int argc, char** argv)
{
	App app;
	return app.Run(argc, argv);
}