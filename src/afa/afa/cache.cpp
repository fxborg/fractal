#include "cache.h"

CCache::CCache():
	m_window_size(0),
	m_max_size(0),
	m_cache(0)
	{}

CCache::CCache(const unsigned int window_size=0, const unsigned int length=0) :
	m_window_size(window_size),
	m_max_size(int(length/window_size)),
	m_cache(window_size)
{}

void CCache::set(const int x, const double v)
{
	int mod = x % m_window_size;
	m_cache[mod][x] = v;
	while (m_cache[mod].size() > m_max_size)
	{
		m_cache[mod].erase(m_cache[mod].begin());
	}
}

void CCache::print(const int x) const
{

	int mod = x % m_window_size;
	if (m_cache[mod].size() == 0) return;
	std::cout << "mod->" << mod <<std::endl;

	std::cout << "x=[";
	for (auto it = m_cache[mod].begin(); it != m_cache[mod].end(); it++)
	{
		std::cout << it->first << ", ";
	}
	std::cout << "]" << std::endl;
	std::cout << "v=[";
	for (auto it = m_cache[mod].begin(); it != m_cache[mod].end(); it++)
	{
		std::cout << it->second << ", ";
	}
	std::cout << "]" << std::endl;

}

double CCache::calc_fractal(const unsigned int x) const
{
	int mod = x % m_window_size;
	if (m_cache[mod].size() < 1) return 0.0;
	double result = 0.0;
	int i = 0;
	for (auto it = m_cache[mod].begin(); it != m_cache[mod].end(); it++)
	{
		result += it->second;
	}
	//std::cout << m_window_size <<" : "<< result << " / " << m_cache[mod].size()*m_window_size - 1 << std::endl;
	return result / (m_cache[mod].size()*m_window_size-1);
}

