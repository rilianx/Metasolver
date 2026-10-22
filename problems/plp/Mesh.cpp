/*
 * Mesh.cpp
 */

#include <fstream>
#include <sstream>
#include <map>
#include <tuple>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <algorithm>
#include "Mesh.h"

using namespace std;

namespace plp {

namespace {

string lower_ext(const string& file) {
	size_t p = file.find_last_of('.');
	if(p==string::npos) return "";
	string e = file.substr(p+1);
	for(char& c : e) c = tolower(c);
	return e;
}

typedef pair<int,int> Edge;

inline Edge edge(int a, int b) { return (a<b)? Edge(a,b) : Edge(b,a); }

}

Mesh Mesh::load(const string& file) {
	string e = lower_ext(file);
	if(e=="obj") return load_obj(file);
	if(e=="stl") return load_stl(file);
	throw runtime_error("formato de malla no soportado: " + file);
}

Mesh Mesh::load_obj(const string& file) {
	ifstream in(file.c_str());
	if(!in) throw runtime_error("no se pudo abrir " + file);

	Mesh M;
	M.groups.push_back(0);
	string line;
	while(getline(in, line)){
		line = line.substr(0, line.find('#'));
		istringstream ss(line);
		string tag;
		if(!(ss >> tag)) continue;
		if(tag=="v"){
			Vec3 v;
			ss >> v.x >> v.y >> v.z;
			M.vertices.push_back(v);
		}else if(tag=="f"){
			vector<int> f;
			string tok;
			while(ss >> tok){
				int k = atoi(tok.substr(0, tok.find('/')).c_str());
				k = (k<0)? (int)M.vertices.size()+k : k-1;
				if(k<0 || k>=(int)M.vertices.size()) throw runtime_error("indice de vertice invalido en " + file);
				//vertices repetidos consecutivos (caras degeneradas)
				if(!f.empty() && f.back()==k) continue;
				f.push_back(k);
			}
			if(f.size()>1 && f.front()==f.back()) f.pop_back();
			if(f.size()>=3) M.faces.push_back(f);
		}else if(tag=="o" || tag=="g"){
			if((int)M.faces.size() > M.groups.back()) M.groups.push_back(M.faces.size());
		}
	}
	if(M.groups.size()>1 && M.groups.back()==(int)M.faces.size()) M.groups.pop_back();
	return M;
}

Mesh Mesh::load_stl(const string& file) {
	ifstream in(file.c_str(), ios::binary);
	if(!in) throw runtime_error("no se pudo abrir " + file);
	string data((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());

	vector<Vec3> tri;
	uint32_t n=0;
	if(data.size()>=84) memcpy(&n, data.data()+80, 4);
	if(data.size()>=84 && data.size()==84+50*(size_t)n){
		//binario
		for(uint32_t t=0; t<n; t++){
			const char* p = data.data()+84+50*t+12;
			for(int k=0;k<3;k++){
				float c[3];
				memcpy(c, p+12*k, 12);
				tri.push_back(Vec3(c[0],c[1],c[2]));
			}
		}
	}else{
		istringstream ss(data);
		string tok;
		while(ss >> tok){
			if(tok=="vertex"){
				Vec3 v;
				ss >> v.x >> v.y >> v.z;
				tri.push_back(v);
			}
		}
		if(tri.size()%3!=0) throw runtime_error("STL invalido: " + file);
	}

	//fusion de vertices identicos
	Mesh M;
	M.groups.push_back(0);
	map<tuple<double,double,double>, int> id;
	for(size_t t=0; t<tri.size(); t+=3){
		vector<int> f;
		for(int k=0;k<3;k++){
			const Vec3& v=tri[t+k];
			auto key = make_tuple(v.x,v.y,v.z);
			auto it = id.find(key);
			if(it==id.end()) { it = id.insert(make_pair(key, (int)M.vertices.size())).first; M.vertices.push_back(v); }
			if(find(f.begin(), f.end(), it->second)==f.end()) f.push_back(it->second);
		}
		if(f.size()==3) M.faces.push_back(f);
	}
	return M;
}

bool Mesh::is_closed(int begin, int end) const {
	map<Edge,int> count;
	for(int j=begin; j<end; j++){
		const vector<int>& f = faces[j];
		for(size_t k=0; k<f.size(); k++) count[edge(f[k], f[(k+1)%f.size()])]++;
	}
	for(auto& e : count) if(e.second!=2) return false;
	return !count.empty();
}

vector< pair<int,int> > Mesh::closed_shells(int begin, int end) const {
	vector< pair<int,int> > shells;
	map<Edge,int> count;
	int open=0, start=begin;
	for(int j=begin; j<end; j++){
		const vector<int>& f = faces[j];
		for(size_t k=0; k<f.size(); k++){
			int& c = count[edge(f[k], f[(k+1)%f.size()])];
			c++;
			if(c==1) open++;
			else if(c==2) open--;
			else return vector< pair<int,int> >();
		}
		if(open==0){
			shells.push_back(make_pair(start, j+1));
			start=j+1;
			count.clear();
		}
	}
	if(start!=end) return vector< pair<int,int> >();
	return shells;
}

double Mesh::volume(int begin, int end) const {
	double v=0;
	for(int j=begin; j<end; j++){
		const vector<int>& f = faces[j];
		for(size_t k=1; k+1<f.size(); k++)
			v += dot(vertices[f[0]], cross(vertices[f[k]], vertices[f[k+1]])) / 6.0;
	}
	return v;
}

} /* namespace plp */
