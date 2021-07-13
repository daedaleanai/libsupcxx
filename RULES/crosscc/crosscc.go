package crosscc

import (
	"fmt"

	"dbt-rules/RULES/cc"
	"dbt-rules/RULES/core"

	"libsupcxx/RULES/crosscc/crosslib"
	"libsupcxx/io"
	"libsupcxx/libruncxx"
	libsupcxx "libsupcxx/libsupcxx/src"
)

// CrossLibrary and CrossBinary are cross-compilation rules. Each CrossLibrary / CrossBinary instance defines rules
// for multiple toolchains (given in the Toolchains field, defaults to all available toolchains).
// When the CrossLibrary / CrossBinary is built, it is compiled for all its toolchains (internally, each
// goes inside its own subdirectory). Other rules can access a specific CrossLibrary / CrossBinary build
// with the WithToolchain(cc.Toolchain) function. XLibraries / XBinares can depend on XLibraries,
// so that the dependency is built for the same platform as the CrossLibrary / CrossBinary that uses it.
//
// CrossBinary describes a full executable, so it is also linked with the boot libraries

type CrossLibrary = crosslib.CrossLibrary
type CrossObject = crosslib.CrossObject

type CrossBinary struct {
	Out           core.OutPath
	Srcs          []core.Path
	CompilerFlags []string
	CrossDeps     []CrossLibrary
	Script        core.Path
	Toolchains    []cc.Toolchain
}

func (xBin CrossBinary) toolchains() []cc.Toolchain {
	if xBin.Toolchains == nil {
		return cc.AllToolchains()
	}
	return xBin.Toolchains
}

func (xBin CrossBinary) WithToolchain(toolchain cc.Toolchain) cc.Binary {
	if xBin.Toolchains != nil && !cc.ContainsToolchain(xBin.Toolchains, toolchain) {
		core.Fatal("CrossBinary %s does not support platform %s", xBin.Out, toolchain.Name)
	}
	return xBin.withToolchain(toolchain)
}

func (xBin CrossBinary) withToolchain(toolchain cc.Toolchain) cc.Binary {
	crossDeps := append(xBin.CrossDeps, libruncxx.CrossLib, libsupcxx.CrossLib, io.CrossLib)
	return cc.Binary{
		Out:           crosslib.OutPathWithToolchain(xBin.Out, toolchain),
		Srcs:          xBin.Srcs,
		Deps:          crosslib.AllLibsWithToolchain(crossDeps, toolchain),
		CompilerFlags: xBin.CompilerFlags,
		Toolchain:     toolchain,
	}
}

func hexOutForElf(elfOut core.OutPath) core.OutPath {
	return elfOut.WithExt("hex")
}

func (xBin CrossBinary) Build(ctx core.Context) {
	for _, toolchain := range xBin.toolchains() {
		ccBin := xBin.withToolchain(toolchain)
		ccBin.Build(ctx)
		ctx.AddBuildStep(core.BuildStep{
			Out:   hexOutForElf(ccBin.Out),
			In:    ccBin.Out,
			Cmd:   toolchain.RawBinary(hexOutForElf(ccBin.Out), ccBin.Out),
			Descr: fmt.Sprintf("Creating raw binary %s", hexOutForElf(ccBin.Out)),
		})
	}
}

func (xBin CrossBinary) Outputs() []core.Path {
	outputs := []core.Path{}
	for _, p := range xBin.toolchains() {
		elfOut := crosslib.OutPathWithToolchain(xBin.Out, p)
		outputs = append(outputs, elfOut, hexOutForElf(elfOut))
	}
	return outputs
}
