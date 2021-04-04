/* See LICENSE file for copyright and license details.
 *
 * quiz is a simple program that lets any user create
 * and/or take quizzes.
 * It features a quiz creator, and the core quiz program.
 *
 * Made by Salonia Matteo <saloniamatteo@pm.me>
 *
 * Exit values:
 * 1	-> Quiz: quiz.db file not found/Cannot read file
 * 2	-> Quiz-Creator: could not open file for writing
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#define BUFSIZE 70

#define __QUIZ_VER_MAJOR "1"
#define __QUIZ_VER_MINOR "1"
#define __QUIZ_VERSION __QUIZ_VER_MAJOR "." __QUIZ_VER_MINOR

/* Quiz structure */
typedef struct quizObj {
	int id;
	int points;
	char qst[BUFSIZE], ans[BUFSIZE];
} quizObj;

/* User structure */
typedef struct quizUsr {
	int score;
	char ans[BUFSIZE];
} quizUsr;

/* User score */
static int usr_score = 0;

/* Last question number */
static int last_qnum = 0;

/* Savefile */
static char savefile[50] = "";

/* Custom username for savefile */
static char username[20];

/* Reserve special commands? (exit, quit, stop) */
static int reserve_special = 0;

/* Quiz DB file */
FILE *dbfile;

/* Quiz Creator file */
FILE *crfile;

/* Save Score file */
FILE *save_file;

/* Function prototypes */
void quizCreator(char *);
void saveScore(void);
void sigHandler(int);

/* Create a new quiz database */
void
quizCreator(char *outfile)
{
	/* Check if file can be opened for writing */
	crfile = fopen(outfile, "a+w");

	if (crfile == NULL) {
		fprintf(stderr, "ERROR: Could not Open file \"%s\"!\n", outfile);
		exit(2);
	}

	/* Ask the user how many questions to enter */
	int qnum;

	for (;;) {
		printf("Choose how many questions you want to enter.\n> ");
		scanf("%d", &qnum);

		/* Check if number is valid */
		if (qnum <= 0)
			fprintf(stderr, "Please enter a valid number!\n");
		else
			break;
	}

	printf("OK. Using %d questions.\n", qnum);

	/* This loop will run until the number of questions is reached */
	for (int i = 0; i < qnum; i++) {
		/* Create new quiz object */
		quizObj quiz[BUFSIZE] = {0};

		/* Advance ID by 1, automatically */
		quiz->id = i + 1;
		printf("Question No.%d\n", quiz->id);

		/* Ask question */
		printf("Enter question.\n> ");
		/* Quick dirty fix: capture newline to let fgets() get user input */
		fgetc(stdin);
		fgets(quiz->qst, BUFSIZE, stdin);
		quiz->qst[strlen(quiz->qst)-1] = '\0';

		/* Ask answer */
		printf("Enter answer.\n> ");
		fgets(quiz->ans, BUFSIZE, stdin);
		quiz->ans[strlen(quiz->ans)-1] = '\0';

		/* Ask how many points to give to the user */
		printf("Enter how many points to give to the user.\n> ");
		scanf("%d", &quiz->points);

		/* Write values to outfile */
		fprintf(crfile, "ID: %d;; Question: %s;; Answer: %s;; Points: %d\n",
				quiz->id, quiz->qst, quiz->ans, quiz->points);
	}

	/* Close file */
	fclose(crfile);
	fprintf(stderr, "[Saved questions to file \"%s\"]\n", outfile);
	exit(0);
}

/* Save score to file */
void
saveScore(void)
{
	/* Check if savefile is not empty */
	if (strcmp(savefile, "") != 0) {

		/* If file can be opened, save score to file */
		save_file = fopen(savefile, "a+w");

		/* Get time */
		time_t ltime = time(NULL);

		if (save_file != NULL)
			fprintf(save_file, "Date: %sUsername: %s\nFinal score: %d\nLast question number: %d\n\n",
					ctime(&ltime), username, usr_score, last_qnum);

		fclose(save_file);
	}
}

/* Signal Handler */
void
sigHandler(int sig)
{
	/* Print gotten signal */
	fprintf(stderr, "\n[Detected Signal %d]\n", sig);

	/* Print final score */
	fprintf(stderr, "Final Score: %d\n", usr_score);

	/* Save score to file */
	saveScore();

	/* Close opened files, if any */
	if (dbfile != NULL)
		fclose(dbfile);
	if (crfile != NULL)
		fclose(crfile);
	if (save_file != NULL)
		fclose(save_file);

	exit(0);
}

int
main(int argc, char **argv)
{
	/* Default quiz database file */
	char quizdb[20] = "quiz.db";

	/* Save current user's name to username, if it is available */
	if (!strncmp(getenv("USER"), "", 2))
		strncpy(username, getenv("USER"), sizeof(username) - 1);

	/* Could not get username, fallback to "Unknown" */
	else
		strncpy(username, "Unknown", sizeof(username) - 1);

	/* Parse commandline options */
	int optind = 0;
	while ((optind = getopt(argc, argv, ":c:d:hrs:u:v")) != 1) {
		switch (optind) {
		/* Enter quiz-creator mode, to create a new quiz database */
		case 'c':
			fprintf(stderr, "[Entering Quiz-Creator mode...]\n");
			quizCreator(optarg);
			break;

		/* Choose custom quizdb file */
		case 'd':
			strncpy(quizdb, optarg, sizeof(quizdb) - 1);
			fprintf(stderr, "[Using quizdb file \"%s\"]\n", quizdb);
			break;
		/* Print help & usage */
		case 'h': case '?':
			fprintf(stderr, "Usage: %s [-c quizdb] [-d quizdb] [-h] [-s save_file] [-u username]\n\n", argv[0]);
			fprintf(stderr, "-c quizdb		Enter quiz-creator mode, to create a new\n"
					"			quiz database, and store it into \"quizdb\"\n"
					"			(Max Length 20)\n"
					"-d quizdb		Use \"quizdb\" as a quiz database file\n"
					"			(Max Length 20) Default: ./quiz.db\n"
					"-h			Print this help\n"
					"-r			Don't reserve special commands (exit, quit, stop)\n"
					"			Default: 0 (reserve special commands)\n"
					"-s save_file		Save date, time, final score and number of\n"
					"			questions to save_file\n"
					"			(Max Length 50) Default: none\n"
					"-u username		Add username \"username\" to save_file\n"
					"			(Max Length 20) Default: current user (%s)\n"
					"-v			Set username to \"Unavailable\"\n",
					username);
			return 0;

		/* Reserve special commands */
		case 'r':
			reserve_special = 1;
			fprintf(stderr, "[Special commands are not allowed]\n");
			break;

		/* Choose custom save file */
		case 's':
			strncpy(savefile, optarg, sizeof(savefile) - 1);
			fprintf(stderr, "[Using file \"%s\" to save scores]\n", savefile);
			break;

		/* Choose custom username */
		case 'u':
			strncpy(username, optarg, sizeof(username) - 1);
			fprintf(stderr, "[Using custom username \"%s\"]\n", username);
			break;

		/* Set username to "Unavailable" */
		case 'v':
			strncpy(username, "Unavailable", sizeof(username) - 1);
			fprintf(stderr, "[Set username to \"Unavailable\"]\n");
			break;
		}

		if (optind <= 0)
			break;
	}

	/* Open quizdb */
	dbfile = fopen(quizdb, "r");

	if (dbfile == NULL) {
		fprintf(stderr, "Could not open file \"%s\"! Exiting...\n", quizdb);
		return 1;
	}

	/* Create new quiz object */
	quizObj quiz[BUFSIZE] = {0};

	/* Create new user */
	quizUsr user[BUFSIZE] = {0};

	/* Print program version */
	printf("Welcome to quiz version %s.\n", __QUIZ_VERSION);

	/* NOTE: quiz database structure example:
		ID: 1;; Question: What is the capital of Italy?;; Answer: Rome;; Points: 5
		Any space can be added for padding, and the program won't break. For example:

		ID: 1;; Question: What is the capital of Italy?;;           Answer: Rome;;           Points: 2
		ID: 2;; Question: Which brand purchased IBM?;;              Answer: Lenovo;;         Points: 2
		ID: 3;; Question: Who is the creator of the Linux Kernel?;; Answer: Linus Torvalds;; Points: 3
		ID: 4;; Question: Who created this program (quiz)?;;        Answer: Matteo Salonia;; Points: 3
	*/

	/* Parse quiz database, store values, and ask questions to user */
	while (fscanf(dbfile, "ID: %d;; Question: %[^;;];; Answer: %[^;;];; Points: %d\n",
			&quiz->id, quiz->qst, quiz->ans, &quiz->points) > 0) {

		/* Handle signals */
		signal(SIGABRT, sigHandler);
		signal(SIGFPE, sigHandler);
		signal(SIGILL, sigHandler);
		signal(SIGINT, sigHandler);
		signal(SIGSEGV, sigHandler);
		signal(SIGTERM, sigHandler);

		/* Print question number, and ask question */
		printf("\nQuestion No.%d (%d Points): %s\n> ", quiz->id, quiz->points,  quiz->qst);

		/* Update question number */
		last_qnum = quiz->id;

		/* Store given answer */
		fgets(user->ans, BUFSIZE, stdin);
		user->ans[strlen(user->ans)-1] = '\0';

		/* Before doing anything else, check if the user's answer
		corresponds to "exit", "quit", or "stop" (case sensitive!),
		if reserve_special is 0 */
		if (!reserve_special &&
			(!strcmp(user->ans, "exit") ||
			!strcmp(user->ans, "quit") ||
			!strcmp(user->ans, "stop"))) {
				sigHandler(0);
				return 0;
		}

		/* Check if answer is correct */
		if (!strncasecmp(quiz->ans, user->ans, BUFSIZE)) {
			/* Add points to user's score */
			user->score += quiz->points;
			usr_score = user->score;
			printf("Your answer is correct! New points: %d\n", quiz->points);

		/* Answer is not correct */
		} else
			printf("Too bad, your answer is NOT correct!\n");

		/* Print Score */
		printf("Total Score: %d\n", user->score);

		/* Reset values */
		quiz->id = quiz->points = 0;
		for (int i = 0; i < BUFSIZE; i++) {
			quiz->qst[i] = 0;
			quiz->ans[i] = 0;
		}
	}

	/* Close file, we do not need it anymore */
	fclose(dbfile);

	/* Check if savefile is not empty */
	saveScore();

	return 0;
}
