#pragma once

const enum debug_flags { real_time_visibility, mouse_LBdown_onWhichFace };
#define debug mouse_LBdown_onWhichFace
//#undef debug
#ifdef debug
#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup") 
#endif // debug

#include <vector>
#include <array>
#include <iostream>
#include "ComputationGeometry.hpp"

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
		std::array<int, 4> P_idx; // 四个顶点的索引，按逆时针顺序存点
		bool visible = false;     // 可见性
	};
	std::array<Face, 6> F;//六色面，依次为蓝红黄白橙绿
	std::vector<Face> Fs;//36面，每9面为一色

	static const enum Color { blue, red, yellow, white, orange, green };//颜色表
	const std::string Color_rev[6] = { "blue", "red", "yellow", "white", "orange", "green" };//DEBUG用

	static const int OutFace = -1;//判断鼠标落点用，该常量表示未落在面上

	struct Layer
	{
		std::array<int, 8> P_idx;// 四个顶点的索引，存点顺序与立方体相同
		Vector3d v;//法向量
	}L[9];

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
		const double Theta = PI / 4, Phi = acos(1 / sqrt(3)); // θ为O_wO_v在xOz平面上的投影与z轴夹角,φ为与y轴夹角

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

		//8点6面的立方体细化为156点54面的魔方
		trans_cube();

		//初始化各层顶点
		//L[0].P_idx = { 0, 1, 2, 3,F[9].P_idx[3],F[40].P_idx[3],F[38].P_idx[0],F[11].P_idx[2] };//蓝
		//L[1].P_idx = { 0, 3, 7, 4,F[0].P_idx[1],F[51].P_idx[0],F[49].P_idx[1],F[47].P_idx[0] };//红
		//L[2].P_idx = { 0, 4, 5, 1,F[0].P_idx[3],F[49].P_idx[3],F[47].P_idx[0],F[2].P_idx[2] };//黄
		//L[3].P_idx = { 6, 7, 3, 2,F[9].P_idx[3],F[47].P_idx[0],F[49].P_idx[3],F[11].P_idx[2] };//白
		//L[4].P_idx = { 6, 2, 1, 5,F[9].P_idx[3],F[47].P_idx[0],F[49].P_idx[3],F[11].P_idx[2] };//橙
		//L[5].P_idx = { 6, 5, 4, 7,F[9].P_idx[3],F[47].P_idx[0],F[49].P_idx[3],F[11].P_idx[2] };//绿
		//L[6].P_idx = { 0, 1, 2, 3,F[9].P_idx[3],F[47].P_idx[0],F[49].P_idx[3],F[11].P_idx[2] };//x轴为法向量
		//L[7].P_idx = { 0, 1, 2, 3,F[9].P_idx[3],F[47].P_idx[0],F[49].P_idx[3],F[11].P_idx[2] };//y轴为法向量
		//L[8].P_idx = { 0, 1, 2, 3,F[9].P_idx[3],F[47].P_idx[0],F[49].P_idx[3],F[11].P_idx[2] };//z轴为法向量

		//DEBUG:
#ifdef debug
		AllocConsole();
		std::cout << "P.size():\t" << P.size() << std::endl;
		std::cout << "Fs.size()\t" << Fs.size() << std::endl;
#endif
	}

private:
	//8点6面的立方体细化为156点54面的魔方
#define Point std::array<double, 4>
	//四则运算
	Point plus(Point p1, Point p2)
	{
		for (int i = 0;i < 3;i++)
			p1[i] += p2[i];
		return p1;
	}
	Point minus(Point p1, Point p2)
	{
		for (int i = 0;i < 3;i++)
			p1[i] -= p2[i];
		return p1;
	}
	Point divide(Point p, double d)
	{
		for (int i = 0;i < 3;i++)
			p[i] /= d;
		return p;
	}
	//该点是否已存
	int search_point(Point p)
	{
		for (int i = 0;i < P.size();i++)
			if (P[i] == p)
				return i;
		return -1;
	}
	//向f.P_idx[id]存点索引
	void check_point(Point p, Face& f, int id)
	{
		int d = search_point(p);
		if (d == -1)
		{
			f.P_idx[id] = P.size();
			P.push_back(p);
		}
		else
			f.P_idx[id] = d;
	}

	void trans_cube(void)
	{
		//4点1面细化为36点9面
		for (int i = 0;i < F.size();i++)
		{
			std::pair<Point, Point>p_mid[6];
			for (int j = 0;j < 4;j++)
				p_mid[j] = middle2(P[F[i].P_idx[j]], P[F[i].P_idx[(j + 1) % 4]]);
			p_mid[4] = middle2(p_mid[0].first, p_mid[2].second);
			p_mid[5] = middle2(p_mid[0].second, p_mid[2].first);

			Face f;
			{
				f.P_idx[0] = F[i].P_idx[0];
				check_point(p_mid[0].first, f, 1);
				check_point(p_mid[4].first, f, 2);
				check_point(p_mid[3].second, f, 3);
				Fs.push_back(f);

				check_point(p_mid[0].first, f, 0);
				check_point(p_mid[0].second, f, 1);
				check_point(p_mid[5].first, f, 2);
				check_point(p_mid[4].first, f, 3);
				Fs.push_back(f);

				f.P_idx[1] = F[i].P_idx[1];
				check_point(p_mid[0].second, f, 0);
				check_point(p_mid[1].first, f, 2);
				check_point(p_mid[5].first, f, 3);
				Fs.push_back(f);

				check_point(p_mid[5].first, f, 0);
				check_point(p_mid[1].first, f, 1);
				check_point(p_mid[1].second, f, 2);
				check_point(p_mid[5].second, f, 3);
				Fs.push_back(f);

				f.P_idx[2] = F[i].P_idx[2];
				check_point(p_mid[5].second, f, 0);
				check_point(p_mid[1].second, f, 1);
				check_point(p_mid[2].first, f, 3);
				Fs.push_back(f);

				check_point(p_mid[4].second, f, 0);
				check_point(p_mid[5].second, f, 1);
				check_point(p_mid[2].first, f, 2);
				check_point(p_mid[2].second, f, 3);
				Fs.push_back(f);

				f.P_idx[3] = F[i].P_idx[3];
				check_point(p_mid[3].first, f, 0);
				check_point(p_mid[4].second, f, 1);
				check_point(p_mid[2].second, f, 2);
				Fs.push_back(f);

				check_point(p_mid[3].second, f, 0);
				check_point(p_mid[4].first, f, 1);
				check_point(p_mid[4].second, f, 2);
				check_point(p_mid[3].first, f, 3);
				Fs.push_back(f);

				check_point(p_mid[4].first, f, 0);
				check_point(p_mid[5].first, f, 1);
				check_point(p_mid[5].second, f, 2);
				check_point(p_mid[4].second, f, 3);
				Fs.push_back(f);
			}
		}
	}
	//找线段的两个中点
	std::pair<Point, Point> middle2(Point p_st, Point p_end)
	{
		Point p = divide(minus(p_end, p_st), 3);
		Point p1 = plus(p_st, p);
		Point p2 = plus(p1, p);
		return make_pair(p1, p2);
	};

#undef Point


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
#define Vector Vector3d
		std::vector<double> a(Fs.size());//夹角和Pi的比值
		Vector vv = { 1, 1, 1 }; // 视向量
		//9个大面的可见性
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

		//DEBUG:
#ifdef debug
		if (debug == real_time_visibility)
		{
			system("cls");
			std::cout.setf(std::ios::fixed);
			for (int i = 0;i < 6;i++)
				std::cout << Color_rev[i] << "\t\t" << a[i] << std::endl;
	}
#endif

		//54个小面的可见性
		for (int i = 0; i < Fs.size(); i++)
		{
			// 计算面的外法向量
			Vector v1 = Vector(P[Fs[i].P_idx[0]], P[Fs[i].P_idx[1]]),
				v2 = Vector(P[Fs[i].P_idx[1]], P[Fs[i].P_idx[2]]);
			Vector vf = Vector::cross(v1, v2);

			// 计算外法向与视向量的夹角
			double Theta = Vector::angel(vf, vv);
			a[i] = Theta / acos(-1);
			// 判断可见性
			if (a[i] >= 0 && a[i] <= 0.5)//夹角小于90度，可见
				Fs[i].visible = true;
			else
				Fs[i].visible = false;
		}
#undef Vector
}

	//判断鼠标落点
private:
	//判断点是否在指定面内
	int onFace(CPoint p, Face f) /// 已验证
	{
		std::array<CPoint, 4> poly;
		Matrix tmp(1);
		for (int i = 0;i < poly.size();i++)//三维坐标系的点转换为二维坐标系的点
		{
			tmp[0] = P[f.P_idx[i]];
			tmp = tmp * T_ws;
			poly[i] = trans_point(tmp[0]);
		}

		//判断点是否在凸包内算法
		int wn = 0;
		for (int i = 0; i < poly.size(); i++)
		{
#define Vector Vector2d
			if (Vector::onSegment(p, poly[i], poly[(i + 1) % poly.size()]))
				return -1; //边界
			int k = Vector::dcmp(Vector::cross(Vector(poly[i], poly[(i + 1) % poly.size()]), Vector(poly[i], p)));
			int d1 = Vector::dcmp(poly[i].y - p.y);
			int d2 = Vector::dcmp(poly[(i + 1) % poly.size()].y - p.y);
			if (k > 0 && d1 <= 0 && d2 > 0)
				wn++;
			if (k < 0 && d2 <= 0 && d1 > 0)
				wn--;
		}
		if (wn != 0)
			return 1; //内部
		return 0;     //外部
#undef Vector
	}

public:
	//判断鼠标落在哪个面上
	int onWhichFace(CPoint mp)//返回面的索引。若返回-1则不在面上
	{
		int res = OutFace;//-1
		for (int i = 0;i < F.size();i++)//先判断大面
		{
			if (F[i].visible == true && onFace(mp, F[i]))
				for (int j = 0;j < 9;j++)//再判断大面内的小面
					if (onFace(mp, Fs[i * 9 + j]))
						res = i * 9 + j;
		}

		//DEBUG:
#ifdef debug
		if (debug == mouse_LBdown_onWhichFace)
			system("cls");
		std::cout << "mouse_pos:\t" << mp.x << '\t' << mp.y << std::endl;
		if (res == OutFace)
			std::cout << "Out of all face." << std::endl;
		else
			std::cout << "on face:Fs[" << res << "]\tcolor:" << Color_rev[res / 9] << std::endl;
#endif

		return res;
	}

	//将三维坐标系的点坐标转换为透视投影后的屏幕坐标
	static CPoint trans_point(std::array<double, 4> arr)
	{
		return { (int)round(arr[0]),(int)round(arr[1]) };
	}
	};
