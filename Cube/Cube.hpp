#pragma once
#include <vector>
#include <array>

typedef std::vector<std::array<double, 4>> Matrix;
inline Matrix operator*(Matrix a, Matrix b) // ����˷�
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
	Matrix P;    // �����嶥������
	Matrix T_ws; // ��������ϵ����Ļ����ϵ��͸��ͶӰ����任����

	Cube(double D = 300, double a = 3, double b = 5) // D������߳���abΪ������
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

		Point O_w = { 0, 0, 0 };             // ��������ϵԭ��
		Point O_s = { a * D, a * D, a * D }; // ��Ļ����ϵԭ��
		Point O_v = { b * D, b * D, b * D }; // �۲�����ϵԭ��
		double R = Point::dist(O_w, O_v);         // RΪO_w��O_v����
		double d = Point::dist(O_s, O_v);         // dΪO_s��O_v����
		const double PI = acos(-1);
		const double Theta = PI / 4, Phi = PI / 4; // ��ΪO_wO_v��z_w��н�,��Ϊ��y_w��н�

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

	// ��x����ת
	void rotate_x(double Beta) // BetaΪ��ת�Ƕ�
	{
		Matrix T_x; // ��x����ת�任����

		T_x.push_back({ 1, 0, 0, 0 });
		T_x.push_back({ 0, cos(Beta), sin(Beta), 0 });
		T_x.push_back({ 0, -sin(Beta), cos(Beta), 0 });
		T_x.push_back({ 0, 0, 0, 1 });

		P = P * T_x;
	}
	// ��y����ת
	void rotate_y(double Beta) // BetaΪ��ת�Ƕ�
	{
		Matrix T_y; // ��y����ת�任����

		T_y.push_back({ cos(Beta), 0, -sin(Beta), 0 });
		T_y.push_back({ 0, 1, 0, 0 });
		T_y.push_back({ sin(Beta), 0, cos(Beta), 0 });
		T_y.push_back({ 0, 0, 0, 1 });

		P = P * T_y;
	}
	// ��z����ת
	void rotate_z(double Beta) // BetaΪ��ת�Ƕ�
	{
		Matrix T_z; // ��z����ת�任����

		T_z.push_back({ cos(Beta), sin(Beta), 0, 0 });
		T_z.push_back({ -sin(Beta), cos(Beta), 0, 0 });
		T_z.push_back({ 0, 0, 1, 0 });
		T_z.push_back({ 0, 0, 0, 1 });

		P = P * T_z;
	}

	//ͨ������ƶ�������ת
	void rotate(int dx, int dy, int limit = 500)//limitΪ����ƶ�����
	{
		double Theta_v = 0, Phi_v = 0; // ��Ļ����ϵ����y�ᣨˮƽ�ƶ�����x�ᣨ�����ƶ�����ת�ĽǶ�
		// ������ƶ�����ת��Ϊ��Ӧ�Ƕ�
		int vx = (dx >= 0) ? 1 : -1, vy = (dy >= 0) ? 1 : -1;//ȷ������
		dx = min(abs(dx), limit);
		dy = min(abs(dy), limit);
		Theta_v = vx * ((double)dx / 500) * acos(-1);//�����ת�Ƕ�Pi
		Phi_v = vy * ((double)dy / 500) * acos(-1);

		double Beta_x = 0, Beta_y = 0, Beta_z = 0; // ��������ϵ������������ת�ĽǶ�
		// ����������ϵ�����������
		rotate_y(-acos(-1) / 4);
		rotate_x(acos(-1) / 4);
		// ��ת
		rotate_y(Theta_v);
		rotate_x(Phi_v);
		// ��ԭ����
		rotate_x(-acos(-1) / 4);
		rotate_y(acos(-1) / 4);
	}

	static CPoint trans_point(std::array<double, 4> arr)
	{
		return { (int)round(arr[0]),(int)round(arr[1]) };
	}
};
