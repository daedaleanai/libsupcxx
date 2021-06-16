package x86_64

import (
	"dbt-rules/RULES/core"
	"dbt-rules/RULES/util"

	"libsupcxx/RULES/boot"
	"libsupcxx/RULES/config"
	"libsupcxx/RULES/libsupcxx/lib"
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

var bootFirst = lib.Library{
	Out: out("libboot_first.a"),
	Srcs: ins(
		"crt0.S",
		"crt1.S",
		"multiboot.cc",
		"setup.cc",
	),
	Objs:          []core.Path{crt2.To},
	AlwaysLink:    true,
	OnlyForConfig: config.X86_64,
}

var bootLast = lib.Library{
	Out: out("libboot_last.a"),
	Srcs: ins(
		"io.cc",
		"crt4.S",
	),
	Objs:          []core.Path{crt3.To},
	AlwaysLink:    true,
	OnlyForConfig: config.X86_64,
}

var Boot = boot.Boot{
	BootFirst:    bootFirst,
	BootLast:     bootLast,
	LinkerScript: in("kernel.ld"),
}
