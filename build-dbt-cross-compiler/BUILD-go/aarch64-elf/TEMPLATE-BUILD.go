package aarch64

import "dbt-rules/RULES/cc"

var Toolchain = cc.GccToolchain{
	Ar:      in("bin/aarch64-elf-ar"),
	As:      in("bin/aarch64-elf-as"),
	Cc:      in("bin/aarch64-elf-gcc"),
	Cpp:     in("bin/aarch64-elf-gcc -E"),
	Cxx:     in("bin/aarch64-elf-g++"),
	Objcopy: in("bin/aarch64-elf-objcopy"),
	Ld:      in("bin/aarch64-elf-ld"),

	Includes: ins(
		"lib/gcc/aarch64-elf/GCC_VER/include",
		"lib/gcc/aarch64-elf/GCC_VER/include-fixed",
	),

	CompilerFlags: []string{
		"-std=c++14",
		"-nostdinc",
		"-fdiagnostics-color=always",
		"-Wall",
		"-Wunused-but-set-parameter",
		"-Wno-free-nonheap-object",
		"-Wno-int-to-pointer-cast",
		"-fno-omit-frame-pointer",
		"-Wno-builtin-macro-redefined",
		"-D__DATE__=\"redacted\"",
		"-D__TIMESTAMP__=\"redacted\"",
		"-D__TIME__=\"redacted\"",
		"-ffreestanding",
	},

	LinkerFlags: []string{
		"-ffreestanding",
		"-nostdlib",
		"-lgcc",
	},

	ArchName:   "aarch64",
	TargetName: "elf64-littleaarch64",
}

var CrtBegin = in("lib/gcc/aarch64-elf/GCC_VER/crtbegin.o")
var CrtEnd = in("lib/gcc/aarch64-elf/GCC_VER/crtend.o")
