/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "bakery.h"

static bool_t choosing[COUNT_CLIENTS] = {FALSE};
static int numbers[COUNT_CLIENTS] = {0};
static int pids[COUNT_CLIENTS] = {0};

int max_number()
{
	int res = numbers[0];

	for (int i = 1; i < COUNT_CLIENTS; i++)
		if (numbers[i] > res)
			res = numbers[i];

	return res;
}

bool_t
get_number_1_svc(struct BAKERY *argp, struct BAKERY *result, struct svc_req *rqstp)
{
	bool_t retval = TRUE;

	pids[argp->ind] = argp->pid;

	choosing[argp->ind] = TRUE;
	result = numbers[argp->ind] = max_number() + 1;
	choosing[argp->ind] = FALSE;

	return retval;
}

bool_t
serve_1_svc(struct BAKERY *argp, struct BAKERY *result, struct svc_req *rqstp)
{
	bool_t retval = TRUE;

	for (int i = 0; i < COUNT_CLIENTS; i++)
	{
		while (choosing[i]) {}
		while (numbers[i] != 0 && (numbers[i] < numbers[argp->ind] || 
								   numbers[i] == numbers[argp->ind] && 
								   pids[i] < pids[argp->ind])) {}
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
	// Начало критической секции
	result = counter++;
	printf("Client with number %d (pid: %d) got result: %d\n", numbers[argp->ind], pids[argp->ind], result);
	numbers[argp->ind] = 0;
	// Конец критической секции

	return retval;
}

int
bakery_prog_1_freeresult (SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
	xdr_free (xdr_result, result);

	/*
	 * Insert additional freeing code here, if needed
	 */

	return 1;
}