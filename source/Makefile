TOOL = ../tolset/z_tools
BUILD = ../build
MAKE = $(TOOL)/make

default :
	$(MAKE) img

ipl.bin : ipl.nas
	$(TOOL)/nask.exe ipl.nas ipl.bin ipl.lst

linos.img : ipl.bin
	$(TOOL)/edimg.exe imgin:$(TOOL)/fdimg0at.tek \
		wbinimg src:ipl.bin len:512 from:0 to:0 imgout:linos.img

img :
	$(MAKE) -r linos.img

asm :
	$(MAKE) -r ipl.bin

build :
	$(MAKE) img
	copy linos.img ..\build\linos.bin

run :
	$(MAKE) build
	$(MAKE) -C $(TOOL)/qemu

clean :
	-del ipl.bin
	-del ipl.lst
	-del linos.img

src_only :
	$(MAKE) clean
	-del ..\build\linos.bin
