#include <iostream>

#include "WorldState.hh"
#include "SimpleMinds.hh"

int main() {
  WorldState world(200, 100);

  world.CreatureBorn().Connect([](const CreaturePhysical& c) {
      std::cout << "Born: " << c << std::endl;
    });
  world.CreatureAttacked().Connect([](const CreaturePhysical& a, const CreaturePhysical& b) {
      std::cout << "Attacker: " << a << "\n"
                << "Victim: " << b << std::endl;
    });
  world.CreatureDied().Connect([](const CreaturePhysical& c) {
      std::cout << "Died: " << c << std::endl;
    });


  // world.FoodSpawned().Connect([](const FoodLocation& f) {
  //     std::cout << "Food: " << f << std::endl;
  //   });
  // world.FoodEaten().Connect([](const CreaturePhysical& c, const FoodLocation& f) {
  //     std::cout << "Eaten: " << f << std::endl;
  //   });
  // world.FoodDepleted().Connect([](const FoodLocation& f) {
  //     std::cout << "Food depleted: " << f << std::endl;
  //   });


  world.AddCreature(run_left, {2,2});
  world.AddCreature(run_right, {2,2});
  world.AddCreature(run_and_eat, {10, 10});
  world.AddCreature(run_eat_kill, {150, 50});

  for(int i=0; i<1e6; i++) {
    // std::cout << world << "\n" << std::endl;
    // std::cin.get();
    // std::cout << "\033[2J" << std::flush;

    world.Update();
  }

  std::cout << "-----------------------------------------" << "\n"
            << world << std::endl;
}
