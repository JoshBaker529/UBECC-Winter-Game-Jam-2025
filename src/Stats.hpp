// This class holds all the information regarding the player stats
#pragma once
#include <iostream>
class Stats {
public:
  Stats();
  Stats(Stats &&) = default;
  Stats(const Stats &) = default;
  Stats &operator=(Stats &&) = default;
  Stats &operator=(const Stats &) = default;
  ~Stats();
  void PrintStats();

  float max_health;
  float max_hunger;
  float max_warmth;
  float health;
  float hunger;
  float warmth;
  float defense;
  float cold_resist;
  float time;
};

static Stats PlayerStats;

Stats::Stats()
    : max_health(100), max_hunger(100), max_warmth(100), health(100),
      hunger(100), warmth(100), defense(0), cold_resist(0), time(0) {}

void Stats::PrintStats() {
  std::cout << "Health: " << health << "/" << max_health
            << "\nHunger: " << hunger << "/" << max_hunger
            << "\nWarmth: " << warmth << "/" << max_warmth
            << "\nDefense: " << defense << "\nCold Resist: " << cold_resist
            << std::endl;
}

Stats::~Stats() {}
