package lib

import (
	"dbt-rules/RULES/cc"
	"dbt-rules/RULES/core"
	"libsupcxx/RULES/config"
	"libsupcxx/libsupcxx/include"
)

type Dep = cc.Dep

type Library struct {
	Out           core.OutPath
	Srcs          []core.Path
	Objs          []core.Path
	CompilerFlags []string
	Deps          []Dep
	AlwaysLink    bool
	OnlyForConfig string
}

func (lib Library) CcLibrary() cc.Library {
	return cc.Library{
		Out:           lib.Out,
		Srcs:          lib.Srcs,
		Objs:          lib.Objs,
		CompilerFlags: lib.CompilerFlags,
		Deps:          lib.Deps,
		Includes:      include.Headers,
		AlwaysLink:    lib.AlwaysLink,
		Toolchain:     config.Toolchain(),
	}
}

func (lib Library) Build(ctx core.Context) {
	if lib.OnlyForConfig == "" || lib.OnlyForConfig == config.Target.Value() {
		lib.CcLibrary().Build(ctx)
	}
}
