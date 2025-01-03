/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "bakery.h"

static int choosing[COUNT_CLIENTS] = {0};
static int numbers[COUNT_CLIENTS] = {0};
int number_last = 0;
int index_last = 0;

int max_number()
{
	int res = 0;

	for (int i = 0; i < COUNT_CLIENTS; i++)
		if (numbers[i] > res)
			res = numbers[i];

	return res;
}

struct BAKERY *
get_number_1_svc(struct BAKERY *argp, struct svc_req *rqstp)
{
	static struct BAKERY  result;

	// pthread_mutex_lock(&mutex);
	int ind = index_last;
	index_last++;

	choosing[ind] = 1;
	numbers[ind] = max_number() + 1;
	choosing[ind] = 0;
	result.num = numbers[ind];
	result.ind = ind;

	return &result;
}

struct BAKERY *
serve_1_svc(struct BAKERY *argp, struct svc_req *rqstp)
{
	static struct BAKERY  result;

	if (numbers[argp->ind] < number_last) {
		result.num = -1;
		return &result;
	}
	
	for (int j = 0; j < COUNT_CLIENTS; j++) {
		while (choosing[argp->ind]);

		time_t start = time(NULL);
		while (numbers[j] > number_last && 
			(numbers[j] < numbers[argp->ind] || 
			(numbers[j] == numbers[argp->ind] && j < argp->ind)) 
			&& difftime(time(NULL), start) < 1);
	}

	switch(argp->op)
	{
		case ADD:
			result.result = argp->arg1 + argp->arg2;
			break;
		case SUB:
			result.result = argp->arg1 - argp->arg2;
			break;
		case MUL:
			result.result = argp->arg1 * argp->arg2;
			break;
		case DIV:
			result.result = argp->arg1 / argp->arg2;
			break;
		default:
			break;
	}

	number_last = numbers[argp->ind];

	return &result;
}
