### DARWIN ###
all: as
	ld -o darwin_shell darwin_shell.o -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _start -arch arm64
	ld -o darwin_bindshell darwin_bindshell.o -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _start -arch arm64
as: darwin_shell.s darwin_bindshell.s
	as -arch arm64 -o darwin_shell.o darwin_shell.s
	as -arch arm64 -o darwin_bindshell.o darwin_bindshell.s
##############