#ifndef SPLITMIX64_HPP
#define SPLITMIX64_HPP
#include <cstdint>

namespace RNG
{

// The following generator was adapted from:
// https://prng.di.unimi.it/splitmix64.c
class SplitMix64
{
public:
	using ResultType = uint64_t;
	using StateType = uint64_t;

	constexpr SplitMix64(StateType initialState) noexcept : s(initialState)
	{}

	constexpr ResultType operator()() noexcept
	{
		uint64_t z = (s += 0x9e3779b97f4a7c15);
		z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
		z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
		return z ^ (z >> 31);
	}

	// NOTE: std::shuffle requires these.
	using result_type = ResultType;
	static constexpr ResultType min() noexcept { return ResultType(0U); }
	static constexpr ResultType max() noexcept { return ResultType(~ResultType(0U)); }
private:
	StateType s;
};

} // namespace RNG

#endif // SPLITMIX64_HPP
