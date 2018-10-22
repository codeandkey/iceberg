#ifndef IB_GAME
#define IB_GAME

#define IB_GAME_UPDATES_PER_SEC 60
#define IB_GAME_MS_PER_UPDATE (1000 / IB_GAME_UPDATES_PER_SEC)

int ib_game_init(void);
void ib_game_free(void);

int ib_game_run(void);

#endif
