// ===[FINETUNA]===
#include "utils.h" // pomocnicze
#include "rules.h" // zasady gry
#include "tuna.h"  // AI: silniki
#include "game.h"  // rozgrywka
#include "board.h" // wizualizacja

// ==[FIXME]==
// 2. test system
// 5. [FEN] czytanie zapisu
// 6. [AI] grajace na protocole

// https://10x10.dse.nl/pdn/index.html

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	// FIXME: rozne tryby (option parser)
	ft_new_game();
	pthread_t threads[2]; int _null, _t1, _t2;
	_t1 = pthread_create(&threads[0], NULL, event_game,  (void *)_null);
	_t2 = pthread_create(&threads[1], NULL, event_board, (void *)_null);
	while(1) {} // FIXME: lepszy env.
	return 0;
}
