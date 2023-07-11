#include<stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>


void test(int *count){
    *count = -5;
}

int main()
{
	double mark = 1, level = 1;
	double p[3];
	int i, j;
	double sum = 0;
	printf("%-20s%-20s%-20s%-20s\n", "75-100", "50-75", "25-50", "0-25");
	for(int i = 1; i <= 15; i++){
		sum = 0;
		if(i < 6){
			mark = 5;
		} 
		else if(i < 11){
			mark = 10;
		}
		else{
			mark = 15;
		}
		p[0] = mark*10/(pow(level, 3)*log(mark))*100;
		p[1] = 2*p[0];
		p[2] = 2*p[1];
		
		for(j = 0; j < 3; j++){
			if((sum + p[j]) > 100){
				p[j] = 100 - sum;
			}
			sum += p[j];
		}
		
		level += 1;
		printf("%-20.3f%-20.3f%-20.3f%-20.3f\n", p[0], p[1], p[2], 100 - p[0] - p[1] - p[2]);
	}
    return 0;
}
