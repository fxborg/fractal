#pragma once
#include <vector> 

#include <iterator>
#include <algorithm>
#include <iostream>
#include "opencv2/core/core.hpp"
class CTrend
{
public:
	explicit CTrend(const unsigned int size, const unsigned int order);
	bool fit(const std::vector<double> & y, std::vector<double> &trend);
	
private:
	unsigned int to_size(const unsigned int size);
	unsigned int to_half(const unsigned int size);
	std::vector<double> CTrend::stitch_trend(const cv::Mat1d & trend1, const cv::Mat1d & trend2);

	cv::Mat1d create_filter(void);
	const unsigned int m_size;
	const unsigned int m_half;
	const unsigned int m_order;
	cv::Mat1d m_filter;

};

