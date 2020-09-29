TOOL 	= ../tolset/z_tools
MAKE 	= $(TOOL)/make.exe
NASK 	= $(TOOL)/nask.exe
EDIMG 	= $(TOOL)/edimg.exe
IMGTOL	= $(TOOL)/imgtol.com

COPY 	= copy
DEL		= del

default :
	$(MAKE) img

ipl10.bin : ipl10.nas
	$(NASK) ipl10.nas ipl10.bin ipl10.lst

linos.sys : linos.nas
	$(NASK) linos.nas linos.sys linos.lst

linos.img : ipl10.bin linos.sys
	$(EDIMG) imgin:$(TOOL)/fdimg0at.tek \
		wbinimg src:ipl10.bin len:512 from:0 to:0 \
		copy from:linos.sys to:@: \
		imgout:linos.img

img :
	$(MAKE) -r linos.img

asm :
	$(MAKE) -r ipl10.bin

build :
	$(MAKE) img
	$(COPY) linos.img linos.bin

run :
	$(MAKE) build
	$(MAKE) -C $(TOOL)/qemu

clean :
	-$(DEL) ipl10.bin
	-$(DEL) ipl10.lst
	-$(DEL) linos.lst
	-$(DEL) linos.img
	-$(DEL) linos.sys

src_only :
	$(MAKE) clean
	-$(DEL) linos.bin
