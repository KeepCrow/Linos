OBJS_BOOTPACK = bootpack.obj naskfunc.obj hankaku.obj graphic.obj dsctbl.obj \
				int.obj fifo.obj

TOOL 	= ../tolset/z_tools
INC 	= $(TOOL)/linos

MAKE 		= $(TOOL)/make.exe -r
NASK 		= $(TOOL)/nask.exe
CC1 		= $(TOOL)/cc1.exe -I$(INC) -Os -Wall -quiet
GAS2NASK	= $(TOOL)/gas2nask.exe -a
OBJ2BIM		= $(TOOL)/obj2bim.exe
MAKEFONT	= $(TOOL)/makefont.exe
BIN2OBJ		= $(TOOL)/bin2obj.exe
BIM2HRB		= $(TOOL)/bim2hrb.exe
RULEFILE	= $(TOOL)/linos/linos.rul
EDIMG 		= $(TOOL)/edimg.exe
IMGTOL		= $(TOOL)/imgtol.com
COPY 	= copy
DEL		= del

default :
	$(MAKE) run

ipl10.bin : ipl10.nas
	$(NASK) ipl10.nas ipl10.bin ipl10.lst

asmhead.bin : asmhead.nas
	$(NASK) asmhead.nas asmhead.bin asmhead.lst

hankaku.bin : hankaku.txt
	$(MAKEFONT) hankaku.txt hankaku.bin

hankaku.obj : hankaku.bin
	$(BIN2OBJ) hankaku.bin hankaku.obj _hankaku

bootpack.bim : $(OBJS_BOOTPACK)
	$(OBJ2BIM) @$(RULEFILE) out:bootpack.bim stack:3136k map:bootpack.map \
		$(OBJS_BOOTPACK)

bootpack.hrb : bootpack.bim
	$(BIM2HRB) bootpack.bim bootpack.hrb 0

linos.sys : asmhead.bin bootpack.hrb
	copy /B asmhead.bin+bootpack.hrb linos.sys

# 这一步的功能是拼装一个img出来，但具体怎么实现的不清楚
linos.img : ipl10.bin linos.sys
	$(EDIMG) imgin:$(TOOL)/fdimg0at.tek \
		wbinimg src:ipl10.bin len:512 from:0 to:0 \
		copy from:linos.sys to:@: \
		imgout:linos.img

%.gas : %.c
	$(CC1) -o $*.gas $*.c

%.nas : %.gas
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas
	$(NASK) $*.nas $*.obj $*.lst

naskfunc.obj : naskfunc.nas
	$(NASK) naskfunc.nas naskfunc.obj naskfunc.lst

img :
	$(MAKE) linos.img

build :
	$(MAKE) img
	$(COPY) linos.img linos.bin

run :
	$(MAKE) build
	$(MAKE) -C $(TOOL)/qemu

clean :
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.gas
	-$(DEL) *.obj
	-$(DEL) bootpack.nas
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.hrb
	-$(DEL) linos.sys

src_only :
	$(MAKE) clean
	-$(DEL) linos.img

