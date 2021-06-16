package libsupcxx

import (
	"dbt-rules/RULES/cc"
	"dbt-rules/RULES/core"
	"fmt"
	"libsupcxx/RULES/boot"
	"libsupcxx/RULES/config"
	"libsupcxx/RULES/libsupcxx/lib"
	linux "libsupcxx/boot/linux-x86_64"
	"libsupcxx/boot/raspi3"
	"libsupcxx/boot/x86_64"
	"libsupcxx/libruncxx"
	libsupcxx "libsupcxx/libsupcxx/src"
)

type Dep = cc.Dep
type Library = lib.Library

type Binary struct {
	Out           core.OutPath
	Srcs          []core.Path
	Deps          []cc.Dep
	OnlyForConfig string
}

func Boot() boot.Boot {
	switch config.Target.Value() {
	case config.X86_64:
		return x86_64.Boot
	case config.RasPi3:
		return raspi3.Boot
	case config.Linux_x86_64:
		return linux.Boot

	}
	return boot.Boot{}
}

func (bin Binary) ccBinary() cc.Binary {
	deps := []Dep{Boot().BootLast, libruncxx.Lib, libsupcxx.Lib}

	for _, dep := range bin.Deps {
		deps = append(deps, dep.CcLibrary())
	}

	deps = append(deps, Boot().BootFirst)

	return cc.Binary{
		Out:       bin.Out,
		Srcs:      bin.Srcs,
		Deps:      deps,
		Script:    Boot().LinkerScript,
		Toolchain: config.Toolchain(),
	}
}

func (bin Binary) Build(ctx core.Context) {
	if bin.OnlyForConfig != "" && bin.OnlyForConfig != config.Target.Value() {
		return
	}
	bin.ccBinary().Build(ctx)
	ctx.AddBuildStep(core.BuildStep{
		Out: bin.hexPath(),
		In:  bin.Out,
		Cmd: fmt.Sprintf(
			"%q -O binary %q %q",
			config.Toolchain().Objcopy,
			bin.Out,
			bin.hexPath(),
		),
		Descr: fmt.Sprintf("Creating raw binary %s", bin.hexPath().Relative()),
	})
}

func (bin Binary) hexPath() core.OutPath {
	return bin.Out.WithSuffix(".hex")
}
