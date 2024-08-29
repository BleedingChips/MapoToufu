add_rules("mode.debug", "mode.release")
set_languages("cxxlatest")

if os.scriptdir() == os.projectdir() then 
    includes("../Potato/")
    includes("../Noodles/")
    includes("../Dumpling/")
end

target("NoodlesDemoGame")
    set_kind("binary")
    add_files("NoodlesDemoGame/Code/*.cpp")
    add_files("NoodlesDemoGame/Code/*/*.ixx")
    add_files("NoodlesDemoGame/Code/*/*.cpp")
    add_deps("Potato")
    add_deps("Noodles")
    add_deps("Dumpling")
target_end()