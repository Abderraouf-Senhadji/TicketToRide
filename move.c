//
// Created by Thib on 14/12/2020.
//

#include "move.h"
#include "game.h"
#include "dijkstra.h"


/* ask for a move */
void askMove(t_move* move){
	/* ask for the type */
	printf("Choose a move\n");
	printf("1. claim a route\n");
	printf("2. draw a blind card\n");
	printf("3. draw a card\n");
	printf("4. draw objectives\n");
	printf("5. choose objectives\n");
	scanf("%d", &move->type);

	/* ask for details */
	if (move->type == CLAIM_ROUTE) {
		printf("Give city1, city2, color and nb of locomotives: ");
		scanf("%d %d %d %d", &move->claimRoute.city1, &move->claimRoute.city2, &move->claimRoute.color,
			  &move->claimRoute.nbLocomotives);
	}
	else if (move->type == DRAW_CARD) {
		printf("Give the color: ");
		scanf("%d", &move->drawCard.card);
	}
	else if (move->type == CHOOSE_OBJECTIVES){
		printf("For each objective, give 0 or 1:");
		scanf("%d %d %d", &move->chooseObjectives.chosen[0], &move->chooseObjectives.chosen[1], &move->chooseObjectives.chosen[2]);
	}

}

void chooseMove (t_move* move, t_game* game, t_board* board, int replay){
	/* The first turn move must be a drawObjectives */
	if (game->firstTurn == 1){
		move->type = DRAW_OBJECTIVES;
	}
	else {
		if (replay){
			if (move->type == DRAW_OBJECTIVES){
				move->type == CHOOSE_OBJECTIVES;
				for (i=0; i<2; i++){
					move->chooseObjectives.chosen[i] = 0;
				}
				move->chooseObjectives.nbObjectives = 0;
				int i;
				int wagonsNeeded = 0;
				/*int index_max = 0
				int max_points = game->players[game->player].latentObjectives[index_max].score; 
				for (i=1; i<2; i++){
					if (game->players[game->player].latentObjectives[i].score > max_points){
						max_points = game->players[game->player].latentObjectives[i].score;
						index_max = i;
					}
				}*/
				t_tracks* objTracks[3];
				for (i=0; i<2; i++){
					objTracks[i] = board->tracks + (game->players[game->player].latentObjectives[i].city1 * board->nbCities + game->players[game->player].latentObjectives[i].city2) * sizeof(t_tracks*);
					if (objTracks[i]->length <= (game->players[game->player].nbWagons - wagonsNeeded)){
						move->chooseObjectives.chosen[index_max] = 1;
						move->chooseObjectives.nbObjectives ++;
						wagonsNeeded += objTracks[i]->length;
					}
				}
			}
		}
	}
}


/* plays the move given as a parameter (send to the server)
 * returns the return code */
t_return_code playOurMove(t_move* move, t_color* lastCard){
	t_return_code ret;

	switch (move->type) {
		case CLAIM_ROUTE:
			ret = claimRoute(move->claimRoute.city1, move->claimRoute.city2, move->claimRoute.color, move->claimRoute.nbLocomotives);
			*lastCard = NONE;
			break;
		case DRAW_CARD:
			ret = drawCard(move->drawCard.card, move->drawCard.faceUp);
			*lastCard = (*lastCard==NONE && move->drawCard.card!= MULTICOLOR) ? move->drawCard.card : NONE;
			break;
		case DRAW_BLIND_CARD:
			ret = drawBlindCard(&move->drawBlindCard.card);
			*lastCard = (*lastCard==NONE) ? move->drawBlindCard.card : NONE;
			break;
		case DRAW_OBJECTIVES:
			ret = drawObjectives(move->drawObjectives.objectives);
			for(int i=0; i<3; i++){
				printf("%d. %d (", i, move->drawObjectives.objectives[i].city1);
				printCity(move->drawObjectives.objectives[i].city1);
				printf(") -> (");
				printCity(move->drawObjectives.objectives[i].city2);
				printf(") %d (%d pts)\n", move->drawObjectives.objectives[i].city2, move->drawObjectives.objectives[i].score);
			}
			*lastCard = NONE;
			break;
		case CHOOSE_OBJECTIVES:
			ret = chooseObjectives(move->chooseObjectives.chosen);
			*lastCard = NONE;
			break;
	}

	return ret;
}


/* tell if we need to replay */
int needReplay(t_move* move, t_color lastCard){
	int replay = 0;

	if (move->type == DRAW_OBJECTIVES)
		replay = 1;
	else if (move->type == DRAW_BLIND_CARD && lastCard == NONE)
		replay = 1;
	else if (move->type == DRAW_CARD && lastCard == NONE && move->drawCard.card != MULTICOLOR)
		replay = 1;

	return replay;
}