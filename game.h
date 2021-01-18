//
// Created by Thib on 14/12/2020.
//

#ifndef T2R_TEST_GAME_H
#define T2R_TEST_GAME_H


#include "TicketToRideAPI.h"



typedef struct{
	int city1, city2;				/* id of the cities */
	int length;						/* length of the track */
	t_color color1, color2;			/* colors */
	int taken;						/* tell if taken */
} t_track;

typedef struct s_board {
	int  nbCities;							/* number of cities */
	int  nbTracks;							/* number of tracks */
	int* arrayTracks;						/* array of int for the tracks */
	t_track** tracks;						/* pointer to the array of t_track pointers */
} t_board;


typedef struct{
	char name[20];					/* name of the player */
	int nbWagons;					/* number of wagons */
	int nbCards;					/* number of cards */
	int cards[10];					/* number of cards of each color, ie cards[YELLOW] is the number of YELLOW cards */
	int nbObjectives;				/* number of objectives */
	t_objective objectives[20];		/* objectives, only objectives with index < nbObjectives are available */
	t_objective latentObjectives[3];/* objectives retured by the drawObjectives move */
	t_track** pClaimedTrack;		
	t_track** pClaimedTrackSymetric;
} t_player;


typedef struct{
	char name[20];					/* name of the game */
	t_color faceUp[5];				/* face up cards */
	int player;						/* player who plays: 0 == US, 1 == OPPONNENT */
	t_player players[2];
	int firstTurn;
} t_game;

void initGame(t_game* game, t_board* board);

void updateGame(t_game* game, t_move* move, t_board* board);

void addCard(t_game* game, t_color card);

void removeCard(t_game* game, t_color card);


#endif //T2R_TEST_GAME_H
