
#ifndef GBA_H
#define GBA_H

int initPalette (int *pal);
int readPalette (FILE *f, int *pal);

struct MAP {
	uint16_t *data;
	int width;
	int height;

	int entries;
	int *w;
	int *h;
	int *x;
	int *y;
};

int initTile (uint8_t *tile, int n);
int readTile (char *tile, FILE *f);
int writeTile (uint8_t *tile, int text, int y, int x, struct BMP *b);
//long fileSize (FILE *f);
int readMap (FILE *f, struct MAP *m);
int renderMapTile (uint8_t *t, struct MAP *m, struct BMP *b);



#endif
