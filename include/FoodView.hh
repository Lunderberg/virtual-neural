#ifndef _FOODVIEW_H_
#define _FOODVIEW_H_

#include "GVector.hh"

struct FoodView {
  GVector<2> rel_position;
  double quantity;
};

#endif /* _FOODVIEW_H_ */
