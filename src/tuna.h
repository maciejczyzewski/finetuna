// FINETUNA

int ft_score(int board[][10]) {
	int score = 0;
	PLAYER ^= 1;
	FOR(i, 0, 10-1) FOR(j, 0, 10-1)
		if ((i+j)%2==1&&__o(board[i][j])==PLAYER&&board[i][j]!=0) {
			int rate_x, rate_y; // kierunek
			struct cell P, Po;  // badany ruch
			Po.y = i; Po.x = j; // zapisujemy

			FOR(_, 0, 4-1) { // wszystkie mozliwe kierunki
				P = Po; rate_x = MOV[_][0], rate_y = MOV[_][1];
				FOR(i, 0, 10) { // maksymalna dlugosc planszy
					P.x += rate_x; P.y += rate_y; // kierunek
					int S = ee_valid_move(board, Po, P); // walidacja
					//AA("-----------> %d", S);
					if (S > 1) {
						AA("---");
						int _board[10][10];
						copy2d(_board, board);
						ee_do_move(_board, Po, P);
						//ee_clear(_board);
						_board[Po.y][Po.x] = -8;
						_board[P.y][P.x] = -9;
						//viz(_board);
						AA("Po(%d %d) -> P(%d %d)", Po.x, Po.y, P.x, P.y);
						AA("DANGER - - -- - - - - %d", S);
						score += S-1;
					}}}}
	PLAYER ^= 1;
	return score; }

void viz(int board[][10]) {
	FOR(i, 0, 10-1) {
		FOR(j, 0, 10-1) fprintf(stderr, "%2d ", board[i][j]);
		fprintf(stderr, "\n");
	}
}

struct move ft_tuna_move(int board[][10]) {
	AA("Krichhh! My turn, my lord!");
	AA("TUNA ===================== %d", PLAYER);
	AA("DEEP %d, LOCKED %d, MUST_LEN %d", DEEP, LOCKED, MUST_LEN);

	if (LOCKED == 1 || DEEP != 0)
		return MUST[(int)rand()%MUST_LEN];

	AA("NO IDEA!");

	struct move best;
	int fmin = +oo;
	best.a = MUST[0].a;
	best.b = MUST[0].b;

	FOR(i, 0, 10-1) FOR(j, 0, 10-1)
		if ((i+j)%2==1&&__o(board[i][j])==PLAYER&&board[i][j]!=0) {
			int rate_x, rate_y; // kierunek
			struct cell P, Po;  // badany ruch
			Po.y = i; Po.x = j; // zapisujemy

			FOR(_, 0, 4-1) { // wszystkie mozliwe kierunki
				P = Po; rate_x = MOV[_][0], rate_y = MOV[_][1];
				FOR(i, 0, 10) { // maksymalna dlugosc planszy
					P.x += rate_x; P.y += rate_y; // kierunek
					int S = ee_valid_move(board, Po, P); // walidacja
					if (S >= 1) {
						int _board[10][10];
						AA("\n");
						copy2d(_board, board);
						ee_do_move(_board, Po, P);
						ee_clear(_board);
						int cur = ft_score(_board);
						AA("====================================>>> %d", cur);
						viz(_board);
						AA("\n");
						if (cur < fmin) {
							AA("SAVE");
							fmin = cur;
							best.a = Po;
							best.b = P; }
					}}}}

	// najwiecej zbic (iles tam prob i tyle --> leci dalej)
	// --> funkcja scorujaca --> symuluje ruch przyciwnika [!]
	// --> wtedy dopiero bilans, okay
	AA("--> %d %d", best.b.x, best.b.y);
	return best;
}

struct move ft_player_move(int board[][10]) {
	//AA("PLAYER");
	struct move _;
	_.a.x = -1; _.a.y = -1;
	_.b.x = -1; _.b.y = -1;
	return _; }
