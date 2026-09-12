#include <stdio.h>
#define N 5

int map[N][N] = {
    {1, 1, 0, 0, 0},
    {1, 1, 0, 1, 1},
    {0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0},
    {1, 0, 0, 1, 1}
};

int dx[] = {-1, 1, 0, 0};
int dy[] = {0, 0, -1, 1};

void dfs(int x, int y){
	if (x < 0 || x >= N || y < 0 || y >= N){
		return;
	}
	if (map[x][y] == 0){
		return;
	}
	
	map[x][y] = 0;
	for(int i = 0; i < 4; i++){
		int nx = x + dx[i];
		int ny = y + dy[i];
		dfs(nx, ny);
	}
}

int main(){
	int count = 0;
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			if(map[i][j] == 1){
				count++;
				dfs(i, j);
			}
		}
	}
	printf("총 개수 %d개\n", count);

	return 0;
}
