
.section .init
.global _init
.type _init, @function
_init:
  pushq %rbp
  movq %rsp, %rbp

.section .fini
.global _fini
.type _fini, @function
_fini:
  pushq %rbp
  movq %rsp, %rbp
