package libruncxx

import (
	"libsupcxx/RULES/crosscc/crosslib"
)

var CrossLib = crosslib.CrossLibrary{
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
