/*
 * Copyright 2000, International Business Machines Corporation and others.
 * All Rights Reserved.
 * 
 * This software has been released under the terms of the IBM Public
 * License.  For details, see the LICENSE file in the top-level source
 * directory or online at http://www.openafs.org/dl/license10.html
 */

#include <afs/param.h>
#include <afsconfig.h>

RCSID("$Header: /tmp/cvstemp/openafs/src/util/test/dtest.c,v 1.1.1.3 2001/07/11 03:11:52 hartmans Exp $");

#include "ktime.h"

main(argc, argv)
int argc;
char **argv; {
    long code, temp;

    if (argc <= 1) {
	printf("dtest: usage is 'dtest <time to interpret>'\n");
	exit(0);
    }

    code = ktime_DateToLong(argv[1], &temp);
    if (code) {
	printf("date parse failed with code %d.\n", code);
    }
    else {
	printf("returned %d, which, run through ctime, yields %s", temp, ctime(&temp));
    }
    exit(0);
}
