/*
 * Json.h
 *
 * Lector/escritor JSON minimo (suficiente para las instancias y soluciones de ESICUP)
 */

#ifndef PLP_JSON_H_
#define PLP_JSON_H_

#include <string>
#include <vector>
#include <utility>
#include <stdexcept>
#include <iostream>

namespace plp {

class Json {
public:
	enum Type { Null, Bool, Number, String, Array, Object };

	Json() : type(Null), b(false), num(0) { }
	Json(double x) : type(Number), b(false), num(x) { }
	Json(int x) : type(Number), b(false), num(x) { }
	Json(bool x) : type(Bool), b(x), num(0) { }
	Json(const std::string& s) : type(String), b(false), num(0), str(s) { }
	Json(const char* s) : type(String), b(false), num(0), str(s) { }

	static Json array() { Json j; j.type=Array; return j; }
	static Json object() { Json j; j.type=Object; return j; }

	static Json parse(const std::string& text);
	static Json parse_file(const std::string& file);

	Type get_type() const { return type; }
	bool is_null() const { return type==Null; }

	double as_number() const;
	const std::string& as_string() const;
	bool as_bool() const;

	//arreglos
	size_t size() const;
	const Json& operator[](size_t i) const;
	void push_back(const Json& v);

	//objetos
	bool has(const std::string& key) const;
	const Json& operator[](const std::string& key) const;
	void set(const std::string& key, const Json& v);

	void write(std::ostream& os, int indent=0) const;

private:
	Type type;
	bool b;
	double num;
	std::string str;
	std::vector<Json> arr;
	std::vector< std::pair<std::string,Json> > obj;

	friend class JsonParser;
};

} /* namespace plp */

#endif /* PLP_JSON_H_ */
