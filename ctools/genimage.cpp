#define RING_IMG_SIZE (10*1024L)

const char IMG[]= "F:\\ring.img";

#include <iostream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <emCpp/const.h>

char buf[512];

int main() {
	memset(buf,0x0A,sizeof(buf));
	cout << endl << "Image: " << IMG << endl;
	FILE *img=fopen(IMG,"wb"); assert(img!=NULL);
	for (unsigned int i=0;i<RING_IMG_SIZE/512;i++) {
		if (i%100==0) cout << ". ";
		fwrite(buf,sizeof(buf),1,img);
	}
	cout << endl;
	fclose(img);
	return 0;
}

