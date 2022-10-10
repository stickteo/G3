
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "bmp.h"
#include "gba.h"

int main (int argc, char** argv) {
	if (argc < 6) {
		printf("Not enough arguments.\n"
			"Usage: G3 pal.bin map.bin tile.bin width height out.bmp\n"
			);
		return 0;
	}

	// redirect stdout to meta.txt
	//if (argc >=6) {
	//	freopen(argv[5], "wb", stdout);
	//}

	struct BMP b;
	//char img[256];
	//uint8_t *img;
	int palette[256];

	//b.width = 16*8;
	//b.height = 64*8;
	b.bitCount = 8;

	//img = malloc(b.width*b.height);

	//b.image = img;
	b.palette = palette;

	FILE *fpal;
	//fpal = fopen("BattlePalBG.bin","rb");
	//fpal = fopen("pal.bin","rb");
	fpal = fopen(argv[1],"rb");

	initPalette(palette);
	readPalette(fpal,palette);
	fclose(fpal);

	FILE *fmap;
	//fmap = fopen("BattleMap.bin","rb");
	fmap = fopen(argv[2],"rb");
	struct MAP m;
	//readMap(fmap,&m);
	readMap2(fmap,&m,atoi(argv[4]),atoi(argv[5]));
	fclose(fmap);

	//for	(int i=0; i<256; i++) {
		//img[i] = i;
	//}

	//char *tile;
	uint8_t *tile;
	tile = malloc(1024*32);
	initTile(tile,1024);
	FILE *ftile;
	//ftile = fopen("BattleTileBG.bin","rb");
	ftile = fopen(argv[3],"rb");
	readTile(tile,ftile);
	fclose(ftile);

	//for (int i=0; i<64; i++) {
		//for (int j=0; j<16; j++) {
			//writeTile(tile,i*16+j,i,j,&b);
		//}
	//}

	uint8_t *img;
	b.width = 8*m.width;
	b.height = 8*m.height;
	img = malloc(b.width*b.height);
	b.image = img;

	renderMapTile(tile,&m,&b);
	//printMapEntries(&m);

	FILE *fout;
	//fout = fopen("test.bmp","wb");
	//fout = fopen(argv[4],"wb");
	fout = fopen(argv[6],"wb");
	
	bmpWrite(fout,&b);

	fclose(fout);
	free(img);
	free(tile);

	//if (argc >=6) {
	//	fclose(stdout);
	//}

	return 0;
}
