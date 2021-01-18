#include <stdio.h>
#include <stdlib.h>
#include "TicketToRideAPI.h"
#include "game.h"

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
	t_track** tracks = malloc(board->nbCities * board->nbCities* sizeof(t_track*));
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
				}
			}
		}
	}
}

void updateGame(t_game* game, t_move* move, t_board* board){
	int i;
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
				if (move->chooseObjectives.chosen[i]==1){
					move->chooseObjectives.nbObjectives++;
					/*Updating the objective cards array of the player*/
					game->players[game->player].objectives[(game->players[game->player].nbObjectives) + (move->chooseObjectives.nbObjectives)] = game->players[game->player].latentObjectives[i];
				}
			}
			/*Updating the number of Objective cards of the player*/
			game->players[game->player].nbObjectives+=move->chooseObjectives.nbObjectives;
			break;
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