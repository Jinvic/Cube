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
public:
	Matrix P;    // �����嶥������
	Matrix T_ws; // ��������ϵ����Ļ����ϵ��͸��ͶӰ����任����

	struct Face // ��
	{
		std::array<int, 4> P_idx; // �ĸ����������
		bool visible = false;     // �ɼ���
	};
	std::array<Face, 6> F;//��ɫ�棬����Ϊ����ư׳���

	static enum Color { blue, red, yellow, white, orange, green };//��ɫ��

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
		const double Theta = PI / 4, Phi = acos(1 / sqrt(3)); // ��ΪO_wO_v��z_w��н�,��Ϊ��y_w��н�

		// ��ʼ��͸��ͶӰ�任����
		T_ws.push_back({ cos(Theta), -cos(Phi) * sin(Theta), 0, -sin(Phi) * sin(Theta) / d });
		T_ws.push_back({ 0, sin(Phi), 0, -cos(Phi) / d });
		T_ws.push_back({ -sin(Theta), -cos(Phi) * cos(Theta), 0, -sin(Phi) * cos(Theta) / d });
		T_ws.push_back({ 0, 0, 0, R / d });

		// ��ʼ�������嶥��
		P.push_back({ D / 2, D / 2, D / 2, 1 });
		P.push_back({ D / 2, D / 2, -D / 2, 1 });
		P.push_back({ -D / 2, D / 2, -D / 2, 1 });
		P.push_back({ -D / 2, D / 2, D / 2, 1 });
		P.push_back({ D / 2, -D / 2, D / 2, 1 });
		P.push_back({ D / 2, -D / 2, -D / 2, 1 });
		P.push_back({ -D / 2, -D / 2, -D / 2, 1 });
		P.push_back({ -D / 2, -D / 2, D / 2, 1 });

		// ��ʼ����Ķ��㣬����ʱ������
		F[blue].P_idx = { 0, 1, 2, 3 };//��
		F[red].P_idx = { 0, 3, 7, 4 };//��
		F[yellow].P_idx = { 0, 4, 5, 1 };//��
		F[white].P_idx = { 6, 7, 3, 2 };//��
		F[orange].P_idx = { 6, 2, 1, 5 };//��
		F[green].P_idx = { 6, 5, 4, 7 };//��
	}

private:
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

public:
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

	// �����㷨
	void HiddenSurfaceRemovalAlgorithm(void)
	{
		struct Vector
		{
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
			static Vector cross(Vector v1, Vector v2)
			{
				Vector v3;
				v3.x = v1.y * v2.z - v2.y * v1.z;
				v3.y = v1.z * v2.x - v2.z * v1.x;
				v3.z = v1.x * v2.y - v2.x * v1.y;
				return v3;
			}

			// �����˷�(������)
			static double dot(Vector v1, Vector v2)
			{
				return
					v1.x * v2.x +
					v1.y * v2.y +
					v1.z * v2.z;
			}

			// ����ģ
			static double length(Vector v)
			{
				double l2 = dot(v, v);
				return (l2 < 0) ? 0 : sqrt(l2);
			}

			// �����н� cos��=a*b/(|a|*|b|)
			static double angel(Vector v1, Vector v2)
			{
				return acos(dot(v1, v2) / length(v1) / length(v2));
			}
		};

		//DEBUG:
#ifdef debug
		AllocConsole();
#endif
		double a[6] = { 0 };//�нǺ�Pi�ı�ֵ
		Vector vv = { 1, 1, 1 }; // ������
		for (int i = 0; i < F.size(); i++)
		{
			// ��������ⷨ����
			Vector v1 = Vector(P[F[i].P_idx[0]], P[F[i].P_idx[1]]),
				v2 = Vector(P[F[i].P_idx[1]], P[F[i].P_idx[2]]);
			Vector vf = Vector::cross(v1, v2);

			// �����ⷨ�����������ļн�
			double Theta = Vector::angel(vf, vv);
			a[i] = Theta / acos(-1);
			// �жϿɼ���
			if (a[i] >= 0 && a[i] <= 0.5)//�н�С��90�ȣ��ɼ�
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

	//����ά����ϵ�ĵ�����ת��Ϊ͸��ͶӰ�����Ļ����
	static CPoint trans_point(std::array<double, 4> arr)
	{
		return { (int)round(arr[0]),(int)round(arr[1]) };
	}
};
