#ifndef XOSHIRO256_HPP
#define XOSHIRO256_HPP
#include <array>
#include <cstdint>

namespace RNG
{

// The following PRNG was adapted from:
// https://prng.di.unimi.it/xoshiro256starstar.c
class Xoshiro256StarStar
{
public:
	using ResultType = uint64_t;
	using StateType = std::array<uint64_t, 4>;

	constexpr Xoshiro256StarStar(const StateType& initialState) noexcept : s(initialState)
	{}

	ResultType operator()() noexcept
	{
		const uint64_t result = rotl(s[1] * 5, 7) * 9;

		const uint64_t t = s[1] << 17;

		s[2] ^= s[0];
		s[3] ^= s[1];
		s[1] ^= s[2];
		s[0] ^= s[3];

		s[2] ^= t;

		s[3] = rotl(s[3], 45);

		return result;
	}

	// NOTE: std::shuffle requires these.
	using result_type = ResultType;
	static constexpr ResultType min() noexcept { return ResultType(0U); }
	static constexpr ResultType max() noexcept { return ResultType(~ResultType(0U)); }
private:
	StateType s;

	static constexpr uint64_t rotl(const uint64_t x, int k)
	{
		return (x << k) | (x >> (64 - k));
	}
};

} // namespace RNG

#endif // XOSHIRO256_HPP
