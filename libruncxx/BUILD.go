package libruncxx

import (
	"libsupcxx/RULES/libsupcxx/xlib"
)

var XLib = xlib.XLibrary{
	Out: out("libruncxx.a"),
	Srcs: ins(
		"dtors.cc",
		"memory.cc",
		"guard.cc",
		"string.cc",
		"eh_alloc.cc",
		"eh_globals.cc",
		"exit.cc",
	),
}
