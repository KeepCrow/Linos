TOOL = ../tolset/z_tools
MAKE = $(TOOL)/make

win_BUILD = ..\build

default :
	$(MAKE) img

ipl.bin : ipl.nas
	$(TOOL)/nask.exe ipl.nas $(win_BUILD)\ipl.bin $(win_BUILD)\ipl.lst

linos.img : ipl.bin
	$(TOOL)/edimg.exe imgin:$(TOOL)/fdimg0at.tek \
		wbinimg src:../build/ipl.bin len:512 from:0 to:0 imgout:../build/linos.img

img :
	$(MAKE) -r linos.img

asm :
	$(MAKE) -r ipl.bin

build :
	$(MAKE) img
	copy $(win_BUILD)\linos.img $(win_BUILD)\linos.bin

run :
	$(MAKE) build
	$(MAKE) -C $(TOOL)/qemu

clean :
	-del $(win_BUILD)\ipl.bin
	-del $(win_BUILD)\ipl.lst
	-del $(win_BUILD)\linos.img

src_only :
	$(MAKE) clean
	-del $(win_BUILD)\linos.bin
