package io

import (
	"dbt-rules/RULES/cc"
)

var Lib = cc.Library{
	Out: out("libio.a"),
	Srcs: ins(
		"printf.cc",
		"io.cc",
	),
}.MultipleToolchains()
