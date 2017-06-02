#include "afa.h"
//+-----------------------------------------------------------------------------------------+
//| This formulation was first formulated in                                                |
//|                                                                                         |
//| J.B. Gao, J. Hu, W.W. Tung, Facilitating joint chaos and fractal analysis of biosignals |
//| through nonlinear adaptive filtering.  PLoS ONE PLoS ONE 6(9): e24331.                  |
//| doi:10.1371/journal.pone.0024331                                                        |
//+-----------------------------------------------------------------------------------------+

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
    // 新規バーの以外は何もしない。
	if (!m_series.is_adding())return -1;
    // スケール分ループ
	std::deque<double> series = m_series.get_series();
	for (unsigned int i = 0; i < m_segments.size(); i++)
	{
		unsigned int step = m_segments[i];	// ハーフサイズ
		unsigned int w = step * 2 + 1;		// ウインドウサイズ
		unsigned int len = step * 3 + 1;	// ウインドウサイズ＋ハーフサイズ
		unsigned int sz = series.size();
		
		if (sz - 1 < len) continue;	//サイズが足りない場合スキップ
		// 計算に使用するデータを deque から vector にコピー
		int offset = sz - len - 1;
		std::vector<double> data;
		std::copy(series.cbegin() + offset, series.cend() - 1, std::back_inserter(data));
		std::vector <double> trend;
		double v1 = 0.0;
		double v2 = 0.0;
		// 多項式フィット
		if (m_filters[i].fit(data, trend)) {
			// 残差絶対値和を求める。
			volatility(data, trend, step, v1, v2);
		}

		// キャッシュに追加
		m_cache[step].set(x - step, v1);
		m_cache[step].set(x, v2);
		
	}
	return result;

}

double CAfa::calculate()
{
	// 末尾の一つ前のインデックス
	int x = m_series.prev_x();
	if (x == -1) return -1.0;
	// スケーリング指数を求める。
	std::vector<Stats> fq;
	for (unsigned int i = 0; i < m_segments.size(); i++)
	{
		unsigned int step = m_segments[i];
		unsigned int w = step * 2 + 1;
		// 残差絶対値和の平均を求める。
		double v = m_cache[step].calc_fractal(x);
		fq.emplace_back(1, log2(w), log2(v));
	}
	// スロープを求める。
	Stats stat = std::accumulate(fq.begin(), fq.end(), Stats());
	double slope = stat.slope();
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
		v1 += abs(y[from_y + i] - y_hat[from_y_hat + i]);
	}
//	std::cout << "1 v1:" <<v1 << std::endl;

	v2 = 0.0;
	for (unsigned int i = step; i < sz; i++)
	{
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
	double tmp = int(step * 2.0)*0.5;
	return std::min(2.0, std::max(0.5, tmp));
}
unsigned int  CAfa::validate_order(const unsigned int order)
{
	return 1u;
}


