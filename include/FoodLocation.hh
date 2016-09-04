#ifndef _FOODLOCATION_H_
#define _FOODLOCATION_H_

#include <ostream>

#include "GVector.hh"
#include "FoodView.hh"

struct FoodLocation {
  FoodLocation(GVector<2> position = {0,0}, double quantity = 0)
    : position(position), quantity(quantity) { }

  friend std::ostream& operator<<(std::ostream& os, const FoodLocation& food);

  /// Quantity of food that decays each tick
  double FoodDecay() const { return 0.01; }

  FoodView AsView() const;

  GVector<2> position;
  double quantity;
};

#endif /* _FOODLOCATION_H_ */
