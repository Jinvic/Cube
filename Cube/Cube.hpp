#pragma once

const enum debug_flags { real_time_visibility, mouse_LBdown_onWhichFace, layer_rotate_test };
#define debug layer_rotate_test
//#undef debug
#ifdef debug
#pragma comment(linker, "/subsystem:console /entry:wWinMainCRTStartup") 
#endif // debug

#include <vector>
#include <array>
#include <set>
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

		std::array<int, 2>L_idx;//��Ӧ��
		std::array<int, 2> rd;//����ת������ʱ��Ϊ�� {1��-1}
		//Fs�����ã�����ÿ��С��Ķ�Ӧ�����ת����
		void set_layer(int id, int lid, int rdid)
		{
			L_idx[id] = lid;
			rd[id] = rdid;
		}
	};
	std::array<Face, 6> F;//��ɫ�棬����Ϊ����ư׳���
	std::vector<Face> Fs;//36�棬ÿ9��Ϊһɫ

	static const enum Color { blue, red, yellow, white, orange, green };//��ɫ��
	const std::string Color_rev[6] = { "blue", "red", "yellow", "white", "orange", "green" };//DEBUG��

	static const int OutFace = -1;//�ж��������ã��ó�����ʾδ��������

	struct Layer
	{
	public:
		std::set<int> P_idx;// �ĸ���������������˳������������ͬ
		static const enum Axis { x, y, z };
		Axis axis;
		Cube* pCube;

	public:
		//��������е�����
		void add_face(int fs_idx)
		{
			for (auto pid : pCube->Fs[fs_idx].P_idx)
				P_idx.insert(pid);
		}
		void add_face(std::array<int, 12> fs_idx_arr)
		{
			for (auto fs_idx : fs_idx_arr)
				for (auto pid : pCube->Fs[fs_idx].P_idx)
					P_idx.insert(pid);
		}

		// ����������ת
		void rotate(double Beta) // BetaΪ��ת�Ƕ�
		{
			Matrix T_x; // ��x����ת�任����
			Matrix T_y; // ��y����ת�任����
			Matrix T_z; // ��z����ת�任����
			{
				T_x.push_back({ 1, 0, 0, 0 });
				T_x.push_back({ 0, cos(Beta), sin(Beta), 0 });
				T_x.push_back({ 0, -sin(Beta), cos(Beta), 0 });
				T_x.push_back({ 0, 0, 0, 1 });
				T_y.push_back({ cos(Beta), 0, -sin(Beta), 0 });
				T_y.push_back({ 0, 1, 0, 0 });
				T_y.push_back({ sin(Beta), 0, cos(Beta), 0 });
				T_y.push_back({ 0, 0, 0, 1 });
				T_z.push_back({ cos(Beta), sin(Beta), 0, 0 });
				T_z.push_back({ -sin(Beta), cos(Beta), 0, 0 });
				T_z.push_back({ 0, 0, 1, 0 });
				T_z.push_back({ 0, 0, 0, 1 });
			}

			Matrix LP(pCube->P.size());
			for (auto idx : P_idx)
				LP[idx] = pCube->P[idx];
			switch (axis)
			{
			case x:
				LP = LP * T_x;
			case y:
				LP = LP * T_y;
			case z:
				LP = LP * T_z;
			default:
				break;
			}
			for (auto idx : P_idx)
				pCube->P[idx] = LP[idx];
		}
	}L[9];//ǰ����Ϊ����ư׳���,������xyz��Ϊ������
	int sum_dx, sum_dy;//���ڻ�ԭ�����徭��������ת

	//���캯�����󲿷ֳ�ʼ�����������
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

		//ָ�����㷨��������ת�ᣩ
		for (int i = 0;i < 9;i++)
			L[i].pCube = this;
		L[0].axis = L[5].axis = Layer::Axis::y;
		L[1].axis = L[3].axis = Layer::Axis::z;
		L[2].axis = L[4].axis = Layer::Axis::x;

		//TODO:��ʼ�����㶥��
		for (int i = 0;i < 6;i++)
			for (int j = 0;j < 9;j++)
				L[i].add_face(i * 9 + j);
		std::array<int, 12> fs_idx_arr;
		fs_idx_arr = { 9,10,11,18,24,25,31,32,33,38,39,40 };
		L[0].add_face(fs_idx_arr);//��
		fs_idx_arr = { 0,6,7,18,19,20,29,30,31,49,50,51 };
		L[1].add_face(fs_idx_arr);//��
		fs_idx_arr = { 0,1,2,9,15,16,40,41,42,47,48,49 };
		L[2].add_face(fs_idx_arr);//��
		fs_idx_arr = { 4,5,6,11,12,13,36,37,38,45,51,52 };
		L[3].add_face(fs_idx_arr);//��
		fs_idx_arr = { 2,3,4,22,23,24,31,32,33,38,39,40 };
		L[4].add_face(fs_idx_arr);//��
		fs_idx_arr = { 9,10,11,18,24,25,27,33,34,45,46,47 };
		L[5].add_face(fs_idx_arr);//��
		fs_idx_arr = { 3,7,8,20,21,22,27,28,29,36,42,43 };
		L[6].add_face(fs_idx_arr);//��x��
		fs_idx_arr = { 12,16,17,19,23,26,30,34,35,37,41,44 };
		L[7].add_face(fs_idx_arr);//��y��
		fs_idx_arr = { 1,5,8,21,25,26,28,32,35,48,52,53 };
		L[8].add_face(fs_idx_arr);//��z��

		//��ʼ�������Ӧ�����ת����
		set_layers(0, { 1,8,4,2,6,3 }, { 1,-1 });
		set_layers(1, { 2,6,3,0,7,5 }, { 1,-1 });
		set_layers(2, { 0,7,5,1,8,4 }, { 1,-1 });
		set_layers(3, { 4,8,1,5,7,0 }, { -1,1 });
		set_layers(4, { 5,7,0,3,6,2 }, { -1,1 });
		set_layers(5, { 3,6,2,4,8,1 }, { -1,1 });


		//DEBUG:
#ifdef debug
		AllocConsole();
		std::cout << "P.size():\t" << P.size() << std::endl;
		std::cout << "Fs.size()\t" << Fs.size() << std::endl;
		std::cout << "F.size()\t" << F.size() << std::endl;
		std::cout << "L.size()\t" << 9 << std::endl;
#endif
	}

private:
	//F�����ã�����ÿ������ĸ���С��
	void set_layers(int F_idx, std::array<int, 6>lid_arr, std::array<int, 2> rd)
	{
		int id = 0;
		std::array<int, 3>fsid_arr;
		fsid_arr = { 0,6,7 };
		for (int i = 0;i < 3;i++)
			Fs[F_idx * 9 + fsid_arr[i]].set_layer(id, lid_arr[id * 3 + 0], rd[id]);
		fsid_arr = { 1,5,8 };
		for (int i = 0;i < 3;i++)
			Fs[F_idx * 9 + fsid_arr[i]].set_layer(id, lid_arr[id * 3 + 1], rd[id]);
		fsid_arr = { 2,3,4 };
		for (int i = 0;i < 3;i++)
			Fs[F_idx * 9 + fsid_arr[i]].set_layer(id, lid_arr[id * 3 + 2], rd[id]);

		id = 1;
		fsid_arr = { 0,1,2 };
		for (int i = 0;i < 3;i++)
			Fs[F_idx * 9 + fsid_arr[i]].set_layer(id, lid_arr[id * 3 + 0], rd[id]);
		fsid_arr = { 3,7,8 };
		for (int i = 0;i < 3;i++)
			Fs[F_idx * 9 + fsid_arr[i]].set_layer(id, lid_arr[id * 3 + 1], rd[id]);
		fsid_arr = { 4,5,6 };
		for (int i = 0;i < 3;i++)
			Fs[F_idx * 9 + fsid_arr[i]].set_layer(id, lid_arr[id * 3 + 2], rd[id]);
	}

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
	//��f.P_idx[id]��������������ظ���
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
	//��f.P_idx[id]������������ظ���
	void add_point(Point p, Face& f, int id)
	{
		f.P_idx[id] = P.size();
		P.push_back(p);
	}
	//���Ƶ�
	void copy_point(Face& f, int id)
	{
		P.push_back(P[f.P_idx[id]]);
		f.P_idx[id] = P.size() - 1;
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

			//ÿ�����㣬�ڲ�����ظ��㣬��ߵ����غϴ�
			Face f;
			{
				f.P_idx[0] = F[i].P_idx[0];
				check_point(p_mid[0].first, f, 1);
				add_point(p_mid[4].first, f, 2);
				check_point(p_mid[3].second, f, 3);
				Fs.push_back(f);

				check_point(p_mid[0].first, f, 0);
				check_point(p_mid[0].second, f, 1);
				add_point(p_mid[5].first, f, 2);
				add_point(p_mid[4].first, f, 3);
				Fs.push_back(f);

				f.P_idx[1] = F[i].P_idx[1];
				check_point(p_mid[0].second, f, 0);
				check_point(p_mid[1].first, f, 2);
				add_point(p_mid[5].first, f, 3);
				Fs.push_back(f);

				add_point(p_mid[5].first, f, 0);
				check_point(p_mid[1].first, f, 1);
				check_point(p_mid[1].second, f, 2);
				add_point(p_mid[5].second, f, 3);
				Fs.push_back(f);

				f.P_idx[2] = F[i].P_idx[2];
				add_point(p_mid[5].second, f, 0);
				check_point(p_mid[1].second, f, 1);
				check_point(p_mid[2].first, f, 3);
				Fs.push_back(f);

				add_point(p_mid[4].second, f, 0);
				add_point(p_mid[5].second, f, 1);
				check_point(p_mid[2].first, f, 2);
				check_point(p_mid[2].second, f, 3);
				Fs.push_back(f);

				f.P_idx[3] = F[i].P_idx[3];
				check_point(p_mid[3].first, f, 0);
				add_point(p_mid[4].second, f, 1);
				check_point(p_mid[2].second, f, 2);
				Fs.push_back(f);

				check_point(p_mid[3].second, f, 0);
				add_point(p_mid[4].first, f, 1);
				add_point(p_mid[4].second, f, 2);
				check_point(p_mid[3].first, f, 3);
				Fs.push_back(f);

				add_point(p_mid[4].first, f, 0);
				add_point(p_mid[5].first, f, 1);
				add_point(p_mid[5].second, f, 2);
				add_point(p_mid[4].second, f, 3);
				Fs.push_back(f);
			}
		}
		//����غϵ���ߵ�
		{
			for (int i = 0;i < 3;i++)//ǰ����ȡ�ұ������
			{
				copy_point(Fs[i * 9 + 1], 0);
				copy_point(Fs[i * 9 + 1], 1);
				copy_point(Fs[i * 9 + 3], 1);
				copy_point(Fs[i * 9 + 3], 2);
			}
			for (int i = 3;i < 6;i++)//������ȡ��������
			{
				copy_point(Fs[i * 9 + 5], 2);
				copy_point(Fs[i * 9 + 5], 3);
				copy_point(Fs[i * 9 + 7], 3);
				copy_point(Fs[i * 9 + 7], 0);
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
	void rotate(int dx, int dy, bool LB_up = false, int limit = 500)//limitΪ����ƶ�����
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

		if (LB_up)
			sum_dx += dx, sum_dy += dy;
	}

	//ͨ������ƶ���ת��
	void rotate_layer(int dx, int dy, int f_idx, bool LB_up = false, int limit = 400)//limitΪ����ƶ�����
	{
		//����ƶ�����ϵ��ͬ����Ҫת��
		dy = -dy;
		//�������ά����ת��Ϊ��ά����
		Face f = Fs[f_idx];
		std::array<CPoint, 4> FP;
		Matrix mtmp(1);
		for (int i = 0;i < 4;i++)//��ά����ϵ�ĵ�ת��Ϊ��ά����ϵ�ĵ�
		{
			mtmp[0] = P[f.P_idx[i]];
			mtmp = mtmp * T_ws;
			FP[i] = trans_point(mtmp[0]);
		}

		Vector2d v0(FP[0], FP[3]), v1(FP[0], FP[1]);//���������ϵ�����
		Vector2d vm(dx, dy);//����ƶ�������
		double len = Vector2d::length(vm);
		double a0 = Vector2d::angel(v0, vm), a1 = Vector2d::angel(v1, vm);//�����н�
		const double PI = acos(-1);
		double Theta;//��ת�Ƕ�
		int id;
		if (min(a0, PI - a0) <= min(a1, PI - a1))//���ĸ�������ת
		{
			len = len * cos(a0);
			Theta = min(abs(len), limit) / limit * PI / 2;//�����ת�Ƕ�PI/2
			id = 0;
		}
		else
		{
			len = len * cos(a1);
			Theta = min(abs(len), limit) / limit * PI / 2;//�����ת�Ƕ�PI/2
			id = 1;
		}
		//�ɿ����ʱ����
		if (LB_up)
		{
			if (abs(Theta) < PI / 4)//�ǶȲ���������ת
				return;
			else//��ת���Ƕ�
				Theta = PI / 2;
		}
		if (Theta * len < 0)Theta = -Theta;//��������ͬ��

		// �������廹ԭ����ʼ��ת״̬��������ԭλ��
		rotate(-sum_dx, -sum_dy);
		//��ת��Ӧ��
		L[f.L_idx[id]].rotate(Theta * f.rd[id]);
		// ��ԭ����
		rotate(sum_dx, sum_dy);

#ifdef debug
		if (debug == layer_rotate_test)
		{
			system("cls");
			std::cout << "length of vector:\t" << len << std::endl;
			std::cout << "angle of rotate:\t" << Theta << std::endl;
			if (Theta * f.rd[id] >= 0)
				std::cout << "direction of rotate:\t" << "foward(anticlockwise)" << std::endl;
			else
				std::cout << "direction of rotate:\t" << "reverse(clockwise)" << std::endl;
		}
#endif // !debug
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
		{
			system("cls");
			std::cout << "mouse_pos:\t" << mp.x << '\t' << mp.y << std::endl;
			if (res == OutFace)
				std::cout << "Out of all face." << std::endl;
			else
			{
				std::cout << "on face:Fs[" << res << "]\tcolor:" << Color_rev[res / 9] << std::endl;
				std::cout << "L_idx:\t" << Fs[res].L_idx[0] << ' ' << Fs[res].L_idx[1] << std::endl;
				std::cout << "rd:\t" << Fs[res].rd[0] << ' ' << Fs[res].rd[1] << std::endl;
			}
		}
#endif

		return res;
	}

	//����ά����ϵ�ĵ�����ת��Ϊ͸��ͶӰ�����Ļ����
	static CPoint trans_point(std::array<double, 4> arr)
	{
		return { (int)round(arr[0]),(int)round(arr[1]) };
	}
};
