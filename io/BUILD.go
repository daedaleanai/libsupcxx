package io

import (
	"libsupcxx/RULES/crosscc/crosslib"
)

var CrossLib = crosslib.CrossLibrary{
	Out: out("libio.a"),
	Srcs: ins(
		"printf.cc",
		"io.cc",
	),
}
