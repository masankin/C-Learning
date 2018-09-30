//============================================================================
// Name        : My_snake.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include "snake.h"
#include "neural.h"
#include "neural_net.cpp"
using namespace std;
#include <cstdlib>
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>



using namespace std;


neural :: neural(int in, int out, int num, int hn, float lrate){
	num_inputs = in;
	num_outputs = out;
	num_layers = num;
	num_hid_nodes = hn;
	num_weights = 0;
	leaning_rate = lrate;
	layers = (layer *)malloc(sizeof(layer) * num);
	layers[0].num_nodes = in;
	layers[0].chr = (node *)malloc(sizeof(node) * in);
	for(int i = 0; i < in; i++){
		(layers[0].chr[i]).num_inputs = 1;
		num_weights += 1;
		(layers[0].chr[i]).weights = (float *)malloc(sizeof(float) * (1));
		(layers[0].chr[i]).inputs = (float *)malloc(sizeof(float) * (1));
		(layers[0].chr[i]).errors = (float *)malloc(sizeof(float) * (1));
		for(int e = 0; e < 1; e++) (layers[0].chr[i]).errors[e] = 0.0;
	}
	for(int i = 1; i < num - 1; i++){
		layers[i].chr = (node *)malloc(sizeof(node) * hn);
		layers[i].num_nodes = hn;
		int nd = layers[i - 1].num_nodes;
		for(int j = 0; j < hn; j++){
			(layers[i].chr[j]).num_inputs = nd + 1;
			num_weights += nd + 1;
			(layers[i].chr[j]).weights = (float *)malloc(sizeof(float) * (nd + 1));
			(layers[i].chr[j]).inputs = (float *)malloc(sizeof(float) * (nd + 1));
			(layers[i].chr[j]).errors = (float *)malloc(sizeof(float) * (nd + 1));
			for(int e = 0; e < nd + 1; e++) (layers[i].chr[j]).errors[e] = 0.0;
		}
	}
	int nd = layers[num - 2].num_nodes;
	layers[num - 1].num_nodes = out;
	layers[num - 1].chr = (node *)malloc(sizeof(node) * out);
	for(int i = 0; i < out; i++){
		(layers[num - 1].chr[i]).num_inputs = nd + 1;
		num_weights += nd + 1;
		(layers[num - 1].chr[i]).weights = (float *)malloc(sizeof(float) * (nd + 1));
		(layers[num - 1].chr[i]).inputs = (float *)malloc(sizeof(float) * (nd + 1));
		(layers[num - 1].chr[i]).errors = (float *)malloc(sizeof(float) * (nd + 1));
		for(int e = 0; e < nd + 1; e++) (layers[num - 1].chr[i]).errors[e] = 0.0;
	}
	weights = (float *)malloc(sizeof(float) * num_weights);
}

neural :: ~neural(){
	for(int i = 0; i < num_layers; i++){
		for(int j = 0; j < layers[i].num_nodes; j++){
			delete[] (layers[i].chr[j]).weights;
			delete[] (layers[i].chr[j]).inputs;
			delete[] (layers[i].chr[j]).errors;
		}
		delete[] layers[i].chr;
	}
	delete[] layers;
}
void neural :: init(){
	float weights[num_weights];
	for(int i = 0; i < num_weights; i++){
		 weights[i] = (float)rand() / (float)RAND_MAX - 0.5;
	}
	put_weights(weights);
	for(int i = num_layers - 2; i >= 0; i--){
		for(int j = 0; j < layers[i].num_nodes ; j++){
			for(int k = 0; k < (layers[i].chr[j]).num_inputs; k++){
				(layers[i].chr[j]).weights[k] = 1;
			}
		}
	}
}
int neural :: get_num_weights(){
	return num_weights;
}

float* neural :: get_weights(){
	int n = 0;
	for(int i = 0; i < num_layers; i++){
		for(int j = 0; j < layers[i].num_nodes; j++){
			for(int k = 0; k < (layers[i].chr[j]).num_inputs; k++){
				weights[n] =  (layers[i].chr[j]).weights[k];
				n++;
			}
		}
	}
	return weights;
}
void neural :: put_weights(float *weights){
	int n = 0;
	for(int i = 0; i < num_layers; i++){
		for(int j = 0; j < layers[i].num_nodes; j++){

			for(int k = 0; k < (layers[i].chr[j]).num_inputs; k++){
				(layers[i].chr[j]).weights[k] = weights[n];
				n++;
			}
		}
	}
}
float* neural :: feed(float *inputs){
	int n = 0;
	float *outputs;
	for(int i = 0; i < num_layers; i++){
		outputs = (float *)malloc(sizeof(float) * layers[i].num_nodes + 1);
		for(int j = 0; j < layers[i].num_nodes; j++){
			float sum = 0.0;
			if(i == 0){
				(layers[i].chr[j]).inputs[0] = inputs[j];
				sum = (layers[i].chr[j]).weights[0] * inputs[j];
			}else{

				for(int k = 0; k < (layers[i].chr[j]).num_inputs; k++){
					(layers[i].chr[j]).inputs[k] = inputs[k];
					sum += (layers[i].chr[j]).weights[k] * inputs[k];
				}
			}
			outputs[j] = sum;
			(layers[i].chr[j]).output = outputs[j];
		}
		outputs[layers[i].num_nodes] = -1.0;
		inputs = outputs;
	}
	return outputs;
}
float neural :: get_weighted_error(int l, int in){
	float sum = 0.0;
	for(int j = 0; j < layers[l].num_nodes; j++){
		float error  = (layers[l].chr[j]).errors[in];
		float weight = (layers[l].chr[j]).weights[in];
		sum += error * weight;
	}
	return sum;
}
void neural :: learn(float *dout){
	int tmp = num_layers - 1;
	for(int j = 0; j < layers[tmp].num_nodes; j++){

		for(int k = 0; k < (layers[tmp].chr[j]).num_inputs; k++){
			(layers[tmp].chr[j]).errors[k] = dout[0];
			(layers[tmp].chr[j]).weights[k] +=
					leaning_rate * (layers[tmp].chr[j]).inputs[k] *
					(layers[tmp].chr[j]).errors[k];
		}
	}

	for(int i = num_layers - 2; i >= 0; i--){
		for(int j = 0; j < layers[i].num_nodes ; j++){

			float sum = get_weighted_error(i + 1, j);
			for(int k = 0; k < (layers[i].chr[j]).num_inputs; k++){

				(layers[i].chr[j]).errors[k] = sum;
				(layers[i].chr[j]).weights[k] +=
					leaning_rate * (layers[i].chr[j]).inputs[k] * (layers[i].chr[j]).errors[k];
			}
		}
	}

}
//************************************************************************************************************




void par(float x1, float x2, float y1, float y2, float z1, float z2){
	glColor3f(1.0, 0.0, 1.0);

	glBegin(GL_QUADS);

	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y1, z1);
	glVertex3f(x2, y2, z1);
	glVertex3f(x1, y2, z1);

	glEnd();
}




void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();

	glTranslatef(0.0, 0.0, -22.0);
	int i;
	sq *p = snake;
	par(-8.7,  9.2,  9.0,  9.2, 0.0, 0.0);
	par(-8.7,  9.2, -8.5, -8.7, 0.0, 0.0);
	par(-8.5, -8.7, -8.7,  9.2, 0.0, 0.0);
	par( 9.2,  9.0, -8.7,  9.2, 0.0, 0.0);
	while(p != NULL){
		par((p -> x)/2.0,(p -> x)/2.0 + 0.4,(p -> y)/2.0,(p -> y)/2.0 + 0.4, 0.0, 0.0);
		p = p -> nexploration_ratet;
	}
	par(food_x/2.0, food_x/2.0 + 0.4 , food_y/2.0 , food_y/2.0 + 0.4, 0.0 , 0.0);
	glutSwapBuffers();
}
float reward(int sx, int sy, int sx1, int sy1){
	if(snake -> x == food_x && snake -> y == food_y){
		add(food_x, food_y);
		fail_count = 0;
		sc++;
		exploration_rate = exploration_rate / 3;
		set_f();
		return 1000.0;
	}else if(tail()){
		fail_count = 0;
		sc = 0;
		start();
		fail_count++;
		return -100000.0;
	}else if(snake -> x > 18 || snake -> x < -18 || snake -> y > 18 || snake -> y < -18){
		start();
		fail_count++;
		return -1000.0;
	}
	if(fail_count > 50){
		exploration_rate = 20;
	}
	float re2 = sqrt((sx1 - food_x) * (sx1 - food_x) + (sy1 - food_y) * (sy1 - food_y));
	return -re2;
}
float *get_q(int sx, int sy){
	float inputs[6];
	inputs[0] = sqrt((sx - food_x) * (sx - food_x) + (sy - food_y) * (sy - food_y));
	inputs[1] = check(sx	, sy	);
	inputs[2] = check(sx + 1, sy	);
	inputs[3] = check(sx    , sy + 1);
	inputs[4] = check(sx - 1, sy);
	inputs[5] = check(sx    , sy - 1);
	return net -> feed(inputs);
}
float max_q(int sx, int sy, int food_x, int food_y){

	float new_q = 0.0;
	int sx1 = sx + 1;
	int sy1 = sy;
	if(check_body(sx1, sy1)){
		sq *last = get_last();
		sx1 = last -> x - last -> mx;
		sy1 = last -> y - last -> my;
	}

	float *out1 = get_q(sx1, sy1);

	sx1 = sx - 1;
	sy1 = sy;
	if(check_body(sx1, sy1)){
		sq *last = get_last();
		sx1 = last -> x - last -> mx;
		sy1 = last -> y - last -> my;
	}

	float *out2 = get_q(sx1, sy1);

	sx1 = sx;
	sy1 = sy + 1;
	if(check_body(sx1, sy1)){
		sq *last = get_last();
		sx1 = last -> x - last -> mx;
		sy1 = last -> y - last -> my;
	}

	float *out3 = get_q(sx1, sy1);

	sx1 = sx;
	sy1 = sy - 1;
	if(check_body(sx1, sy1)){
		sq *last = get_last();
		sx1 = last -> x - last -> mx;
		sy1 = last -> y - last -> my;
	}

	float *out4 = get_q(sx1, sy1);

	if(out1[0] > out2[0]){
		if(out1[0] > out3[0]){
			if(out1[0] > out4[0]){
				new_q = out1[0];
				if(mx == -1) rev();
				else{
					mx =  1;
					my =  0;
				}
			}else{
				new_q = out4[0];
				if(my == 1) rev();
				else{
					mx =  0;
					my = -1;
				}
			}
		}else{
			if(out3[0] > out4[0]){
				new_q = out3[0];
				if(my == -1) rev();
				else{
					mx =  0;
					my =  1;
				}
			}else{
				new_q = out4[0];
				if(my == 1) rev();
				else{
					mx =  0;
					my = -1;
				}
			}
		}
	}else{
		if(out2[0] > out3[0]){
			if(out2[0] > out4[0]){
				new_q = out2[0];
				if(mx == 1) rev();
				else{
					mx = -1;
					my =  0;
				}
			}else{
				new_q = out4[0];
				if(my == 1) rev();
				else{
					mx =  0;
					my = -1;
				}
			}
		}else{
			if(out3[0] > out4[0]){
				new_q = out3[0];
				if(my == -1) rev();
				else{
					mx =  0;
					my =  1;
				}
			}else{
				new_q = out4[0];
				if(my == 1) rev();
				else{
					mx =  0;
					my = -1;
				}
			}
		}
	}
	return new_q;
}

void itera(){
	iterations++;
	int sx = snake -> x;
	int sy = snake -> y;

	float inputs[6];
	int sx1 = sx;
	int sy1 = sy;

	float new_q;
	if(rand() % 100 > exploration_rate){
		new_q = max_q(sx, sy, food_x, food_y);
	}else{
		int a = rand() % 4;
		if(a == 0){
			sx1 = sx + 1;
			sy1 = sy;
			if(check_body(sx1, sy1)){
				sq *last = get_last();
				sx1 = last -> x - last -> mx;
				sy1 = last -> y - last -> my;
			}
			if(mx == -1) rev();
			else{
				mx =  1;
				my =  0;
			}
		}else if(a == 1){
			sx1 = sx - 1;
			sy1 = sy;
			if(check_body(sx1, sy1)){
				sq *last = get_last();
				sx1 = last -> x - last -> mx;
				sy1 = last -> y - last -> my;
			}
			if(mx == 1) rev();
			else{
				mx = -1;
				my =  0;
			}
		}else if(a == 2){
			sx1 = sx;
			sy1 = sy + 1;
			if(check_body(sx1, sy1)){
				sq *last = get_last();
				sx1 = last -> x - last -> mx;
				sy1 = last -> y - last -> my;
			}
			if(my == -1) rev();
			else{
				mx =  0;
				my =  1;
			}
		}else{
			sx1 = sx;
			sy1 = sy - 1;
			if(check_body(sx1, sy1)){
				sq *last = get_last();
				sx1 = last -> x - last -> mx;
				sy1 = last -> y - last -> my;
			}
			if(my == 1) rev();
			else{
				mx =  0;
				my = -1;
			}
		}

		float *out1 = get_q(sx1, sy1);
		new_q = out1[0];
	}
	get_q(sx1, sy1);
	move();
	sx1 = snake -> x;
	sy1 = snake -> y;

	float dout[1];
	float re = reward(sx, sy, sx1, sy1);
	dout[0] =  re + 0.9 * new_q - old_q;
	net -> learn(dout);
	old_q = new_q;
}
void myIdleFunc(int a) {
	if(!pus){
		itera();
	}
	cout << "iterations : " << iterations << " score : " << sc << endl;
	glutPostRedisplay();
	glutTimerFunc(tmp, myIdleFunc, 0);
}
void keyboard(unsigned char key, int x, int y)
{
	if((char)key == 'p'){
		if(pus) pus = false;
		else pus = true;
	}else if((char)key == 't'){
		tmp--;
	}else if((char)key == 'y'){
		if(tmp < 0) tmp = 0;
		tmp++;
	}else if((char)key == 's'){
		set_f();
	}else if((char)key == 'q'){
		cout << "-- SKIPING 500 STEPS --" << endl;
		pus = true;
		for(int i = 0; i < 500; i++) itera();
		pus = false;
	}
}

void init()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glLoadIdentity ();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

	GLfloat acolor[] = {1.4, 1.4, 1.4, 1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, acolor);

}
void Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w/(float)h, 0.1, 200.0);

}
int main(int argc, char** argv)
{

	cout << "-----------------------------------------------" << endl;
	cout << endl;
	cout << "       --- SNAKE GAME A.I. - Q LEARNING ---    " << endl;
	cout << endl;
	cout << "created by : tharindra galahena (inf0_warri0r) " << endl;
	cout << "blog       : www.inf0warri0r.blogspot.com      " << endl;
	cout << endl;
	cout << "-----------------------------------------------" << endl;

	srand(time(NULL));
	net = new neural(num_inputs, num_outputs, num_layers, 10, learning_rate);
	net -> init();
	start();
	set_f();
	glutInit(&argc,argv);
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(450,450);
	glutInitWindowPosition(500,0);
	glutCreateWindow("snake game - Q learning with Neural Network");
	init();
	glutTimerFunc(400, myIdleFunc, 0);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc( keyboard );
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}
