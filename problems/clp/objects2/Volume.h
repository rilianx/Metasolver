/*
 * Volume.h
 *
 *  Created on: 02-06-2017
 *      Author: ignacio
 */

#include <iostream>
#include "Vector3.h"

#ifndef OBJECTS_VOLUME_H_
#define OBJECTS_VOLUME_H_

namespace clp {

class Volume : public Vector3 {
public:
	Volume(double l, double w, double h) : Vector3(l,w,h), volume(l*w*h) {}

	long getL() const {return getX();}
	long getW() const {return getY();}
	long getH() const {return getZ();}

	double getL_d() const {return getX_d();}
	double getW_d() const {return getY_d();}
	double getH_d() const {return getZ_d();}

	void setL(double xx) {volume/=getX(); volume*=xx; setX(xx);}
	void setW(double yy) {volume/=getY(); volume*=yy; setY(yy);}
	void setH(double zz) {volume/=getZ(); volume*=zz; setZ(zz);}

	long get_i(int i) const{
		switch(i){
			case 0: return getX();
			case 1: return getY();
			case 2: return getZ();
		}
		return -1;
	}

	void set_i(long xx, int i){
		switch(i){
			case 0: setL(xx); break;
			case 1: setW(xx); break;
			case 2: setH(xx); break;
		}
	}

	double getVolume() const {return volume;}

	//virtual ~Volume();

protected:
	double volume;
};

} /* namespace clp */

#endif /* OBJECTS_VOLUME_H_ */
