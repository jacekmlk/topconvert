/*
## TOPOCONVERT ##
Read pockettopo txt export and write survex file
*/

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
    char comment[257];
} shot;

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

    char line[1024]; // temporary line buffer

    // Read and write header
    int d1;
    int d2;

    while(fgets(line, 1024, topfile) != NULL && !(d1 == 1 && d2 == 1))
    {
        d1 = d2;
        d2 = strlen(line);

        fputs(line, svxfile);
    }

    
    while(fgets(line, 1024, topfile) != NULL)
    {
        fputs(line, svxfile);
    }




    //Check if splay

    //If splay -> write into splay struct

    //If shot -> write into shot struct / read next two line, averange _> write result into svx file

    //Write splays at the end into file


    //Close opened file and svx file
    fclose(topfile);
    fclose(svxfile);
}