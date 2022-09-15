### LINUX  ###
lall: linux_shell.s linux_bindshell.s
	as -o linux_shell.o linux_shell.s && ld -o linux_shell linux_shell.o
	as -o linux_bindshell.o linux_bindshell.s && ld -o linux_bindshell linux_bindshell.o
	objcopy -O binary linux_shell linux_shell.bin
	objcopy -O binary linux_bindshell linux_bindshell.bin
# hexdump -v -e '"\\""x" 1/1 "%02x" ""' linux_shell.bin && echo
# hexdump -v -e '"\\""x" 1/1 "%02x" ""' linux_bindshell.bin && echo
### DARWIN ###
dall: as
	ld -o darwin_shell darwin_shell.o -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _start -arch arm64
	ld -o darwin_bindshell darwin_bindshell.o -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _start -arch arm64
	gobjcopy -O binary darwin_shell darwin_shell.bin
	gobjcopy -O binary darwin_bindshell darwin_bindshell.bin
das: darwin_shell.s darwin_bindshell.s
	as -arch arm64 -o darwin_shell.o darwin_shell.s
	as -arch arm64 -o darwin_bindshell.o darwin_bindshell.s
##############