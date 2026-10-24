/*
 * test_plp.cpp
 *
 * Pruebas de la geometria, la lectura de instancias y el verificador de soluciones.
 * uso: test_plp <carpeta benchs>
 */

#include <iostream>
#include <cmath>
#include <cstdio>
#include "ConvexPolytope.h"
#include "Instance.h"
#include "Solution.h"
#include "SmallLP.h"
#include "plpState.h"
#include "Greedy.h"
#include "LPBackend.h"

using namespace std;
using namespace plp;

static int failures=0;

#define CHECK(cond) do{ if(!(cond)) { cerr << __FILE__ << ":" << __LINE__ << ": FAIL: " #cond << endl; failures++; } }while(0)

static bool near(double a, double b, double eps=1e-9) { return fabs(a-b) <= eps; }

static ConvexPolytope cube(double s) {
	vector<Vec3> p;
	for(int i=0;i<8;i++) p.push_back(Vec3((i&1)? s:0, (i&2)? s:0, (i&4)? s:0));
	p.push_back(Vec3(s/2,s/2,s/2));   //punto interior
	p.push_back(Vec3(s,s,s));         //repetido
	p.push_back(Vec3(s/2,0,s/2));     //punto en una cara
	return ConvexPolytope::hull(p);
}

static void test_hull() {
	ConvexPolytope C = cube(2.0);
	CHECK(C.get_vertices().size()==8);
	CHECK(C.get_facets().size()==6);
	CHECK(C.get_edge_dirs().size()==3);
	CHECK(near(C.get_volume(), 8.0));
	CHECK(C.contains(Vec3(1,1,1)));
	CHECK(!C.contains(Vec3(1,1,2.1)));

	vector<Vec3> t = { Vec3(0,0,0), Vec3(1,0,0), Vec3(0,1,0), Vec3(0,0,1) };
	ConvexPolytope T = ConvexPolytope::hull(t);
	CHECK(T.get_facets().size()==4);
	CHECK(near(T.get_volume(), 1.0/6.0));
	CHECK(T.get_edge_dirs().size()==6);
}

static void test_penetration() {
	ConvexPolytope A = cube(1.0);
	CHECK(penetration_depth(A, A.translated(Vec3(1,0,0))) <= 1e-12);     //se tocan
	CHECK(penetration_depth(A, A.translated(Vec3(1.5,0,0))) < 0);         //separados
	CHECK(near(penetration_depth(A, A.translated(Vec3(0.9,0,0))), 0.1));
	CHECK(near(penetration_depth(A, A.translated(Vec3(0.7,0.6,0.8))), 0.2));

	//arista contra arista: cubo rotado 45 grados en z y luego en x
	Mat3 Rz, Rx;
	double c=cos(M_PI/4), s=sin(M_PI/4);
	Rz.m[0][0]=c; Rz.m[0][1]=-s; Rz.m[1][0]=s; Rz.m[1][1]=c;
	Rx.m[1][1]=c; Rx.m[1][2]=-s; Rx.m[2][1]=s; Rx.m[2][2]=c;
	Mat3 R;
	for(int i=0;i<3;i++) for(int j=0;j<3;j++){
		R.m[i][j]=0;
		for(int k=0;k<3;k++) R.m[i][j]+=Rx.m[i][k]*Rz.m[k][j];
	}
	CHECK(R.is_rotation());
	ConvexPolytope B = A.transformed(R, Vec3(0,0,0));
	CHECK(near(B.get_volume(), 1.0));
	//se separa B hacia arriba hasta que apenas toque a A
	double zmin = B.get_bbox().lo.z;
	ConvexPolytope B1 = B.translated(Vec3(0, 0, 1.0-zmin+1e-9));
	CHECK(penetration_depth(A, B1) <= 0);
	ConvexPolytope B2 = B.translated(Vec3(0, 0, 1.0-zmin-0.05));
	double d = penetration_depth(A, B2);
	CHECK(d > 0 && d <= 0.05+1e-12);
}

static void test_solution(const string& benchs) {
	Instance inst = Instance::load(benchs + "/Stoyan et al. 2005/Example1.json");
	CHECK(inst.n_items()==7);
	CHECK(near(inst.W, 12) && near(inst.D, 10));

	Solution S = Solution::load(benchs + "/Stoyan et al. 2005/Solution1.json", inst);
	Report r = S.verify();
	CHECK(r.feasible);
	CHECK(near(r.height, 17.463067949140886, 1e-6));   //las coordenadas del archivo tienen precision simple

	//guardar y volver a leer
	string tmp = "test_plp_solution.json";
	S.save(tmp);
	Solution S2 = Solution::load(tmp, inst);
	remove(tmp.c_str());
	Report r2 = S2.verify();
	CHECK(r2.feasible);
	CHECK(near(r2.height, r.height));

	//una pieza sobre otra
	Solution S3 = S;
	S3.items[1].t = S3.items[0].t;
	CHECK(!S3.verify().feasible);

	//una pieza fuera del contenedor
	Solution S4 = S;
	S4.items[0].t.x = inst.W;
	CHECK(!S4.verify().feasible);

	//falta una pieza
	Solution S5 = S;
	S5.items.pop_back();
	CHECK(!S5.verify().feasible);
	CHECK(S5.verify(1e-6, true).feasible);
}

static void test_instances(const string& benchs) {
	//piezas no convexas descritas como union de convexos
	Instance inst = Instance::load(benchs + "/Stoyan et al. 2004/Example2.json");
	CHECK(inst.n_items()==20);
	for(const Piece& P : inst.pieces) CHECK(P.convex_parts);
	CHECK(near(inst.pieces[0].volume, 1056));

	//STL binario/ASCII
	Instance eg = Instance::load(benchs + "/Egeblad et al. 2009/Merged1.json");
	CHECK(eg.n_items()==15);
	CHECK(near(eg.pieces[0].volume, 256, 1e-6));
}

static void test_lp() {
	//min z - x  s.a.  0 <= x,y,z <= 1,  x + z >= 1  (optimo: x=1, z=0)
	SmallLP lp;
	for(int k=0; k<3; k++){
		Vec3 u; u[k]=1;
		lp.add(u, 1.0);
		lp.add(-u, 0.0);
	}
	lp.add(Vec3(-1,0,-1), -1.0);
	bool opt;
	Vec3 x = lp.minimize(Vec3(-1,0,1), Vec3(0.5,0.5,1), &opt);
	CHECK(opt);
	CHECK(near(x.x, 1.0) && near(x.z, 0.0));
}

static void test_greedy(const string& benchs) {
	Instance inst = Instance::load(benchs + "/Stoyan et al. 2005/Example1.json");
	Placer placer(inst);
	PLP_Function f(placer);
	plpState s(inst);
	Greedy gr(&f);
	gr.run(s);
	CHECK(s.get_n_left()==0);
	Report r = s.get_solution().verify();
	CHECK(r.feasible);
	CHECK(near(s.get_value(), r.density));
}

static void test_compact(const string& benchs) {
	LPBackend* lp = LPBackend::create();
	if(!lp) { cout << "(sin SoPlex: no se prueba la compactacion)" << endl; return; }
	Instance inst = Instance::load(benchs + "/Stoyan et al. 2005/Example3.json");
	Placer placer(inst);
	PLP_Function f(placer);
	plpState s(inst);
	Greedy gr(&f);
	gr.run(s);
	vector<int> pieces;
	vector<Vec3> t;
	s.get_positions(pieces, t);
	double H0 = s.get_height();
	double H = placer.compact(pieces, t, *lp);
	CHECK(H <= H0 + 1e-9);
	s.set_positions(t);
	CHECK(near(s.get_height(), H));
	CHECK(s.get_solution().verify().feasible);
	delete lp;
}

int main(int argc, char** argv) {
	if(argc<2) { cerr << "uso: test_plp <carpeta benchs>" << endl; return 1; }
	try{
		test_hull();
		test_penetration();
		test_solution(argv[1]);
		test_instances(argv[1]);
		test_lp();
		test_greedy(argv[1]);
		test_compact(argv[1]);
	}catch(exception& e){
		cerr << "ERROR: " << e.what() << endl;
		return 1;
	}
	if(failures) { cerr << failures << " pruebas fallidas" << endl; return 1; }
	cout << "OK" << endl;
	return 0;
}
