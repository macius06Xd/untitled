#include <string.h>
int comp_str(void *p, void *q){
    char *l = (char *)p;
    char *r = (char *)q;

    int L = strlen(l);
    int R = strlen(r);

    if (L>R)
		return 1;

	// equality
	else if (L==R)
		return (0);
    
    // righit has higher value
	return -1;
}

void print_str(void *data){
    char *a = (char *)data;
    printf("%s\n",a);
}