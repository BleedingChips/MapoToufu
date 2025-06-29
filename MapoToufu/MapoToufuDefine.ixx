module;

export module MapoToufuDefine;

import std;

export namespace MapoToufu
{
	enum class ThreadMask : std::size_t
	{
		MainThread = 1,
	};

	constexpr std::size_t operator*(ThreadMask mask) { return static_cast<std::size_t>(mask); }
};

