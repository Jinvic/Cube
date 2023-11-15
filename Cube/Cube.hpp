#pragma once
#include <vector>
#include <array>

typedef std::vector<std::array<double, 4>> Matrix;
inline Matrix operator*(Matrix a, Matrix b) // 矩阵乘法
{
	Matrix c(a.size());
	for (int i = 0; i < a.size(); i++)
	{
		for (int j = 0; j < 4; j++)
		{
			c[i][j] = 0;
			for (int k = 0; k < b.size(); k++)
				c[i][j] += a[i][k] * b[k][j];
		}
	}
	return c;
}

struct Cube
{
	Matrix P;    // 立方体顶点坐标
	Matrix T_ws; // 世界坐标系到屏幕坐标系的透视投影整体变换矩阵

	Cube(double D = 300, double a = 3, double b = 5) // D正方体边长，ab为比例尺
	{
		struct Point
		{
			double x, y, z;
			static double dist(Point a, Point b)
			{
				return sqrt((a.x - b.x) * (a.x - b.x) +
					(a.y - b.y) * (a.y - b.y) +
					(a.y - b.z) * (a.z - b.z));
			}
		};

		Point O_w = { 0, 0, 0 };             // 世界坐标系原点
		Point O_s = { a * D, a * D, a * D }; // 屏幕坐标系原点
		Point O_v = { b * D, b * D, b * D }; // 观察坐标系原点
		double R = Point::dist(O_w, O_v);         // R为O_w间O_v距离
		double d = Point::dist(O_s, O_v);         // d为O_s间O_v距离
		const double PI = acos(-1);
		const double Theta = PI / 4, Phi = PI / 4; // θ为O_wO_v与z_w轴夹角,φ为与y_w轴夹角

		T_ws.push_back({ cos(Theta), -cos(Phi) * sin(Theta), 0, -sin(Phi) * sin(Theta) / d });
		T_ws.push_back({ 0, sin(Phi), 0, -cos(Phi) / d });
		T_ws.push_back({ -sin(Theta), -cos(Phi) * cos(Theta), 0, -sin(Phi) * cos(Theta) / d });
		T_ws.push_back({ 0, 0, 0, R / d });

		P.push_back({ D / 2, D / 2, D / 2, 1 });
		P.push_back({ D / 2, D / 2, -D / 2, 1 });
		P.push_back({ -D / 2, D / 2, -D / 2, 1 });
		P.push_back({ -D / 2, D / 2, D / 2, 1 });
		P.push_back({ D / 2, -D / 2, D / 2, 1 });
		P.push_back({ D / 2, -D / 2, -D / 2, 1 });
		P.push_back({ -D / 2, -D / 2, -D / 2, 1 });
		P.push_back({ -D / 2, -D / 2, D / 2, 1 });
	}

	// 绕x轴旋转
	void rotate_x(double Beta) // Beta为旋转角度
	{
		Matrix T_x; // 绕x轴旋转变换矩阵

		T_x.push_back({ 1, 0, 0, 0 });
		T_x.push_back({ 0, cos(Beta), sin(Beta), 0 });
		T_x.push_back({ 0, -sin(Beta), cos(Beta), 0 });
		T_x.push_back({ 0, 0, 0, 1 });

		P = P * T_x;
	}
	// 绕y轴旋转
	void rotate_y(double Beta) // Beta为旋转角度
	{
		Matrix T_y; // 绕y轴旋转变换矩阵

		T_y.push_back({ cos(Beta), 0, -sin(Beta), 0 });
		T_y.push_back({ 0, 1, 0, 0 });
		T_y.push_back({ sin(Beta), 0, cos(Beta), 0 });
		T_y.push_back({ 0, 0, 0, 1 });

		P = P * T_y;
	}
	// 绕z轴旋转
	void rotate_z(double Beta) // Beta为旋转角度
	{
		Matrix T_z; // 绕z轴旋转变换矩阵

		T_z.push_back({ cos(Beta), sin(Beta), 0, 0 });
		T_z.push_back({ -sin(Beta), cos(Beta), 0, 0 });
		T_z.push_back({ 0, 0, 1, 0 });
		T_z.push_back({ 0, 0, 0, 1 });

		P = P * T_z;
	}

	//通过鼠标移动进行旋转
	void rotate(int dx, int dy, int limit = 500)//limit为最大移动距离
	{
		double Theta_v = 0, Phi_v = 0; // 屏幕坐标系中绕y轴（水平移动）和x轴（上下移动）旋转的角度
		// 将鼠标移动距离转换为对应角度
		int vx = (dx >= 0) ? 1 : -1, vy = (dy >= 0) ? 1 : -1;//确定方向
		dx = min(abs(dx), limit);
		dy = min(abs(dy), limit);
		Theta_v = vx * ((double)dx / 500) * acos(-1);//最大旋转角度Pi
		Phi_v = vy * ((double)dy / 500) * acos(-1);

		double Beta_x = 0, Beta_y = 0, Beta_z = 0; // 世界坐标系绕绕坐标轴旋转的角度
		// 将两个坐标系的坐标轴对齐
		rotate_y(-acos(-1) / 4);
		rotate_x(acos(-1) / 4);
		// 旋转
		rotate_y(Theta_v);
		rotate_x(Phi_v);
		// 还原对齐
		rotate_x(-acos(-1) / 4);
		rotate_y(acos(-1) / 4);
	}

	static CPoint trans_point(std::array<double, 4> arr)
	{
		return { (int)round(arr[0]),(int)round(arr[1]) };
	}
};
