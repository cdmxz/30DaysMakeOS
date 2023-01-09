TOOLPATH = ../../tolset/z_tools/
MAKE     = $(TOOLPATH)make.exe -r
NASK     = $(TOOLPATH)nask.exe
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com

# 默认操作

default :
	$(MAKE) src_only

# 文件生成规则

ipl.bin : ipl.asm Makefile
	$(NASK) ipl.asm ipl.bin ipl.lst

helloos.img : ipl.bin Makefile
	$(EDIMG) imgin:../../tolset/z_tools/fdimg0at.tek wbinimg src:ipl.bin len:512 from:0 to:0   imgout:helloos.img

# 命令
asm :
	$(MAKE)make.exe -r ipl.bin

img :
	$(MAKE) helloos.img

run :
	$(MAKE) img
	copy helloos.img ..\..\tolset\z_tools\qemu\fdimage0.bin
	$(MAKE) -C ../../tolset/z_tools/qemu

install :
	$(MAKE) img
	../../tolset/z_tools/imgtol.com w a: helloos.img

clean :
	-del ipl.bin
	-del ipl.lst

src_only :
	$(MAKE) clean
	-del helloos.img
