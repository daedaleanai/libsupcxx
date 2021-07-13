package crosslib

import (
	"path"

	"dbt-rules/RULES/cc"
	"dbt-rules/RULES/core"
)

// This package is only meant to be used within libsupcxx, outside code should use
// libsupcxx/RULES/libsupcxx. See there for info about CrossLibrary.
type CrossLibrary struct {
	Out           core.OutPath
	Srcs          []core.Path
	Includes      []core.Path
	CompilerFlags []string
	CrossDeps     []CrossLibrary
	CrossObjs     []CrossObject
	Shared        bool
	AlwaysLink    bool
	Toolchains    []cc.Toolchain
}

type CrossObject interface {
	WithToolchain(p cc.Toolchain) core.OutPath
}

func OutPathWithToolchain(base core.OutPath, toolchain cc.Toolchain) core.OutPath {
	return base.WithPrefix("/" + path.Base(base.Relative()) + "-" + toolchain.Name() + "/")
}

func (xLib CrossLibrary) toolchains() []cc.Toolchain {
	if xLib.Toolchains == nil {
		return cc.AllToolchains()
	}
	return xLib.Toolchains
}

func AllLibsWithToolchain(xLibs []CrossLibrary, toolchain cc.Toolchain) []cc.Dep {
	libs := make([]cc.Dep, len(xLibs))
	for i, xLib := range xLibs {
		libs[i] = xLib.WithToolchain(toolchain)
	}
	return libs
}

func AllObjsWithToolchain(xObjs []CrossObject, toolchain cc.Toolchain) []core.Path {
	paths := make([]core.Path, len(xObjs))
	for i, xObj := range xObjs {
		paths[i] = xObj.WithToolchain(toolchain)
	}
	return paths
}

func (xLib CrossLibrary) WithToolchain(toolchain cc.Toolchain) cc.Library {
	if xLib.Toolchains != nil && !cc.ContainsToolchain(xLib.Toolchains, toolchain) {
		core.Fatal("CrossLibrary %s does not support platform %s", xLib.Out, toolchain.Name)
	}
	return xLib.withToolchain(toolchain)
}

func (xLib CrossLibrary) withToolchain(toolchain cc.Toolchain) cc.Library {
	return cc.Library{
		Out:           OutPathWithToolchain(xLib.Out, toolchain),
		Srcs:          xLib.Srcs,
		Objs:          AllObjsWithToolchain(xLib.CrossObjs, toolchain),
		Includes:      xLib.Includes,
		CompilerFlags: xLib.CompilerFlags,
		Deps:          AllLibsWithToolchain(xLib.CrossDeps, toolchain),
		Shared:        xLib.Shared,
		AlwaysLink:    xLib.AlwaysLink,
		Toolchain:     toolchain,
	}
}

func (xLib CrossLibrary) Build(ctx core.Context) {
	for _, toolchain := range xLib.toolchains() {
		xLib.withToolchain(toolchain).Build(ctx)
	}
}
