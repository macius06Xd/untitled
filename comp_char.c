int comp_char(void *p, void *q){
    char l = *(char *)p;
    char r = *(char *)q;
    int L = (int)l;
    int R = (int)r;
    if (L>R)
		return 1;

	// equality
	else if (L==R)
		return (0);
    
    // righit has higher value
	return -1;
}

void print_char(void *data){
    char a = *(char *)data;
    printf("%c\n",a);
}