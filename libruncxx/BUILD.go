package libruncxx

import (
	"dbt-rules/RULES/cc"
)

var Lib = cc.Library{
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
	AlwaysLink: true,
}.MultipleToolchains()
