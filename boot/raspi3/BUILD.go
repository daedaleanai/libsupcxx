package raspi3

import (
	"dbt-rules/RULES/cc"
	"dbt-rules/RULES/core"
	"dbt-rules/RULES/util"

	"libsupcxx/libsupcxx/include"

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

var bootFirst = cc.Library{
	Out: out("libboot_first.a"),
	Srcs: ins(
		"crt0.S",
		"setup.cc",
	),
	Objs:       []core.Path{crt2.To},
	AlwaysLink: true,
	Includes:   include.Headers,
	Toolchain:  gcc.Toolchain,
}

var bootLast = cc.Library{
	Out:        out("libboot_last.a"),
	Srcs:       ins("io.cc"),
	Objs:       []core.Path{crt3.To},
	AlwaysLink: true,
	Includes:   include.Headers,
	Toolchain:  gcc.Toolchain,
}

var Toolchain = gcc.Toolchain.NewWithStdLib(
	append(gcc.Toolchain.Includes, include.Headers...),
	[]cc.Dep{bootLast, bootFirst}, in("kernel.ld"),
	"raspi3-gcc",
).Register()
