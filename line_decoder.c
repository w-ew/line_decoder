#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char luma;

const luma tresh = 192;		/* luma threshold for white */
const int dot = 4;			/* 4 pixels per dot */


luma sample(int pos, luma *y, int length) {
	if(pos > length) return 0;
	if(y[pos] > tresh) return 1; else return 0;
};


int decode(int pos, luma *y, int length) {
	int r = 0;
	int i;

	for(i = 4; i >= 1; i--) {
		r <<= 1;
		if(sample(pos + 1 + i * dot, y, length)) r |= 1;
	};

	if(sample(pos + 1 + 5 * dot, y, length) != 0) return -1; /* missing stop bit */

	return r;
}

int main(int argc, char *argv[]) {

	int length;

	if(argc < 2) {
		fprintf(stderr, "usage: %s LENGTH\n", argv[0]);
		return -1;
	};

	length = atoi(argv[1]);

	if(length < 32 || length > 2000) {
		fprintf(stderr, "LENGTH must be between 32 and 2000\n");
		return -1;
	};

	luma *y;
	y = malloc(length);

	for(int i = 0; i < length; i++) {
		int t;
		scanf("%d", &t);
		y[i] = t;
	};

	

/* 
30 characters per line, 6 dots per character = 180 dots
720 pixels from usb007
4 pixels per dot
 */

	luma c, p = 0;
	int v;

	int spc = 0;

	for(int i = 0; i < length; i++) {
		c = sample(i, y, length);

		if(!c) spc += 1;
		if(c) {
			if(spc >= 6 * dot - 2) fprintf(stdout, " ");
			spc = 0;
		}

		if(c && !p) { /* rising edge */

			if(sample(i+1, y, length) && sample(i+2, y, length)) { /* at least 3 white pixels */

				v = decode(i, y, length);
				if(v >= 0) {
					if(v == 0xA) printf(".");
					else printf("%X", v);
					i += 5 * dot + 1;
					p = 0;
					continue;
				};
			};
		};

		p = c;
	};

	printf("\n");

	free(y);
	return 0;
}
