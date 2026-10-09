/**
 * Tony Givargis
 * Copyright (C), 2023-2026
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * jitc.c
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dlfcn.h>
#include "system.h"
#include "jitc.h"

/**
 * Needs:
 *   fork()
 *   execv()
 *   waitpid()
 *   WIFEXITED()
 *   WEXITSTATUS()
 *   dlopen()
 *   dlclose()
 *   dlsym()
 */

struct jitc {
	void *handle;
};

int
jitc_compile(const char *input, const char *output)
{
	char *argv[8];
	pid_t pid;
	int status;

	if (!safe_strlen(input) || !safe_strlen(output)) {
		TRACE("invalid argument");
		return -1;
	}

	argv[0] = "gcc";
	argv[1] = "-O3";
	argv[2] = "-fpic";
	argv[3] = "-shared";
	argv[4] = "-o";
	argv[5] = (char *)output;
	argv[6] = (char *)input;
	argv[7] = NULL;

	if (0 > (pid = fork())) {
		TRACE("fork()");
		return -1;
	}
	if (0 == pid) {
		/* child: become the compiler */
		execv("/usr/bin/gcc", argv);
		TRACE("execv()");
		_exit(127);
	}

	/* parent: wait for the compiler to finish */
	while (0 > waitpid(pid, &status, 0)) {
		if (EINTR != errno) {
			TRACE("waitpid()");
			return -1;
		}
	}
	if (!WIFEXITED(status) || WEXITSTATUS(status)) {
		TRACE("compilation failed");
		return -1;
	}
	return 0;
}

struct jitc *
jitc_open(const char *pathname)
{
	struct jitc *jitc;
	const char *err;

	if (!safe_strlen(pathname)) {
		TRACE("invalid argument");
		return NULL;
	}
	if (!(jitc = malloc(sizeof (struct jitc)))) {
		TRACE("out of memory");
		return NULL;
	}
	if (!(jitc->handle = dlopen(pathname, RTLD_NOW | RTLD_LOCAL))) {
		err = dlerror(); /* TRACE evaluates its argument twice */
		TRACE(err);
		FREE(jitc);
		return NULL;
	}
	return jitc;
}

void
jitc_close(struct jitc *jitc)
{
	if (jitc) {
		if (jitc->handle) {
			dlclose(jitc->handle);
		}
		FREE(jitc);
	}
}

long
jitc_lookup(struct jitc *jitc, const char *symbol)
{
	const char *err;
	void *addr;

	if (!jitc || !jitc->handle || !safe_strlen(symbol)) {
		TRACE("invalid argument");
		return 0;
	}
	dlerror(); /* clear any stale error */
	if (!(addr = dlsym(jitc->handle, symbol))) {
		err = dlerror();
		TRACE(err);
		return 0;
	}
	return (long)addr;
}
