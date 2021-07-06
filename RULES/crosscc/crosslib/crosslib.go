package crosslib

import (
	"path"

	"dbt-rules/RULES/cc"
	"dbt-rules/RULES/core"

	"libsupcxx/RULES/platform"
	"libsupcxx/libsupcxx/include"
)

// This package is only meant to be used within libsupcxx, outside code should use
// libsupcxx/RULES/libsupcxx. See there for info about CrossLibrary.
type CrossLibrary struct {
	Out           core.OutPath
	Srcs          []core.Path
	Includes      []core.Path
	CompilerFlags []string
	XDeps         []CrossLibrary
	Shared        bool
	AlwaysLink    bool
	Platforms     []platform.Platform
}

func OutPathForPlatform(base core.OutPath, p platform.Platform) core.OutPath {
	return base.WithPrefix("/" + path.Base(base.Relative()) + "-" + p.Name + "/")
}

func (xLib CrossLibrary) platforms() []platform.Platform {
	if xLib.Platforms == nil {
		return platform.AllPlatforms()
	}
	return xLib.Platforms
}

func AllForPlatform(xLibs []CrossLibrary, p platform.Platform) []cc.Dep {
	libs := make([]cc.Dep, len(xLibs))
	for i, xLib := range xLibs {
		libs[i] = xLib.ForPlatform(p)
	}
	return libs
}

func (xLib CrossLibrary) ForPlatform(p platform.Platform) cc.Library {
	if xLib.Platforms != nil && !platform.ContainsPlatform(xLib.Platforms, p) {
		core.Fatal("CrossLibrary %s does not support platform %s", xLib.Out, p.Name)
	}
	return xLib.forPlatform(p)
}

func (xLib CrossLibrary) forPlatform(p platform.Platform) cc.Library {
	return cc.Library{
		Out:           OutPathForPlatform(xLib.Out, p),
		Srcs:          xLib.Srcs,
		Includes:      append(xLib.Includes, include.Headers...),
		CompilerFlags: xLib.CompilerFlags,
		Deps:          AllForPlatform(xLib.XDeps, p),
		Shared:        xLib.Shared,
		AlwaysLink:    xLib.AlwaysLink,
		Toolchain:     p.GccToolchain,
	}
}

func (xLib CrossLibrary) Build(ctx core.Context) {
	for _, p := range xLib.platforms() {
		xLib.forPlatform(p).Build(ctx)
	}
}
