#include <array>
#include <algorithm>
#include <functional>

template<class T, std::size_t N>
auto utility::ProperlySeededRandomEngine() -> typename std::enable_if_t<!!N, T>
{
	std::array<typename T::result_type, N> seed_data;
	thread_local static std::random_device source;
	std::generate(std::begin(seed_data), std::end(seed_data), std::ref(source));
	std::seed_seq seeds(std::begin(seed_data), std::end(seed_data));
	thread_local static T seeded_engine(seeds);
	return seeded_engine;
}

template<typename T>
T utility::random(T min, T max)
{
	static auto RandomEngine = ProperlySeededRandomEngine();

	decltype(dist<T>()) uniformDistribution(min, max);

	return uniformDistribution(RandomEngine);
}

template<typename C>
auto utility::randomChoice(const C& constainer)
{
	return constainer[random(0u, constainer.size() - 1)];
}