#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "game.h"

void dijkstra(int src, int dest, t_board* board, int* D, int* Prec);

int minDistance (int* D, int* visited, int N);

int displayRoute (int src, int dest, t_board* board, int* Prec, t_track* objectiveTracks);

#endif //DIJKSTRA_H