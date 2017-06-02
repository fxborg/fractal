#include "series.h"

CSeries::CSeries(const unsigned int period) : 
	m_series_size(std::max(1u, std::min(10000u, period))),
	m_last_t(0),
	m_last_x(-1),
	m_prev_x(-1),
	m_is_adding(false)
{}

void CSeries::erase(void)
{
	m_buf_stats.erase(m_buf_stats.begin(), m_buf_stats.end());
}

int CSeries::last_x() const
{
	return m_last_x;
}

int CSeries::prev_x() const
{
	return m_prev_x;
}

bool CSeries::is_adding(void) const
{
	return m_is_adding;
}

unsigned int CSeries::size(void) const
{
	return m_buf_stats.size();
}

int CSeries::push(const int x, const double y, const time_t t0, const time_t t1)
{
	m_is_adding = false;
	if (m_last_x > x) return -1;

	if (m_last_t != 0)
	{
		if (m_last_x == x && m_last_t != t0) throw std::out_of_range("x");
		else if (m_last_x < x && m_last_t != t1) throw std::out_of_range("x");
	}

	int last_x = m_last_x;
	m_last_x = x;
	m_last_t = t0;

	if (last_x == x)
	{
		auto rit_stats = m_buf_stats.rbegin();
		*rit_stats = y;
	}
	else
	{
		m_prev_x = last_x;
		m_is_adding = true;
		m_buf_stats.emplace_back(y);
		
		if (m_buf_stats.size() > m_series_size)
		{
			unsigned int delsz = m_buf_stats.size() - m_series_size;
			m_buf_stats.erase(m_buf_stats.begin(), m_buf_stats.begin() + (delsz));
		}

	}
	return x;
}

const std::deque<double>& CSeries::get_series() const
{
	return m_buf_stats;
}


