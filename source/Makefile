TOOL 	= ../tolset/z_tools
MAKE 	= $(TOOL)/make.exe
NASK 	= $(TOOL)/nask.exe
EDIMG 	= $(TOOL)/edimg.exe
IMGTOL	= $(tool)/imgtol.com
COPY	= copy
DEL 	= del

default :
	$(MAKE) img

ipl.bin : ipl.nas
	$(NASK) ipl.nas ipl.bin ipl.lst

linos.sys : linos.nas
	$(NASK) linos.nas linos.lst

linos.img : ipl.bin
	$(EDIMG) imgin:$(TOOL)/fdimg0at.tek \
		wbinimg src:ipl.bin len:512 from:0 to:0 \
		imgout:linos.img

img :
	$(MAKE) -r linos.img

asm :
	$(MAKE) -r ipl.bin

build :
	$(MAKE) init
	$(MAKE) img
	COPY linos.img linos.bin

run :
	$(MAKE) build
	$(MAKE) -C $(TOOL)/qemu

clean :
	-DEL ipl.bin
	-DEL ipl.lst
	-DEL linos.img
	-DEl linos.lst
	-DEL linos.sys

src_only :
	$(MAKE) clean
	-DEL linos.bin
