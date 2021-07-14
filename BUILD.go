package libsupcxx

import (
	"dbt-rules/RULES/cc"
	"libsupcxx/io"
	"libsupcxx/libruncxx"
	libsupcxx_src "libsupcxx/libsupcxx/src"
)

var CommonLibs = []cc.Dep{
	libruncxx.Lib,
	libsupcxx_src.Lib,
	io.Lib,
}
