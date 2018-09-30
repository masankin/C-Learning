/*
 * snake.h
 *
 *  Created on: 2015/11/29
 *      Author: dou
 */

#ifndef SNAKE_H_
#define SNAKE_H_

#include "neural.h"
using namespace std;
#include <cstdlib>
#include <math.h>
#include <iostream>
#include <cstdlib>


#include <stdio.h>


typedef struct sq{
	int x;
	int y;
	int mx;
	int my;
	struct sq *nexploration_ratet;
} sq;

sq *snake = NULL;

int      mx;
int      my;
neural *net;

int food_x       =     -6;
int food_y       =     -6;
int sc           =      0;
int num_layers   =      2;
int num_inputs   =      6;
int num_outputs  =      1;
int tmp          =     50;
int iterations   =      0;
bool  pus        =  false;
float old_q      =    0.0;
int   fail_count =      0;

int	  exploration_rate  = 	     40;
float learning_rate     = 0.0000001;

void add(int x, int y){
	sq *tmp = (sq *)malloc(sizeof(sq));
	tmp -> x = x;
	tmp -> y = y;
	tmp -> mx = 1;
	tmp -> my = 0;
	tmp -> nexploration_ratet = snake;
	snake = tmp;
}
bool check_body(int x, int y){
	if(x == snake -> nexploration_ratet -> x && y == snake -> nexploration_ratet -> y) return true;
	return false;
}
sq *get_last(){
	sq *p = snake;
	while(p -> nexploration_ratet != NULL) p = p -> nexploration_ratet;
	return p;
}
void start(){
	snake = NULL;
	add(0, 0);
	add(1, 0);
	add(2, 0);
	add(3, 0);
	add(4, 0);
	mx = 1;
	my = 0;
}
void set_f(){
	bool f = true;
	while(f){
		srand(time(NULL));
		food_x = (rand() % 34) - 17;
		srand(time(NULL));
		food_y = (rand() % 34) - 17;
		sq *p = snake;
		while(p != NULL){
			if(p -> x == food_x && p -> y == food_y){
				f = true;
				break;
			}
			f = false;
			p = p -> nexploration_ratet;
		}
	}
}
bool tail(){
	sq *p = snake;
	while(p -> nexploration_ratet != NULL){
		if(p -> nexploration_ratet -> x == snake -> x && p -> nexploration_ratet -> y == snake -> y)
			return true;
		p = p -> nexploration_ratet;
	}
	return false;
}
bool tail2(int sx, int sy){
	sq *p = snake;
	while(p -> nexploration_ratet != NULL){
		if(p -> nexploration_ratet -> x == sx && p -> nexploration_ratet -> y == sy)
			return true;
		p = p -> nexploration_ratet;
	}
	return false;
}
float check2(int x, int y){
	sq *p = snake;
	while(p -> nexploration_ratet != NULL){
		if(p -> nexploration_ratet -> x == x && p -> nexploration_ratet -> y == y)
			return -1.0;
		p = p -> nexploration_ratet;
	}
	if(x > 18 || x < -18 || y > 18 || y < -18) return -1.0;
	return 1.0;
}
float check(int x, int y){
	sq *p = snake;
	while(p != NULL){
		if(p -> x == x && p -> y == y)
			return -1.0;
		p = p -> nexploration_ratet;
	}
	if(x > 18 || x < -18 || y > 18 || y < -18) return -1.0;
	return 1.0;
}
void rev(){
	sq *snake2 = NULL;
	sq *p = snake;
	while(p != NULL){
		sq *tmp = (sq *)malloc(sizeof(sq));
		tmp -> x = p -> x;
		tmp -> y = p -> y;
		tmp -> mx = -1 * p -> mx;
		tmp -> my = -1 * p -> my;
		tmp -> nexploration_ratet = snake2;
		snake2 = tmp;
		sq *x = p -> nexploration_ratet;
		free(p);
		p = x;
	}
	snake = snake2;
	mx = snake -> mx;
	my = snake -> my;
}

void move(){
	sq *p = snake;
	int x = p -> x;
	int y = p -> y;
	int tmx = p -> mx;
	int tmy = p -> my;
	while(p -> nexploration_ratet != NULL){
		sq *q = p -> nexploration_ratet;
		int tmp = q -> x;
		q -> x = x;
		x = tmp;

		tmp = q -> y;
		q -> y = y;
		y = tmp;

		tmp = q -> mx;
		q -> mx = tmx;
		tmx = tmp;

		tmp = q -> my;
		q -> my = tmy;
		tmy = tmp;

		p = p -> nexploration_ratet;
	}
	snake -> mx = mx;
	snake -> my = my;
	snake -> x += mx;
	snake -> y += my;
}


#endif /* SNAKE_H_ */
