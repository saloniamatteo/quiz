## quiz

`quiz` is a simple program that lets any user create and/or take quizzes.

It features a quiz creator, and the core quiz program.

It can also save scores to a custom file, read a custom quiz database,
use a custom username, and store how many questions were taken.

Initially written on March 24, 2021. Current version can be found in `quiz.c`

## Flags
Currently, `quiz` supports the following command-line flags:

| Flag | Argument | Description                                  |
|------|----------|----------------------------------------------|
| `-c` | Required | Enter [Quiz-Creator](#Quiz-Creator) mode     |
| `-d` | Required | Load a custom quiz database                  |
| `-h` | Optional | Print help and exit                          |
| `-s` | Required | Save scores to file                          |
| `-r` | None     | Disallow special commands (exit, quit, stop) |
| `-u` | Required | Set custom username to use in save file      |
| '-v' | None     | Disable username                             |

## Quiz-Creator
`quiz` features a Quiz-Creator, which simply lets any user to create quiz databases, especially if they do not want to
manually enter the question ID everytime.

For example, to create a new quiz database with 3 questions, run `./quiz -c mynewdb`.
The file `mynewdb` will be used to store the new database, and will be created if it does not exist.
You can also add new questions to existing databases with the same command.

Next, you will get the following output:

```
[Entering Quiz-Creator mode...]
Choose how many questions you want to enter.
> 3
OK. Using 3 questions.
Question No.1
Enter Question.
> Who created this program (quiz)?
Enter answer.
> Matteo Salonia
Enter how many points to give to the user.
> 50
Question No.2
Enter Question.
> Did you donate to Matteo?
Enter answer.
> Yes
Enter how many points to give to the user.
> 100
Question No.3
Enter Question.
> Do you like this program?
Enter answer.
> Absolutely
Enter how many points to give to the user.
> 45
```

The resulting file `mynewdb` will have the following contents:

```
ID: 1;; Question: Who created this program (quiz)?;; Answer: Matteo Salonia;; Points: 50
ID: 2;; Question: Did you donate to Matteo?;; Answer: Yes;; Points: 100
ID: 3;; Question: Do you like this program?;; Answer: Absolutely;; Points: 45
```

## Statically linking & building
If you want to statically link `quiz` to share it, all you need is a compiler. (No fancy libraries used)

(NOTE: I recommend using `musl-gcc` instead of `gcc` because program size is much, much smaller (See [Musl-GCC vs GCC](#musl-gcc-vs-gcc)))

Then, run the following:

```bash
cd quiz
make static
```

## Musl-GCC vs GCC
+ Time (Static) [`time make static`]
	- Musl-GCC compilation time: `0,11s user 0,02s system 47% cpu 0,270 total`
	- GCC compilation time: `0,19s user 0,03s system 52% cpu 0,401 total`

+ Time (Dynamic) [`time make`]
	- Musl-GCC compilation time: `0,09s user 0,02s system 47% cpu 0,232 total`
	- GCC compilation time: `0,13s user 0,00s system 50% cpu 0,250 total`

+ Executable Size (Static) [`ls -l quiz`]
	- Musl-GCC size: `73k`
	- GCC size: `978k`

+ Executable Size (Dynamic) [`ls -l quiz`]
	- Musl-GCC size: `16k`
	- GCC size: `18k`

## Help
If you need help, you can:
- Create an issue;
- Open a pull request;
- Send me an email [(saloniamatteo@pm.me](mailto:saloniamatteo@pm.me) or [matteo@mail.saloniamatteo.top)](mailto:matteo@mail.saloniamatteo.top).
