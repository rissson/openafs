/*
 * Copyright 2000, International Business Machines Corporation and others.
 * All Rights Reserved.
 * 
 * This software has been released under the terms of the IBM Public
 * License.  For details, see the LICENSE file in the top-level source
 * directory or online at http://www.openafs.org/dl/license10.html
 */

/*
 *	Implementation of basic procedures for the AFS user account
 *	facility.
 */

/*
 * --------------------- Required definitions ---------------------
 */
#include <afs/param.h>
#include <afsconfig.h>

RCSID("$Header: /tmp/cvstemp/openafs/src/uss/uss_procs.c,v 1.1.1.3 2001/07/11 03:11:40 hartmans Exp $");

#include "uss_procs.h"		/*Module interface*/
#include "uss_common.h"		/*Common defs & operations*/
#include "uss_acl.h"		/*ACL-related operations*/
#include <errno.h>		/*Unix error codes*/
#include <pwd.h>		/*Password info*/
#include <sys/stat.h>		/*Stat defs*/
#include <dirent.h>		/*Directory package*/
#include <sys/file.h>		/*O_EXCL, O_CREAT, etc*/
#ifdef	AFS_SUN5_ENV
#include <fcntl.h>
#endif
#include <afs/kautils.h>	/*MAXKTCREALMLEN*/

extern int errno;

#undef USS_PROCS_DB
#undef USS_PROCS_DB_INSTANCE
#undef USS_PROCS_DB_BUILDDIR

#define uss_procs_MAX_SIZE	2048

char temp[1000];
extern int line;
extern char *rindex();

static int Copy();
static int Echo();

/*-----------------------------------------------------------------------
 * EXPORTED uss_procs_BuildDir
 *
 * Environment:
 *	Called from the code generated by the uss grammar.
 *
 * Side Effects:
 *	As advertised.
 *------------------------------------------------------------------------*/

afs_int32 uss_procs_BuildDir(a_path, a_mode, a_owner, a_access)
    char *a_path;
    char *a_mode;
    char *a_owner;
    char *a_access;

{ /*uss_procs_BuildDir*/

    int m, o;
    char buf[1000];
    struct uss_subdir *new_dir;
    struct stat stbuf;

    /*
     * Don't do anything if there's already a problem.
     */
    if (uss_syntax_err)
	return(1);
    
    if (uss_verbose)
	printf("Building directory '%s'; owner: '%s', ACL: '%s'\n",
	       a_path, a_owner, a_access);

    /*
     * If we've not been given permission to overwrite things, make sure
     * the target doesn't exist before doing anything.
     */
    if (!uss_OverwriteThisOne) {
	strcpy(temp, a_path);
	if (!stat(temp, &stbuf)) {
	    if (uss_verbose)
		printf("\t[Directory exists, NOT overwriting it]\n");
	    return(0);
	}
    }

    sscanf(a_mode, "%o", &m);
    o = uss_procs_GetOwner(a_owner);
#ifdef USS_PROCS_DB_BUILDDIR
    printf("DEBUGGING: Owner '%s' is uid %d\n", a_owner, o);
#endif /* USS_PROCS_DB_BUILDDIR */

    if (!uss_DryRun) {
	if (mkdir(a_path, m)) {
	    /*
	     * Can't make the directory.  Complain if the directory doesn't
	     * already exist.
	     */
	    if (errno != EEXIST) {
		uss_procs_PrintErr(line,
				   "Failed to create directory '%s': %s\n",
				   a_path, sys_errlist[errno]);
		return(1);
	    } /*Directory didn't exist*/
	} /*Create the directory*/
    } /*Not a dry run*/
    else {
	if (uss_OverwriteThisOne)
	    fprintf(stderr, "\t[Dry run: mkdir %s, mode %o]\n",
		    a_path, m);
    } /*Dry run*/

    if (!uss_DryRun) {
	if (chmod(a_path, m)) {
	    uss_procs_PrintErr(line,
		"Can't chmod() directory '%s' to be '%s' : %s\n",
		a_path, a_mode, sys_errlist[errno]);
		return(1);
	} /* chmod the directory */
	if (chown(a_path, o, -1)) {
	    uss_procs_PrintErr(line,
			       "Can't chown() directory '%s' to be owned by '%s' (uid %d): %s\n",
			       a_path, a_owner, o, sys_errlist[errno]);
	    return(1);
	} /*Couldn't chown*/
    } /*Not a dry run*/
    else {
	fprintf(stderr,
		"\t[Dry run: chown() directory '%s' to be owned by user]\n",
		a_path);
    } /*Dry run*/

    /*
      * Set the ACL for this new directory so that the uss_AccountCreator
      * is the only party that has rights.  This will be corrected as the
      * final action performed on the account.
      */
    sprintf(buf, "%s %s all", a_path, uss_AccountCreator);
    if (!uss_DryRun) {
	if (uss_verbose)
	    fprintf(stderr,
		    "Setting ACL: '%s'\n", buf);
	if (uss_acl_SetAccess(buf, 1, 0))
	    return(1);
    } /*Not a dry run*/
    else {
	fprintf(stderr, "\t[Dry run: uss_acl_SetAccess(%s) on '%s']\n",
		buf, a_path);
    } /*Dry run*/

    /*
     * Use our linked list to remember this directory's true ACL setting so
     * we may set it correctly at the tail end of the account creation.
     */
    new_dir = (struct uss_subdir *) malloc(sizeof(struct uss_subdir));
    new_dir->previous = uss_currentDir;
    new_dir->path = (char *) malloc(strlen(a_path)+1);
    strcpy(new_dir->path, a_path);
    new_dir->finalACL = (char *) malloc(strlen(a_access)+1);
    strcpy(new_dir->finalACL, a_access);
    uss_currentDir = new_dir;

    /*
     * Return the happy news.
     */
    return(0);

} /*uss_procs_BuildDir*/


/*-----------------------------------------------------------------------
 * EXPORTED uss_procs_CpFile
 *
 * Environment:
 *	Called from the code generated by the uss grammar.
 *
 * Side Effects:
 *	As advertised.
 *------------------------------------------------------------------------*/

afs_int32 uss_procs_CpFile(a_path, a_mode, a_owner, a_proto)
    char *a_path;
    char *a_mode;
    char *a_owner;
    char *a_proto;
    
{ /*uss_procs_CpFile*/
    
    int m, o;
    struct stat stbuf;
    char *cp;

    /*
     * Don't do anything if something has already gone wrong.
     */
    if (uss_syntax_err)
	return(1);

    if (uss_verbose)
	printf("Installing '%s'\n", a_path);

    /*
     * If we've not been given permission to overwrite things, make sure
     * the target doesn't exist before doing anything.
     */
    if (!uss_OverwriteThisOne) {
	strcpy(temp, a_path);
	if (!stat(temp, &stbuf)) {
	    if (uss_verbose)
		printf("\t[Entry exists, NOT overwriting it]\n");
	    return(0);
	}
    }

    sscanf(a_mode, "%o", &m);
    o = uss_procs_GetOwner(a_owner);

    strcpy(temp, a_proto);

    if (stat(temp, &stbuf)) {
	uss_procs_PrintErr(line, "Failed to stat '%s': %s\n",
			   a_proto, sys_errlist[errno]);
	return(1);
    }

    if (stbuf.st_mode & S_IFDIR) {
	if ((cp = rindex(a_path, '/')) == NULL) {
	    strcat(a_proto, "/");
	    strcat(a_proto, a_path);
	}
	else {
	    /*
	     * Append the last part (file name).
	     */
	    strcat(a_proto, cp);
	}
    } /*Target is a directory*/

    if (!uss_DryRun) {
	if (Copy(a_proto, a_path, m)) {
	    uss_procs_PrintErr(line,
			       "Failed to copy '%s' to '%s'\n",
			       a_proto, a_path);
	    return(1);
	} /*Copy failed*/
    } /*Not a dry run*/
    else {
	fprintf(stderr, "\t[Dry run: Copying '%s' to '%s', mode %o]\n",
		a_proto, a_path, m);
    }

    if (!uss_DryRun) {
	if (chown(a_path, o, -1)) {
	    uss_procs_PrintErr(line,
			       "Can't chown() file '%s' to be owned by '%s' (uid %d): %s\n",
			       a_path, a_owner, o, sys_errlist[errno]);
	    return(1);
	} /*chown failed*/
    } /*Not a dry run*/
    else {
	fprintf(stderr, "\t[Dry run: chown() file '%s' to be owned by user]\n",
		a_path);
    } /*Dry run*/

    /*
     * Return the happy news.
     */
    return(0);
    
} /*uss_procs_CpFile*/


/*-----------------------------------------------------------------------
 * EXPORTED uss_procs_EchoToFile
 *
 * Environment:
 *	Called from the code generated by the uss grammar.
 *
 * Side Effects:
 *	As advertised.
 *------------------------------------------------------------------------*/

afs_int32 uss_procs_EchoToFile(a_path, a_mode, a_owner, a_content)
    char *a_path;
    char *a_mode;
    char *a_owner;
    char *a_content;
    
{ /*uss_procs_EchoToFile*/
    
    int m, o;
    struct stat stbuf;
    
    /*
     * Don't do anything if something has already gone wrong.
     */
    if (uss_syntax_err)
	return(1);
    
    if (uss_verbose)
	printf("Echoing to '%s'\n", a_path);
    
    /*
     * If we've not been given permission to overwrite things, make sure
     * the target doesn't exist before doing anything.
     */
    if (!uss_OverwriteThisOne) {
	strcpy(temp, a_path);
	if (!stat(temp, &stbuf)) {
	    if (uss_verbose)
		printf("\t[Entry exists, NOT overwriting it]\n");
	    return(0);
	}
    }
    
    sscanf(a_mode, "%o", &m);
    o = uss_procs_GetOwner(a_owner);
    
    if (!uss_DryRun) {
	if (Echo(a_content, a_path, m)){
	    uss_procs_PrintErr(line,
			       "Failed to echo string '%s' to file '%s'\n",
			       a_content, a_path);
	    return(1);
	}
    } /*Not a dry run*/
    else {
	fprintf(stderr, "\t[Dry run: Echoing '%s' into file '%s']\n",
		a_content, a_path);
    } /*Dry run*/
    
    if (!uss_DryRun) {
	if (chown(a_path, o, -1)){
	    uss_procs_PrintErr(line,
			       "Can't chown() file '%s' to be owned by '%s' (uid %d): %s\n",
			       a_path, a_owner, o, sys_errlist[errno]);
	    return(1);
	}
    } /*Not a dry run*/
    else {
	fprintf(stderr,
		"\t[Dry run: chown() file '%s' to be owned by user]\n",
		a_path);
    } /*Dry run*/
    
    /*
     * Return the happy news.
     */
    return(0);
    
} /*uss_procs_EchoToFile*/


/*-----------------------------------------------------------------------
 * EXPORTED uss_procs_Exec
 *
 * Environment:
 *	Called from the code generated by the uss grammar, as well as
 *	from uss.c itself.
 *
 * Side Effects:
 *	As advertised.
 *------------------------------------------------------------------------*/

afs_int32 uss_procs_Exec(a_command)
    char *a_command;

{ /*uss_procs_Exec*/

    if (uss_verbose)
      printf("Running '%s'\n", a_command);

    if (!uss_DryRun) {
      if (system(a_command)) {
	uss_procs_PrintErr(line,
			   "Failed to run the '%s' command: %s\n",
			   a_command, sys_errlist[errno]);
	return(1);
      }
    } /*Not a dry run*/
    else {
      fprintf(stderr, "\t[Dry run: executing '%s']\n", a_command);
    } /*Dry run*/

    /*
     * Return the happy news.
     */
    return(0);

} /*uss_procs_Exec*/


/*-----------------------------------------------------------------------
 * EXPORTED uss_procs_SetLink
 *
 * Environment:
 *	Called from the code generated by the uss grammar.
 *
 * Side Effects:
 *	As advertised.
 *------------------------------------------------------------------------*/

afs_int32 uss_procs_SetLink(a_path1, a_path2, a_type)
    char *a_path1;
    char *a_path2;
    char a_type;

{ /*uss_procs_SetLink*/

    struct stat stbuf;

    if (uss_verbose)
      printf("Setting link '%s' to '%s'\n", a_path1, a_path2);

    /*
     * If we've not been given permission to overwrite things, make sure
     * the target doesn't exist before doing anything.
     */
    if (!uss_OverwriteThisOne) {
      strcpy(temp, a_path2);
      if (!stat(temp, &stbuf)) {
	  if (uss_verbose)
	      printf("\t[Entry exists, NOT overwriting it]\n");
	  return(0);
      }
    }

    if (a_type == 's') {
      /*
       * Symbolic link.
       */
      if (!uss_DryRun) {
	if (symlink(a_path1, a_path2)) {
	  uss_procs_PrintErr(line,
			     "Failed to make symlink '%s' to '%s': %s\n",
			     a_path1, a_path2, sys_errlist[errno]);
	  return(1);
	}
      } /*Dry run*/
      else {
	fprintf(stderr, "\t[Dry run: Making symlink '%s' to '%s']\n",
		a_path1, a_path2);
      } /*Not a dry run*/
    } /*Symbolic link*/
    else {
      /*
       * Hard link.
       */
      if (!uss_DryRun) {
	if (link(a_path1, a_path2)) {
	  uss_procs_PrintErr(line,
			     "Failed to make hard link '%s' to '%s': %s\n",
			     a_path1, a_path2, sys_errlist[errno]);
	  return(1);
	}
      } /*Dry run*/
      else {
	fprintf(stderr, "\t[Dry run: Making hard link '%s' to '%s']\n",
		a_path1, a_path2);
      } /*Not a dry run*/
    } /*Hard link*/

    /*
      * Return the happy news.
      */
    return(0);

} /*uss_procs_SetLink*/


/*-----------------------------------------------------------------------
 * EXPORTED uss_procs_GetOwner
 *
 * Environment:
 *	Nothing interesting.
 *
 * Side Effects:
 *	As advertised.
 *------------------------------------------------------------------------*/

int uss_procs_GetOwner(a_ownerStr)
    char *a_ownerStr;

{ /*uss_procs_GetOwner*/

    struct passwd *pw;	/*Ptr to password file entry*/
    int ownerID;	/*Numerical owner*/

    ownerID = atoi(a_ownerStr);
    if ((ownerID == 0) && (a_ownerStr[0] != '0')) {
	/*
	 * The owner is not in numerical format
	 */
	if ((pw = getpwnam(a_ownerStr)) == NULL) {
	    uss_procs_PrintErr(line,
			       "Owner '%s' is an unknown user\n",
			       a_ownerStr);
	    return(1);
	}
	return(pw->pw_uid);
    }
    else
	return(ownerID);

} /*uss_procs_GetOwner*/

/*-----------------------------------------------------------------------
 * static Copy
 *
 * Description:
 *	 Copies the "from" file to the "to" file and sets the mode. 
 *
 * Arguments:
 *	a_from : File to copy from.
 *	a_to   : File to copy to.
 *	a_mode : New Unix mode to set.
 *
 * Returns:
 *	0 if everything went well,
 *	if there was a problem.
 *
 * Environment:
 *	Nothing interesting.
 *
 * Side Effects:
 *	As advertised.
 *------------------------------------------------------------------------*/

static int Copy(a_from, a_to, a_mode)
    char *a_from;
    char *a_to;
    int a_mode;

{ /*Copy*/

    register int fd1, fd2;
    char buf[BUFSIZ];
    int cnt, rc;

    umask(0);
    fd1 = open(a_to, O_EXCL | O_CREAT | O_WRONLY, a_mode);
    if (fd1 < 0) {
	if (errno == EEXIST) {
	    /*
	     * The file exists.  Since we can only be called when overwriting
	     * has been enabled, we back off and open the file normally (not
	     * supplying O_EXCL), then continue normally.
	     */
	    fd1 = open(a_to, O_CREAT | O_WRONLY, a_mode);
	    if (fd1 < 0) {
		uss_procs_PrintErr(line,
				   "%s: Failed to open '%s' for overwrite: %s.\n",
				   uss_whoami, a_to, sys_errlist[errno]);
		return(1);
	    }
	} else {
	    uss_procs_PrintErr(line, "%s: Failed to open '%s': %s.\n",
			       uss_whoami, a_to, sys_errlist[errno]);
	    return(1);
	}
    }

    if ((fd2 = open(a_from, O_RDONLY, 0)) < 0) {
	uss_procs_PrintErr(line, "%s: Error reading '%s': %s\n",
			   uss_whoami, a_from, sys_errlist[errno]);
	close(fd1);
	return(1);
    }
    while ((cnt = read(fd2, buf, BUFSIZ)) == BUFSIZ)
	write(fd1, buf, cnt);

    write(fd1, buf, cnt);
    rc = close(fd1);
    if (rc) {
	uss_procs_PrintErr(line,
			   "Failed to close '%s' %s\n",
			   a_to, sys_errlist[errno]);
	return(1);
    }
    if(rc = close(fd2))
	uss_procs_PrintErr(line, "Warning: Failed to close '%s': %s\n",
			   a_from, sys_errlist[errno]);
    return(0);

} /*Copy*/

/*-----------------------------------------------------------------------
 * static Echo
 *
 * Description:
 *	 Writes a string into a file and sets its mode.
 *
 * Arguments:
 *	a_s    : String to write.
 *	a_f    : Filename to write to.
 *	a_mode : New Unix mode to set.
 *
 * Returns:
 *	0 if everything went well,
 *	if there was a problem.
 *
 * Environment:
 *	Nothing interesting.
 *
 * Side Effects:
 *	As advertised.
 *------------------------------------------------------------------------*/

static int Echo(a_s, a_f, a_mode)
    char *a_s;
    char *a_f;
    int a_mode;

{ /*Echo*/

    register int fd;

    umask(0);
    fd = open(a_f, O_EXCL | O_CREAT | O_WRONLY, a_mode);
    if (fd < 0) {
	if (errno == EEXIST) {
	    /*
	     * The file exists.  Since we can only be called when
	     * overwriting has been enabled, we back off and open the
	     * file normally (not supplying the O_EXCL), then continue
	     * normally.  Notice that we must truncate the file, since
	     * if the original file is longer than the stuff we're about
	     * to put in, all the old data past the current write will
	     * still be there.
	     */
	    fd = open(a_f, O_TRUNC | O_WRONLY, a_mode);
	    if (fd < 0) {
		uss_procs_PrintErr(line,
				   "%s: Failed to open '%s' for overwrite: %s.\n",
				   uss_whoami, a_f, sys_errlist[errno]);
		return(1);
	    }
	} else {
	    uss_procs_PrintErr(line, "%s: Failed to open '%s': %s. \n",
			       uss_whoami, a_f, sys_errlist[errno]);
	    return(1);
	}
    }
    write(fd, a_s, strlen(a_s));
    write(fd, "\n", 1);
    if (close(fd)){
	uss_procs_PrintErr(line, "Failed to close '%s': %s\n",
			   a_f, sys_errlist[errno]);
	return(1);
    }
    return(0);

} /*Echo*/

/*-----------------------------------------------------------------------
 * static uss_procs_PickADir
 *
 * Description:
 *	Tries to replace $AUTO by a subdir.  Subdirs are given by means
 *	of "G" in the configuration file.  Each of the directories is
 *	examined, and the one with the inimum number of entries is
 *	picked.
 *
 * Arguments:
 *	a_path : ???
 *	a_cp   : ???
 *
 * Returns:
 *	0 if everything went well,
 *	-1 if there was a problem.
 *
 * Environment:
 *	Called with THREE parameters from lex.c!
 *
 * Side Effects:
 *	As advertised.
 *------------------------------------------------------------------------*/

afs_int32 uss_procs_PickADir(path, cp)
    char *path;
    char *cp;

{ /*uss_procs_PickADir*/

    char cd[300]; /*Current  directory for search*/

    int i, count, MinIndex, mina = 10000;
    struct dirent *dp;
    DIR *dirp;
    char dirname[300];

    if (uss_NumGroups == 0){
	fprintf(stderr,"%s: No choice yet given to replace $AUTO\n",
		uss_whoami);
	fprintf(stderr,"%s: Use the G command before $AUTO in config file\n",
		uss_whoami);
	return(-1);
    }

    if (uss_Auto[0] != '\0')
	return(0); /* we have already done this for this user */

    if (*(cp-1) == '/') { /*it's ..../$AUTO*/
	for (i = 0; &path[i] != cp; i++)
	    cd[i] = path[i];
	cd[i] = '\0';
    }
    else {
	if (path != cp){ 
	    fprintf(stderr,"%s: $AUTO must be used to replace the whole path or the whole name of a subdirectory. Found: %s$AUTO\n", uss_whoami, path);
	    return(-1);
	}
	cd[0] = '/'; cd[1] = '\0';
    }

    /* 
     * We now have the current dir (cd).  Search all of the given
     * subdirs (by G in template), count the number of entries in
     * each and pick the minimum.
     */
    for (i=0; i < uss_NumGroups; i++) {
	sprintf(dirname, "%s/%s", cd, uss_DirPool[i]);
	if ((dirp = opendir(dirname)) == NULL) {
	    if (errno != ENOTDIR)
		fprintf(stderr,
			"%s: Warning: Can't open dir '%s' (errno=%d). Skipped.\n",
			uss_whoami, dirname, errno);
	    continue;	/*Skip and continue anyway*/
	}
	count = 0;
	for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp))
	    if (dp->d_name[0] != '.')
		count++; /* forget about files starting with .*/
#ifdef USS_PROCS_DB
	printf("debug: Dir '%s' has %d entries\n", dirname, count);
#endif /* USS_PROCS_DB */
	if (count < mina) {
	    mina = count;
	    MinIndex = i;
	}
	closedir(dirp);
    }
    if (mina == 10000) { /* We found nothing */
	fprintf(stderr,"%s: Warning: No valid choice to replace $AUTO\n",
		uss_whoami);
	uss_Auto[0]= '\0';
    }
    else {
	strcpy(uss_Auto, uss_DirPool[MinIndex]);
	if (uss_verbose)
	    printf("Picking dir w/minimum number of entries: '%s'\n",
		   uss_Auto);
    }
    return(0);

} /*uss_procs_PickADir*/

/*-----------------------------------------------------------------------
 * EXPORTED uss_procs_AddToDirPool
 *
 * Environment:
 *	Called from the code generated by the uss grammar.
 *
 * Side Effects:
 *	As advertised.
 *------------------------------------------------------------------------*/

int uss_procs_AddToDirPool(a_dirToAdd)
    char *a_dirToAdd;
{
    if (uss_NumGroups > 99) {
       return(-1);
    }
    strcpy(uss_DirPool[uss_NumGroups++], a_dirToAdd);
    return 0;
}

/*-----------------------------------------------------------------------
 * EXPORTED uss_procs_FindAndOpen
 *
 * Environment:
 *	Nothing interesting.
 *
 * Side Effects:
 *	As advertised.
 *------------------------------------------------------------------------*/

FILE *uss_procs_FindAndOpen(a_fileToOpen)
    char *a_fileToOpen;

{ /*uss_procs_FindAndOpen*/

#define NUM_TPL_PATHS 3

    FILE *rv;				 /*Template file descriptor*/
    int i;				 /*Loop counter*/
    char tmp_str[uss_MAX_SIZE];		 /*Tmp string*/
    static char
      TemplatePath[NUM_TPL_PATHS][1024]; /*Template directories*/
    int cant_read;			 /*Can't read the file?*/

    /*
     * If a full pathname was given, just take it as is.
     */
    if (index(a_fileToOpen, '/')) {
	strcpy(tmp_str, a_fileToOpen);
	rv = fopen(a_fileToOpen, "r");
    }
    else {
	/*
	 * A relative pathname was given.  Try to find the file in each of
	 * the default template directories.
	 */
	cant_read = 0;
	
	sprintf(TemplatePath[0], "%s", ".");
	sprintf(TemplatePath[1], "/afs/%s/common/uss", uss_Cell);
	sprintf(TemplatePath[2], "%s", "/etc");
	
	for (i = 0; i < NUM_TPL_PATHS; i++) {
	    sprintf(tmp_str, "%s/%s", TemplatePath[i], a_fileToOpen);
	    if ((rv = fopen(tmp_str, "r")) != NULL)
		break;

	    /*
	     * If the file was there but couldn't be opened, we have
	     * to report this.
	     */
	    if (errno != ENOENT) {
		cant_read = 1;
		break;
	    }
	} /*Look in template directories*/
	
	/*
	 * If we found and opened the file, we're happy.  Otherwise,
	 * print out what went wrong.
	 */
	if (rv != NULL) {
	    if (uss_verbose)
		fprintf(stderr, "Using template '%s'\n", tmp_str);
	} /*Got it*/
	else {
	    /*
	     * Check to see if we specifically found the file but
	     * couldn't read it.
	     */
	    if (cant_read)
		fprintf(stderr,
			"%s: Can't open template '%s': %s\n",
			uss_whoami, tmp_str, sys_errlist[errno]);
	    else {
		fprintf(stderr,
			"%s: Can't find template '%s' in searchlist",
			uss_whoami, a_fileToOpen);
		for (i = 0; i < NUM_TPL_PATHS; i++)
		    fprintf(stderr, " '%s'", TemplatePath[i]);
		fprintf(stderr, "\n");
	    } /*Can't find template*/
	} /*Didn't get it*/
    } /*Relative pathname given*/
    
    /*
     * Whatever happened, return what we got.
     */
    return(rv);

} /*uss_procs_FindAndOpen*/


/*-----------------------------------------------------------------------
 * EXPORTED uss_procs_PrintErr
 *
 * Environment:
 *	Nothing interesting.
 *
 * Side Effects:
 *	As advertised.
 *------------------------------------------------------------------------*/

void uss_procs_PrintErr(a_lineNum, a_fmt, a_1, a_2, a_3, a_4, a_5)
    int a_lineNum;
    char *a_fmt;
    char *a_1;
    char *a_2;
    char *a_3;
    char *a_4;
    char *a_5;

{ /*uss_procs_PrintErr*/

    uss_syntax_err++;
    fprintf(stderr,"%s: Template file, line %d: ", uss_whoami, a_lineNum);
    fprintf(stderr, a_fmt, a_1, a_2, a_3, a_4, a_5);

} /*uss_procs_PrintErr*/
