package crosscc

import (
	"fmt"

	"dbt-rules/RULES/cc"
	"dbt-rules/RULES/core"

	"libsupcxx/RULES/crosscc/crosslib"
	"libsupcxx/RULES/platform"
	"libsupcxx/libruncxx"
	libsupcxx "libsupcxx/libsupcxx/src"
)

// CrossLibrary and CrossBinary are cross-compilation rules. Each CrossLibrary / CrossBinary instance defines rules
// for multiple platforms (given in the Platforms field, defaults to all available platforms).
// When the CrossLibrary / CrossBinary is built, it is compiled for all its platforms (internally, each
// goes inside its own subdirectory). Other rules can access a specific CrossLibrary / CrossBinary build
// with the ForPlatform(platform.Platform) function. XLibraries / XBinares can depend on XLibraries,
// so that the dependency is built for the same platform as the CrossLibrary / CrossBinary that uses it.
//
// CrossBinary describes a full executable, so it is also linked with the boot libraries

type CrossLibrary = crosslib.CrossLibrary

type CrossBinary struct {
	Out           core.OutPath
	Srcs          []core.Path
	CompilerFlags []string
	XDeps         []CrossLibrary
	Script        core.Path
	Platforms     []platform.Platform
}

func (xBin CrossBinary) platforms() []platform.Platform {
	if xBin.Platforms == nil {
		return platform.AllPlatforms()
	}
	return xBin.Platforms
}

func (xBin CrossBinary) ForPlatform(p platform.Platform) cc.Binary {
	if xBin.Platforms != nil && !platform.ContainsPlatform(xBin.Platforms, p) {
		core.Fatal("CrossBinary %s does not support platform %s", xBin.Out, p.Name)
	}
	return xBin.forPlatform(p)
}

func (xBin CrossBinary) forPlatform(p platform.Platform) cc.Binary {
	xDeps := []CrossLibrary{libruncxx.XLib, libsupcxx.XLib}
	xDeps = append(xDeps, xBin.XDeps...)

	deps := []cc.Dep{p.BootLast}
	deps = append(deps, crosslib.AllForPlatform(xDeps, p)...)
	deps = append(deps, p.BootFirst)

	return cc.Binary{
		Out:           crosslib.OutPathForPlatform(xBin.Out, p),
		Srcs:          xBin.Srcs,
		Deps:          deps,
		CompilerFlags: xBin.CompilerFlags,
		Script:        p.LinkerScript,
		Toolchain:     p.GccToolchain,
	}
}

func hexOutForElf(elfOut core.OutPath) core.OutPath {
	return elfOut.WithExt("hex")
}

func (xBin CrossBinary) Build(ctx core.Context) {
	for _, p := range xBin.platforms() {
		ccBin := xBin.forPlatform(p)
		ccBin.Build(ctx)
		ctx.AddBuildStep(core.BuildStep{
			Out: hexOutForElf(ccBin.Out),
			In:  ccBin.Out,
			Cmd: fmt.Sprintf(
				"%q -O binary %q %q",
				p.GccToolchain.Objcopy,
				ccBin.Out,
				hexOutForElf(ccBin.Out),
			),
			Descr: fmt.Sprintf("Creating raw binary %s", hexOutForElf(ccBin.Out)),
		})
	}
}

func (xBin CrossBinary) Outputs() []core.Path {
	outputs := []core.Path{}
	for _, p := range xBin.platforms() {
		elfOut := crosslib.OutPathForPlatform(xBin.Out, p)
		outputs = append(outputs, elfOut, hexOutForElf(elfOut))
	}
	return outputs
}
