    .code64
    .extern     environ
    .type       environ,        @object
    .extern     main
    .type       main,           @function
    .extern     _init_signal
    .type       _init_signal,   @function
    .section    .text
    .global     _start
    .type       _start,     @function
_start:
    movq        %rdx,       environ
    pushq       %rbp
    movq        %rsp,       %rbp
    pushq       %rdi
    pushq       %rsi
    call        _init
    call        _init_signal
    popq        %rsi
    popq        %rdi
    call        main
    movq        %rax,       %rdi
    jmp         _exit
    .size       _start,     .-_start
