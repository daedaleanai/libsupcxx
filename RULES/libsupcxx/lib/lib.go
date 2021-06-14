package lib

import (
	"dbt-rules/RULES/cc"
	"dbt-rules/RULES/core"
	"libsupcxx/RULES/flags"
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
		Includes:      include.LibSupCxxHeaders,
		AlwaysLink:    lib.AlwaysLink,
		Toolchain:     flags.Toolchain(),
	}
}

func (lib Library) Build(ctx core.Context) {
	if lib.OnlyForConfig == "" || lib.OnlyForConfig == flags.Target.Value() {
		lib.CcLibrary().Build(ctx)
	}
}
