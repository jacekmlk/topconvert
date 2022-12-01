/*
## TOPOCONVERT ##
Read pockettopo txt export and write survex file
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define _GNU_SOURCE
#include <string.h>

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

    char c;

    while(fread(&c, sizeof(char), 1, topfile))
    {
        fwrite(&c, sizeof(char), 1, svxfile);
    }







    //Write header
    //Copy first line to file

    //RM empty line

    //Copy third line to file

    //RM empty line

    //RM empty line

    //Read first line and write into svx file

    //Check if splay

    //If splay -> write into splay struct

    //If shot -> write into shot struct / read next two line, averange _> write result into svx file

    //Write splays at the end into file


    //Close opened file and svx file
    fclose(topfile);
    fclose(svxfile);





}