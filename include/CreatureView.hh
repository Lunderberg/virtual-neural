#ifndef _CREATUREVIEW_H_
#define _CREATUREVIEW_H_

#include "GVector.hh"

struct CreatureView {
  GVector<2> rel_position;
  double max_hp, hp;
  double max_food, food;
  double size;
};

#endif /* _CREATUREVIEW_H_ */
