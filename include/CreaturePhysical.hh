#ifndef _CREATUREPHYSICAL_H_
#define _CREATUREPHYSICAL_H_

#include <ostream>

#include "CreatureAction.hh"
#include "CreatureView.hh"
#include "CreatureSelfView.hh"
#include "GVector.hh"

struct CreaturePhysical {
  CreaturePhysical()
    : id(0), position(0,0),
      max_hp(0), hp(0),
      max_food(0), food(0),
      size(0) { }

  friend std::ostream& operator<<(std::ostream& os, const CreaturePhysical& creature);

  /// Number of tiles that can be seen
  double ViewDistance() const { return 20; }

  /// Maximum distance moved in a single tick
  double MaxMovement() const { return 1; }
  /// HP cost of movement
  double MovementHPCost() const { return 0.01; }

  /// Range of mating, relative to creature's size
  double MatingRange() const { return 1.5; }
  /// Required accuracy of mating, relative to creature's size
  double MatingSize() const { return 0.8; }
  /// HP cost of mating
  double MatingHPCost() const { return 50; }

  /// Range of attack, relative to creature's size
  double AttackRange() const { return 1.5; }
  /// Required accuracy of an attack, relative to creature's size
  double AttackSize() const { return 0.8; }
  /// Damage dealt on an attack
  double AttackDamage() const { return 1; }
  /// HP cost of attacking
  double AttackHPCost() const { return 0.05; }

  /// Food eaten per bite
  double BiteSize() const { return 1; }
  /// HP cost of eating
  double EatHPCost() const { return 0.01; }

  /// HP healed while sleeping
  double RecoveryRate() const { return 0.5; }
  /// Food required per HP recovered
  double RecoveryEfficiency() const { return 1; }
  /// HP cost of sleeping
  double SleepHPCost() const { return 0.001; }

  /// Whether the creature is going to die at the end of the tick
  bool AboutToDie() const { return hp <= 0; }


  CreatureView AsView() const;
  CreatureSelfView AsSelfView() const;

  long id;

  GVector<2> position;

  double max_hp, hp;
  double max_food, food;
  double size;
};

#endif /* _CREATUREPHYSICAL_H_ */
