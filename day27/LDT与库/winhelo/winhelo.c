#include "..\apilib.h"
char buf[150 * 50];
void HariMain(void)
{
	int win;
	win = api_openwin(buf, 150, 50, -1, "hello");
	for (;;) { /*�Ӵ˿�ʼ*/
		if (api_getkey(1) == 0x0a) {
			break; /*���»س�����break; */
		}
	} /*���˽���*/
	api_end();
}