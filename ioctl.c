#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/capability.h>
#include <unistd.h>
#include <argp.h>


#define DEFAULT_CAP cap_from_text("cap_setfcap=eip")

char** str_split(char* a_str, const char a_delim);

const char *argp_program_version =
  "ioctl 1.0";
const char *argp_program_bug_address =
  "<radni@radni.ir>";

/* Program documentation. */
static char doc[] =
	"IOCTL user space program, this utility will help user for requesting IOCTL command to a specefic path.\nbefor using set permission for this utility as follow: \nsudo setcap cap_setfcap=eip ./ioctl";
/* A description of the arguments we accept. */
static char args_doc[] = "COMMAND PATH";

/* The options we understand. */
static struct argp_option options[] = {
  {"output",   'o', "FILE", 0,
   "Output to FILE instead of standard output" },
  {"set-capability", 's', "CAP FORMAT", 0, "Befor sending IOCTL command the Capability will be set to this file. you can find the CAP FORMAT in capability man page."},
  { 0 }
};

/* Used by main to communicate with parse_opt. */
struct arguments
{
	char *args[2];                /* arg1 & arg2 */
	char *output_file;
	int output_flag;
	char *set_cap_buffer;
	int set_cap_flag;
};


/* Parse a single option. */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
	struct arguments *arguments = state->input;

	switch (key)
	{
		case 's':
			arguments->set_cap_flag = 1;
			arguments->set_cap_buffer = arg;
			break;
		case 'o':
			arguments->output_file = arg;
			arguments->output_flag = 1;
			break;

		case ARGP_KEY_ARG:
			if (state->arg_num >= 2)
				/* Too many arguments. */
				argp_usage (state);

			arguments->args[state->arg_num] = arg;

			break;

		case ARGP_KEY_END:
			if (state->arg_num < 2)
				/* Not enough arguments. */
				argp_usage (state);
			break;

		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

/* Our argp parser. */
static struct argp argp = { options, parse_opt, args_doc, doc };

int
main (int argc, char **argv)
{
	struct arguments arguments;

	/* Default values. */
	arguments.output_file = "-";
	arguments.set_cap_buffer = "-";

	arguments.set_cap_flag = 0;
	arguments.output_flag = 0;

	/* Parse our arguments; every option seen by parse_opt will
	 be reflected in arguments. */
	argp_parse (&argp, argc, argv, 0, 0, &arguments);
	

	int fd;

	if ( (fd=open(arguments.args[1], O_RDONLY)) <= 0 ) {
		perror("Could not open file.");
		exit(EXIT_FAILURE);
	}

	if ( arguments.set_cap_flag )
	{

		char buff[256];
		snprintf(buff, sizeof buff, "cap_setfcap,%s", arguments.set_cap_buffer);
		cap_t cap = cap_from_text(buff);
		if ( cap_set_file(*argv, cap) ){
			perror("Applying new Capability to this file faild.");
			exit(EXIT_FAILURE);
		}
		execv(*argv, (char *[]){ *argv, arguments.args[0], arguments.args[1], NULL });
	}

	long command = atol(arguments.args[0]);

	char output[300];

	if( ioctl(fd, command, output) < 0 ){

		perror("IOCTL request rejected.");
		goto revert;
		exit(EXIT_FAILURE);
	}

	if (arguments.output_flag)
	{
		FILE* file = fopen(arguments.output_file, "a+");
		fprintf(file, "%s", output);
		fclose(file);
	}
	else
		printf("%s\n", output);

	/*  Reverting File capabilities to default. */

revert:	
	if ( cap_set_file(*argv, DEFAULT_CAP) ) {

		perror("Reverting file capabilities to default faild.");
		exit(EXIT_FAILURE);
	}
	

	close(fd);
	return 0;
}
