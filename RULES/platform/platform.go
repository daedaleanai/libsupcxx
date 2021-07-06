package platform

import (
	"dbt-rules/RULES/cc"
	"dbt-rules/RULES/core"
)

type Platform struct {
	Name         string
	BootFirst    cc.Library
	BootLast     cc.Library
	LinkerScript core.Path
	GccToolchain cc.GccToolchain
}

var platforms = []Platform{}

func ContainsPlatform(ps []Platform, p Platform) bool {
	for _, el := range ps {
		if el.Name == p.Name {
			return true
		}
	}
	return false
}

func (p Platform) Register() Platform {
	if ContainsPlatform(platforms, p) {
		core.Fatal("A platform with name %s has already been registered", p.Name)
	}
	platforms = append(platforms, p)
	return p
}

func AllPlatforms() []Platform {
	return platforms
}
