package io

import (
	"libsupcxx/RULES/libsupcxx/xlib"
)

var XLib = xlib.XLibrary{
	Out: out("libio.a"),
	Srcs: ins(
		"printf.cc",
		"io.cc",
	),
}
