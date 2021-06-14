package tests

import (
	"libsupcxx/RULES/libsupcxx"
	"libsupcxx/io"
)

var Test00 = libsupcxx.Binary{
	Out:  out("test-00-global-ctors-dtors.elf"),
	Srcs: ins("test-00-global-ctors-dtors.cc"),
	Deps: []libsupcxx.Dep{io.Lib},
}

var Test01 = libsupcxx.Binary{
	Out:  out("test-01-initializers-operators.elf"),
	Srcs: ins("test-01-initializers-operators.cc"),
	Deps: []libsupcxx.Dep{io.Lib},
}

var Test02 = libsupcxx.Binary{
	Out:  out("test-02-virtuals.elf"),
	Srcs: ins("test-02-virtuals.cc"),
	Deps: []libsupcxx.Dep{io.Lib},
}

var Test03 = libsupcxx.Binary{
	Out:  out("test-03-heap.elf"),
	Srcs: ins("test-03-heap.cc"),
	Deps: []libsupcxx.Dep{io.Lib},
}

var Test04 = libsupcxx.Binary{
	Out:  out("test-04-rtti.elf"),
	Srcs: ins("test-04-rtti.cc"),
	Deps: []libsupcxx.Dep{io.Lib},
}

var Test05 = libsupcxx.Binary{
	Out:  out("test-05-local-statics.elf"),
	Srcs: ins("test-05-local-statics.cc"),
	Deps: []libsupcxx.Dep{io.Lib},
}

var Test06 = libsupcxx.Binary{
	Out:  out("test-06-trivial-exception.elf"),
	Srcs: ins("test-06-trivial-exception.cc"),
	Deps: []libsupcxx.Dep{io.Lib},
}

var Test07 = libsupcxx.Binary{
	Out:  out("test-07-throw-clean-up-rethrow.elf"),
	Srcs: ins("test-07-throw-clean-up-rethrow.cc"),
	Deps: []libsupcxx.Dep{io.Lib},
}

var Test08 = libsupcxx.Binary{
	Out:  out("test-08-ccompat.elf"),
	Srcs: ins("test-08-ccompat.cc"),
	Deps: []libsupcxx.Dep{io.Lib},
}

var Test09 = libsupcxx.Binary{
	Out:  out("test-09-numeric-limits.elf"),
	Srcs: ins("test-09-numeric-limits.cc"),
	Deps: []libsupcxx.Dep{io.Lib},
}

var Test10 = libsupcxx.Binary{
	Out:  out("test-10-type-traits.elf"),
	Srcs: ins("test-10-type-traits.cc"),
	Deps: []libsupcxx.Dep{io.Lib},
}
