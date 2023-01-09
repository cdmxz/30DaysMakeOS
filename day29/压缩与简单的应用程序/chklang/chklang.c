#include "apilib.h"

void HariMain(void)
{
    int langmode = api_getlang();
    static char s1[20] = {
        0xce, 0xd2, 0x20, 0xca, 0xc7, 0xa1, 0xa2, 0xa1, 0xa3, 0x0a, 0x00
    };
    int i; char j;
    if (langmode == LANGMODE_ENGLISH) {
        api_putstr0("English ASCII mode\n");
    }
    if (langmode == LANGMODE_CHINESE) {// 支持中文
        api_putstr0("Chinese 支持中文！\n");
        api_putstr0(s1);
        for (i = 0xa1; i < 0xcc; i++)
        {
            s1[0] = 0xa1;
            j = i;
            s1[1] = j;
            s1[2] = 0x00;
            api_putstr0(s1);
        }
    }
    api_end();
}