add_rules("mode.debug", "mode.release")
set_languages("cxxlatest")

if os.scriptdir() == os.projectdir() then 
    includes("../Potato/")
    includes("../Noodles/")
    includes("../Dumpling/")
end

target("MapoToufu")
    set_kind("static")
    add_files("MapoToufu/*.cpp")
    add_files("MapoToufu/*.ixx", {public=true})
    add_deps("Potato")
    add_deps("Noodles")
    add_deps("Dumpling")
    add_deps("DumplingImGui")
    add_deps("DumplingHLSLComplier")
target_end()


if os.scriptdir() == os.projectdir() then
    includes("Demos/StartupDemo/")
end 