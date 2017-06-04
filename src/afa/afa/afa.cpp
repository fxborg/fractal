#include "afa.h"


EXPORT CAfa * __stdcall Create(	const unsigned int length,	 const unsigned int order)
{
	return new CAfa(length, 1u, order);
}

EXPORT void __stdcall Destroy(CAfa * instance)
{
	delete instance;
}

EXPORT int __stdcall Push(CAfa * instance, const int x, const double y, const time_t t0, const time_t t1)
{
	return instance->push(x, y, t0, t1);
}

EXPORT double __stdcall Calculate(CAfa * instance)
{
	return instance->calculate();
}

CAfa::CAfa(const unsigned int length,const double step,const unsigned int order) :
	m_length(validate_length(length)),
	m_step(validate_step(step)),
	m_order(validate_order(order)),
	m_series(CSeries(m_length + 1))
{
	// ウインドウサイズの初期化
	int imax = int(std::round(std::log2(m_length)));
	int sz = int((imax - 2) / step) + 1;
	for (double m = 2; m < imax; m += m_step)
		{
		unsigned int w = int(round(pow(2, m) + 1));
		if ((w % 2) == 0) w += 1u;
		// セグメントの追加
		m_segments.emplace_back(unsigned int((w-1)*0.5));
		// トレンドフィルタの追加
		m_filters.emplace_back(w, m_order);
	}
	// キャッシュの初期化
	for (auto it = m_segments.begin(); it != m_segments.end(); it++)
	{
		
		m_cache.insert(std::make_pair(*it, CCache(*it, m_length)));

	}
}


int CAfa::push(const int x, const double y, const time_t t0, const time_t t1)
{
	int result = 0;
	try
	{
		result = m_series.push(x, y, t0, t1);

	}
	catch (...)
	{
		return -9999;
	}

	if (!m_series.is_adding())return -1;
	std::deque<double> series = m_series.get_series();
	for (unsigned int i = 0; i < m_segments.size(); i++)
	{

		unsigned int step = m_segments[i];
		//		std::cout << step << std::endl;
		unsigned int w = step * 2 + 1;
		unsigned int len = step * 3 + 1;
		unsigned int sz = series.size();
		if (sz - 1 < len) continue;
		int offset = sz - len - 1;
		std::vector<double> data;
		std::copy(series.cbegin() + offset, series.cend() - 1, std::back_inserter(data));
		std::vector <double> trend;
		double v1 = 0.0;
		double v2 = 0.0;
		if (m_filters[i].fit(data, trend)) {
			//if (x%step == 0 && w == 9)
			//{

			//	for (unsigned int i = 0; i < step; i++)
			//	{
			//		unsigned int o = data.size() - (step * 2);
			//		//std::cout <<x <<", " << w << "]:" << data[o + i] << " - " << trend[i] << " ;" << std::endl;
			//	}
			//}
			volatility(data, trend, step, v1, v2);
		}

		// キャッシュに追加
		
		m_cache[step].set(x - step, v1);
		m_cache[step].set(x, v2);
//		std::cout << w << ":" << step << std::endl;

//		if (x%step == 2)
//		{
//			std::cout << "[" << x - step << "]<-" << v1 << ", ";
//			std::cout << "[" << x << "]<-" << v2 << std::endl;
//			m_cache[step].print(x);
//		}
		
	}
	return result;

}

double CAfa::calculate()
{
	int x = m_series.last_x();

	std::vector<Stats> fq;
	//std::cout << "------------------------------" << std::endl;
	for (unsigned int i = 0; i < m_segments.size(); i++)
	{
		unsigned int step = m_segments[i];
		unsigned int w = step * 2 + 1;
		double v = m_cache[step].calc_fractal(x);
		fq.emplace_back(1, log2(w), log2(v));
		//std::cout<< (w) << "," <<  (v) << std::endl;
	}

	Stats stat = std::accumulate(fq.begin(), fq.end(), Stats());
	double slope = stat.slope();
//	std::cout <<"slope:"<< slope << std::endl;
	return (isnan(slope)) ? -1.0 : slope;
}
void CAfa::volatility(std::vector<double> &y, std::vector<double> &y_hat, const unsigned int step, double &v1,double &v2)
{
	unsigned int sz = step*2;
	unsigned int from_y = y.size() - sz;
	unsigned int from_y_hat = y_hat.size() - sz;

	v1 = 0.0;
	for (unsigned int i = 0; i < step; i++)
	{
//		std::cout << "1 y:" << from_y+i << " y^:" << from_y_hat+i << std::endl;
//		std::cout << "1 v:" << y[from_y + i] << " v^:" << y_hat[from_y_hat + i] << std::endl;
//		std::cout << "1 volat:" << y[from_y + i] - y_hat[from_y_hat + i] << std::endl;
		v1 += abs(y[from_y + i] - y_hat[from_y_hat + i]);
	}
//	std::cout << "1 v1:" <<v1 << std::endl;

	v2 = 0.0;
	for (unsigned int i = step; i < sz; i++)
	{
//		std::cout << "2 x:" << from_y + i << " y^:" << from_y_hat + i << std::endl;
//		std::cout << "2 v:" << y[from_y + i] << " v^:" << y_hat[from_y_hat + i] << std::endl;
//		std::cout << "2 volat:" << y[from_y + i] - y_hat[from_y_hat + i] << std::endl;
		v2 += abs(y[from_y + i] - y_hat[from_y_hat + i]);
	}
//	std::cout << "2 v2:" << v2 << std::endl;

}

bool CAfa::get_results(const unsigned int idx, double &y)
{
	if (m_results.size() <= idx) return false;
	y = m_results[idx];
	return true;
}


unsigned int CAfa::validate_length(const unsigned int len)
{
	unsigned int l = int(pow(2, int(log2(len))));
	return std::max(16u, std::min(1024u, l));
}

double CAfa::validate_step(const double step)
{
	return 1u;
}
unsigned int  CAfa::validate_order(const unsigned int order)
{
	return std::min(3u, std::max(1u, order));
}


