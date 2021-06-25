package tests

import (
	"libsupcxx/RULES/libsupcxx"
	"libsupcxx/io"
)

var Test00 = libsupcxx.XBinary{
	Out:   out("test-00-global-ctors-dtors.elf"),
	Srcs:  ins("test-00-global-ctors-dtors.cc"),
	XDeps: []libsupcxx.XLibrary{io.XLib},
}

var Test01 = libsupcxx.XBinary{
	Out:   out("test-01-initializers-operators.elf"),
	Srcs:  ins("test-01-initializers-operators.cc"),
	XDeps: []libsupcxx.XLibrary{io.XLib},
}

var Test02 = libsupcxx.XBinary{
	Out:   out("test-02-virtuals.elf"),
	Srcs:  ins("test-02-virtuals.cc"),
	XDeps: []libsupcxx.XLibrary{io.XLib},
}

var Test03 = libsupcxx.XBinary{
	Out:   out("test-03-heap.elf"),
	Srcs:  ins("test-03-heap.cc"),
	XDeps: []libsupcxx.XLibrary{io.XLib},
}

var Test04 = libsupcxx.XBinary{
	Out:   out("test-04-rtti.elf"),
	Srcs:  ins("test-04-rtti.cc"),
	XDeps: []libsupcxx.XLibrary{io.XLib},
}

var Test05 = libsupcxx.XBinary{
	Out:   out("test-05-local-statics.elf"),
	Srcs:  ins("test-05-local-statics.cc"),
	XDeps: []libsupcxx.XLibrary{io.XLib},
}

var Test06 = libsupcxx.XBinary{
	Out:   out("test-06-trivial-exception.elf"),
	Srcs:  ins("test-06-trivial-exception.cc"),
	XDeps: []libsupcxx.XLibrary{io.XLib},
}

var Test07 = libsupcxx.XBinary{
	Out:   out("test-07-throw-clean-up-rethrow.elf"),
	Srcs:  ins("test-07-throw-clean-up-rethrow.cc"),
	XDeps: []libsupcxx.XLibrary{io.XLib},
}

var Test08 = libsupcxx.XBinary{
	Out:   out("test-08-ccompat.elf"),
	Srcs:  ins("test-08-ccompat.cc"),
	XDeps: []libsupcxx.XLibrary{io.XLib},
}

var Test09 = libsupcxx.XBinary{
	Out:   out("test-09-numeric-limits.elf"),
	Srcs:  ins("test-09-numeric-limits.cc"),
	XDeps: []libsupcxx.XLibrary{io.XLib},
}

var Test10 = libsupcxx.XBinary{
	Out:   out("test-10-type-traits.elf"),
	Srcs:  ins("test-10-type-traits.cc"),
	XDeps: []libsupcxx.XLibrary{io.XLib},
}
