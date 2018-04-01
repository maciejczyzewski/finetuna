// FIXME: dodac prefiks (RR) i dodatkowe funkcje

// pojedyncza komorka
struct cell { int x, y; };

// przypus bicia z a do b
struct move {
	struct cell a, b;
};

int MOV[4][2] = { // bicia po skosie
	{-1, -1}, {-1,  1},  // ll, lr
	{ 1, -1}, { 1,  1}}; // rl, rr

int PLAYER = 0; // kolejnosc graczy

// funkcje pomocnicze
bool    __o(int val) { return abs(val) != val; }       // czyj?
bool __norm(int val) { return 1 == abs(val) ? 1 : 0; } // normalny?
bool __spec(int val) { return 2 == abs(val) ? 1 : 0; } // damka?
int  __rate(int val) { return val < 0 ? -1 : 1; }      // kierunek?

int BOARD[10][10], DEFAULT_BOARD[10][10] = {
	{ 0, -1,  0, -1,  0, -1,  0, -1,  0, -1},
	{-1,  0, -1,  0, -1,  0, -1,  0, -1,  0},
	{ 0, -1,  0, -1,  0, -1,  0, -1,  0, -1},
	{-1,  0, -1,  0, -1,  0, -1,  0, -1,  0},
	{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
	{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
	{ 0,  1,  0,  1,  0,  1,  0,  1,  0,  1},
	{ 1,  0,  1,  0,  1,  0,  1,  0,  1,  0},
	{ 0,  1,  0,  1,  0,  1,  0,  1,  0,  1},
	{ 1,  0,  1,  0,  1,  0,  1,  0,  1,  0},
};

// czyszczenie planszy z niepotrzebnych informacji
void ee_clear(int board[][10]) {
	FOR(_i, 0, 10-1) FOR(_j, 0, 10-1)
		if (board[_i][_j] == +oo) board[_i][_j] = 0;
}

// sprawdzenie czy ruch jest prawidlowy
int ee_valid_move(int board[][10], \
	struct cell a, struct cell b) {
	// dane pomocnicze
	int type = board[a.y][a.x];                             // typ
	int dist = (abs(a.x-b.x) + abs(a.y-b.y))/2;             // odleglosc
	int rate_x = __rate(b.x-a.x), rate_y = __rate(b.y-a.y); // kierunek

	// [*] poza plansza
	if (b.x < 0 || b.y < 0 || \
		b.x >= 10 || b.y >= 10) return false;

	// [0] czyj ruch (odpowiedni grasz)
	if (__o(type) != PLAYER) return false;
	// [1] czarne pola (nie mozna tam nic postawic)
	if ((a.x+a.y)%2 == 0 || (b.x+b.y)%2 == 0) return false;
	// [2] cos juz jest na polu (nieprawidlowy ruch)
	if (board[b.y][b.x] != 0) return false;
	// [3] sprawdz czy dobra przekatna (dynamika gry)
	if (abs(a.x-b.x) != abs(a.y-b.y)) return false;
	// [4] daleki ruch (normalny nie ma zasiegu)
	if (__norm(type) && dist > 2) return false;

	// [5] ilosc bic i czy jest miejsce
	struct cell P = a; int bic = 0, l = 0;
	FOR(i, 1, dist) { // z punktu a do b
		P.x += rate_x; P.y += rate_y; // przesuwamy
		int t = board[P.y][P.x]; // typ
		if (t == +oo) return false; // blokada pola
		if (__o(t) == __o(type) && t != 0) // swoj pion na trasie
		{ return false; } // no to nie mozna
		if (__o(t) != __o(type) && t != 0) {
			if (l==1) return false; // puste pole
			l=1,bic++; // prawidlowe bicie
		} else l=0; }

	// [6] normalny pionek ma ograniczenia (bicie/ruch)
	if (__norm(type) && bic != 1 && dist == 2) return false;
	// [7] normalny do tylu (nie mozna sie cofac)
	if (__norm(type) && bic == 0 && !__o(type) && \
		(rate_y > 0)) return false;
	if (__norm(type) && bic == 0 && __o(type)  && \
		(rate_y < 0)) return false;

	return bic+1; } // <ilosc bic>

// wykonaj ruch z a do b
void ee_do_move(int board[][10], struct cell a, struct cell b) {
	// dane pomocnicze
	int type = board[a.y][a.x];                             // typ
	int dist = (abs(a.x-b.x) + abs(a.y-b.y))/2;             // odleglosc
	int rate_x = __rate(b.x-a.x), rate_y = __rate(b.y-a.y); // kierunek

	struct cell P = a; // wykonanie ruchu
	FOR(i, 1, dist) { // wykonujemy bicia
		P.x += rate_x; P.y += rate_y; // przesuwamy
		int t = board[P.y][P.x]; // typ
		if (__o(t) != __o(type) && \
			t != 0) board[P.y][P.x] = +oo; }
	// zaznaczamy jako zablokowany

	// zapis przesuniecia
	board[b.y][b.x] = type; // docelowe
	board[a.y][a.x] = 0;    // zrodlo
}

int cache[1024]; int cache_len = 0;

int ee_check(int board[][10]) {
	bool ok = false; int h = 0;
	FOR(i, 0, 10-1) FOR(j, 0, 10-1)
		if ((i+j)%2==1&&board[i][j]!=0) {
			h += (board[i][j] << 2) ^ (i * j) ^ j ^ i ^ (board[i][j] << 5);

			int rate_x, rate_y; // kierunek
			struct cell P, Po;  // badany ruch
			Po.y = i; Po.x = j; // zapisujemy

			FOR(_, 0, 4-1) { // wszystkie mozliwe kierunki
				P = Po; rate_x = MOV[_][0], rate_y = MOV[_][1];
				FOR(i, 0, 10) { // maksymalna dlugosc planszy
					P.x += rate_x; P.y += rate_y; // kierunek
					int S = ee_valid_move(board, Po, P); // walidacja
					if (S >= 1) ok = true;
				}
			}
		}
	int rep = 0;
	FOR(i, 0, cache_len-1) if (h == cache[i]) rep++;
	if (rep > 15) return -1;
	cache[cache_len++] = h;
	if (ok) return 1;
	return 0; }

// dla ruchow przymusowych
struct cell LOCK; int LOCKED = 0;                     // wybrany ruch
struct move MUST[50]; int MUST_LEN = 0; int DEEP = 0; // mozliwosci ruchu

// symulacja najdluzszego bicia (zasada gry)
void ee_simulate(int board[][10], int x, int y, int deep) {
	int rate_x, rate_y; // kierunek
	struct cell P, Po;  // badany ruch
	Po.y = x; Po.x = y; // zapisujemy

	// mamy nowy najdluzszy
	if (deep > DEEP) DEEP = deep;

	FOR(_, 0, 4-1) { // wszystkie mozliwe kierunki
		P = Po; rate_x = MOV[_][0], rate_y = MOV[_][1];
		FOR(i, 0, 10) { // maksymalna dlugosc planszy
			P.x += rate_x; P.y += rate_y; // kierunek
			int S = ee_valid_move(board, Po, P); // walidacja
			if (S == 2) { // FIXME: tylko jedno bicie
				int board_next[10][10]; // chwilowy board
				FOR(_i, 0, 10-1) FOR(_j, 0, 10-1) // kopiujemy
					board_next[_i][_j] = board[_i][_j];

				int cur_deep = DEEP; DEEP = 0; // stary wynik

				ee_do_move(board_next, Po, P);             // wykonaj
				ee_simulate(board_next, P.y, P.x, deep+1); // badaj dalej

				if (cur_deep < DEEP && deep == 0) { // nowy lepszy
					MUST_LEN = 0; // resetujemy
					MUST[0].a.x = Po.x; MUST[0].a.y = Po.y;
					MUST[0].b.x =  P.x; MUST[0].b.y =  P.y;
					MUST_LEN++; }
				if (cur_deep == DEEP && deep == 0) { // taki sam
					MUST[MUST_LEN].a.x = Po.x; MUST[MUST_LEN].a.y = Po.y;
					MUST[MUST_LEN].b.x =  P.x; MUST[MUST_LEN].b.y =  P.y;
					MUST_LEN++; } // dodajemy
				if (cur_deep > DEEP) DEEP = cur_deep; // bez zmian
			}}}
}

// ruch gracza (ogolna funkcja z a do b)
bool ee_move_piece(struct cell a, struct cell b) {
	//AA("\n[%d %d] -> [%d %d]", a.x, a.y, b.x, b.y);

	// sprawdza czy ten ruch jest dozwolony
	int valid = ee_valid_move(BOARD, a, b);
	if (!valid) return false; // nieprawidlowy

	// [1] przymusowe bicie
	MUST_LEN = 0, DEEP = 0;

	if (LOCKED == 0) { // szukaj przymusowych bic
		FOR(i, 0, 10-1) FOR(j, 0, 10-1)
			if (__o(BOARD[j][i]) == PLAYER && BOARD[j][i] != 0)
				ee_simulate(BOARD, j, i, 0);
	} else { // jestesmy w trakcie przymusowego bicia
		AA("LOCKED == 1 ==");
		ee_simulate(BOARD, LOCK.y, LOCK.x, 0);
	}

	AA("@1 DEEP = %d LOCK = %d", DEEP, LOCKED);

	if (DEEP != 0) { // wybor przymusowego ruchu
		AA("MUST_LEN == %d", MUST_LEN);
		bool okay = false; // wykonuje maksymalna ilosc bic
		FOR(i, 0, MUST_LEN-1) { // szukamy prawidlowego
			AA("----> [%d %d], [%d %d]", MUST[i].a.x, MUST[i].a.y,
					MUST[i].b.x, MUST[i].b.y);
			if (MUST[i].a.x == a.x && MUST[i].a.y == a.y && \
			    MUST[i].b.x == b.x && MUST[i].b.y == b.y)
			{ LOCK = b; LOCKED = 1; okay = true; break; }}
		if (!okay) return false; } // zapisujemy wybrana opcje

	ee_do_move(BOARD, a, b); // wykonaj

	if (LOCKED == 1) { // w trakcie
		MUST_LEN = 0, DEEP = 0; // sprawdz czy koniec
		AA("LOCKED == 2 == \t\t(%d %d)", LOCK.x, LOCK.y);
		ee_simulate(BOARD, LOCK.y, LOCK.x, 0); }

	AA("@2 DEEP = %d LOCK = %d", DEEP, LOCKED);

	if (DEEP == 0) { // koniec ruchu
		LOCKED = 0; PLAYER ^= 1; // zmiana gracza
		ee_clear(BOARD);
		AA("ZMIANA GRACZA"); // a moze to damka??? (sprawdz)
		if (__o(BOARD[b.y][b.x]) == 0 && b.y ==   0) BOARD[b.y][b.x] =  2;
		if (__o(BOARD[b.y][b.x]) == 1 && b.y ==10-1) BOARD[b.y][b.x] = -2;
	}

	return true; }
