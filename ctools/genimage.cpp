#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define M1 (1024L*1024)
#define G1 (1M*1024)

using namespace std;

const char IMG[]= "F:\\ring.img";

char buf[512];

int main() {
	cout << endl << endl;
	cout << "Image: " << IMG << endl;
	FILE *img=fopen(IMG,"wb"); assert(img!=NULL);
	for (unsigned int i=0;i<M1/512;i++) {
		cout << i << " ";
		memset(buf,(i+1)%0x100,sizeof(buf));
		fwrite(buf,sizeof(buf),1,img);
	}
	fclose(img);
	cout << endl << endl;
	return 0;
}

