package x86_64

import (
	"dbt-rules/RULES/cc"
	"dbt-rules/RULES/core"
	"dbt-rules/RULES/util"

	"libsupcxx"
	"libsupcxx/libsupcxx/include"

	gcc "x86_64-elf-gcc"
)

var bootToolchain = gcc.Toolchain.NewWithStdLib(
	append(gcc.Toolchain.Includes, include.Headers...),
	libsupcxx.CommonLibs,
	in("kernel.ld"),
	"x86_64-libsupcxx",
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
		"multiboot.cc",
		"setup.cc",
	),
	Objs:       []core.Path{crt2.To},
	AlwaysLink: true,
	Toolchain:  bootToolchain,
}

var bootLast = cc.Library{
	Out: out("libboot_last.a"),
	Srcs: ins(
		"io.cc",
		"crt4.S",
	),
	Objs:       []core.Path{crt3.To},
	AlwaysLink: true,
	Toolchain:  bootToolchain,
}

var Toolchain = bootToolchain.NewWithStdLib(
	bootToolchain.Includes,
	append(bootToolchain.StdDeps(), bootLast, bootFirst),
	bootToolchain.LinkerScript,
	bootToolchain.Name(),
).Register()
