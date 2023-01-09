void api_putchar(int c);
void api_end(void);
void HariMain(void)
{
	*((char*)0x00102600) = 0;
	api_putchar('b');
	api_putchar('o');
	api_putchar('o');
	api_putchar('m');
	api_end();
	return;
}
