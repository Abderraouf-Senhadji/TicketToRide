#include <stdio.h>
#include <stdlib.h>
#include "TicketToRideAPI.h"
#include "game.h"

//consider using general constants N for the number of tracks one for infinity

void djikstra(int src, t_board* boardtracks, int* D, int* Prec){
	int i,u,v;
	int visited [N];
	for(i=0; i< N; i++){
		D[i]=20;
		visited[i]=0;
	} 
	D[src]=0;
	for(i=0; i< N; i++){
		u = minDistance(D,visited);
		visited[u]=1;
		for(v=0; v < board->nbCities; v++){
			// Chercher comment repérer une route prise par soi
			if (visited[v]==0 && tracks[u][v]!=NULL)
				&& (D[u] + tracks[u][v]->length) < D[v] 
				&& tracks[u][v]->taken==0){
				D[v] = D[u] + tracks[u][v]->length;
				Prec[v] = u;
			}
		}
	}
}

int minDistance (int* D, int* visited){
	int i, min, min_index;
	min=20;
	for (i=0; i < N; i++){
		if (visited[i]==0 && D[i] < min){
			min = D[i];
			min_index = i;
		}
	}
	return min_index;
}