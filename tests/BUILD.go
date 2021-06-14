package tests

import (
	"libsupcxx/RULES/libsupcxx"
	"libsupcxx/io"
)

var GlobalCTorsDTors = libsupcxx.Binary{
	Out: out("test-00-global-ctors-dtors"),
	Srcs: ins("test-00-global-ctors-dtors.cc"),
	Deps: []libsupcxx.Dep{io.Lib},
}

var ThrowCleanUpRethrow = libsupcxx.Binary{
	Out: out("test-07-throw-clean-up-rethrow"),
	Srcs: ins("test-07-throw-clean-up-rethrow.cc"),
	Deps: []libsupcxx.Dep{io.Lib},
}
