//== ALLEGRO
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
bool running = true, redraw = true, force = false;

const float FPS = 200, X = 1250, Y = 1250;
const float _x = 125, _y = 125;

#define BLACK  al_map_rgb(255, 255, 255)
#define WHITE  al_map_rgb(0, 0, 0)
#define RED    al_map_rgb(255, 0, 0)
#define GREEN  al_map_rgb(0, 255, 0)
#define BLUE   al_map_rgb(0, 0, 255)
#define YELLOW al_map_rgb(255, 255, 0)

int _BOARD[10][10];

////////////////////////////////////////////////////////////////////////////////

struct cell ft_cell_from_position(int x, int y) {
	struct cell p; // znajdz w odpowiedniej skali
	p.x = (int)(x/((int)_x)); // szuflada x
	p.y = (int)(y/((int)_y)); // szuflada y
	return p; }

void ft_draw_piece(int type, int x, int y) { // narysuj pionka
	if (__o(type))    al_draw_filled_circle(x, y, (_x+_y)/5, RED);
	else              al_draw_filled_circle(x, y, (_x+_y)/5, GREEN);
	if (type == +oo)  al_draw_filled_circle(x, y, (_x+_y)/5, YELLOW);
	if (__spec(type)) al_draw_filled_circle(x, y, (_x+_y)/9, BLUE); }

void ft_draw_board(int board[10][10]) {
	ALLEGRO_COLOR grid_colors[2] = {BLACK, WHITE};

	FOR(i, 0, 10-1) FOR(j, 0, 10-1) // kratownica
		al_draw_filled_rectangle(i*_x, j*_y, (i+1)*_x, (j+1)*_y,
			grid_colors[(i+j)%2]); // szachownica

	FOR(i, 0, MUST_LEN-1) // narysuj poczatki przymusowych
		al_draw_filled_rectangle(MUST[i].a.x*_x, MUST[i].a.y*_y,
				(MUST[i].a.x+1)*_x, (MUST[i].a.y+1)*_y,
			BLUE); // szachownica

	FOR(i, 0, MUST_LEN-1) // narysuj konce przymusowych
		al_draw_filled_rectangle(MUST[i].b.x*_x, MUST[i].b.y*_y,
				(MUST[i].b.x+1)*_x, (MUST[i].b.y+1)*_y,
			BLUE); // szachownica

	FOR(i, 0, 10-1) FOR(j, 0, 10-1)
		if (board[j][i] != 0) // jesli nie puste
			ft_draw_piece(board[j][i], i*_x+_x/2, j*_y+_y/2); }

////////////////////////////////////////////////////////////////////////////////

struct move MMOV;
int mouse, piece;

void ft_mouse_init(void)
	{ mouse = 0, piece = +oo; }

void ft_mouse_event(void) {
	ALLEGRO_MOUSE_STATE state;
	al_get_mouse_state(&state);
	if (state.buttons & 1) {
		MMOV.b = ft_cell_from_position(state.x, state.y);

		if (BOARD[MMOV.b.y][MMOV.b.x] == 0 && \
			mouse == 0) return; // puste pole
		if (mouse == 0) { // poczatkowy
			MMOV.a = MMOV.b; piece = BOARD[MMOV.a.y][MMOV.a.x];
			_BOARD[MMOV.a.y][MMOV.a.x] = 0;
		} else { // w trakcie
			ft_draw_board(_BOARD);
			ft_draw_piece(piece, state.x, state.y);
			al_flip_display(); }

		mouse++; redraw = false;
	} else {
		if (mouse > 0) { // wykonanie ruchu
			ee_move_piece(MMOV.a, MMOV.b);
			copy2d(_BOARD, BOARD); }
		mouse = 0; piece = +oo; }}

////////////////////////////////////////////////////////////////////////////////

void *event_board(void *argument) {
	copy2d(_BOARD, BOARD);

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;

	al_init();                     // przygotowanie programu
	al_init_primitives_addon();    // podstawowe funkcje geometryczne
	al_install_mouse();            // obsluga myszki
	al_init_native_dialog_addon(); // obsluga paska

	timer = al_create_timer(1.0 / FPS);
	display = al_create_display(X, Y);
	event_queue = al_create_event_queue();
	
	al_register_event_source(event_queue,
		al_get_display_event_source(display));
	al_register_event_source(event_queue,
		al_get_timer_event_source(timer));
	al_register_event_source(event_queue,
		al_get_default_menu_event_source());

	al_start_timer(timer);
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();

	ft_mouse_init();

///////////////////////////////////////
ALLEGRO_MENU *menu = al_create_menu();
ALLEGRO_MENU *file_menu = al_create_menu();
ALLEGRO_MENU *game_menu = al_create_menu();

al_append_menu_item(menu, "File", 0, 0, NULL, file_menu);
al_append_menu_item(file_menu, "&New...",  5, 0, NULL, NULL);
al_append_menu_item(file_menu, "&Open...", 10, 0, NULL, NULL);
al_append_menu_item(file_menu, "&Save...", 20, 0, NULL, NULL);

al_append_menu_item(menu, "Game", 0, 0, NULL, game_menu);
al_append_menu_item(game_menu, "&1. Human vs AI",    1, 0, NULL, NULL);
al_append_menu_item(game_menu, "&2. AI vs Human",    2, 0, NULL, NULL);
al_append_menu_item(game_menu, "&3. Human vs Human", 3, 0, NULL, NULL);
al_append_menu_item(game_menu, "&4. AI vs AI",       4, 0, NULL, NULL);

al_set_display_menu(display, menu);
/////////////////////////////////////

	while (running) {
		ALLEGRO_EVENT     event; // poszczegolny event
		ALLEGRO_TIMEOUT timeout; // przerwa na timerze

		al_init_timeout(&timeout, 1.0 / FPS);
		bool get_event = al_wait_for_event_until(\
			event_queue, &event, &timeout);
	
		//== MOVE
		if (EVENT_MOVE) {
			copy2d(_BOARD, BOARD);
			EVENT_MOVE = 0; }

		//== SYSTEM
		if (get_event) {
			switch (event.type) {
				case ALLEGRO_EVENT_TIMER:
					redraw = true; break;
				case ALLEGRO_EVENT_DISPLAY_CLOSE:
					running = false; break;
				case ALLEGRO_EVENT_MENU_CLICK:
					{

AA("%d %d %d\n", event.user.data1, event.user.data2, event.user.data3);
if(event.user.data1 >= 10) {
	ALLEGRO_FILECHOOSER *filechooser;
	if(event.user.data1 == 10) {
	filechooser = al_create_native_file_dialog("~/", "Choose a file.", "*.*;*.pdn;", 1);
	al_show_native_file_dialog(display, filechooser);
	const char* path = al_get_native_file_dialog_path(filechooser, 0);
	ft_load_from_file(path);
	} else {
	filechooser = al_create_native_file_dialog("~/", "Choose a file.", "*.*;*.pdn;", ALLEGRO_FILECHOOSER_SAVE);
	al_show_native_file_dialog(display, filechooser);
	const char* path = al_get_native_file_dialog_path(filechooser, 0);
	ft_save_to_file(path);
	}
} else if (event.user.data1 < 5){
	ft_load_env(event.user.data1);
} else {
	ft_new_game();
}

}
copy2d(_BOARD, BOARD);
force = true; break;
			}}

		//== RYSOWANIE
		if ((redraw && al_is_event_queue_empty(event_queue)) || force) {
			al_clear_to_color(al_map_rgb(0, 0, 0));
			ft_draw_board(_BOARD);
			al_flip_display();
			redraw = false;
			force = false; }

		//== MYSZKA
		ft_mouse_event();

		//== KONIEC
		if (STATUS != -2) {
			char s[3][100] = { "REMIS", "PRZEGRANA", "WYGRANA" };
			int i = STATUS == -1 ? 0 : (PLAYER == 1 ? 2 : 1);
			char str[80];
			AA("INTEKS %d %s", i, s[i]);
			strcpy(str, s[i]);
			strcat(str, "! Gramy ponownie?");
			int q = al_show_native_message_box(
			display,
     "Result", "Result",
	 str,
     NULL,
     ALLEGRO_MESSAGEBOX_YES_NO); // FIXME, drawwing
			if (q == 1) { ft_new_game(); copy2d(_BOARD, BOARD); force = true; }
			else running = false;
		}
	}

	// konczymy program
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	exit(EXIT_SUCCESS); }
