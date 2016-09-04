#ifndef _CURRENTVIEW_H_
#define _CURRENTVIEW_H_

#include <vector>

#include "CreatureSelfView.hh"
#include "CreatureView.hh"
#include "FoodView.hh"
#include "GVector.hh"

struct CurrentView {
  CreatureSelfView self;
  std::vector<CreatureView> creature_views;
  std::vector<FoodView> food_views;

  GVector<2> ClosestCreature() const;
  GVector<2> ClosestFood() const;
};

#endif /* _CURRENTVIEW_H_ */
