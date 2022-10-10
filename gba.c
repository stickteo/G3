
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "bmp.h"
#include "gba.h"

int initPalette (int *pal) {
	// green = 0x00FF00
	// magenta = 0xFF00FF

	for (int i=0; i<256; i+=2) {
		pal[i] = 0x00FF00;
	}

	for (int i=1; i<256; i+=2) {
		pal[i] = 0xFF00FF;
	}

	return 0;
}

int readPalette (FILE *f, int *pal) {
	int v,r,g,b,n;

	uint8_t in[512];
	n = fread(in,1,512,f) / 2;
	//printf("%d\n",n);

	for (int i=0; i<n; i++) {
		v = in[i*2] + (in[i*2+1]<<8);

		// 0b0bbbbbgggggrrrrr
		r = ( v     &31) * 33/4;
		g = ((v>>5) &31) * 33/4;
		b = ((v>>10)&31) * 33/4;
		
		//printf("%x %x %x %x\n",v,b,g,r);

		// 0x00rrggbb
		pal[i] = (r<<16)+(g<<8)+b;
	}

	return n;
}

int initTile (uint8_t *tile, int n) {
	int m = n*32;
	for (int i=0; i<m; i++) {
		tile[i] = 0xA5;
	}

	return 0;
}

int readTile (char *tile, FILE *f) {
	int n;
	n = fread(tile,1,32768,f);
	return n/32;
}

int writeTile (uint8_t *tile, int text, int y, int x, struct BMP *b) {
	//int *pal;
	int w;
	//pal = b->palette;
	w = b->width;

	int n,vh,p;
	n = text&1023;
	vh = (text>>10)&3;
	p = (text>>12)&15;

	uint8_t *t;
	t = tile + n*32;

	uint8_t *img;
	img = b->image;

	int s;
	s = 0;

	if (vh == 0) {
		for (int i=0; i<8; i++) {
			for (int j=0; j<8; j+=2) {
				int k;
				k = (y*8+i)*w + (x*8+j);

				img[k] = p*16+(t[s]&15);
				img[k+1] = p*16+(t[s]>>4);
				s++;
			}
		}
	} else if (vh == 1) { // h
		for (int i=0; i<8; i++) {
			for (int j=6; j>=0; j-=2) {
				int k;
				k = (y*8+i)*w + (x*8+j);

				img[k+1] = p*16+(t[s]&15);
				img[k] = p*16+(t[s]>>4);
				s++;
			}
		}
	} else if (vh == 2) { // v
		for (int i=7; i>=0; i--) {
			for (int j=0; j<8; j+=2) {
				int k;
				k = (y*8+i)*w + (x*8+j);

				img[k] = p*16+(t[s]&15);
				img[k+1] = p*16+(t[s]>>4);
				s++;
			}
		}
	} else { // both
		for (int i=7; i>=0; i--) {
			for (int j=6; j>=0; j-=2) {
				int k;
				k = (y*8+i)*w + (x*8+j);

				img[k+1] = p*16+(t[s]&15);
				img[k] = p*16+(t[s]>>4);
				s++;
			}
		}
	}

	return 0;
}

long fileSize (FILE *f) {
	long s, out;
	s = ftell(f);
	fseek(f,0,SEEK_END);
	out = ftell(f);
	fseek(f,s,SEEK_SET);
	return out;
}

int readMap (FILE *f, struct MAP *m) {
	int n;
	n = fileSize(f);
	//printf("map size: %d bytes\n",n);

	uint16_t *data;
	data = malloc(n);
	
	int d;
	d = n/2;
	fread(data, 2, d, f);

	//for (int i=0; i<d; i++) {
		//printf("%04x ",data[i]);
	//}

	int c, t;
	int width, height;
	c = 0;
	t = 0;
	width = 0;
	height = 0;

	// count entries
	for (int i=0; i<d; i++) {
		switch (data[i]) {
			case 0xFFFF:
				c++;
				if(t) height++;
				if(t>width) width = t;
				t = 0;
				//printf("end\n");
				break;
			case 0xFFFE:
				height++;
				if(t>width) width = t;
				t = 0;
				//printf("\n");
				break;
			default:
				t++;
				//printf("%04x ",data[i]);
		}
	}

	//printf("height = %d, width = %d, entries = %d\n",height,width,c);

	int *x, *y, *w, *h;
	x = malloc(sizeof(int)*c);
	y = malloc(sizeof(int)*c);
	w = malloc(sizeof(int)*c);
	h = malloc(sizeof(int)*c);

	for (int i=0; i<c; i++) {
		x[i] = 0;
	}

	for (int i=0; i<c; i++) {
		y[i] = -1;
	}
	
	uint16_t *data2;
	data2 = malloc(sizeof(uint16_t)*width*height);
	for (int i=0; i<width*height; i++) {
		//data2[i] = 0;
		data2[i] = 1023;
	}

	// copy entries
	c = 0;
	t = 0;
	height = 0;

	for (int i=0; i<d; i++) {
		switch (data[i]) {
			case 0xFFFF:
				
				if(t) {
					height++;
					h[c] = height - y[c];
				} else {
					h[c] = 0;
					y[c] = height;
				}
				w[c] = t;
				//if(t>width) width = t;
				t = 0;
				c++;
				break;
			case 0xFFFE:
				height++;
				//if(t>width) width = t;
				t = 0;
				break;
			default:
				data2[height*width + t] = data[i];
				if(y[c]==-1) y[c] = height;
				t++;
		}
	}

	/*
	for (int i=0; i<c; i++) {
		printf("%d %d %d %d\n",x[i],y[i],w[i],h[i]);
	}

	for (int i=0; i<height; i++) {
		for (int j=0; j<width; j++) {
			printf("%04x ", data2[i*width+j]);
		}
		printf("\n");
	}
	*/

	m->data = data2;
	m->width = width;
	m->height = height;
	m->entries = c;
	m->w = w;
	m->h = h;
	m->x = x;
	m->y = y;

	free(data);

	return d;
	
}

int readMap2 (FILE *f, struct MAP *m, int width, int height) {
	int n;
	n = fileSize(f);
	//printf("map size: %d bytes\n",n);

	uint16_t *data;
	data = malloc(n);
	
	int d;
	d = n/2;
	fread(data, 2, d, f);

	m->data = data;
	m->width = width;
	m->height = height;

	return d;
}

int renderMapTile (uint8_t *t, struct MAP *m, struct BMP *b) {
	for (int i=0; i<m->height; i++) {
		for (int j=0; j<m->width; j++) {
			writeTile(t, m->data[i*m->width+j], i, j, b);
		}
	}

	return 0;
}

int printMapEntries (struct MAP *m) {
	int n;
	n = m->entries;

	printf("; entries = %d\n",m->entries);
	printf("; width = %d\n",m->width);
	printf("; height = %d\n",m->height);
	printf("; entry #, x, y, w, h\n");

	for (int i=0; i<n; i++) {
		printf("#%d, %d, %d, %d, %d\n",i,m->x[i],m->y[i],m->w[i],m->h[i]);
	}

	return n;
}
