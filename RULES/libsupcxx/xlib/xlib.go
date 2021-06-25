package xlib

import (
	"fmt"
	"os"
	"path"

	"dbt-rules/RULES/cc"
	"dbt-rules/RULES/core"

	"libsupcxx/RULES/target"
	"libsupcxx/libsupcxx/include"
)

// This package is only meant to be used within libsupcxx, outside code should use
// libsupcxx/RULES/libsupcxx. See there for info about XLibrary.
type XLibrary struct {
	Out           core.OutPath
	Srcs          []core.Path
	Includes      []core.Path
	CompilerFlags []string
	XDeps         []XLibrary
	Shared        bool
	AlwaysLink    bool
	Targets       []target.Target
}

func ContainsTarget(ts []target.Target, t target.Target) bool {
	for _, el := range ts {
		if el.Name == t.Name {
			return true
		}
	}
	return false
}

func OutPathForTarget(base core.OutPath, t target.Target) core.OutPath {
	return base.WithPrefix("/" + path.Base(base.Absolute()) + "-" + t.Name + "/")
}

func (xLib XLibrary) targets() []target.Target {
	if xLib.Targets == nil {
		return target.AllTargets()
	}
	return xLib.Targets
}

func AllForTarget(xLibs []XLibrary, t target.Target) []cc.Dep {
	libs := make([]cc.Dep, len(xLibs))
	for i, xLib := range xLibs {
		libs[i] = xLib.ForTarget(t)
	}
	return libs
}
func (xLib XLibrary) ForTarget(t target.Target) cc.Library {
	if xLib.Targets != nil && !ContainsTarget(xLib.Targets, t) {
		fmt.Printf("XLibrary %s does not support target %s", xLib.Out, t.Name)
		os.Exit(1)
	}
	return cc.Library{
		Out:           OutPathForTarget(xLib.Out, t),
		Srcs:          xLib.Srcs,
		Includes:      append(xLib.Includes, include.Headers...),
		CompilerFlags: xLib.CompilerFlags,
		Deps:          AllForTarget(xLib.XDeps, t),
		Shared:        xLib.Shared,
		AlwaysLink:    xLib.AlwaysLink,
		Toolchain:     t.GccToolchain,
	}
}

func (xLib XLibrary) Build(ctx core.Context) {
	for _, t := range xLib.targets() {
		xLib.ForTarget(t).Build(ctx)
	}
}
