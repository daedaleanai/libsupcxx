package linux_x86_64

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

var BootFirst = lib.Library{
	Out: out("libboot_first.a"),
	Srcs: ins(
		"crt0.S",
		"crt1.S",
		"setup.cc",
		"io.cc",
	),
	Objs:          []core.Path{crt2.To},
	AlwaysLink:    true,
	OnlyForConfig: config.Linux_x86_64,
}

var BootLast = lib.Library{
	Out: out("libboot_last.a"),
	Srcs: ins(
		"crt4.S",
	),
	Objs:          []core.Path{crt3.To},
	AlwaysLink:    true,
	OnlyForConfig: config.Linux_x86_64,
}

var Boot = boot.Boot{
	BootFirst:    BootFirst,
	BootLast:     BootLast,
	LinkerScript: in("kernel.ld"),
}