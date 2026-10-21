/*
 * CPSAT.cpp
 */

#include <dlfcn.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "CPSAT.h"

using namespace std;

namespace clp {

namespace {

//numeros de campo de cp_model.proto y sat_parameters.proto (OR-Tools 9)
enum {
	MODEL_VARIABLES=2, MODEL_CONSTRAINTS=3, MODEL_OBJECTIVE=4, MODEL_HINT=6,
	VARIABLE_DOMAIN=2,
	CONSTRAINT_ENFORCEMENT=2, CONSTRAINT_BOOL_OR=3, CONSTRAINT_LINEAR=12, CONSTRAINT_EXACTLY_ONE=29,
	CONSTRAINT_INTERVAL=19, CONSTRAINT_CUMULATIVE=22,
	INTERVAL_START=4, INTERVAL_END=5, INTERVAL_SIZE=6,
	CUMULATIVE_CAPACITY=1, CUMULATIVE_INTERVALS=2, CUMULATIVE_DEMANDS=3,
	EXPR_VARS=1, EXPR_COEFFS=2, EXPR_OFFSET=3,
	LINEAR_VARS=1, LINEAR_COEFFS=2, LINEAR_DOMAIN=3,
	BOOL_LITERALS=1,
	OBJECTIVE_VARS=1, OBJECTIVE_SCALING=3, OBJECTIVE_COEFFS=4,
	HINT_VARS=1, HINT_VALUES=2,
	PARAMS_MAX_TIME=36, PARAMS_NUM_WORKERS=206,
	RESPONSE_STATUS=1, RESPONSE_SOLUTION=2, RESPONSE_OBJECTIVE=3, RESPONSE_BOUND=4
};

enum { VARINT=0, FIXED64=1, LEN=2 };

void put_varint(string& s, uint64_t v){
	while(v>=0x80){ s.push_back((char)(v|0x80)); v>>=7; }
	s.push_back((char)v);
}

void put_key(string& s, int field, int wire){ put_varint(s, ((uint64_t)field<<3) | wire); }

void put_bytes(string& s, int field, const string& bytes){
	put_key(s, field, LEN);
	put_varint(s, bytes.size());
	s += bytes;
}

template<class T>
void put_packed(string& s, int field, const vector<T>& values){
	if(values.empty()) return;
	string p;
	for(T v : values) put_varint(p, (uint64_t)(int64_t) v);
	put_bytes(s, field, p);
}

void put_double(string& s, int field, double d){
	put_key(s, field, FIXED64);
	uint64_t u; memcpy(&u, &d, 8);
	for(int i=0; i<8; i++) s.push_back((char)((u>>(8*i)) & 0xff));
}

bool get_varint(const unsigned char*& p, const unsigned char* end, uint64_t& v){
	v=0;
	for(int shift=0; p<end && shift<64; shift+=7){
		unsigned char b=*p++;
		v |= (uint64_t)(b & 0x7f) << shift;
		if(!(b & 0x80)) return true;
	}
	return false;
}

/** LinearExpressionProto: var + offset (var<0: constante) */
string expression(int var, int64_t offset){
	string e;
	if(var>=0){
		put_packed(e, EXPR_VARS, vector<int>{var});
		put_packed(e, EXPR_COEFFS, vector<int64_t>{1});
	}
	if(offset!=0){
		put_key(e, EXPR_OFFSET, VARINT);
		put_varint(e, (uint64_t) offset);
	}
	return e;
}

}

int CpModel::new_var(int64_t lb, int64_t ub){
	domains.push_back(lb);
	domains.push_back(ub);
	return domains.size()/2-1;
}

void CpModel::add_linear(const vector<int>& vars, const vector<int64_t>& coeffs,
		int64_t lb, int64_t ub, const vector<int>& enforcement){
	string lin;
	put_packed(lin, LINEAR_VARS, vars);
	put_packed(lin, LINEAR_COEFFS, coeffs);
	put_packed(lin, LINEAR_DOMAIN, vector<int64_t>{lb, ub});
	string c;
	put_packed(c, CONSTRAINT_ENFORCEMENT, enforcement);
	put_bytes(c, CONSTRAINT_LINEAR, lin);
	constraints.push_back(c);
}

void CpModel::add_bool_or(const vector<int>& literals, const vector<int>& enforcement){
	string b;
	put_packed(b, BOOL_LITERALS, literals);
	string c;
	put_packed(c, CONSTRAINT_ENFORCEMENT, enforcement);
	put_bytes(c, CONSTRAINT_BOOL_OR, b);
	constraints.push_back(c);
}

void CpModel::add_exactly_one(const vector<int>& literals){
	string b;
	put_packed(b, BOOL_LITERALS, literals);
	string c;
	put_bytes(c, CONSTRAINT_EXACTLY_ONE, b);
	constraints.push_back(c);
}

int CpModel::add_interval(int start, int64_t size, int presence){
	string it;
	put_bytes(it, INTERVAL_START, expression(start, 0));
	put_bytes(it, INTERVAL_END, expression(start, size));
	put_bytes(it, INTERVAL_SIZE, expression(-1, size));
	string c;
	put_packed(c, CONSTRAINT_ENFORCEMENT, vector<int>{presence});
	put_bytes(c, CONSTRAINT_INTERVAL, it);
	constraints.push_back(c);
	return constraints.size()-1;
}

int CpModel::add_fixed_interval(int64_t start, int64_t size){
	string it;
	put_bytes(it, INTERVAL_START, expression(-1, start));
	put_bytes(it, INTERVAL_END, expression(-1, start+size));
	put_bytes(it, INTERVAL_SIZE, expression(-1, size));
	string c;
	put_bytes(c, CONSTRAINT_INTERVAL, it);
	constraints.push_back(c);
	return constraints.size()-1;
}

void CpModel::add_cumulative(int64_t capacity, const vector<int>& intervals, const vector<int64_t>& demands){
	string cu;
	put_bytes(cu, CUMULATIVE_CAPACITY, expression(-1, capacity));
	put_packed(cu, CUMULATIVE_INTERVALS, intervals);
	for(int64_t d : demands) put_bytes(cu, CUMULATIVE_DEMANDS, expression(-1, d));
	string c;
	put_bytes(c, CONSTRAINT_CUMULATIVE, cu);
	constraints.push_back(c);
}

void CpModel::maximize(const vector<int>& vars, const vector<int64_t>& coeffs){
	//CP-SAT minimiza: se minimiza -f con factor de escala -1
	obj_vars=vars;
	obj_coeffs.clear();
	for(int64_t c : coeffs) obj_coeffs.push_back(-c);
}

void CpModel::add_hint(int var, int64_t value){
	hint_vars.push_back(var);
	hint_values.push_back(value);
}

string CpModel::serialize() const{
	string s;
	for(size_t i=0; i<domains.size(); i+=2){
		string v;
		put_packed(v, VARIABLE_DOMAIN, vector<int64_t>{domains[i], domains[i+1]});
		put_bytes(s, MODEL_VARIABLES, v);
	}
	for(const string& c : constraints) put_bytes(s, MODEL_CONSTRAINTS, c);
	if(!obj_vars.empty()){
		string o;
		put_packed(o, OBJECTIVE_VARS, obj_vars);
		put_double(o, OBJECTIVE_SCALING, -1.0);
		put_packed(o, OBJECTIVE_COEFFS, obj_coeffs);
		put_bytes(s, MODEL_OBJECTIVE, o);
	}
	if(!hint_vars.empty()){
		string h;
		put_packed(h, HINT_VARS, hint_vars);
		put_packed(h, HINT_VALUES, hint_values);
		put_bytes(s, MODEL_HINT, h);
	}
	return s;
}

CpSolver::CpSolver(const string& library) : objective(0.0), bound(0.0) {
	void* handle = dlopen(library.c_str(), RTLD_NOW | RTLD_LOCAL);
	if(!handle){
		cerr << "No se pudo cargar OR-Tools (" << library << "): " << dlerror() << endl;
		exit(1);
	}
	solve_function = (SolveFunction) dlsym(handle, "SolveCpModelWithParameters");
	if(!solve_function){
		cerr << "La biblioteca no contiene SolveCpModelWithParameters: " << library << endl;
		exit(1);
	}
}

CpSolver::Status CpSolver::solve(const CpModel& model, double max_time, int num_workers, vector<int64_t>& solution){
	string req = model.serialize();
	string params;
	put_double(params, PARAMS_MAX_TIME, max_time);
	put_key(params, PARAMS_NUM_WORKERS, VARINT);
	put_varint(params, num_workers);

	void* res=NULL;
	int res_len=0;
	solve_function(req.data(), req.size(), params.data(), params.size(), &res, &res_len);

	//se decodifican el estado y la solucion
	Status status=UNKNOWN;
	solution.clear();
	objective=0.0; bound=0.0;
	const unsigned char* p = (const unsigned char*) res;
	const unsigned char* end = p + res_len;
	while(p<end){
		uint64_t key;
		if(!get_varint(p, end, key)) break;
		int field = key>>3, wire = key&7;
		if(wire==VARINT){
			uint64_t v; get_varint(p, end, v);
			if(field==RESPONSE_STATUS) status = (Status) v;
			else if(field==RESPONSE_SOLUTION) solution.push_back((int64_t) v);
		}else if(wire==LEN){
			uint64_t len; get_varint(p, end, len);
			const unsigned char* q = p;
			p += len;
			if(field==RESPONSE_SOLUTION)
				while(q<p){ uint64_t v; get_varint(q, p, v); solution.push_back((int64_t) v); }
		}else if(wire==FIXED64){
			if(p+8>end) break;
			double d; memcpy(&d, p, 8);
			if(field==RESPONSE_OBJECTIVE) objective=d;
			else if(field==RESPONSE_BOUND) bound=d;
			p+=8;
		}
		else if(wire==5) p+=4;
		else break;
	}
	free(res);
	return status;
}

} /* namespace clp */
