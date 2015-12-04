template<typename U>
auto utility::random(U min, U max)
{
	static auto seed = static_cast<unsigned long>(std::time(nullptr));
	static auto RandomEngine = std::default_random_engine(seed);

	decltype(dist<U>()) uniformDistribution(min, max);

	return uniformDistribution(RandomEngine);
}

template<typename C>
auto utility::randomChoice(const C& constainer)
{
	return constainer[random(0u, constainer.size() - 1)];
}