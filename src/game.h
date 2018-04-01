struct move MOVE; int EVENT_MOVE = 0, STATUS = -2;
struct move (*ft_engines[2]) (int board[][10]) \
	= {ft_player_move, ft_tuna_move};

// FIXME troche poprawic!

void ft_load_from_file(const char* path) {
	AA("L--> %s", path);
	FILE *fp; fp = fopen(path, "r");
	fscanf(fp, "%d", &PLAYER);
	FOR(i, 0, 10-1) FOR(j, 0, 10-1)
		fscanf(fp, "%d", &BOARD[i][j]);
	fclose(fp); }

void ft_save_to_file(const char* path) {
	AA("S--> %s", path);
	FILE *fp; fp = fopen(path, "w");
	fprintf(fp, "%2d\n", PLAYER);
	FOR(i, 0, 10-1) {
		FOR(j, 0, 10-1) fprintf(fp, "%2d ", BOARD[i][j]);
		fprintf(fp, "\n"); }
	fclose(fp); }

void ft_load_env(int option) {
	switch (option) {
		case 1: ft_engines[0] = ft_player_move;
			    ft_engines[1] = ft_tuna_move; break;
		case 2: ft_engines[0] = ft_tuna_move;
			    ft_engines[1] = ft_player_move; break;
		case 3: ft_engines[0] = ft_player_move;
			    ft_engines[1] = ft_player_move; break;
		case 4: ft_engines[0] = ft_tuna_move;
			    ft_engines[1] = ft_tuna_move; break; }}


void ft_new_game(void) { // FIXME: jako globalna funkcja
	struct move tmp; tmp.a.x = -1; tmp.a.y = -1;
	tmp.b.x = -1;  tmp.b.y = -1; 
	FOR(i, 0, 10) MUST[i] = tmp;
	
	MUST_LEN = 0; DEEP = 0; LOCKED = 0;
	STATUS = -2; PLAYER = 0; EVENT_MOVE = 0;
	copy2d(BOARD, DEFAULT_BOARD); cache_len = 0; }

void *event_game(void *argument) {
	while (1) {
		// przymusowe bicie
		MUST_LEN = 0; DEEP = 0;
		if (LOCKED == 0) {
			FOR(i, 0, 10-1) FOR(j, 0, 10-1)
				if (__o(BOARD[j][i]) == PLAYER && BOARD[j][i] != 0)
					ee_simulate(BOARD, j, i, 0);
		} else { ee_simulate(BOARD, LOCK.y, LOCK.x, 0); }

		// FIXME: sprawdz czy gracz moze wykonac jakis ruch [OKAY]
		// FIXME: jesli nie istnieja zadne pionki przeciwnika [OKAY]
		// FIXME: 15 razy ta sama pozycja

		MOVE = ft_engines[PLAYER](BOARD);
		bool ok = ee_move_piece(MOVE.a, MOVE.b);
		if (ok) {
			EVENT_MOVE = 1;
			AA("EVENT_MOVE;");

			int status = ee_check(BOARD);
			if (status != 1) { // sprawdza rezultat!
				AA("END, %d (status: %d)", PLAYER, status);
				STATUS = status;
				} // FIXME: ALERT!!!

			/*MUST_LEN = 0; DEEP = 0;*/}

		usleep(100000);
		//usleep(1000);
	}}
