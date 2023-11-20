#pragma once

//2D����,�����жϵ��Ƿ���͹����
struct Vector2d
{
#define Vector Vector2d
	double x, y;

	// Ĭ�Ϲ��캯��
	Vector(double a = 0, double b = 0)
	{
		x = a;
		y = b;
	}

	// p1ָ��p2�������Ĺ��캯��
	Vector(CPoint p1, CPoint p2)
	{
		x = p2.x - p1.x;
		y = p2.y - p1.y;
	}

	// �����˷�(������)
	inline static double cross(Vector v1, Vector v2)
	{
		return  v1.x * v2.y - v2.x * v1.y;
	}

	// �����˷�(������)
	inline static double dot(Vector v1, Vector v2)
	{
		return v1.x * v2.x + v1.y * v2.y;
	}

	//��0�Ƚ� > = < 1 0 -1
	inline static int dcmp(double x)
	{
		if (fabs(x) < 1e-8)
			return 0;
		else
			return x < 0 ? -1 : 1;
	}

	//����p�߶���(�����˵�)
	inline static bool onSegment(CPoint p, CPoint a1, CPoint a2)
	{ // pa1��pa2���ߣ���a1a2��p����
		return
			dcmp(cross(Vector(p, a1), Vector(p, a2))) == 0 &&
			dcmp(dot(Vector(p, a1), Vector(p, a2))) <= 0;
	}

	// ����ģ
	inline static double length(Vector v)
	{
		double l2 = dot(v, v);
		return (l2 < 0) ? 0 : sqrt(l2);
	}

	// �����н� cos��=a*b/(|a|*|b|)
	inline static double angel(Vector v1, Vector v2)
	{
		return acos(dot(v1, v2) / length(v1) / length(v2));
	}
#undef Vector
};

//3D����
struct Vector3d
{
#define Vector Vector3d
	double x, y, z;

	// Ĭ�Ϲ��캯��
	Vector(double a = 0, double b = 0, double c = 0)
	{
		x = a;
		y = b;
		z = c;
	}

	// p1ָ��p2�������Ĺ��캯��
	Vector(std::array<double, 4> p1, std::array<double, 4> p2)
	{
		x = p2[0] - p1[0];
		y = p2[1] - p1[1];
		z = p2[2] - p1[2];
	}

	// �����˷�(������)
	inline static Vector cross(Vector v1, Vector v2)
	{
		Vector v3;
		v3.x = v1.y * v2.z - v2.y * v1.z;
		v3.y = v1.z * v2.x - v2.z * v1.x;
		v3.z = v1.x * v2.y - v2.x * v1.y;
		return v3;
	}

	// �����˷�(������)
	inline static double dot(Vector v1, Vector v2)
	{
		return
			v1.x * v2.x +
			v1.y * v2.y +
			v1.z * v2.z;
	}

	// ����ģ
	inline static double length(Vector v)
	{
		double l2 = dot(v, v);
		return (l2 < 0) ? 0 : sqrt(l2);
	}

	// �����н� cos��=a*b/(|a|*|b|)
	inline static double angel(Vector v1, Vector v2)
	{
		return acos(dot(v1, v2) / length(v1) / length(v2));
	}
#undef Vector
};