/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "bakery.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

union argument_t {
	struct BAKERY get_number_1_arg;
	struct BAKERY serve_1_arg;
};
struct thr_data
{
	struct svc_req *rqstp;
	SVCXPRT *transp;
	union argument_t argument;
};

pthread_t p_thread;
pthread_attr_t attr;

void *serv_request(void *arg) {
	struct thr_data *data_ptr = arg;
	struct svc_req *rqstp = data_ptr->rqstp;
	register SVCXPRT *transp = data_ptr->transp;
	union argument_t argument = data_ptr->argument;

	union {
		struct BAKERY get_number_1_res;
		struct BAKERY serve_1_res;
	} result;
	bool_t retval;
	xdrproc_t _xdr_argument, _xdr_result;
	bool_t (*local)(char *, void *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		pthread_exit(0);

	case GET_NUMBER:
		_xdr_argument = (xdrproc_t) xdr_BAKERY;
		_xdr_result = (xdrproc_t) xdr_BAKERY;
		local = (bool_t (*) (char *, void *,  struct svc_req *))get_number_1_svc;
		break;

	case SERVE:
		_xdr_argument = (xdrproc_t) xdr_BAKERY;
		_xdr_result = (xdrproc_t) xdr_BAKERY;
		local = (bool_t (*) (char *, void *,  struct svc_req *))serve_1_svc;
		break;

	default:
		svcerr_noproc (transp);
		pthread_exit(0);
	}
	// memset ((char *)&argument, 0, sizeof (argument));
	// if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
	// 	svcerr_decode (transp);
	// 	pthread_exit(0);
	// }
	retval = (bool_t) (*local)((char *)&argument, (void *)&result, rqstp);
	if (retval > 0 && !svc_sendreply(transp, (xdrproc_t) _xdr_result, (char *)&result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		pthread_exit(0);
	}
	if (!bakery_prog_1_freeresult (transp, _xdr_result, (caddr_t) &result))
		fprintf (stderr, "%s", "unable to free results");

	pthread_exit(0);
}

static void
bakery_prog_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	struct thr_data *data_ptr = malloc(sizeof(struct thr_data));

	if (!data_ptr)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	data_ptr->rqstp = rqstp;
	data_ptr->transp = transp;
	memset ((char *)&data_ptr->argument, 0, sizeof (data_ptr->argument));
	if (!svc_getargs (transp, (xdrproc_t) xdr_BAKERY_CALCULATOR, (caddr_t) &data_ptr->argument)) {
		svcerr_decode (transp);
		return;
	}

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&p_thread, &attr, serv_request, data_ptr);
	usleep(300);
}

int
main (int argc, char **argv)
{
	register SVCXPRT *transp;
	printf("init mutex\n");
	for (size_t i = 0; i < COUNT_CLIENTS; ++i) {
		pthread_mutex_init(&(mut[i]), NULL);
	}

	pmap_unset (BAKERY_PROG, BAKERY_VER);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, BAKERY_PROG, BAKERY_VER, bakery_prog_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (BAKERY_PROG, BAKERY_VER, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, BAKERY_PROG, BAKERY_VER, bakery_prog_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (BAKERY_PROG, BAKERY_VER, tcp).");
		exit(1);
	}

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}