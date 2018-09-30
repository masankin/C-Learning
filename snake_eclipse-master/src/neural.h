/*
 * neural.h
 *
 *  Created on: 2015/11/29
 *      Author: dou
 */

#ifndef NEURAL_H_
#define NEURAL_H_

typedef struct node{
	int num_inputs;
	float *weights;
	float *inputs;
	float *errors;
	float output;
} node;

typedef struct layer{
	int num_nodes;
	node *chr;
} layer;
class neural{
	private:
		int num_inputs;
		int num_outputs;
		int num_layers;
		int num_weights;
		int num_hid_nodes;
		float leaning_rate;
		layer *layers;
		float *weights;

	public:
		neural();
		neural(int in, int out, int num, int hn, float lrate);
		void init();
		int get_num_weights();
		float *get_weights();
		void put_weights(float *weights);
		float* feed(float *inputs);
		float get_weighted_error(int l, int in);
		void learn(float *dout);
		float convert(float input);
		~neural();
};

neural :: neural(){
}


#endif /* NEURAL_H_ */
