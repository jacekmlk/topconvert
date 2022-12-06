/* ## TOPOCONVERT ## */
/*Read pockettopo txt export and write survex file */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define LENSTAT 8 //Lenght of station string
#define LENCOMM 256 // Lenght of comment string
#define LENTAB 4 // Lenght of tab

typedef struct shot
{
    char from[LENSTAT + 1];
    char to[LENSTAT + 1];
    float tape ;
    float compass;
    float clino;
    char comment[LENCOMM + 1];
} shot;

void trailspace(char *station);
void statedit(char *station);
void medit(char *measure, char *word, int from, int to);
void clean(char *str);

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



    // Read and write header
	char name[LENCOMM + 1];
	char lenght[12];
	char numteam[LENSTAT + 1];
	char snum[LENSTAT + 1];
	char date[13];
	char declination[LENSTAT + 1];
	char team[LENCOMM + 1];
	

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
	trailspace(team);
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

	shot *ptrshot = malloc(sizeof(shot));
	if(ptrshot == NULL)
	{
		return 1;
	}


	shot *ptrsplay[linecount];
	int splaycount = 0;

	fputs("\n*data normal from to tape compass clino\n", svxfile);

	//Write simpler version of code below using fscanf

	while(feof(topfile) == 0)
	{
		fseek(topfile, LENTAB, SEEK_CUR);
		//Read stations
		fread(&ptrshot->from, sizeof(char), LENSTAT, topfile);
		ptrshot->from[LENSTAT] = '\0';
		fseek(topfile, 2, SEEK_CUR);
		fread(&ptrshot->to, sizeof(char), LENSTAT, topfile);
		ptrshot->to[LENSTAT] = '\0';
		fseek(topfile, 2, SEEK_CUR);
	
		fscanf(topfile, "%f %f %f", &ptrshot->tape, &ptrshot->compass, &ptrshot->clino);

		//Read comment
		fgets(ptrshot->comment, 256, topfile);
		trailspace(ptrshot->comment);

		//Process
		statedit(ptrshot->from);
		statedit(ptrshot->to);

		//Put shots into file

		// 1. Main shots
		if(ptrshot->to[0] !='\0')
		{

			fprintf(svxfile, "%s\t%s\t%.3f\t%.2f\t%.2f\t; %s\n", ptrshot->from, ptrshot->to, ptrshot->tape, ptrshot->compass, ptrshot->clino, ptrshot->comment);
		}
		else
		{
			//Collect splays
			ptrsplay[splaycount] = ptrshot;
			splaycount++;

			ptrshot = NULL;
			ptrshot = malloc(sizeof(shot));
		}
	}

	//Put splays into file
	fputs("*flags splay\n", svxfile);
	for(int i = 0; i < splaycount; i++)
	{
		fprintf(svxfile, "%s\t\t\t%.3f\t%.2f\t%.2f\t; %s\n", ptrsplay[i]->from, ptrsplay[i]->tape, ptrsplay[i]->compass, ptrsplay[i]->clino, ptrsplay[i]->comment);
	}
	fputs("*end", svxfile);

	/*
    while(fgets(line, 1024, topfile) != NULL)
    {
        //Write into shot struct

		statedit(measure.from, line, 0, 8);
		statedit(measure.to, line, 12, 20);

		medit(measure.tape, line, 24, 32);
		medit(measure.compass, line, 32, 40);
		medit(measure.clino, line, 40, 48);

		if (strlen(line) > 58)
		{
			commedit(measure.comment, line, 57);
		}

		//Put path into file
		if(strcmp(measure.to, "\0") != 0)
		{
			fputs(measure.from, svxfile);
			fputc('\t', svxfile);
			fputs(measure.to, svxfile);
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
			strcpy(splay[splaycount].fromstation, measure.from);
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
	*/

	free(ptrshot);
	for(int i = 0; i < splaycount; i++)
	{
		free(ptrsplay[i]);
	}

    fclose(topfile);
    fclose(svxfile);
}

//Remove trailing spaces
void trailspace(char *station)
{
	int c = 0;
	for (int i = 0; station[i] != '\0'; i++)
	{
		if(station[i] != ' ' || c > 0)
		{
			station[c] = station[i];
			c++;
		}

	}
	station[c] = '\0';
}

//Station edit function
void statedit(char *station)
{
	for( int i = 0; station[i] != '\0'; i++)
	{
		if(station[i] == '.')
		{
			station[i] = '_';
		}
	}

	int c = 0;
	for (int i = 0; station[i] != '\0'; i++)
	{
		if(station[i] != ' ')
		{
			station[c] = station[i];
			c++;
		}

	}
	station[c] = '\0';
}

void clean(char *str)
{
	int i = 0;
	{
		while(str[i] != '\0')
		{
			str[i] = '\0';
			i++;
		}
	}

}