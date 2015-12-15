#include <array>
#include <algorithm>
#include <functional>

template<class T, std::size_t N>
auto utility::randomEngine() -> std::enable_if_t<!!N, T>
{
	std::array<typename T::result_type, N> seed_data;
	thread_local std::random_device source;
	std::generate(std::begin(seed_data), std::end(seed_data), std::ref(source));
	std::seed_seq seeds(std::begin(seed_data), std::end(seed_data));
	thread_local T seeded_engine(seeds);
	return seeded_engine;
}

template <class Dist>
typename Dist::result_type utility::random(Dist& dist)
{
	static auto& RandomEngine = randomEngine();
	return dist(RandomEngine);
}

template<typename C>
auto utility::randomChoice(const C& constainer)
{
	auto& d = std::uniform_int_distribution<unsigned>(0, constainer.size() - 1);
	return constainer[random(d)];
}