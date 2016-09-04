#include "CurrentView.hh"

#include <cmath>

GVector<2> CurrentView::ClosestCreature() const {
  GVector<2> closest(NAN, NAN);
  for(auto& view : creature_views) {
    if(!(view.rel_position.Mag() > closest.Mag())) {
      closest = view.rel_position;
    }
  }
  return closest;
}

GVector<2> CurrentView::ClosestFood() const {
  GVector<2> closest(NAN, NAN);
  for(auto& view : food_views) {
    if(!(view.rel_position.Mag() > closest.Mag())) {
      closest = view.rel_position;
    }
  }
  return closest;
}
