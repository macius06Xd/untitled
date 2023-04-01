#include <string.h>
int comp_str2(void *p, void *q){
    char *l = (char *)p;
    char *r = (char *)q;

    int L = strlen(l);
    int R = strlen(r);
    int c = strcmp(l, r);
    int c2 = c*(-1);
    return c;
    if (L>R)
		return 1;

	// equality
	else if (L==R)
		return (0);
    
    // righit has higher value
	return -1;
}