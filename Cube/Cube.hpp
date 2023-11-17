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
		std::array<int, 4> P_idx; // �ĸ����������������ʱ��˳����
		bool visible = false;     // �ɼ���
	};
	std::array<Face, 6> F;//��ɫ�棬����Ϊ����ư׳���
	std::vector<Face> Fs;//36�棬ÿ9��Ϊһɫ

	static const enum Color { blue, red, yellow, white, orange, green };//��ɫ��
	const std::string Color_rev[6] = { "blue", "red", "yellow", "white", "orange", "green" };//DEBUG��

	static const int OutFace = -1;//�ж��������ã��ó�����ʾδ��������

	struct Layer
	{
		std::array<int, 8> P_idx;// �ĸ���������������˳������������ͬ
		Vector3d v;//������
	}L[9];

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
		const double Theta = PI / 4, Phi = acos(1 / sqrt(3)); // ��ΪO_wO_v��xOzƽ���ϵ�ͶӰ��z��н�,��Ϊ��y��н�

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

		//8��6���������ϸ��Ϊ156��54���ħ��
		trans_cube();

		//��ʼ�����㶥��
		//L[0].P_idx = { 0, 1, 2, 3,F[9].P_idx[3],F[40].P_idx[3],F[38].P_idx[0],F[11].P_idx[2] };//��
		//L[1].P_idx = { 0, 3, 7, 4,F[0].P_idx[1],F[51].P_idx[0],F[49].P_idx[1],F[47].P_idx[0] };//��
		//L[2].P_idx = { 0, 4, 5, 1,F[0].P_idx[3],F[49].P_idx[3],F[47].P_idx[0],F[2].P_idx[2] };//��
		//L[3].P_idx = { 6, 7, 3, 2,F[9].P_idx[3],F[47].P_idx[0],F[49].P_idx[3],F[11].P_idx[2] };//��
		//L[4].P_idx = { 6, 2, 1, 5,F[9].P_idx[3],F[47].P_idx[0],F[49].P_idx[3],F[11].P_idx[2] };//��
		//L[5].P_idx = { 6, 5, 4, 7,F[9].P_idx[3],F[47].P_idx[0],F[49].P_idx[3],F[11].P_idx[2] };//��
		//L[6].P_idx = { 0, 1, 2, 3,F[9].P_idx[3],F[47].P_idx[0],F[49].P_idx[3],F[11].P_idx[2] };//x��Ϊ������
		//L[7].P_idx = { 0, 1, 2, 3,F[9].P_idx[3],F[47].P_idx[0],F[49].P_idx[3],F[11].P_idx[2] };//y��Ϊ������
		//L[8].P_idx = { 0, 1, 2, 3,F[9].P_idx[3],F[47].P_idx[0],F[49].P_idx[3],F[11].P_idx[2] };//z��Ϊ������

		//DEBUG:
#ifdef debug
		AllocConsole();
		std::cout << "P.size():\t" << P.size() << std::endl;
		std::cout << "Fs.size()\t" << Fs.size() << std::endl;
#endif
	}

private:
	//8��6���������ϸ��Ϊ156��54���ħ��
#define Point std::array<double, 4>
	//��������
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
	//�õ��Ƿ��Ѵ�
	int search_point(Point p)
	{
		for (int i = 0;i < P.size();i++)
			if (P[i] == p)
				return i;
		return -1;
	}
	//��f.P_idx[id]�������
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
		//4��1��ϸ��Ϊ36��9��
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
	//���߶ε������е�
	std::pair<Point, Point> middle2(Point p_st, Point p_end)
	{
		Point p = divide(minus(p_end, p_st), 3);
		Point p1 = plus(p_st, p);
		Point p2 = plus(p1, p);
		return make_pair(p1, p2);
	};

#undef Point


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
#define Vector Vector3d
		std::vector<double> a(Fs.size());//�нǺ�Pi�ı�ֵ
		Vector vv = { 1, 1, 1 }; // ������
		//9������Ŀɼ���
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

		//54��С��Ŀɼ���
		for (int i = 0; i < Fs.size(); i++)
		{
			// ��������ⷨ����
			Vector v1 = Vector(P[Fs[i].P_idx[0]], P[Fs[i].P_idx[1]]),
				v2 = Vector(P[Fs[i].P_idx[1]], P[Fs[i].P_idx[2]]);
			Vector vf = Vector::cross(v1, v2);

			// �����ⷨ�����������ļн�
			double Theta = Vector::angel(vf, vv);
			a[i] = Theta / acos(-1);
			// �жϿɼ���
			if (a[i] >= 0 && a[i] <= 0.5)//�н�С��90�ȣ��ɼ�
				Fs[i].visible = true;
			else
				Fs[i].visible = false;
		}
#undef Vector
}

	//�ж�������
private:
	//�жϵ��Ƿ���ָ������
	int onFace(CPoint p, Face f) /// ����֤
	{
		std::array<CPoint, 4> poly;
		Matrix tmp(1);
		for (int i = 0;i < poly.size();i++)//��ά����ϵ�ĵ�ת��Ϊ��ά����ϵ�ĵ�
		{
			tmp[0] = P[f.P_idx[i]];
			tmp = tmp * T_ws;
			poly[i] = trans_point(tmp[0]);
		}

		//�жϵ��Ƿ���͹�����㷨
		int wn = 0;
		for (int i = 0; i < poly.size(); i++)
		{
#define Vector Vector2d
			if (Vector::onSegment(p, poly[i], poly[(i + 1) % poly.size()]))
				return -1; //�߽�
			int k = Vector::dcmp(Vector::cross(Vector(poly[i], poly[(i + 1) % poly.size()]), Vector(poly[i], p)));
			int d1 = Vector::dcmp(poly[i].y - p.y);
			int d2 = Vector::dcmp(poly[(i + 1) % poly.size()].y - p.y);
			if (k > 0 && d1 <= 0 && d2 > 0)
				wn++;
			if (k < 0 && d2 <= 0 && d1 > 0)
				wn--;
		}
		if (wn != 0)
			return 1; //�ڲ�
		return 0;     //�ⲿ
#undef Vector
	}

public:
	//�ж���������ĸ�����
	int onWhichFace(CPoint mp)//�������������������-1��������
	{
		int res = OutFace;//-1
		for (int i = 0;i < F.size();i++)//���жϴ���
		{
			if (F[i].visible == true && onFace(mp, F[i]))
				for (int j = 0;j < 9;j++)//���жϴ����ڵ�С��
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

	//����ά����ϵ�ĵ�����ת��Ϊ͸��ͶӰ�����Ļ����
	static CPoint trans_point(std::array<double, 4> arr)
	{
		return { (int)round(arr[0]),(int)round(arr[1]) };
	}
	};
