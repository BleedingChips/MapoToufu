set_project("MapoToufuDemo")
target("StartupDemo")
    set_kind("binary")
    add_files("*.cpp")
    add_deps("MapoToufu")
target_end()