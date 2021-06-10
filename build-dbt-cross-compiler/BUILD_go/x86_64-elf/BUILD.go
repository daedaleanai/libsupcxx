package x86_64

import "dbt-rules/RULES/cc"

var Toolchain = cc.GccToolchain{
	Ar:      in("bin/x86_64-elf-ar"),
	As:      in("bin/x86_64-elf-as"),
	Cc:      in("bin/x86_64-elf-gcc"),
	Cpp:     in("bin/x86_64-elf-gcc -E"),
	Cxx:     in("bin/x86_64-elf-g++"),
	Objcopy: in("bin/x86_64-elf-objcopy"),

	Includes: ins(
		"lib/gcc/x86_64-elf/10.2.0/include",
		"lib/gcc/x86_64-elf/10.2.0/include-fixed",
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
		"-mno-red-zone",
	},

	LinkerFlags: []string{
		"-ffreestanding",
		"-nostdlib",
		"-lgcc",
		"-z",
		"max-page-size=0x1000",
	},

	ArchName:   "i386",
	TargetName: "elf64-x86-64",
}

var CrtBegin = in("lib/gcc/x86_64-elf/10.2.0/crtbegin.o")
var CrtEnd = in("lib/gcc/x86_64-elf/10.2.0/crtend.o")
