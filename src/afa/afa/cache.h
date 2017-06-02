#include <numeric>
#include <vector>
#include <deque>
#include <map>
#include <algorithm>
#include <iostream>

class CCache
{
public:
	CCache();
	explicit CCache(const unsigned int window, const unsigned int length); 
	void set(const int x, const double v); 
	double CCache::calc_fractal(const unsigned int x) const;

	void print(const int mod) const;
private:
	const unsigned int m_window_size; 
	const unsigned int m_max_size;
	std::deque<std::map<unsigned int, double>> m_cache;
};

