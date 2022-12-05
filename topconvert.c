/* ## TOPOCONVERT ## */
/*Read pockettopo txt export and write survex file */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct shot
{
    char fromstation[9];
    char tostation[9];
    char tape[9];
    char compass[9];
    char clino[9];
    char comment[258];
} shot;

void statedit(char *station, char *word, int from, int to);
void medit(char *measure, char *word, int from, int to);
void commedit(char *comment, char *word, int from);

int main(int argc, char *argv[])
{
    //Open file -> location == argc
    FILE *topfile = fopen("test.txt", "r");
    if (topfile == NULL)
    {
        printf("ERROR!");
        return 1;
    }

    //Open svx file to write
    FILE *svxfile = fopen("close.txt", "w");
    if (svxfile == NULL)
    {
        printf("ERROR!");
        return 1;
    }

    char *line; // temporary line buffer
    line = malloc(sizeof(char) * 1024);

    // Read and write header


	char name[257];
	char lenght[12];
	char numteam[9];
	char snum[9];
	char date[12];
	char declination[9];
	char team[257];
	

	fscanf(topfile, "%256s %11s %8s %8s %11s %8s", name, lenght, numteam, snum, date, declination);
	fgets(team, 256, topfile);

	fputs("*begin", svxfile);
	fputs("\n*name\t",svxfile);
	fputs(name, svxfile);
	fputs("\n*date\t", svxfile);

	//Date: change '/' into dot.
	int index = 0;
	while(date[index] != '\0')
	{	
		if (date[index] == '/')
		{
			date[index] = '.';
		}
		index++;
	}

	fputs(date, svxfile);
	fputs("\n*team\t", svxfile);
	fputs(team, svxfile);

	//Detect end of the header
    char d1;
    char d2;
    while(!(d1 == '\n' && d2 == '\n'))
    {
		char c = fgetc(topfile);
        d1 = d2;
        d2 = c;
    }

	//Check amount of lines in file
    fpos_t position;
	int linecount = 1;
	char c;

	fgetpos(topfile, &position);
	for (c = fgetc(topfile); c != EOF; c = fgetc(topfile))
	{
		if (c == '\n')
		{
			linecount++;
		}
	}
    fsetpos(topfile, &position);

    shot measure;
	shot splay[linecount];
	int splaycount = 0;

	fputs("\n*data normal from to compass clino tape\n", svxfile);

	//Write simpler version of code below using fscanf

    while(fgets(line, 1024, topfile) != NULL)
    {
        //Write into shot struct

		statedit(measure.fromstation, line, 0, 8);
		statedit(measure.tostation, line, 12, 20);

		medit(measure.tape, line, 24, 32);
		medit(measure.compass, line, 32, 40);
		medit(measure.clino, line, 40, 48);

		if (strlen(line) > 58)
		{
			commedit(measure.comment, line, 57);
		}

		//Put path into file
		if(strcmp(measure.tostation, "\0") != 0)
		{
			fputs(measure.fromstation, svxfile);
			fputc('\t', svxfile);
			fputs(measure.tostation, svxfile);
			fputc('\t', svxfile);
			fputs(measure.tape, svxfile);
			fputc('\t', svxfile);
			fputs(measure.compass, svxfile);
			fputc('\t', svxfile);
			fputs(measure.clino, svxfile);
			fputs(measure.comment, svxfile);

			fputs("\n\n", svxfile);
		}
		else
		{
			//Collect splays
			strcpy(splay[splaycount].fromstation, measure.fromstation);
			strcpy(splay[splaycount].tape, measure.tape);
			strcpy(splay[splaycount].compass, measure.compass);
			strcpy(splay[splaycount].clino, measure.clino);
			strcpy(splay[splaycount].comment, measure.comment);

			splaycount++;
		}
	strcpy(measure.comment, "");
    }

	//Put splays into file
	fputs("*flags splay\n", svxfile);
	for(int i = 0; i < splaycount; i++)
	{
			fputs(splay[i].fromstation, svxfile);
			fputs("\t\t\t", svxfile);
			fputs(splay[i].tape, svxfile);
			fputc('\t', svxfile);
			fputs(splay[i].compass, svxfile);
			fputc('\t', svxfile);
			fputs(splay[i].clino, svxfile);
			fputc('\t', svxfile);
			fputs(splay[i].comment, svxfile);
			fputs("\n", svxfile);
	}
	fputs("*end", svxfile);

    free(line);
    fclose(topfile);
    fclose(svxfile);
}

//Station edit function
void statedit(char *station, char *word, int from, int to)
{
	int c = 0;

	for (int i = from; i < to; i++)
	{
		if(word[i] != ' ')
		{
			if(word[i] == '.') //change dot into '_'
			{
				station[c] = '_'; 
				c++;
			}
			else
			{
				station[c] = word[i];
				c++;
			}
		}

	}
	station[c] = '\0';
}

//Measure edit function
void medit(char *measure, char *word, int from, int to)
{
	int c = 0;

	for (int i = from; i < to; i++)
	{
		if(word[i] != ' ')
		{
			measure[c] = word[i];
			c++;
		}

	}
	measure[c] = '\0';
}

//Comment edit function
void commedit(char *comment, char *word, int from)
{
	comment[0] = ';';
	int c = 1;
	for (int i = from; i < strlen(word); i++)
	{
		if (word[i] == '\n')
		{
			comment[c] = '\0';
			c++;
		}
		else if (word[i] != '"') // remove ""
		{
			comment[c] = word[i];
			c++;
		}
	}

}