#pragma once

//2D向量,用于判断点是否在凸包内
struct Vector2d
{
#define Vector Vector2d
	double x, y;

	// 默认构造函数
	Vector(double a = 0, double b = 0)
	{
		x = a;
		y = b;
	}

	// p1指向p2的向量的构造函数
	Vector(CPoint p1, CPoint p2)
	{
		x = p2.x - p1.x;
		y = p2.y - p1.y;
	}

	// 向量乘法(向量积)
	inline static double cross(Vector v1, Vector v2)
	{
		return  v1.x * v2.y - v2.x * v1.y;
	}

	// 向量乘法(数量积)
	inline static double dot(Vector v1, Vector v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}

	//和0比较 > = < 1 0 -1
	inline static int dcmp(double x)
	{
		if (fabs(x) < 1e-8)
			return 0;
		else
			return x < 0 ? -1 : 1;
	}

	//点在p线段上(包括端点)
	inline static bool onSegment(CPoint p, CPoint a1, CPoint a2)
	{ // pa1和pa2共线，且a1a2在p两端
		return
			dcmp(cross(Vector(p, a1), Vector(p, a2))) == 0 &&
			dcmp(dot(Vector(p, a1), Vector(p, a2))) <= 0;
	}

	// 向量模
	inline static double length(Vector v)
	{
		double l2 = dot(v, v);
		return (l2 < 0) ? 0 : sqrt(l2);
	}

	// 向量夹角 cosθ=a*b/(|a|*|b|)
	inline static double angel(Vector v1, Vector v2)
	{
		return acos(dot(v1, v2) / length(v1) / length(v2));
	}
#undef Vector
};

//3D向量
struct Vector3d
{
#define Vector Vector3d
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
	inline static Vector cross(Vector v1, Vector v2)
	{
		Vector v3;
		v3.x = v1.y * v2.z - v2.y * v1.z;
		v3.y = v1.z * v2.x - v2.z * v1.x;
		v3.z = v1.x * v2.y - v2.x * v1.y;
		return v3;
	}

	// 向量乘法(数量积)
	inline static double dot(Vector v1, Vector v2)
	{
		return
			v1.x * v2.x +
			v1.y * v2.y +
			v1.z * v2.z;
	}

	// 向量模
	inline static double length(Vector v)
	{
		double l2 = dot(v, v);
		return (l2 < 0) ? 0 : sqrt(l2);
	}

	// 向量夹角 cosθ=a*b/(|a|*|b|)
	inline static double angel(Vector v1, Vector v2)
	{
		return acos(dot(v1, v2) / length(v1) / length(v2));
	}
#undef Vector
};