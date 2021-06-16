package config

import (
	"dbt-rules/RULES/cc"
	"dbt-rules/RULES/core"

	aarch64 "aarch64-elf-gcc"
	x86_64 "x86_64-elf-gcc"
)

const (
	RasPi3       = "raspi3"
	X86_64       = "x86_64"
	Linux_x86_64 = "linux-x86_64"
)

var Target = core.StringFlag{
	Name:          "target",
	Description:   "target architecture",
	AllowedValues: []string{RasPi3, X86_64, Linux_x86_64},
	DefaultFn:     func() string { return X86_64 },
}.Register()

func Toolchain() cc.GccToolchain {
	switch Target.Value() {
	case RasPi3:
		return aarch64.Toolchain
	case Linux_x86_64:
		return x86_64.Toolchain
	case X86_64:
		return x86_64.Toolchain
	}
	return cc.GccToolchain{}
}
