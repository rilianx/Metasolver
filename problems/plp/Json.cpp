/*
 * Json.cpp
 */

#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iomanip>
#include "Json.h"

using namespace std;

namespace plp {

class JsonParser {
public:
	JsonParser(const string& s) : s(s), i(0) { }

	Json parse() {
		Json v = value();
		ws();
		if(i!=s.size()) error("texto despues del valor");
		return v;
	}

private:
	const string& s;
	size_t i;

	void error(const string& msg) {
		throw runtime_error("JSON: " + msg + " (posicion " + to_string(i) + ")");
	}

	void ws() { while(i<s.size() && isspace((unsigned char)s[i])) i++; }

	void expect(char c) {
		ws();
		if(i>=s.size() || s[i]!=c) error(string("se esperaba '") + c + "'");
		i++;
	}

	Json value() {
		ws();
		if(i>=s.size()) error("fin inesperado");
		char c=s[i];
		if(c=='{') return object();
		if(c=='[') return array();
		if(c=='"') return Json(str());
		if(s.compare(i,4,"true")==0) { i+=4; return Json(true); }
		if(s.compare(i,5,"false")==0) { i+=5; return Json(false); }
		if(s.compare(i,4,"null")==0) { i+=4; return Json(); }
		return number();
	}

	Json number() {
		const char* begin = s.c_str()+i;
		char* end;
		double x = strtod(begin, &end);
		if(end==begin) error("valor invalido");
		i += end-begin;
		return Json(x);
	}

	string str() {
		expect('"');
		string out;
		while(i<s.size() && s[i]!='"'){
			if(s[i]=='\\'){
				i++;
				if(i>=s.size()) break;
				switch(s[i]){
				case 'n': out+='\n'; break;
				case 't': out+='\t'; break;
				case 'r': out+='\r'; break;
				case 'b': out+='\b'; break;
				case 'f': out+='\f'; break;
				case 'u': {
					//solo ASCII/latin-1 (basta para nombres de archivo)
					unsigned code = strtoul(s.substr(i+1,4).c_str(), NULL, 16);
					if(code < 0x80) out+=(char)code;
					else if(code < 0x800) { out+=(char)(0xC0|(code>>6)); out+=(char)(0x80|(code&0x3F)); }
					else { out+=(char)(0xE0|(code>>12)); out+=(char)(0x80|((code>>6)&0x3F)); out+=(char)(0x80|(code&0x3F)); }
					i+=4;
					break;
				}
				default: out+=s[i];
				}
				i++;
			}else out+=s[i++];
		}
		if(i>=s.size()) error("string sin cerrar");
		i++;
		return out;
	}

	Json array() {
		expect('[');
		Json a = Json::array();
		ws();
		if(i<s.size() && s[i]==']') { i++; return a; }
		while(true){
			a.arr.push_back(value());
			ws();
			if(i<s.size() && s[i]==',') { i++; continue; }
			expect(']');
			return a;
		}
	}

	Json object() {
		expect('{');
		Json o = Json::object();
		ws();
		if(i<s.size() && s[i]=='}') { i++; return o; }
		while(true){
			ws();
			string key = str();
			expect(':');
			o.obj.push_back(make_pair(key, value()));
			ws();
			if(i<s.size() && s[i]==',') { i++; continue; }
			expect('}');
			return o;
		}
	}
};

Json Json::parse(const string& text) {
	return JsonParser(text).parse();
}

Json Json::parse_file(const string& file) {
	ifstream in(file.c_str());
	if(!in) throw runtime_error("no se pudo abrir " + file);
	stringstream ss;
	ss << in.rdbuf();
	return parse(ss.str());
}

double Json::as_number() const {
	if(type!=Number) throw runtime_error("JSON: se esperaba un numero");
	return num;
}

const string& Json::as_string() const {
	if(type!=String) throw runtime_error("JSON: se esperaba un string");
	return str;
}

bool Json::as_bool() const {
	if(type!=Bool) throw runtime_error("JSON: se esperaba un booleano");
	return b;
}

size_t Json::size() const {
	if(type==Array) return arr.size();
	if(type==Object) return obj.size();
	return 0;
}

const Json& Json::operator[](size_t i) const {
	if(type!=Array || i>=arr.size()) throw runtime_error("JSON: indice fuera de rango");
	return arr[i];
}

void Json::push_back(const Json& v) {
	if(type!=Array) throw runtime_error("JSON: push_back en un valor que no es arreglo");
	arr.push_back(v);
}

bool Json::has(const string& key) const {
	if(type!=Object) return false;
	for(auto& kv : obj) if(kv.first==key) return true;
	return false;
}

const Json& Json::operator[](const string& key) const {
	if(type==Object)
		for(auto& kv : obj) if(kv.first==key) return kv.second;
	throw runtime_error("JSON: falta el campo \"" + key + "\"");
}

void Json::set(const string& key, const Json& v) {
	if(type!=Object) throw runtime_error("JSON: set en un valor que no es objeto");
	for(auto& kv : obj) if(kv.first==key) { kv.second=v; return; }
	obj.push_back(make_pair(key, v));
}

namespace {

void write_string(ostream& os, const string& s) {
	os << '"';
	for(char c : s){
		switch(c){
		case '"': os << "\\\""; break;
		case '\\': os << "\\\\"; break;
		case '\n': os << "\\n"; break;
		case '\t': os << "\\t"; break;
		default: os << c;
		}
	}
	os << '"';
}

}

void Json::write(ostream& os, int indent) const {
	string pad(indent+4, ' '), pad0(indent, ' ');
	switch(type){
	case Null: os << "null"; break;
	case Bool: os << (b? "true":"false"); break;
	case Number: os << setprecision(17) << num; break;
	case String: write_string(os, str); break;
	case Array: {
		//los arreglos de numeros van en una linea
		bool flat=true;
		for(const Json& v : arr) if(v.type==Array || v.type==Object) flat=false;
		os << "[";
		for(size_t k=0; k<arr.size(); k++){
			if(!flat) os << "\n" << pad;
			arr[k].write(os, indent+4);
			if(k+1<arr.size()) os << (flat? ", ":",");
		}
		if(!flat && !arr.empty()) os << "\n" << pad0;
		os << "]";
		break;
	}
	case Object:
		os << "{";
		for(size_t k=0; k<obj.size(); k++){
			os << "\n" << pad;
			write_string(os, obj[k].first);
			os << ": ";
			obj[k].second.write(os, indent+4);
			if(k+1<obj.size()) os << ",";
		}
		if(!obj.empty()) os << "\n" << pad0;
		os << "}";
		break;
	}
}

} /* namespace plp */
