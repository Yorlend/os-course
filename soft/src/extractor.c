#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "extractor.h"
#include "cbmp.h"

int extract_fingerprint(const char* filename, const struct signature* signature)
{
    srand(time(NULL));
    int index = rand() % 408;
    char fp[1024];

    snprintf(fp, sizeof(fp), "../dataset/CrossMatch_Sample_DB/fp/%d.bmp", index);

    BMP* bmp = bopen(fp);

    unsigned int width = get_width(bmp);
    unsigned int height = get_height(bmp);

    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            unsigned char r, g, b;
            get_pixel_rgb(bmp, x, y, &r, &g, &b);

            b = 255 - (double)b * b / 255;
            r = 255 - r;
            g = 255 - (double)g * g / 255;

            set_pixel_rgb(bmp, x, y, r, g, b);
        }
    }

    bwrite(bmp, filename);
    bclose(bmp);

    return 0;
}
