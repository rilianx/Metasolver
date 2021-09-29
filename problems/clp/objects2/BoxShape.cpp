/*
 * Box.cpp
 *
 *  Created on: 2 jun. 2017
 *      Author: iaraya
 */

#include "BoxShape.h"
#include <math.h>

namespace clp {

BoxShape::BoxShape(int id, double l, double w, double h, bool rot1, bool rot2, bool rot3, double weight, double profit) :
	Volume(l,w,h), id(id), weight(weight), profit(profit) {
    is_valid_orientation[LWH] = true;
    is_valid_orientation[WLH]=rot3;
    //is_valid_orientation[LWH]=rot3;

	  is_valid_orientation[WHL]=rot1;
	  is_valid_orientation[HWL]=rot1;

	  is_valid_orientation[HLW]=rot2;
	  is_valid_orientation[LHW]=rot2;

}

//get i-th oriented dimensions of the box
long BoxShape::get(int i, Orientation o) const{
  switch (i){
  	  case 0: return getL(o);
  	  case 1: return getW(o);
  	  case 2: return getH(o);
  }
  return 0;
}

//get oriented dimensions of the box
double BoxShape::getL_d(Orientation o) const{
  if(o==WLH || o==WHL) return Volume::getW_d();
  else if(o==LWH || o==LHW || o==OR) return Volume::getL_d();
  else if(o==HLW || o==HWL) return Volume::getH_d();
  return 0;
}

double BoxShape::getW_d(Orientation o) const{
  if(o==LWH || o==HWL || o==OR) return Volume::getW_d();
  else if(o==WLH || o==HLW) return Volume::getL_d();
  else if(o==WHL || o==LHW) return Volume::getH_d();
  return 0;
}

double BoxShape::getH_d(Orientation o) const{
  if(o==HLW || o==LHW) return Volume::getW_d();
  else if(o==HWL || o==WHL) return Volume::getL_d();
  else if(o==LWH || o==WLH || o==OR) return Volume::getH_d();
  return 0;
}

//get oriented dimensions of the box
long BoxShape::getL(Orientation o) const{
  if(o==WLH || o==WHL) return Volume::getW();
  else if(o==LWH || o==LHW || o==OR) return Volume::getL();
  else if(o==HLW || o==HWL) return Volume::getH();
  return 0;
}

 long BoxShape::getW(Orientation o) const{
  if(o==LWH || o==HWL || o==OR) return Volume::getW();
  else if(o==WLH || o==HLW) return Volume::getL();
  else if(o==WHL || o==LHW) return Volume::getH();
  return 0;
}

 long BoxShape::getH(Orientation o) const{
  if(o==HLW || o==LHW) return Volume::getW();
  else if(o==HWL || o==WHL) return Volume::getL();
  else if(o==LWH || o==WLH || o==OR) return Volume::getH();
  return 0;
}


} /* namespace clp */
