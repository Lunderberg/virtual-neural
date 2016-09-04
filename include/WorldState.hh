#ifndef _WORLDSTATE_H_
#define _WORLDSTATE_H_

#include <functional>
#include <map>
#include <utility>
#include <vector>

#include "CurrentView.hh"
#include "CreaturePhysical.hh"
#include "FoodLocation.hh"
#include "GVector.hh"
#include "Signal.hh"

class WorldState {
public:
  WorldState(double height, double width);
  ~WorldState();

  friend std::ostream& operator<<(std::ostream& os, const WorldState& world);

  void AddCreature(std::function<CreatureAction(CurrentView)> mind,
                   GVector<2> pos);

  void Update();

  double GetHeight() const { return height; }
  double GetWidth() const { return width; }

  Signal<const CreaturePhysical&>& CreatureBorn() { return signal_creature_born; }
  Signal<const CreaturePhysical&, const CreaturePhysical&>&
  CreatureAttacked() { return signal_creature_attacked; }
  Signal<const CreaturePhysical&>& CreatureDied() { return signal_creature_died; }

  Signal<const FoodLocation&>& FoodSpawned() { return signal_food_spawned; }
  Signal<const CreaturePhysical&, const FoodLocation&>& FoodEaten() { return signal_food_eaten; }
  Signal<const FoodLocation&>& FoodDepleted() { return signal_food_depleted; }

private:
  /// Returns the relative position between two positions
  /**
     The world itself is a torus, and so this will return the shortest path.
   */
  GVector<2> rel_position(GVector<2> from, GVector<2> to) const;

  /// Maps the vector given into the torus of the map
  /**
     The X value of the output will be in the range [0,width).
     The Y value of the output will be in the range [0,height).
   */
  GVector<2> apply_torus(GVector<2> vec) const;

  /// Maps a displacement given into the range allowed.
  /**
     The X value of the output will be in the range [-width/2,width/2).
     The Y value of the output will be in the range [-height/2,height/2).
   */
  GVector<2> apply_torus_disp(GVector<2> vec) const;

  /// Gets all the actions to be taken in a single round
  std::vector<CreatureAction> GetAllActions();

  /// Get the world, as seen by a particular viewpoint
  CurrentView GetViewFrom(const CreaturePhysical& viewer) const;

  /// Apply actions
  /**
     The i-th element of actions should correspond to the action taken by the i-th element of creatures.
   */
  void ApplyActions(std::vector<CreatureAction> actions);

  std::vector<std::pair<int,int> > FindMatingPairs(std::vector<CreatureAction>& actions);
  void DoSoloActions(std::vector<CreatureAction>& actions);
  void DoMovement(std::vector<CreatureAction>& actions);
  void MakeNewCreatures(const std::vector<std::pair<int,int> >& mating_pairs);

  void CreatureMove(CreaturePhysical& creature, GVector<2> direction);
  void CreatureAttack(CreaturePhysical& creature, GVector<2> direction);
  void CreatureEat(CreaturePhysical& creature);
  void CreatureSleep(CreaturePhysical& creature);

  CreaturePhysical* GetAttackTarget(CreaturePhysical& creature, GVector<2> direction);
  std::vector<FoodLocation*> GetFoodTargets(CreaturePhysical& creature);

  void DecayFood();
  void SpawnFood();

  void RemoveEmptyFood();
  void RemoveDeadCreatures();

  std::map<long, std::function<CreatureAction(CurrentView)> > minds;
  std::vector<CreaturePhysical> creatures;
  std::vector<FoodLocation> food_locations;

  double height;
  double width;
  long num_updates;
  long next_creature_id;

  Signal<const CreaturePhysical&> signal_creature_born;
  Signal<const CreaturePhysical&, const CreaturePhysical&> signal_creature_attacked;
  Signal<const CreaturePhysical&> signal_creature_died;
  Signal<const FoodLocation&> signal_food_spawned;
  Signal<const CreaturePhysical&, const FoodLocation&> signal_food_eaten;
  Signal<const FoodLocation&> signal_food_depleted;
};

#endif /* _WORLDSTATE_H_ */
