package boot

import (
	"dbt-rules/RULES/core"
	"libsupcxx/RULES/libsupcxx/lib"
)

type Boot struct {
	BootFirst    lib.Library
	BootLast     lib.Library
	LinkerScript core.Path
}
