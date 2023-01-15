#pragma once

#include "game/enemy/KartEnemy.hh"

namespace Enemy {

class EnemyManager {
public:
    void calc();

    static EnemyManager *Instance();

private:
    REPLACE static bool isOfflineBattle(void);
    bool REPLACED(hasCpus)() const;
    REPLACE bool hasCpus() const;

    u8 _00[0x9c - 0x00];

    static EnemyManager *s_instance;
};
static_assert(sizeof(EnemyManager) == 0x9c);

} // namespace Enemy
