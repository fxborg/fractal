#define EXPORT  extern "C" // __declspec(dllexport)  
#include <deque> 
#include <vector> 
#include <iterator> 
#include <numeric>
#include <cmath>
#include <algorithm>
#include<iterator>
#include <unordered_map>
#include "series.h"
#include "trend.h"
#include "cache.h"
#include "stats.h"
class CAfa
{
public:
	// コンストラクタ
	explicit CAfa(const unsigned int length, const double step, const unsigned int order);

	int push(const int x, const double y, const time_t t0, const time_t t1);

	double calculate();

	bool get_results(unsigned int idx,double & y);

private:
	unsigned int validate_length(const  unsigned int len);
	double validate_step(const double step);
	unsigned int validate_order(const  unsigned int order);
	void CAfa::volatility(std::vector<double> &y, std::vector<double> &y_hat, const unsigned int step, double &v1, double &v2);

	const unsigned int m_length;
	const double m_step;
	const unsigned int m_order;
	CSeries m_series;
	std::vector<unsigned int> m_segments;
	std::vector<CTrend> m_filters;
	std::unordered_map<unsigned int, CCache> m_cache;
	std::vector<double>m_results;
};

//--- インスタンスを生成
EXPORT CAfa * __stdcall Create(const unsigned int length,  const unsigned int order);
//--- インスタンスを破棄
EXPORT void __stdcall Destroy(CAfa* instance);
//--- インスタンス経由でpushメソッドをコール
EXPORT int __stdcall Push(CAfa* instance, const int x, const double y, const time_t t0, const time_t t1);
//--- 予測値を計算
EXPORT double  __stdcall Calculate(CAfa* instance);
