package target

import (
	"dbt-rules/RULES/cc"
	"dbt-rules/RULES/core"
)

type Target struct {
	Name         string
	BootFirst    cc.Library
	BootLast     cc.Library
	LinkerScript core.Path
	GccToolchain cc.GccToolchain
}

var targets = []Target{}

func (target Target) Register() Target {
	targets = append(targets, target)
	return target
}

func AllTargets() []Target {
	return targets
}
