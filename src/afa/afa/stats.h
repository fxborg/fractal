#include <cmath>
struct Stats
{
	int n;
	double x;
	double xx;
	double y;
	double yy;
	double xy;

	Stats() :
		n(0),
		x(0.0),
		xx(0.0),
		y(0.0),
		yy(0.0),
		xy(0.0)
	{}

	Stats(
		const int n,
		const double x ,
		const double y):
		n(n),
		x(x),
		xx(x*x),
		y(y),
		yy(y*y),
		xy(x*y)
	{}

	Stats(
		const int n,
		const double x,
		const double xx,
		const double y,
		const double yy,
		const double xy) :
		n(n),
		x(x),
		xx(xx),
		y(y),
		yy(yy),
		xy(xy)
	{}

	Stats operator+(const Stats& a) const
	{
		return Stats(
			n + a.n,
			x + a.x,
			xx + a.xx,
			y + a.y,
			yy + a.yy,
			xy + a.xy
		);
	}

	Stats operator-(const Stats& a) const
	{
		return Stats(
			n-a.n ,
			x-a.x ,
			xx-a.xx ,
			y-a.y ,
			yy-a.yy ,
			xy-a.xy 
		);
	}


	//Žc·•½•û˜a
	double residuals() const
	{
		double devsqx = dev_sq_x();
		return (devsqx > 0) ? dev_sq_y() - (std::pow(dev_prod_xy(), 2) / devsqx) : 0.0;
	}

	// Ø•Ð
	double intercept() const
	{
		return (n > 0) ? (y - slope() * x) / n : mean_y();
	}

	// ŒX‚«
	double slope() const
	{
		double devsqx = dev_sq_x();
		return (devsqx > 0) ? dev_prod_xy() / devsqx : 0.0;
	}


	// •Î·•½•û˜a ‚w
	double dev_sq_x() const
	{
		return (n > 0) ? (xx*n - x * x) / n : 0.0;
	}
	// •Î·•½•û˜a Y
	double dev_sq_y() const
	{
		return (n > 0) ? (yy * n - y * y) / n : 0.0;
	}
	// •Î·Ï˜a ‚w‚x
	double dev_prod_xy() const
	{
		return (n > 0) ? (xy * n - x * y) / n : 0.0;
	}
	// •½‹Ï‚x
	double mean_y() const
	{
		return (n > 0) ? y / n : 0.0;
	}

};

