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
	mv linos.img $(BUILD)/linos.bin

run :
	$(MAKE) build
	$(MAKE) -C $(TOOL)/qemu

clean :
	-rm ipl.bin
	-rm ipl.lst

src_only :
	$(MAKE) clean
	-rm $(BUILD)/linos.bin
