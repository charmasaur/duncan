#include <cstdio>
#include <ctime>
#include <cmath>
#include <cstdlib>

int main() {
	srand(time(NULL));
	char filename[256];
	printf("Filename: ");
	scanf("%s",&filename);
	int w, h;
	printf("Width height: ");
	scanf("%d %d",&w,&h);
	FILE *out=fopen(filename,"w");
	fprintf(out,"%d %d\n",w,h);
	for(int i=0;i<h;i++) {
		for(int j=0;j<w;j++) fprintf(out,"%c",(rand()%100==0)?'#':'.');
		fprintf(out,"\n");
	}
	fclose(out);
	return 0;
}
