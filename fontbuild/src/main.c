#include <stdlib.h>
#include <stdio.h>
#include <string.h>

long fsize(FILE* f)
{
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    return size;
}

void usage()
{
    fprintf(stderr, "fontbuild - build a font for use in Dream vs. Dream.\n\tUsage: fontbuild font.png font.txt out.rf\n");
    exit(1);
}

int main(int argc, char** argv)
{
    if(argc < 4) usage();

    FILE* img = fopen(argv[1], "rb");
    if(!img)
    {
        fprintf(stderr, "Cannot read from %s.\n", argv[1]);
        usage();
    }

    FILE* data = fopen(argv[2], "rb");
    if(!data)
    {
        fprintf(stderr, "Cannot read from %s.\n", argv[2]);
        fclose(img);
        usage();
    }

    FILE* out = fopen(argv[3], "wb");
    if(!out)
    {
        fprintf(stderr, "Cannot write to %s.\n", argv[3]);
        fclose(img);
        fclose(data);
        usage();
    }

    long imgSize = fsize(img);
    long dataSize = fsize(data);
    char* imgL = malloc(imgSize);
    char* dataL = malloc(dataSize);
    fread(imgL, imgSize, 1, img);
    fread(dataL, dataSize, 1, data);
    fclose(img);
    fclose(data);

    int mono = 0; //width of characters; 0 for variable
    int sensitive = 0; //false, case insensitive; true, case sensitive
    int option = 0; //increment until 3, then read data
    int first = 0; //index of first character in option string

    int pos = 0;
    long i;
    char flag = 1;
    for(i = 0; i < dataSize; i++)
    {
        if(dataL[i] == '\n' || dataL[i] == '\f' || dataL[i] == '\r')
        {
            dataL[i] = 0;
            if(i && dataL[i-1])
            {
                //Copy everything up to this line
                if(dataL[first] >= '0' && dataL[first] <= '9')
                    mono = atoi(dataL + first);
                else if(!strcasecmp(dataL + first, "var")) mono = 0;
                else if(!strcasecmp(dataL + first, "insensitive")) sensitive = 0;
                else if(!strcasecmp(dataL + first, "sensitive")) sensitive = 1;
                option++;
            }
        }
        else if(option == 1)
        {
            option++;
            first = i;
        }
        if(option == 3) break;
    }
    fwrite(&mono, 1, 1, out);
    fwrite(&sensitive, 1, 1, out);
    for(; i < dataSize; i++)
    {
        if(!dataL[i] || dataL[i] == '\n' || dataL[i] == '\f' || dataL[i] == '\r' || dataL[i] == '\t') continue;
        if(flag)
        {
            if(mono)
                printf("%c\n", dataL[i]);
            else
                printf("%c: ", dataL[i]);
            fwrite(dataL + i, 1, 1, out);
        }
        else
        {
            char buff[8];
            int j;
            for(j = 0; dataL[i+j] && dataL[i+j] >= '0' && dataL[i+j] <= '9'; j++);
            memcpy(buff, dataL + i, j);
            buff[j] = 0;
            unsigned char width = atoi(buff) - pos + 1;
            i += j;

            /*for(; dataL[i] && dataL[i] < '0' && dataL[i] > '9'; i++);
            for(j = 0; dataL[i+j] && dataL[i+j] >= '0' && dataL[i+j] <= '9'; j++);
            memcpy(buff, dataL + i, j);
            buff[j] = 0;
            unsigned char width = atoi(buff) - pos + 1;
            i += j;*/

            printf("%d (%d)\n", pos, width);
            fwrite(&pos, 2, 1, out);
            fwrite(&width, 1, 1, out);

            pos += width;
        }
        if(!mono) flag = !flag;
    }
    flag = 0;
    fwrite(&flag, 1, 1, out);
    fwrite(imgL, imgSize, 1, out);
    fclose(out);

    printf("Successfully built %s.", argv[3]);

    return 0;
}
