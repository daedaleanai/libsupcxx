package linux_x86_64

import (
	"dbt-rules/RULES/cc"
	"dbt-rules/RULES/core"
	"dbt-rules/RULES/util"

	"libsupcxx/RULES/target"
	"libsupcxx/libsupcxx/include"

	gcc "x86_64-elf-gcc"
)

var crt2 = util.CopyFile{
	From: gcc.CrtBegin,
	To:   out("crt2.o"),
}

var crt3 = util.CopyFile{
	From: gcc.CrtEnd,
	To:   out("crt3.o"),
}

var bootFirst = cc.Library{
	Out: out("libboot_first.a"),
	Srcs: ins(
		"crt0.S",
		"crt1.S",
		"setup.cc",
		"io.cc",
	),
	Objs:       []core.Path{crt2.To},
	AlwaysLink: true,
	Includes:   include.Headers,
	Toolchain:  gcc.Toolchain,
}

var bootLast = cc.Library{
	Out: out("libboot_last.a"),
	Srcs: ins(
		"crt4.S",
	),
	Objs:       []core.Path{crt3.To},
	AlwaysLink: true,
	Includes:   include.Headers,
	Toolchain:  gcc.Toolchain,
}

var Target = target.Target{
	Name:         "linux-x86_64",
	BootFirst:    bootFirst,
	BootLast:     bootLast,
	LinkerScript: in("kernel.ld"),
	GccToolchain: gcc.Toolchain,
}.Register()
