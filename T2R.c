#include <stdio.h>
#include <stdlib.h>
#include "TicketToRideAPI.h"
#include "move.h"
#include "game.h"

int main() {
	t_game game;
	t_board board;
	t_return_code retCode;
	int replay = 0;			/* boolean, tells if the player replays */
	t_move move;			/* current move */
	t_color lastMove = NONE;	/* last Card taken we need to replay else NONE */
	
	/* connect to server */
	connectToServer("li1417-56.members.linode.com", 5678, "TEST");

	/* intialize a game */
	initGame(&game, &board);
	
	/* game loop */
	do{
		/* display map */
		if (!replay)
			printMap();

		/* the opponent plays ??*/
		if (game.player == 1){

			retCode = getMove(&move, &replay);
			// update game
			updateGame(&game, &move, &board);
		}
		else {
			/* we play */
			askMove(&move);
			replay = needReplay(&move, lastMove);
			retCode = playOurMove(&move, &lastMove);
			// update game
			updateGame(&game, &move, &board);
		}
		/* change player */
		if (retCode == NORMAL_MOVE && !replay)
			game.player = !game.player;

	} while (retCode == NORMAL_MOVE);

	/* check who wins */
	if ((retCode == WINNING_MOVE && game.player == 0) || (retCode == LOOSING_MOVE && game.player == 1))
		printf("We are the best !\n");
	else
		printf("The opponent was lucky\n");


	closeConnection();
	return 0;

}
