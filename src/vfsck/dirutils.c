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

RCSID("$Header: /tmp/cvstemp/openafs/src/vfsck/dirutils.c,v 1.1.1.3 2001/07/11 03:12:03 hartmans Exp $");

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <afs/partition.h>

/* ensure that we don't have a "/" instead of a "/dev/rxd0a" type of device.
 * Overwrites abuffer with the corrected name.
 */
EnsureDevice(abuffer)
char *abuffer; {
    struct dirent *dp;
    char pbuffer[128];
    struct stat tstat;
    DIR *dirp;
    char *dirName, dirn[100];
    int code, i;
    short dev;

    code = stat(abuffer, &tstat);
    if (code) return code;
    if (((tstat.st_mode & S_IFMT) == S_IFBLK) || ((tstat.st_mode & S_IFMT) == S_IFCHR)) {
	return 0;	/* already a block or char device */
    }
    /* otherwise, assume we've got a normal file, and we look up its device */
    dev = tstat.st_dev;		/* remember device for this file */

    /* now, look in /dev for the appropriate file */
    dirp = opendir(dirName = AFS_DSKDEV);
    while (dp=readdir(dirp)) {
	strcpy(pbuffer, dirName);
	strcat(pbuffer, "/");
	strcat(pbuffer, dp->d_name);
	if (stat(pbuffer, &tstat) != -1 &&
	    (tstat.st_mode & S_IFMT) == S_IFBLK &&
	    (tstat.st_rdev == dev)) {
		strcpy(abuffer, pbuffer);
		closedir(dirp);
		return 0;
	    }
    }
    closedir(dirp);

    return 1;	/* failed */
}

