#include "..\apilib.h"
char buf[150 * 50];
void HariMain(void)
{
	int win;
	win = api_openwin(buf, 150, 50, -1, "hello");
	for (;;) { /*从此开始*/
		if (api_getkey(1) == 0x0a) {
			break; /*按下回车键则break; */
		}
	} /*到此结束*/
	api_end();
}