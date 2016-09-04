#include "FoodLocation.hh"

FoodView FoodLocation::AsView() const {
  FoodView output;
  output.quantity = quantity;
  return output;
}

std::ostream& operator<<(std::ostream& os, const FoodLocation& food) {
  os << "Food at " << food.position << " = " << food.quantity;

  return os;
}
