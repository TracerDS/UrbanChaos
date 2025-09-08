#pragma once

class Animal {
protected:
    Entity* target;               // chasing, barking
    std::uint16_t m_counter;       // random delays etc
    std::uint16_t m_dist;          // generically useful when pathfinding, chasing, etc
    std::uint16_t m_starty;        // initial height? remove?
    std::uint8_t m_animalType;     // species
    std::uint8_t m_substate;       // animal behaviour code
    std::uint8_t m_map_x;          // these store the ready-shifted world position
    std::uint8_t m_map_z;          // of the animal
    std::uint8_t m_dest_x, dest_z; // same, for a temporary destination point
    std::uint8_t m_home_x, home_z; // spawn point for homing
    std::uint8_t m_extra;          // animal-specific

public:
};