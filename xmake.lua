-- add_rules("mode.debug", "mode.release")
add_repositories("xege-repo git@gitee.com:xege/ege-xrepo.git")
add_requires("xege 20.08")
add_requires("imgui", {configs = {glfw_opengl3 = true}})

add_requires("freetype")
-- add_requires("llvm")
-- set_toolchains("@llvm")
add_rules("mode.debug", "mode.release")

add_defines("__STDC_LIMIT_MACROS")
includes("./codebase/lib")
includes("./codebase/app")
includes("./codebase/ui")
includes("./codebase/elib")
target ("main")
	add_rules("mode.debug")
	add_packages("xege")
	add_packages("freetype")
	set_kind ("binary")
	set_languages("c++2a")
	set_encodings("utf-8")
	-- add_files ("$(projectdir)/src/core.cc")
	add_deps("app", "animation")
	add_deps("game_res")
	add_deps("font")
	add_deps("ui", "elib")
	add_deps("hanoi", "snake")
	add_deps("modern-art-generator")
	add_deps("libxbmp", "base64", "u8g2", "tinyexpr")
	add_files ("$(projectdir)/src/测试文本.c")
	add_files ("$(projectdir)/src/main.cc")
	set_warnings("everything")
	set_rundir("$(projectdir)")
