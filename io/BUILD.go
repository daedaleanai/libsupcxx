package io

import (
	"libsupcxx/RULES/libsupcxx/lib"
)

var Lib = lib.Library{
	Out: out("libio.a"),
	Srcs: ins(
		"printf.cc",
		"io.cc",
		"string.cc",
	),
	AlwaysLink: false,
}
