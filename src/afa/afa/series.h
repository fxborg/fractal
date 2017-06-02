#pragma once
#include <deque> 
#include <algorithm>
#include <iostream>


class CSeries
{
public:
	explicit CSeries(const unsigned int period); 
	int push(const int x, const double y, const time_t t0, const time_t t1); 
	const std::deque<double>& get_series() const;
	void erase(void); 
	int last_x() const;
	int prev_x() const;
	bool is_adding(void) const;
	unsigned int size(void) const; 
private:
	const unsigned int m_series_size; 
	int m_last_x;
	int m_prev_x;
	time_t m_last_t;
	std::deque<double> m_buf_stats;
	bool m_is_adding;
};

