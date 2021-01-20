#include <stdio.h>
#include <stdlib.h>
#include "TicketToRideAPI.h"
#include "game.h"
#include "dijkstra.h"

void initGame(t_game* game, t_board* board){

	waitForT2RGame("TRAINING DO_NOTHING timeout=10000", game->name, &(board->nbCities), &(board->nbTracks));

	board->arrayTracks = malloc(5*board->nbTracks*sizeof(int));

	t_color ourCards[4];	/* our cards */

	game->player = getMap(board->arrayTracks, game->faceUp, ourCards);

	/*Initializing the first turn indicator*/
	game->firstTurn = 1;

	int i,j;

	/*Initializing the hand of each player*/
	for (i=0; i<2; i++){
		game->players[i].nbWagons=45;
		game->players[i].nbCards=4;
		game->players[i].nbObjectives=0;
	}

	/*Inspecting the initial cards*/
	for (i=0; i<4; i++){
		for (j=0; j<10; j++){
			if (ourCards[i]==j)
				game->players[game->player].cards[j]++;
		}
	}

	/* Creating an array of pointer to tracks to check for existence */
	t_track** tracks = malloc(board->nbCities * board->nbCities * sizeof(t_track*));
	board->tracks =tracks;
	int k;
	int kmax = board->nbCities/5 + 1;
	for (i=0; i < board->nbCities; i++){
		for (j=0; j < board->nbCities; j++){
			/* Initializing the track pointer to NULL (no track)*/
			t_track* currentTrack = *(board->tracks + ((i * board->nbCities + j) * sizeof(t_track*))); 
			currentTrack = NULL;
			for (k=0; k < kmax; k++){
				/* check for existing track between city_i and city_j*/
				if (board->arrayTracks[5*k]==i && board->arrayTracks[5*k+1]==j){
					currentTrack = malloc(sizeof(t_track));
					/* create the track */
					currentTrack->city1 = i;
					currentTrack->city2 = j;
					currentTrack->length = board->arrayTracks[5*k+2];
					currentTrack->color1 = board->arrayTracks[5*k+3];
					currentTrack->color2 = board->arrayTracks[5*k+4];
					currentTrack->taken = 0;
					currentTrack->takenByUs = 0;
				}
			}
		}
	}

	/* Creating a 2D array of integers Prec[20][nbCities] 
	that indicates the preceeding city for the city Prec[][city] to complete objective Prec[objective][] */
	int** Prec = malloc (20 * board->nbCities * sizeof(int*));
	game->players[game->player].Prec = Prec;

	/* Creating a 2D array of pointers to t_track types objectiveTracks [20][nbTracks] for each objective
	this array indicates the tracks that need to be claimed to achieve the objective as given by the dijkstra algorithm */
	t_track** objectiveTracks = malloc(20 * board->nbTracks * sizeof(t_track*));
	game->players[game->player].objectiveTracks = objectiveTracks;
	for (i=0; i < 20; i++){
		for (j=0; j < board->nbTracks; j++){
			/* Initializing the track pointer to NULL (no track)*/
			*(objectiveTracks + (board->nbTracks * i + j) * sizeof(t_track*)) = NULL;
		}
	}

	int** Dist = malloc (20 * board->nbCities * sizeof(int*));
	game->players[game->player].Dist = Dist;

}

void updateGame(t_game* game, t_move* move, t_board* board){
	int i,j;
	switch (move->type) {
		case CLAIM_ROUTE:
			game->players[game->player].pClaimedTrack = ( board->tracks + ((move->claimRoute.city1 * board->nbCities + move->claimRoute.city2) * sizeof(t_track*)));
			game->players[game->player].pClaimedTrackSymetric = ( board->tracks + ((move->claimRoute.city2 * board->nbCities + move->claimRoute.city1) * sizeof(t_track*)));
			if ( *(game->players[game->player].pClaimedTrack) != NULL){
				/* Removing the locomotive of the player*/
				for(i=0; i < move->claimRoute.nbLocomotives; i++){
					removeCard(game, MULTICOLOR);
				}	
				/* Removing the color cards corresponding to the track color*/
				for(i=0; i < ((*(game->players[game->player].pClaimedTrack))->length - move->claimRoute.nbLocomotives); i++){
					removeCard(game, move->claimRoute.color);
				}
				/* Removing the wagons of the player*/	
				game->players[game->player].nbWagons--;
				/* Marking the track and its twin as taken*/
				(*(game->players[game->player].pClaimedTrack))->taken=1;
				(*(game->players[game->player].pClaimedTrackSymetric))->taken=1;
				/* Marking the track as taken by us */
				if (game->player == 0){
					(*(game->players[game->player].pClaimedTrack))->takenByUs=1;
					(*(game->players[game->player].pClaimedTrackSymetric))->takenByUs=1;
				}

			}
			break;
		case DRAW_CARD:
			addCard(game, move->drawCard.card);
			for(i=0; i<5; i++){
				game->faceUp[i]=move->drawCard.faceUp[i];
			}
			break;
		case DRAW_BLIND_CARD:
			addCard(game, move->drawBlindCard.card);
			break;
		case DRAW_OBJECTIVES:
			/* Obtianing the objective card that were drawn */
			for(i=0; i<3; i++){
				game->players[game->player].latentObjectives[i] = move->drawObjectives.objectives[i];
			}
			break;
		case CHOOSE_OBJECTIVES:
			/*Updating the objectives cards and their number for the player*/
			move->chooseObjectives.nbObjectives = 0;
			for(i=0; i<3; i++){
				while (game->players[game->player].nbObjectives + move->chooseObjectives.nbObjectives < 20){
					if (move->chooseObjectives.chosen[i]==1){
						move->chooseObjectives.nbObjectives++;
						/*Updating the objective cards array of the player*/
						game->players[game->player].objectives[(game->players[game->player].nbObjectives) + (move->chooseObjectives.nbObjectives)] = game->players[game->player].latentObjectives[i];
					}
				}
			}
			/* Creating the tables of tracks for each of objective using the dijkstra algorithm */
			for (i = game->players[game->player].nbObjectives; i < (game->players[game->player].nbObjectives + move->chooseObjectives.nbObjectives); i++ ){
				/*printf("0\n");
				int* currentD = *(game->players[game->player].Dist + ((i * board->nbCities) * sizeof(int*)));
				printf("1\t");
				int objectiveSRC = game->players[game->player].objectives[i].city1;
				int objectiveDST = game->players[game->player].objectives[i].city2; 
				printf("2\t");
				int* currentObjectivePrec = *(game->players[game->player].Prec + ((i * board->nbCities) * sizeof(int*)));
				printf("3\t");
				dijkstra(objectiveSRC, objectiveDST, board, currentD, currentObjectivePrec);
				int numberTracks;
				t_track* currentObjectiveTracks = *(game->players[game->player].objectiveTracks + (board->nbTracks * i * sizeof(t_track*)));
				numberTracks = displayRoute (objectiveSRC, objectiveDST, board, currentObjectivePrec, currentObjectiveTracks); */
				for (j=0; j < numberTracks; j++){
					printf("The %d th road needed to achieve objective %d is %d to %d", j, i, (*(currentObjectiveTracks + j * sizeof(t_track*))).city1, (*(currentObjectiveTracks + j * sizeof(t_track*))).city2 );
				} 
			}

			/*Updating the number of Objective cards of the player*/
			game->players[game->player].nbObjectives += move->chooseObjectives.nbObjectives;

			break;
	}
	if (game->firstTurn == 1 && game->player == 0){
		game->firstTurn = 0;
	}
}

void addCard(t_game* game, t_color card){
	game->players[game->player].cards[card]++;
	game->players[game->player].nbCards++;
}

void removeCard(t_game* game, t_color card){
	game->players[game->player].cards[card]--;
	game->players[game->player].nbCards--;
}
