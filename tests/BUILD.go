package tests

import (
	"libsupcxx/RULES/crosscc"
)

var Test00 = crosscc.CrossBinary{
	Out:  out("test-00-global-ctors-dtors.elf"),
	Srcs: ins("test-00-global-ctors-dtors.cc"),
}

var Test01 = crosscc.CrossBinary{
	Out:  out("test-01-initializers-operators.elf"),
	Srcs: ins("test-01-initializers-operators.cc"),
}

var Test02 = crosscc.CrossBinary{
	Out:  out("test-02-virtuals.elf"),
	Srcs: ins("test-02-virtuals.cc"),
}

var Test03 = crosscc.CrossBinary{
	Out:  out("test-03-heap.elf"),
	Srcs: ins("test-03-heap.cc"),
}

var Test04 = crosscc.CrossBinary{
	Out:  out("test-04-rtti.elf"),
	Srcs: ins("test-04-rtti.cc"),
}

var Test05 = crosscc.CrossBinary{
	Out:  out("test-05-local-statics.elf"),
	Srcs: ins("test-05-local-statics.cc"),
}

var Test06 = crosscc.CrossBinary{
	Out:  out("test-06-trivial-exception.elf"),
	Srcs: ins("test-06-trivial-exception.cc"),
}

var Test07 = crosscc.CrossBinary{
	Out:  out("test-07-throw-clean-up-rethrow.elf"),
	Srcs: ins("test-07-throw-clean-up-rethrow.cc"),
}

var Test08 = crosscc.CrossBinary{
	Out:  out("test-08-ccompat.elf"),
	Srcs: ins("test-08-ccompat.cc"),
}

var Test09 = crosscc.CrossBinary{
	Out:  out("test-09-numeric-limits.elf"),
	Srcs: ins("test-09-numeric-limits.cc"),
}

var Test10 = crosscc.CrossBinary{
	Out:  out("test-10-type-traits.elf"),
	Srcs: ins("test-10-type-traits.cc"),
}
