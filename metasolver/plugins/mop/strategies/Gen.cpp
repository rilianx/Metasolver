
#include "Gen.h"

Gen::Gen(){
	setType(type) = 0;
	setN(n) = 0;
	setO(o) = 0;
}

Gen::Gen(int type, int n, int o){
	setType(type) = type;
	setN(n) = n;
	setO(o) = o;
}

void Gen::setType(int type){
	this->type = type;
}

void Gen::setN(int n){
	this->n = n;
}

void Gen::setO(int o){
	this->o = o;
}

int Gen::getType(){
	return type;
}

int Gen::getN(){
	return n;
}

int Gen::getO(){
	return o;
}
