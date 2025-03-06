// grenade.h
//
// grenade code

// initialize system
void InitGrenades();

// create a grenade with these parameters
bool CreateGrenade(Thing* owner, std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t dx, std::int32_t dy, std::int32_t dz, std::int32_t timer);

// create a grenade from the person data
bool CreateGrenadeFromPerson(Thing* p_person, std::int32_t timer);

// draw the grenades
void DrawGrenades();

// process the grenades
void ProcessGrenades();

// create a grenade explosion
void CreateGrenadeExplosion(std::int32_t x, std::int32_t y, std::int32_t z, Thing* owner);
