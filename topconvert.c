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
    FILE *topfile = fopen("test.txt", "r");
    if (topfile == NULL)
    {
        printf("ERROR!");
        return 1;
    }

    FILE *srvfile = fopen("test.txt", "r");
    if (srvfile == NULL)
    {
        printf("ERROR!");
        return 1;
    }

    //Open file -> location == argc
    //Open svx file to write

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
    fclose(srvfile);





}