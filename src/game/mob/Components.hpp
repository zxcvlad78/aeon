#include "../../../include/Components.hpp"

struct Mob {
    
};

struct MobAI {
    //unsigned int // мб сделать какой то тип уровень агрессии или че то типо того ярость (rage))) ) и тд хз крч
    
};


struct MobAttackMelee {

};

struct MobAttackRanged {
    Projectile projectile;
    Velocity initial_velocity;
    std::string projectile_atlas_path;
    std::string projectile_spritesheet_path;
};

