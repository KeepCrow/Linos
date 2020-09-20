copy linos.img ..\build\linos.bin
:: 前半部分用'\'是因为在Windows下使用路径
:: 而后半部分使用'/'是因为要将路径当作参数传给make，而make无法识别'\'
..\tolset\z_tools\make.exe -C ../tolset/z_tools/qemu