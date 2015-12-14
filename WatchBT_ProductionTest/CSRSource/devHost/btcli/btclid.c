/*
 * MODIFICATION HISTORY
 *	#9     13:oct:03 pws    B-1219: Tweak compilation for Linux.
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#ifndef _WIN32_WCE
#include <errno.h>
#endif
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/fcntl.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#ifndef _WIN32_WCE
#include <sys/errno.h>
#endif
#include <netinet/in.h>
#include <signal.h>

/*
It is possible to ''shell'' commands with btclid.
*/

/* These are the defaults */
#define DEFAULT_PORT  2811
#ifdef linux
#define DEFAULT_EXE   "btcli"
#else
#define DEFAULT_EXE   "./btcli.exe"
#endif

/*
The 'command' class is used to construct the characters received from the
remote machine into commands for btcli.  we buffer until we have a complete
line, and then send it to btcli.  This is done so that we can detect the
'die' command.
*/
typedef struct command_s command;

struct command_s
{
    /* The buffer of input from this stream */
    char	buf[1024];
    int		ptr;
    /* The handle that we read from (and write to, if this is not STDIN_FILENO) */
    int		handle;
    /* A LinkedList of all of these */
    command*	next;
};

typedef struct btcli_s btcli;

struct btcli_s
{
    int pid;
    int read;
    int write;
};


/* file local procedures */
static void command_write(command* cmd, int fn);
static void command_add_frag(command* cmd, int file, const char* buf, int len);
static void do_server(int port);
static void usage(void);
static void spawn_child(int ptc[2], int pfc[2]);
static void close_btcli(btcli* child);
static int start_btcli(btcli* child);
static void do_system(const char* cmd);
static void alarm_signal_handler(int p);

/* The name of this program */
static char* progname;

/* What mode are we in.  Look at ''do_server'' for more info. */
static int mode;

#define MODE_RUN		0
#define MODE_DIE		1
#define MODE_START		2
#define MODE_STOP		3
#define MODE_RESTART		4
#define MODE_SHELL		5
#define MODE_DEVINFO		6

static const char* const mode_str[] =
{
	"RUN",
	"DIE",
	"START",
	"STOP",
	"RESTART",
	"SHELL",
	"DEVINFO",
};

/* This buffer stores the shell command */
static char shell[2048];

/* This is non-zero if we should output the btcli session to the terminal */
/* -v ->  print_output |=  1 */
/* -q ->  print_output &= ~1 */
/* -V ->  print_output |=  2 */
/* if(print_output & 1) { Print standard trace output } */
/* if(print_output & 2) { Print extra debug trace output } */
static int print_output = 1;

/* These are the arguments to the child process */
/* either a NULL poointer or a malloc'd bit of memory */
static char* args[64];

/* MAIN */
int main (int argc, char *argv[])
{
    int port;
    int argn;
    int n;

    progname = argv[0];

    /*
     * Begin by reading the command line arguments
     */
    port = DEFAULT_PORT;
    args[0] = DEFAULT_EXE;
    argn = 1;

    for(n=1; n<argc; n++)
    {
	if (argv[n][0] == '-')
	{
	    switch (argv[n][1])
	    {
	    case 'p':
		if(n+1 >= argc)
		    usage();
	        port = atoi (argv[++n]);
		break;

	    case 'b':
		if(n+1 >= argc)
		    usage();
		args[0] = argv[++n];
		break;

	    case 'x':
	        for(n++; n<argc; n++)
		{
		    args[argn] = (char *)malloc(strlen(argv[n])+1);
		    strcpy(args[argn++], argv[n]);
		}
		break;

	    case 'v':
		print_output |= 1;
		break;

	    case 'q':
		print_output &= ~1;
		break;

	    case 'V':
		print_output |= 2;
		break;

	    default:
		usage();
		break;
	    }
	}
	else
	{
	    usage();
	}
    }
    args[argn] = NULL;

    /* Setup the signal handler, so that we dont crash on broken PIPE! */
#ifdef SIGPIPE
    signal (SIGPIPE, SIG_IGN);
#endif

#ifdef SIGALRM
    if((print_output & 2) != 0)
        printf("Setting up a Sig Alarm handler\n");

    /* Set up the handler */
    signal (SIGALRM, alarm_signal_handler);
#endif

    /* Everything of interest happens in ''do_server'' */
    do_server(port);

    printf ("Done\n");

    exit(0);
}

static void set_non_block(int fd)
{
    int flg;

    flg = fcntl(fd, F_GETFL);
/*  if ((print_output & 2) != 0)
        printf("Socket Flags = 0x%x\n", flg);*/

    fcntl(fd, F_SETFL, flg|O_NONBLOCK);

    flg = fcntl(fd, F_GETFL);
/*  if ((print_output & 2) != 0)
	printf("Socket Flags = 0x%x\n", flg);*/
}

command cmd_stdin;
command* cmd_head;

/*
 * select reckons that we have someone wanting to connect to us
 * - let them.
 */
static void add_client(int server)
{
    int saclientlen, client;
    struct sockaddr_in saclient;
    command* cmd;

    saclientlen = sizeof (saclient);
    client = accept (server, (struct sockaddr *) &saclient, &saclientlen);

    /* The accept failed, either an error, or there is no one there */
    if (client == -1)
    {
	if(errno != EWOULDBLOCK)
	{
	    perror (progname);
	    exit (EXIT_FAILURE);
	}

	return;
    }

    if((print_output & 2) != 0)
	printf ("Accepted\n");

    set_non_block(client);

    write (client, "hello\r\n", 7);

    cmd = malloc(sizeof(command));
    cmd->ptr = 0;
    cmd->handle = client;
    cmd->next = cmd_head;
    cmd_head = cmd;
}

/*
 * Write to all of the clients (apart from the console)
 */
static void write_to_clients(const char* data, int len)
{
    const command *cp;

    for (cp = cmd_head; cp != NULL; cp = cp->next)
    {
	if (cp->handle >= 0 && cp->handle != STDIN_FILENO)
	{
	    write (cp->handle, data, len);
	}
    }
}

/*
 * remove any client from the linked list that has a -1 handle
 */
static void remove_clients(void)
{
    command** cp, *tp;

    for (cp = &cmd_head; (*cp) != NULL; cp = &((*cp)->next))
    {
	if ((*cp)->handle == -1)
	{
	    tp = (*cp);
	    *cp = tp->next;
	    free(tp);
	}
    }
}

static void do_server(int port)
{
    fd_set readfds;
    int server;
    struct sockaddr_in saserver;
    command* cp;
    int maxd, n;
    int flg;
    btcli child;
    char buf[2048];

    /* Create a socket, and start listening. */

    server = socket (AF_INET, SOCK_STREAM, 0);
    if (server == -1)
    {
	printf("socket creation failed!\n");
	perror (progname);
	exit (EXIT_FAILURE);
    }

    memset (&saserver, 0, sizeof (saserver));
    saserver.sin_family = AF_INET;
    saserver.sin_addr.s_addr = INADDR_ANY;
    saserver.sin_port = htons (port);

    if (bind (server, (struct sockaddr *) &saserver, sizeof (saserver)))
    {
	printf("socket bind failed!\n");
	perror (progname);
	exit (EXIT_FAILURE);
    }

    if (listen (server, 1))
    {
	printf("listen failed!\n");
	perror (progname);
	exit (EXIT_FAILURE);
    }

    /*
     * This loop below is controlled by ''mode''.  It takes on a
     * number of different values, at different times to mean
     * different things.
     *  RUN	- We might have a BTCLI running, but should
     *            keep pumping data anyway.
     *  DIE	- We should exit as soon as possible.
     *            We should kill btcli, and btclid.
     *            All connected users are dropped
     *  START   - We should start a copy of btcli
     *  STOP    - We should kill the current copy of BTCLI.
     *  RESTART - We should kill btcli, and then start it again
     *  SHELL   - We should stop btcli, run a command and restart btcli
     *  DEVINFO - Output the parameters to btcli
     * 
     */
    mode = MODE_RUN;

    /*
    Change to having one big loop (not two nested)
    Select on the following things:
    ''accept'' on the socket server, so that new people can telnet in.
    ''read'' from each of the sockets for commands.
    ''read'' from stdin, to check that no one is typing on the screen.
    ''read'' from the btcli (if it exists), and buffer the data
    */

    child.pid = 0;
    child.read = child.write = -1;
    start_btcli(&child);

    if((print_output & 2) != 0)
	printf("Starting 'do_server' loop\n");

    set_non_block(server);

    set_non_block(STDIN_FILENO);

    setbuf(stdin, NULL);

    flg = fcntl(STDIN_FILENO, F_GETFL);
/*  if((print_output & 2) != 0)
	printf("STDIN Flags = 0x%x\n", flg);*/

    fcntl(STDIN_FILENO, F_SETFL, flg|O_NDELAY);

    flg = fcntl(STDIN_FILENO, F_GETFL);
/*  if((print_output & 2) != 0)
	printf("STDIN Flags = 0x%x\n", flg);*/

    cmd_stdin.ptr = 0;
    cmd_stdin.handle = STDIN_FILENO;
    cmd_stdin.next = NULL;
    cmd_head = &cmd_stdin;

    do
    {
	FD_ZERO (&readfds);

	FD_SET (server, &readfds);
	maxd = server;

	/* Read from btcli */
	if(child.read != -1)
	{
	    FD_SET (child.read, &readfds);
	    if(child.read > maxd)
		maxd = child.read;
	}

	/* Read from the ''command'' list (stdin and socket connections) */
	for(cp = cmd_head; cp != NULL; cp = cp->next)
	{
	    if (cp->handle >= 0)
	    {
		FD_SET (cp->handle, &readfds);
		if(cp->handle > maxd)
		    maxd = cp->handle;
	    }
	}

	/* Perform the select */
	/* printf ("[%d\n", count_clients()); */
  	n = select (maxd + 1, &readfds, NULL, NULL, NULL);
	/* printf ("]\n"); */

	if (n == -1)
	{
	    printf("select -> got -1\n");
	    perror (progname);
	    continue;
	}

	/* Someone want to connect to the socket */
	if (FD_ISSET (server, &readfds))
	{
	    /* Add to list of clients */
	    add_client(server);
	}

	/* read the output from btcli */
	if (child.read != -1 && FD_ISSET (child.read, &readfds))
	{
	    n = read (child.read, buf, sizeof (buf) - 1);
	    if(n > 0)
	    {
		 /*
		  * (Has anyone noticed this is C, not C++?)
		  * If there isn't a newline at the end, add one.
		  * This is bad if we got a partial line; we
		  * could do tricks with select to help here.
		  */
		 if (n < sizeof(buf) - 1 && buf[n-1] != '\n')
		      buf[n++] = '\n';
		 buf[n] = '\0';

		/* Output the data from btcli to the screen */
		if ((print_output & 1) != 0)
		{
		    printf ("%s\0", buf);
		}

		/* output the data to the sockets */
		write_to_clients(buf, n);
	    }
	    else
	    {
		/* we failed to read anything from btcli (but select said that we could) */
		if (n < 0)
		{
		    printf("read (from child) -> got -1\n");
		    perror (progname);
		}
/*		mode = MODE_RESTART;*/
		/* Wait for explicit instruction to restart */
		mode = MODE_STOP;
	    }
	}

	/* This is where we handle data coming in from the socket
	   connection. */
	for (cp = cmd_head; cp != NULL; cp = cp->next)
	{
	    if (cp->handle >= 0 && FD_ISSET (cp->handle, &readfds))
	    {
#ifdef SIGALRM
		if(cp->handle == STDIN_FILENO)
		    alarm (1);
#endif
		n = read (cp->handle, buf, sizeof (buf) - 1);
#ifdef SIGALRM
		if(cp->handle == STDIN_FILENO)
		    alarm (0);
#endif
		if (n > 0)
		{
		    buf[n] = 0;
		    command_add_frag(cp, child.write, buf, n);
		}
		else
		{
		    if(n < 0)
		    {
			if(errno != EINTR)
			{
			    printf("read (from client) -> got -1\n");
			    perror (progname);
			}
		    }
		    else
		    {
			if((print_output & 2) != 0)
			    printf ("EOF on client\n");
		    }
		    if(cp->handle != STDIN_FILENO)
		    {
			close(cp->handle);	/* Close the socket handle (needed!?) */
			cp->handle = -1;	/* Mark the client for removal */
		    }
		}
	    }
	}
	remove_clients();

	/* Now we do different thing if we have been given a special btclid command */

	switch (mode)
	{
	case MODE_RESTART:

	    if((print_output & 2) != 0)
		printf("ReStarting\n");

	    close_btcli(&child);
	    start_btcli(&child);

	    mode = MODE_RUN;

	    break;

	case MODE_SHELL:

	    close_btcli(&child);

	    if((print_output & 2) != 0)
		printf("Running shell command ''%s''\n", shell);

	    do_system(shell);

	    memset(shell, 0, sizeof(shell));

	    start_btcli(&child);

	    mode = MODE_RUN;

	    break;

	case MODE_DEVINFO:

	    strcpy(buf, "ARGS:");
	    for(n=1; (n<64)&&(args[n]!=NULL); n++)
	    {
		strcat(buf, " ");
		strcat(buf, args[n]);
	    }
	    strcat(buf, "\r\n");

	    write_to_clients(buf, strlen(buf));

	    if((print_output & 1) != 0)
		printf ("%s", buf);

	    mode = MODE_RUN;

	    break;

	case MODE_STOP:

	    if(child.pid != 0)
		close_btcli(&child);

	    mode = MODE_RUN;

	    break;

	case MODE_START:

	    if(child.pid == 0)
		start_btcli(&child);

	    mode = MODE_RUN;

	    break;
	}
    }
    while (mode != MODE_DIE);

    close_btcli(&child);

    close (server);
}

#define PIPE_READEND 0
#define PIPE_WRITEEND 1

static int start_btcli(btcli* child)
{
    /* The handles of the pipes */
    int pipetochild[2], pipefromchild[2];
    int pid;

    if((print_output & 2) != 0)
	printf("Starting BTCLI\n");

    /* Create the pipes */
    if (pipe (pipetochild))
    {
	printf("pipe (to child) -> got !0\n");
	perror (progname);
	exit (EXIT_FAILURE);
    }

    if (pipe (pipefromchild))
    {
	printf("pipe (from child) -> got !0\n");
	perror (progname);
	exit (EXIT_FAILURE);
    }

    /* FORK so that the child can execute (inhereting files) */
    pid = fork ();
    switch (pid)
    {
    case -1:
	printf("fork -> got -1\n");
	perror (progname);
	exit (EXIT_FAILURE);
	break;

    case 0:
        signal (SIGPIPE, SIG_IGN);

	spawn_child(pipetochild, pipefromchild);
	_exit (EXIT_FAILURE);
	break; 

    default:
	if((print_output & 2) != 0)
	    printf ("Parent (PID %i); child PID is %i\n", getpid (), pid);
	close (pipefromchild[PIPE_WRITEEND]);
	close (pipetochild[PIPE_READEND]);
    }

    child->read  = pipefromchild[PIPE_READEND];
    child->write = pipetochild[PIPE_WRITEEND];
    child->pid   = pid;

    return pid;
}

static volatile int alarm_hit = 0;

static void close_btcli(btcli* child)
{
    int stat;

    /* If we have no pid, then it is dead already */
    if(child->pid == 0)
    {
        if((print_output & 2) != 0)
            printf("BTCLI dead already!\n");
        return;
    }

    if((print_output & 2) != 0)
	printf("Closing BTCLI\n");

    /* If btcli is still reading from stdin, these closes will cause
     * it to end
     */
    if(child->read != -1)
    {
        close(child->read);
        child->read = -1;
    }
    if(child->write != -1)
    {
        close(child->write);
        child->write = -1;
    }

#ifdef SIGALRM
    /* give btcli 4 seconds to die */
    alarm (4);
    alarm_hit = 0;
#endif

    if((print_output & 2) != 0)
	printf("Waiting for btcli to die\n");

    /* Wait for btcli to die, or the timer to expire */
    while (wait(&stat) != child->pid && errno != EINTR && alarm_hit == 0)
	  /* EMPTY */;

    /* Send a kill.  This will fail if there is no btcli */
    if (kill (child->pid, SIGKILL) != -1)
	printf("Killed btclid the hard way.\n");

#ifdef SIGALRM
    /* remove the alarm */
    alarm (0);

    /* Print out the number of times that the alarm was hit */
    if((print_output & 2) != 0)
	if(alarm_hit != 0)
	    printf("Alarm hit = %d\n", alarm_hit);
#endif

    /* Remeber that we have killed btcli */
    child->pid = 0;
    child->read = -1;
    child->write = -1;

    if((print_output & 2) != 0)
	printf("btcli is dead\n");
}

static void alarm_signal_handler(int p)
{
    alarm_hit++;
}

static void usage(void)
{
    fprintf (stdout, "%s:\n", progname);
    fprintf (stdout, "\t[-b path]\t\tPath to the BTCLI program (default %s)\n", DEFAULT_EXE);
    fprintf (stdout, "\t[-p num]\t\tThe port number to use (default %d)\n", DEFAULT_PORT);
    fprintf (stdout, "\t[-v]\t\t\tPrint the output of BTCLI (on by default)\n");
    fprintf (stdout, "\t[-q]\t\t\tDo not print the output of BTCLI\n");
    fprintf (stdout, "\t[-V]\t\t\tPrint extra debug info\n");
    fprintf (stdout, "\t[-x { .. args .. }]\tThe args to pass to BTCLI\n");
    fprintf (stdout, "\n");
    fprintf (stdout, "btclid is a btcli daemon.  It is used to give remote access to an instance of\n");
    fprintf (stdout, "btcli.  It runs under Linux or WinNT/Cygwin.\n");
    exit (EXIT_FAILURE);
}

/*
The 'command' class is used to construct the characters received from the
remote machine into commands for btcli.  we buffer until we have a complete
line, and then send it to btcli.  This is done so that we can detect the
'die' command.
*/
static void command_write(command* cmd, int fn)
{
    if(cmd->ptr <= 0)
    {
	return;
    }
    if(cmd->ptr == 3 && memcmp(cmd->buf, "die", 3) == 0)
    {
	mode = MODE_DIE;
	memset(cmd->buf, 0, sizeof(cmd->buf));
	cmd->ptr = 0;
	return;
    }
    else if(cmd->ptr >= 4 && memcmp(cmd->buf, "quit", 4) == 0)
    {
	memset(cmd->buf, 0, sizeof(cmd->buf));
	cmd->ptr = 0;
	return;
    }
    else if(cmd->ptr == 4 && memcmp(cmd->buf, "stop", 4) == 0)
    {
	mode = MODE_STOP;
	memset(cmd->buf, 0, sizeof(cmd->buf));
	cmd->ptr = 0;
	return;
    }
    else if(cmd->ptr == 5 && memcmp(cmd->buf, "start", 5) == 0)
    {
	mode = MODE_START;
	memset(cmd->buf, 0, sizeof(cmd->buf));
	cmd->ptr = 0;
	return;
    }
    else if(cmd->ptr == 7 && memcmp(cmd->buf, "restart", 7) == 0)
    {
	mode = MODE_RESTART;
	memset(cmd->buf, 0, sizeof(cmd->buf));
	cmd->ptr = 0;
	return;
    }
    else if(cmd->ptr > 6 && memcmp(cmd->buf, "shell ", 6) == 0)
    {
	memset(shell, 0, sizeof(shell));
	memcpy(shell, cmd->buf+6, cmd->ptr-6);
	mode = MODE_SHELL;
	memset(cmd->buf, 0, sizeof(cmd->buf));
	cmd->ptr = 0;
	return;
    }
    else if(cmd->ptr > 6 && ((memcmp(cmd->buf, "pathF ", 6) == 0) || (memcmp(cmd->buf, "pathS ", 6) == 0)))
    {
	int i;
	for(i=0; (i<64)&&(args[i]!=NULL); i++)
	{
	  /*
	   * Check we are looking at a path; avoid hijacking -FB, for example.
	   * Arbitrarily assume the path will be at lest four characters.
	   * The current code assumes the path is in the current word.
	   * This violates the usual assumptions for Unix options,
	   * but so does the whole rest of the shebang.
	   */
	    if(args[i][0] == '-' && args[i][1] == cmd->buf[4]
	       && strlen(args[i]+2) > 4)
		break;
	}
	if(i < 64)
	{
	    char* q;
	    char* p = malloc(cmd->ptr-6+3);
	    p[0] = '-';
	    p[1] = cmd->buf[4];
	    strcpy(p+2, cmd->buf+6);
	    if((q = strpbrk(p+2, " \t\n\r")) != NULL)
		*q = 0;
	    if(args[i] == NULL && i < 63)
		args[i+1] = NULL;
	    if(args[i] != NULL)
		free(args[i]);
	    args[i] = p;
	}
	memset(cmd->buf, 0, sizeof(cmd->buf));
	cmd->ptr = 0;
	return;
    }
    else if(cmd->ptr == 7 && memcmp(cmd->buf, "devinfo", 7) == 0)
    {
	mode = MODE_DEVINFO;
	memset(cmd->buf, 0, sizeof(cmd->buf));
	cmd->ptr = 0;
	return;
    }

    cmd->buf[cmd->ptr++] = '\n';
    cmd->buf[cmd->ptr] = 0;

    if(fn >= 0)
    {
	if (write (fn, cmd->buf, cmd->ptr) != cmd->ptr)
	{
	    if((print_output & 2) != 0)
		fprintf (stderr, "Failed to write %u bytes to child\n", cmd->ptr);
	}
    }

    if((print_output & 1) != 0)
	printf("%s", cmd->buf);

    memset(cmd->buf, 0, sizeof(cmd->buf));
    cmd->ptr = 0;
    return;
}

/*
 * Add ''len'' bytes from ''buf'' to the command buffer for the socket ''cmd''
 * If we have a full line, then send the line to ''file''.
 */
static void command_add_frag(command* cmd, int file, const char* buf, int len)
{
    int i;

    for(i=0; i<len; i++)
    {
	switch(buf[i])
	{
	case 0:
	    printf("read a '0' ?\n");
	case '\n':
	case '\r':
	    command_write(cmd, file);
	    break;

	default:
	    cmd->buf[cmd->ptr++] = buf[i];
	    if(cmd->ptr >= sizeof(cmd->buf)-1)
	    {
		printf("buf overflow!\n");
		command_write(cmd, file);
	    }
	    break;
	}
    }
}


/*
 */
static void spawn_child(int ptc[2], int pfc[2])
{
    /* Switch of buffering of STDOUT (hopefully) */
    fflush (stdout);
    if (setvbuf (stdout, NULL, _IONBF, BUFSIZ) != 0)
    {
	printf("setvbuf -> !0 (spawn_child)\n");
	perror (progname);
	_exit (EXIT_FAILURE);
    }

    if((print_output & 2) != 0)
	printf ("Child (PID %i); parent PID is %i\n", getpid (), getppid ());

    /* Setup the child's STDOUT */
    if (dup2 (pfc[PIPE_WRITEEND], STDOUT_FILENO) == -1)
    {
	printf("dup2 -> -1 (spawn_child - STDOUT)\n");
	perror (progname);
	_exit (EXIT_FAILURE);
    }
    close (pfc[PIPE_READEND]);
    close (pfc[PIPE_WRITEEND]);

    /* Setup the childs STDIN */
    if (dup2 (ptc[PIPE_READEND], STDIN_FILENO) == -1)
    {
	printf("dup2 -> -1 (spawn_child - STDIN)\n");
	perror (progname);
	_exit (EXIT_FAILURE);
    }
    close (ptc[PIPE_READEND]);
    close (ptc[PIPE_WRITEEND]);

    /* Run the child program */
    execvp (args[0], args);

    /* exiting this function is an error ! */
    perror(progname);
}

/*
 * Run a ''system'' command.  pipe the output to all of the clients.
 */
static void do_system(const char* cmd)
{
    /* The handles of the pipes */
    int pipetochild[2], pipefromchild[2];
    int pid;
    char buf[256];
    int n;

    if((print_output & 2) != 0)
	printf("Starting a ''system'' command\n");

    /* Create the pipes */
    if (pipe (pipetochild))
    {
	printf("pipe creation (do_system)\n");
	perror (progname);
	return;
	/*exit (EXIT_FAILURE);*/
    }

    if (pipe (pipefromchild))
    {
	printf("pipe creation (do_system 2)\n");
	perror (progname);
	return;
	/*exit (EXIT_FAILURE);*/
    }

    /* FORK so that the child can execute (inhereting files) */
    pid = fork ();
    switch (pid)
    {
    case -1:
	printf("fork -> -1 (do_system)\n");
	perror (progname);
	return;

    case 0:
        signal (SIGPIPE, SIG_IGN);

	/* Switch of buffering of STDOUT (hopefully) */
	fflush (stdout);
	if (setvbuf (stdout, NULL, _IONBF, BUFSIZ) != 0)
	{
	    perror (progname);
	    _exit (EXIT_FAILURE);
	}

        if((print_output & 2) != 0)
	    printf ("Child (PID %i); parent PID is %i\n", getpid (), getppid ());

	/* Setup the child's STDOUT */
	if (dup2 (pipefromchild[PIPE_WRITEEND], STDOUT_FILENO) == -1)
	{
	    perror (progname);
	    _exit (EXIT_FAILURE);
	}
	close (pipefromchild[PIPE_READEND]);
	close (pipefromchild[PIPE_WRITEEND]);

	/* Setup the childs STDIN */
	if (dup2 (pipetochild[PIPE_READEND], STDIN_FILENO) == -1)
	{
	    perror (progname);
	    _exit (EXIT_FAILURE);
	}
	close (pipetochild[PIPE_READEND]);
	close (pipetochild[PIPE_WRITEEND]);

	/* Run the child program */
	system(cmd);

	/* exiting this function is OK! */
	_exit (EXIT_SUCCESS);
	break; 

    default:
	/*printf ("Parent (PID %i); child PID is %i\n", getpid (), pid);*/
	;
    }

    close (pipefromchild[PIPE_WRITEEND]);
    close (pipetochild[PIPE_READEND]);

    if((print_output & 2) != 0)
	printf("Doing ''system'' command\n");

    do
    {
	n = read(pipefromchild[PIPE_READEND], buf, sizeof(buf));
	if(n > 0)
	{
	    buf[n] = 0;
	    write_to_clients(buf, n);
	    if((print_output & 1) != 0)
		printf("%s", buf);
	}
    }
    while(n > 0);

    if((print_output & 2) != 0)
	printf("''system'' command ended\n");

    close(pipefromchild[PIPE_READEND]);
    close(pipetochild[PIPE_WRITEEND]);
}
