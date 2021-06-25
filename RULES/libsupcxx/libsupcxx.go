package libsupcxx

import (
	"fmt"
	"os"

	"dbt-rules/RULES/cc"
	"dbt-rules/RULES/core"

	"libsupcxx/RULES/libsupcxx/xlib"
	"libsupcxx/RULES/target"
	"libsupcxx/libruncxx"
	libsupcxx "libsupcxx/libsupcxx/src"
)

// XLibrary and XBinary are cross-compilation rules. Each XLibrary / XBinary instance defines rules
// for multiple targets (given in the Targets field, defaults to all available targets).
// When the XLibrary / XBinary is built, it is compiled for all its targets (internally, each
// goes inside its own subdirectory). Other rules can access a specific XLibrary / XBinary build
// with the ForTarget(target.Target) function. XLibraries / XBinares can depend on XLibraries,
// so that the dependency is built for the same target as the XLibrary / XBinary that uses it.
//
// XBinary describes a full executable, so it is also linked with the boot libraries

type XLibrary = xlib.XLibrary

type XBinary struct {
	Out           core.OutPath
	Srcs          []core.Path
	CompilerFlags []string
	XDeps         []XLibrary
	Script        core.Path
	Targets       []target.Target
}

func (xBin XBinary) targets() []target.Target {
	if xBin.Targets == nil {
		return target.AllTargets()
	}
	return xBin.Targets
}

func (xBin XBinary) ForTarget(t target.Target) cc.Binary {
	if xBin.Targets != nil && !xlib.ContainsTarget(xBin.Targets, t) {
		fmt.Printf("XBinary %s does not support target %s", xBin.Out, t.Name)
		os.Exit(1)
	}
	return xBin.forTarget(t)
}

func (xBin XBinary) forTarget(t target.Target) cc.Binary {
	xDeps := []XLibrary{libruncxx.XLib, libsupcxx.XLib}
	xDeps = append(xDeps, xBin.XDeps...)

	deps := []cc.Dep{t.BootLast}
	deps = append(deps, xlib.AllForTarget(xDeps, t)...)
	deps = append(deps, t.BootFirst)

	return cc.Binary{
		Out:           xlib.OutPathForTarget(xBin.Out, t),
		Srcs:          xBin.Srcs,
		Deps:          deps,
		CompilerFlags: xBin.CompilerFlags,
		Script:        t.LinkerScript,
		Toolchain:     t.GccToolchain,
	}
}

func hexOutForElf(elfOut core.OutPath) core.OutPath {
	return elfOut.WithExt("hex")
}

func (xBin XBinary) Build(ctx core.Context) {
	for _, t := range xBin.targets() {
		ccBin := xBin.forTarget(t)
		ccBin.Build(ctx)
		ctx.AddBuildStep(core.BuildStep{
			Out: hexOutForElf(ccBin.Out),
			In:  ccBin.Out,
			Cmd: fmt.Sprintf(
				"%q -O binary %q %q",
				t.GccToolchain.Objcopy,
				ccBin.Out,
				hexOutForElf(ccBin.Out),
			),
			Descr: fmt.Sprintf("Creating raw binary %s", hexOutForElf(ccBin.Out)),
		})
	}
}

func (xBin XBinary) Outputs() []core.Path {
	outputs := []core.Path{}
	for _, t := range xBin.targets() {
		elfOut := xlib.OutPathForTarget(xBin.Out, t)
		outputs = append(outputs, elfOut, hexOutForElf(elfOut))
	}
	return outputs
}
