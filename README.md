# MapoToufu

一个游戏Demo，开发中

## 安装

从 Github 上将以下库Clone到本地，并使其保持在同一个路径下：

	```
    https://github.com/BleedingChips/Dumpling.git
	https://github.com/BleedingChips/Noodles.git
	https://github.com/BleedingChips/Potato.git
    https://github.com/BleedingChips/MapoToufu.git
	```

在包含该项目的 xmake.lua 上，添加如下代码即可：

	```lua
	includes("../Potato/")
	includes("../Noodles/")
    includes("../Dumpling/")
    includes("../MapoToufu/")

	target(xxx)
		...
		add_deps("MapoToufu")
	target_end()
	```

运行 `xmake_install.ps1` 安装 `xmake`，运行`xmake_generate_vs_project.ps1`将在vsxmake2022下产生vs的项目文件。
