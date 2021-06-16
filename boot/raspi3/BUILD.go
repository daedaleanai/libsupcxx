package raspi3

import (
	"dbt-rules/RULES/core"
	"dbt-rules/RULES/util"

	gcc "aarch64-elf-gcc"
	"libsupcxx/RULES/boot"
	"libsupcxx/RULES/config"
	"libsupcxx/RULES/libsupcxx/lib"
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
		"setup.cc",
	),
	Objs:          []core.Path{crt2.To},
	AlwaysLink:    true,
	OnlyForConfig: config.RasPi3,
}

var bootLast = lib.Library{
	Out:           out("libboot_last.a"),
	Srcs:          ins("io.cc"),
	Objs:          []core.Path{crt3.To},
	AlwaysLink:    true,
	OnlyForConfig: config.RasPi3,
}

var Boot = boot.Boot{
	BootFirst:    bootFirst,
	BootLast:     bootLast,
	LinkerScript: in("kernel.ld"),
}
