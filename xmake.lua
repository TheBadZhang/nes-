-- add_rules("mode.debug", "mode.release")
add_repositories("xege-repo git@gitee.com:xege/ege-xrepo.git")
add_requires("xege 20.08")
add_requires("imgui", {configs = {glfw_opengl3 = true}})


target("imgui-scaffold")
    set_kind("binary")
    add_files("test/imgui/main.cpp")
    add_packages("imgui")
    set_languages("c++14")

target ("main")
	-- add_rules("mode.release")
	add_defines("__STDC_LIMIT_MACROS")
	add_packages("xege")
	set_kind ("binary")
	set_languages("c++2a")
	add_files ("$(projectdir)/src/main.cc")
	set_warnings("everything")
	set_rundir("$(projectdir)")

target("microui")
	add_includedirs("$(projectdir)/3rd/microui-impl-ege")
	add_files("$(projectdir)/3rd/microui-impl-ege/microui.c")
	add_files("$(projectdir)/3rd/microui-impl-ege/microui_impl_ege.cpp")
	add_files("$(projectdir)/3rd/microui-impl-ege/demo/microui_demo/microui_demo.c")



target ("u8g2_font_test")
	add_defines("__STDC_LIMIT_MACROS")
	add_packages("xege")
	set_kind("binary")
	set_languages("c++2a")
	add_files("$(projectdir)/u8g2_font_test/main.cc")
	set_rundir("$(projectdir)")