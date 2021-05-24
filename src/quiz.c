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

#define _GNU_SOURCE
#define _FORTIFY_SOURCE 2
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <getopt.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <pwd.h>
#include <err.h>

#define BUFSIZE 70

#define __QUIZ_VER_MAJOR "1"
#define __QUIZ_VER_MINOR "2"
#define __QUIZ_VERSION __QUIZ_VER_MAJOR "." __QUIZ_VER_MINOR

#define QUIZ_TMP	"quiz-tmp"
#define QUIZDB_DEFAULT	"/usr/local/share/quiz/quiz.db"

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

/* Global variables */
static int usr_score = 0;	/* User score */
static int last_qnum = 0;	/* Last question number */
static int reserve_special = 0;	/* Reserve special commands? (exit, quit, stop) */
static char savefile[50] = "";	/* Savefile */
static char username[20];	/* Custom username for savefile */
static char *tmp_file;		/* Temporary file */
static char cur_time[30];	/* Current time */
static FILE *dbfile;		/* Quiz DB file */
static FILE *crfile;		/* Quiz Creator file */
static FILE *save_file;		/* Save Score file */

/* Function prototypes */
static void save_time(char *);
static void quizCreator(char *);
static void saveScore(void);
static void sigHandler(int);

/* Save current time into cur_time */
static void
save_time(char *format)
{
	time_t ltime = time(NULL);
	struct tm *tm = localtime(&ltime);

	strftime(cur_time, sizeof(cur_time), format, tm);
}

/* Create a new quiz database */
static void __attribute__((noreturn))
quizCreator(char *outfile)
{
	/* Check if file can be opened for writing */
	crfile = fopen(outfile, "a+w");

	if (crfile == NULL)
		err(2, "Error: Could not Open file \"%s\"! Exiting...\n",
		    outfile);

	/* Ask the user how many questions to enter */
	int qnum;

 ask_question_num:
	for (;;) {
		printf("Choose how many questions you want to enter.\n> ");

		/* Check if number is valid */
		if (scanf("%d", &qnum) == 0 || qnum <= 0)
			goto ask_question_num;
		else
			break;
	}

	printf("OK. Using %d questions.\n", qnum);

	/* Get current date and time (Format: dd/mm/YYYY at HH:MM:SS) */
	save_time("%d/%m/%Y at %T");

	/* Write our header at the top of the file */
	fprintf(crfile, "# QuizDB generated automatically by quiz on %s.\n",
		cur_time);

	/* This loop will run until the number of questions is reached */
	for (int i = 0; i < qnum; i++) {
		/* Create new quiz object */
		quizObj quiz[BUFSIZE] = { 0 };

		/* Advance ID by 1, automatically */
		quiz->id = i + 1;
		printf("Question No.%d\n", quiz->id);

 ask_question:
		/* Ask question */
		printf("Enter question.\n> ");
		/* Quick dirty fix: capture newline to let fgets() get user input */
		fgetc(stdin);
		if (fgets(quiz->qst, BUFSIZE, stdin) == NULL)
			goto ask_question;
		quiz->qst[strlen(quiz->qst) - 1] = '\0';

 ask_answer:
		/* Ask answer */
		printf("Enter answer.\n> ");
		if (fgets(quiz->ans, BUFSIZE, stdin) == NULL)
			goto ask_answer;
		quiz->ans[strlen(quiz->ans) - 1] = '\0';

 ask_points:
		/* Ask how many points to give to the user */
		printf("Enter how many points to give to the user.\n> ");
		if (scanf("%d", &quiz->points) == 0)
			goto ask_points;

		/* Write values to outfile */
		fprintf(crfile,
			"ID: %d;; Question: %s;; Answer: %s;; Points: %d\n",
			quiz->id, quiz->qst, quiz->ans, quiz->points);
	}

	/* Close file */
	fclose(crfile);
	fprintf(stderr, "[Saved questions to file \"%s\"]\n", outfile);
	exit(0);
}

/* Save score to file */
static void
saveScore(void)
{
	/* Check if savefile is not empty */
	if (strcmp(savefile, "") != 0) {

		/* If file can be opened, save score to file */
		save_file = fopen(savefile, "a+w");

		save_time("%d/%m/%Y at %T");

		if (save_file != NULL)
			fprintf(save_file,
				"Date: %sUsername: %s\nFinal score: %d\nLast question number: %d\n\n",
				cur_time, username, usr_score, last_qnum);

		fclose(save_file);
	}
}

/* Signal Handler */
static void __attribute__((noreturn))
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
	int ind = 0;

	while ((ind = getopt(argc, argv, ":c:d:hrs:u:v")) != 1) {
		switch (ind) {
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
		case 'h':
		case '?':
			fprintf(stderr,
				"Usage: %s [-c quizdb] [-d quizdb] [-h] [-s save_file] [-u username]\n\n",
				argv[0]);
			fprintf(stderr,
				"-c quizdb		Enter quiz-creator mode, to create a new\n"
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
			fprintf(stderr, "[Using file \"%s\" to save scores]\n",
				savefile);
			break;

			/* Choose custom username */
		case 'u':
			strncpy(username, optarg, sizeof(username) - 1);
			fprintf(stderr, "[Using custom username \"%s\"]\n",
				username);
			break;

			/* Set username to "Unavailable" */
		case 'v':
			strncpy(username, "Unavailable", sizeof(username) - 1);
			fprintf(stderr, "[Set username to \"Unavailable\"]\n");
			break;
		}

		if (ind <= 0)
			break;
	}

	/* Open quizdb */
	dbfile = fopen(quizdb, "r");

	if (dbfile == NULL) {
		fprintf(stderr,
			"Warning: Missing QuizDB file. Opening default QuizDB file.\n");

		dbfile = fopen(QUIZDB_DEFAULT, "r");

		if (dbfile == NULL)
			err(1,
			    "Error: Unable to open default QuizDB! Exiting.\n");
	}

	/* Create new quiz object */
	quizObj quiz[BUFSIZE] = { 0 };

	/* Create new user */
	quizUsr user[BUFSIZE] = { 0 };

	/* Print program version */
	printf("Welcome to quiz version %s.\n", __QUIZ_VERSION);

	FILE *fd;
	char *buf = NULL;
	size_t len = 0;
	ssize_t read;

	/* NOTE: quiz database structure example:
	   ID: 1;; Question: What is the capital of Italy?;; Answer: Rome;; Points: 5
	   Any space can be added for padding, and the program won't break. For example:

	   ID: 1;; Question: What is the capital of Italy?;;           Answer: Rome;;           Points: 2
	   ID: 2;; Question: Which brand purchased IBM?;;              Answer: Lenovo;;         Points: 2
	   ID: 3;; Question: Who is the creator of the Linux Kernel?;; Answer: Linus Torvalds;; Points: 3
	   ID: 4;; Question: Who created this program (quiz)?;;        Answer: Matteo Salonia;; Points: 3

	   Lines starting with spaces and tabs will be ignored, just like comments ("# test"), and empty lines:
	   # QuizDB generated automatically by quiz on dd/mm/yyyy at HH:MM:SS
	   ID: 1;; Question: What is the capital of Italy?;;           Answer: Rome;;           Points: 2
	   This line starting with a space is ignored.
	   Same thing for this line starting with a tab, and the one below (newline)

	   ID: 2;; Question: Which brand purchased IBM?;;              Answer: Lenovo;;         Points: 2
	   ID: 3;; Question: Who is the creator of the Linux Kernel?;; Answer: Linus Torvalds;; Points: 3
	   ID: 4;; Question: Who created this program (quiz)?;;        Answer: Matteo Salonia;; Points: 3
	 */

	/* Try to open a new temporary file, which will contain
	   the linted QuizDB (no comments, tabs, newlines, spaces, etc.) */
	fd = fopen(QUIZ_TMP, "w");

	if (fd == NULL) {
		fprintf(stderr, "Warning: Cannot open temporary file %s.\n\
Attempting to write into user's home directory...\n", QUIZ_TMP);

		/* Get user's home directory */
		struct passwd *pw = getpwuid(getuid());

		tmp_file = pw->pw_dir;

		strncat(tmp_file, "/", 2);
		strncat(tmp_file, QUIZ_TMP, strlen(QUIZ_TMP) + 1);

		fd = fopen(tmp_file, "w");

		if (fd == NULL) {
			fprintf(stderr,
				"Warning: Cannot open %s; Trying to open /tmp/%s\n",
				tmp_file, QUIZ_TMP);

			strncpy(tmp_file, "/tmp/", 6);
			strncat(tmp_file, QUIZ_TMP, strlen(QUIZ_TMP) + 1);

			fd = fopen(tmp_file, "w");

			if (fd == NULL)
				fprintf(stderr,
					"Unable to write into /tmp! Aborting.\n");
		}
	} else
		tmp_file = QUIZ_TMP;

	/* Read file, without adding comments. */
	while ((read = getline(&buf, &len, dbfile)) != -1) {
		switch (buf[0]) {
		case ' ':
		case '\n':
		case '#':
		case '\t':
			break;
		default:
			fprintf(fd, "%s", buf);
			break;
		}
	}

	/* Open fd in read-only mode */
	fd = freopen(tmp_file, "r", fd);

	/* Parse quiz database, store values, and ask questions to user */
	while (fscanf
	       (fd, "ID: %d;; Question: %[^;;];; Answer: %[^;;];; Points: %d\n",
		&quiz->id, quiz->qst, quiz->ans, &quiz->points) > 0) {

		/* Handle signals */
		signal(SIGABRT, sigHandler);
		signal(SIGFPE, sigHandler);
		signal(SIGILL, sigHandler);
		signal(SIGINT, sigHandler);
		signal(SIGSEGV, sigHandler);
		signal(SIGTERM, sigHandler);

		/* Print question number, and ask question */
		printf("\nQuestion No.%d (%d Points): %s\n> ", quiz->id,
		       quiz->points, quiz->qst);

		/* Update question number */
		last_qnum = quiz->id;

		/* Store given answer */
		if (fgets(user->ans, BUFSIZE, stdin) == NULL)
			goto exit;

		user->ans[strlen(user->ans) - 1] = '\0';

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
			printf("Your answer is correct! New points: %d\n",
			       quiz->points);

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

 exit:
	/* Close files that we do not need anymore */
	fclose(dbfile);
	fclose(fd);

	/* Remove temporary file */
	remove(tmp_file);

	/* Check if savefile is not empty */
	saveScore();

	return 0;
}
