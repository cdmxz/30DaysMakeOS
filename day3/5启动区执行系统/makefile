TOOLPATH = ../../../../tolset/z_tools/
MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com

# 默认操作

default :
	$(MAKE) src_only

# 文件生成规则

ipl_1.bin : ipl_1.asm Makefile
	$(NASK) ipl_1.asm ipl_1.bin ipl_1.lst

haribote.sys : haribote.asm Makefile
	$(NASK) haribote.asm haribote.sys haribote.lst

haribote.img : ipl_1.bin haribote.sys Makefile
	$(EDIMG) imgin:$(TOOLPATH)fdimg0at.tek \
		wbinimg src:ipl_1.bin len:512 from:0 to:0 \
		copy from:haribote.sys to:@: \
		imgout:haribote.img

# 命令
asm :
	$(MAKE)make.exe -r ipl_1.bin

img :
	$(MAKE) haribote.img

run :
	$(MAKE) img
	copy haribote.img ..\..\..\..\tolset\z_tools\qemu\fdimage0.bin
	$(MAKE) -C $(TOOLPATH)qemu

install :
	$(MAKE) img
	$(TOOLPATH)imgtol.com w a: haribote.img

clean :
	-del ipl_1.bin
	-del ipl_1.lst
	-del haribote.sys
	-del haribote.lst

src_only :
	$(MAKE) clean
	-del haribote.img
