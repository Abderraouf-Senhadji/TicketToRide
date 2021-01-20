#include <stdio.h>
#include <stdlib.h>
#include "TicketToRideAPI.h"
#include "game.h"
#include "dijkstra.h"

#define INFINITY 50;

void dijkstra(int src, int dest, t_board* board, int* D, int* Prec){
	int i,u,v;
 	int N = board->nbCities;
	int visited [N];
	for(i=0; i < N; i++){
		D[i] = INFINITY;
		visited[i] = 0;
	} 
	printf("hu");
	D[src] = 0;
	while(u != dest){
		u = minDistance(D,visited,N);
		visited[u] = 1;
		for(v=0; v < board->nbCities; v++){
			t_track* currentTrack = *(board->tracks + (( u* board->nbCities + v) * sizeof(t_track*)));
			if (currentTrack != NULL){
				if (currentTrack->taken == 0){
					/* if the distance between src and any given non-visited city -which is not claimed by the opponent-
					is shorter by passing through u, D[v] is updated to the new value */
					if (visited[v]==0 && (D[u] + currentTrack->length) < D[v]){
						D[v] = D[u] + currentTrack->length;
						Prec[v] = u;
					}
				}
				else{
					/*if the track between u and v is already ours */
					if (currentTrack->takenByUs){
						D[v] = D[u];
						Prec[v] = u;
					}
				}
			}
		}
	}
	printf("huhuhu");
}
/* THis function return the closest non-visited city */
int minDistance (int* D, int* visited, int N){
	int i, min, min_index;
	min=INFINITY;
	for (i=0; i < N; i++){
		if (visited[i]==0 && D[i] < min){
			min = D[i];
			min_index = i;
		}
	}
	return min_index;
}

int displayRoute (int src, int dest, t_board* board, int* Prec, t_track* objectiveTracks){
	int v = dest;
	int numberTracks = 0;
	while (v != src){
		t_track* objTrk = objectiveTracks + numberTracks * sizeof(t_track*);
		objTrk = *(board->tracks + (board->nbTracks * v + Prec[v]) * sizeof(t_track*));
		v = Prec[v];
		numberTracks++;
	}
	return numberTracks;
}