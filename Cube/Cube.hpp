#pragma once

#define debug 1
#undef debug
#ifdef debug
#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup") 
#endif // debug

#include <vector>
#include <array>
#include <iostream>

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
public:
	Matrix P;    // 立方体顶点坐标
	Matrix T_ws; // 世界坐标系到屏幕坐标系的透视投影整体变换矩阵

	struct Face // 面
	{
		std::array<int, 4> P_idx; // 四个顶点的索引
		bool visible = false;     // 可见性
	};
	std::array<Face, 6> F;//六色面，依次为蓝红黄白橙绿

	static enum Color { blue, red, yellow, white, orange, green };//颜色表

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
		const double Theta = PI / 4, Phi = acos(1 / sqrt(3)); // θ为O_wO_v与z_w轴夹角,φ为与y_w轴夹角

		// 初始化透视投影变换矩阵
		T_ws.push_back({ cos(Theta), -cos(Phi) * sin(Theta), 0, -sin(Phi) * sin(Theta) / d });
		T_ws.push_back({ 0, sin(Phi), 0, -cos(Phi) / d });
		T_ws.push_back({ -sin(Theta), -cos(Phi) * cos(Theta), 0, -sin(Phi) * cos(Theta) / d });
		T_ws.push_back({ 0, 0, 0, R / d });

		// 初始化立方体顶点
		P.push_back({ D / 2, D / 2, D / 2, 1 });
		P.push_back({ D / 2, D / 2, -D / 2, 1 });
		P.push_back({ -D / 2, D / 2, -D / 2, 1 });
		P.push_back({ -D / 2, D / 2, D / 2, 1 });
		P.push_back({ D / 2, -D / 2, D / 2, 1 });
		P.push_back({ D / 2, -D / 2, -D / 2, 1 });
		P.push_back({ -D / 2, -D / 2, -D / 2, 1 });
		P.push_back({ -D / 2, -D / 2, D / 2, 1 });

		// 初始化面的顶点，点逆时针排序
		F[blue].P_idx = { 0, 1, 2, 3 };//蓝
		F[red].P_idx = { 0, 3, 7, 4 };//红
		F[yellow].P_idx = { 0, 4, 5, 1 };//黄
		F[white].P_idx = { 6, 7, 3, 2 };//白
		F[orange].P_idx = { 6, 2, 1, 5 };//橙
		F[green].P_idx = { 6, 5, 4, 7 };//绿
	}

private:
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

public:
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

	// 消隐算法
	void HiddenSurfaceRemovalAlgorithm(void)
	{
		struct Vector
		{
			double x, y, z;

			// 默认构造函数
			Vector(double a = 0, double b = 0, double c = 0)
			{
				x = a;
				y = b;
				z = c;
			}

			// p1指向p2的向量的构造函数
			Vector(std::array<double, 4> p1, std::array<double, 4> p2)
			{
				x = p2[0] - p1[0];
				y = p2[1] - p1[1];
				z = p2[2] - p1[2];
			}

			// 向量乘法(向量积)
			static Vector cross(Vector v1, Vector v2)
			{
				Vector v3;
				v3.x = v1.y * v2.z - v2.y * v1.z;
				v3.y = v1.z * v2.x - v2.z * v1.x;
				v3.z = v1.x * v2.y - v2.x * v1.y;
				return v3;
			}

			// 向量乘法(数量积)
			static double dot(Vector v1, Vector v2)
			{
				return
					v1.x * v2.x +
					v1.y * v2.y +
					v1.z * v2.z;
			}

			// 向量模
			static double length(Vector v)
			{
				double l2 = dot(v, v);
				return (l2 < 0) ? 0 : sqrt(l2);
			}

			// 向量夹角 cosθ=a*b/(|a|*|b|)
			static double angel(Vector v1, Vector v2)
			{
				return acos(dot(v1, v2) / length(v1) / length(v2));
			}
		};

		//DEBUG:
#ifdef debug
		AllocConsole();
#endif
		double a[6] = { 0 };//夹角和Pi的比值
		Vector vv = { 1, 1, 1 }; // 视向量
		for (int i = 0; i < F.size(); i++)
		{
			// 计算面的外法向量
			Vector v1 = Vector(P[F[i].P_idx[0]], P[F[i].P_idx[1]]),
				v2 = Vector(P[F[i].P_idx[1]], P[F[i].P_idx[2]]);
			Vector vf = Vector::cross(v1, v2);

			// 计算外法向与视向量的夹角
			double Theta = Vector::angel(vf, vv);
			a[i] = Theta / acos(-1);
			// 判断可见性
			if (a[i] >= 0 && a[i] <= 0.5)//夹角小于90度，可见
				F[i].visible = true;
			else
				F[i].visible = false;
		}
#ifdef debug
		system("cls");
		std::cout.setf(std::ios::fixed);
		std::cout << "blue\t\t" << a[0] << "\tred\t" << a[1] << std::endl <<
			"yellow\t\t" << a[2] << "\twhite\t" << a[3] << std::endl <<
			"orange\t\t" << a[4] << "\tgreen\t" << a[5] << std::endl;
#endif
	}

	//将三维坐标系的点坐标转换为透视投影后的屏幕坐标
	static CPoint trans_point(std::array<double, 4> arr)
	{
		return { (int)round(arr[0]),(int)round(arr[1]) };
	}
};
