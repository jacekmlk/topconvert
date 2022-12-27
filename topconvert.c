/* ## TOPOCONVERT ## */
/*Read pockettopo txt export and write survex file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LENSTAT 8 //Lenght of station string
#define LENCOMM 256 // Lenght of comment string
#define LENTAB 4 // Lenght of tab

typedef struct shot
{
    char from[LENSTAT + 1];
    char to[LENSTAT + 1];
    float tape;
    float compass;
    float clino;
    char comment[LENCOMM + 1];
} shot;


void trailspace(char *station);
void afterspace(char *word);
void statedit(char *station);

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("ERROR! Usage: ./topconvert file.txt\n");
		return 1;
	}

    //Open file -> location == argc
    FILE *topfile = fopen(argv[1], "r");
    if (topfile == NULL)
    {
        printf("ERROR! Usage: ./topconvert file.txt\n");
        return 1;
    }
    char namefile[LENCOMM + 1];
    strcpy(namefile, argv[1]);

    char *postxt = strstr(namefile, "txt");
    if(postxt == NULL)
    {
        printf("ERROR! Usage: ./topconvert file.txt\n");
        return 1;
    }

    char *str = "svx";
    for (int i = 0; postxt[i] != '\0'; i++)
    {
        postxt[i] = str[i];
    }

    //Open svx file to write
    FILE *svxfile = fopen(namefile, "w");
    if (svxfile == NULL)
    {
        printf("ERROR!");
        return 1;
    }

    // Read and write header
	char name[LENCOMM + 1];
	char lenght[13];
	char numteam[LENSTAT + 1];
	char snum[LENSTAT + 1];
	char date[13];
	char declination[LENSTAT + 1];
	char team[LENCOMM + 1];
	
	fscanf(topfile, "%256s %11s %8s %8s %11s %8s", name, lenght, numteam, snum, date, declination);

	char tempteam[LENCOMM + 1];
	fgets(tempteam, LENCOMM, topfile);

	int i = 0;
	while (snum[i] != '\0')
	{
		team[i] = snum[i];
		i++;
	}
	int j = 0;
	while (tempteam[j] != '\0')
	{
		team[j+i] = tempteam[j];
		j++;
	}

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

	trailspace(team);

	//Print header and add CRLF handle
	fprintf(svxfile, "*begin\n*name\t%s\n*date\t%s\n*team\t%s; Declination: %s\n", name, date, team, declination);

	//Detect end of the header - Make version of CRLF
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

	//and add CRLF handling
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
        printf("ERROR!");
		return 1;
	}

	shot *ptrleg[linecount];
	int legcount = 0;
	shot *ptrsplay[linecount];
	int splaycount = 0;

	fputs("\n*data normal from to tape compass clino\n", svxfile);


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
		afterspace(ptrshot->comment);

		//Process
		statedit(ptrshot->from);
		statedit(ptrshot->to);

		//Put shots into file

		// 1. Legs
		if(ptrshot->to[0] !='\0')
		{
			//Collect legs
			ptrleg[legcount] = ptrshot;
			legcount++;

			ptrshot = NULL;
			ptrshot = malloc(sizeof(shot));

			if(ptrshot == NULL)
			{
				printf("ERROR!");
				return 1;
			}
		}
		else
		{
			if(ptrshot->tape == 0 && ptrshot->compass == 0 && ptrshot->clino == 0)
			{
				fprintf(svxfile,"\t; %s\t%s\n", ptrshot->from, ptrshot->comment);
			}
			else
			{
				//Collect splays
				ptrsplay[splaycount] = ptrshot;
				splaycount++;

				ptrshot = NULL;
				ptrshot = malloc(sizeof(shot));

				if(ptrshot == NULL)
				{
					printf("ERROR!");
					return 1;
				}
			}
		}
	}
	int count = 0;
	float sumtape = 0;
	float sumcompass = 0;
	float sumclino = 0;
	char comment[LENCOMM + 1];

	//Process legs and put into file
	for(int i = 0; i < legcount; i++)
	{
		//Print as comment to check
		fprintf(svxfile, "\n\t; %s\t%s\t%.3f\t%.2f\t%.2f\t; %s\n", ptrleg[i]->from, ptrleg[i]->to, ptrleg[i]->tape, ptrleg[i]->compass, ptrleg[i]->clino, ptrleg[i]->comment);

		sumtape = sumtape + ptrleg[i]->tape;
		sumcompass = sumcompass + ptrleg[i]->compass;
		sumclino = sumclino + ptrleg[i]->clino;
		strcat(comment, ptrleg[i]->comment);

		count++;


		if(i < legcount - 1)
		{
			if(strcmp(ptrleg[i]->from, ptrleg[i+1]->from) != 0 || strcmp(ptrleg[i]->to, ptrleg[i+1]->to) != 0)
			{
				fprintf(svxfile, "\n%s\t%s\t%.3f\t%.2f\t%.2f\t; %s\n", ptrleg[i]->from, ptrleg[i]->to, (sumtape / count), (sumcompass / count), (sumclino / count), comment);
				sumtape = 0;
				sumcompass = 0;
				sumclino = 0;
				comment[0] = '\0';
				count = 0;
			}
		}
		else
		{
			fprintf(svxfile, "\n%s\t%s\t%.3f\t%.2f\t%.2f\t; %s\n", ptrleg[i]->from, ptrleg[i]->to, (sumtape / count), (sumcompass / count), (sumclino / count), comment);
			for (int j = i - count; j < i; j++)
			sumtape = 0;
			sumcompass = 0;
			sumclino = 0;
			comment[0] = '\0';
			count = 0;
		}

	}

	//Put splays into file
	fputs("\n*flags splay\n", svxfile);
	for(int i = 0; i < splaycount; i++)
	{
		fprintf(svxfile, "%s\t\t\t%.3f\t%.2f\t%.2f\t; %s\n", ptrsplay[i]->from, ptrsplay[i]->tape, ptrsplay[i]->compass, ptrsplay[i]->clino, ptrsplay[i]->comment);
	}
	fputs("*end", svxfile);

	//Free memory
	free(ptrshot);
	for(int i = 0; i < splaycount; i++)
	{
		free(ptrsplay[i]);
	}
	for(int i = 0; i < legcount; i++)
	{
		free(ptrleg[i]);
	}

	//Close files
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

//Remove after space, change \n into \0 
void afterspace(char *word)
{
	for(int i = 0; word[i] != '\0'; i++)
	{
		if(word[i] == '\n')
		{
			word[i] = '\0';
		}
	}

	int pos;
	for(int i = 0; word[i] != '\0'; i++)
	{
		if(word[i] != ' ')
		{
			pos = i;
		}
	}
	word[pos+1] = '\0';
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
