package raspi3

import (
	"dbt-rules/RULES/util"
	"dbt-rules/RULES/core"

	"libsupcxx/RULES/boot"
	"libsupcxx/RULES/flags"
	"libsupcxx/RULES/libsupcxx/lib"
	gcc "aarch64-elf-gcc"
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
	Out:  out("libboot_first.a"),
	Srcs: ins(
		"crt0.S",
		"setup.cc",
	),
	Objs: []core.Path{crt2.To},
	AlwaysLink: true,
	OnlyForConfig: flags.RasPi3,
}

var BootLast = lib.Library{
	Out:  out("libboot_last.a"),
	Srcs: ins("io.cc"),
	Objs: []core.Path{crt3.To},
	AlwaysLink: true,
	OnlyForConfig: flags.RasPi3,
}

var Boot = boot.Boot{
	BootFirst: BootFirst,
	BootLast: BootLast,
	LinkerScript : in("kernel.ld"),
}
