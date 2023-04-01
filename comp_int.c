int comp_int( void *p,  void *q)
{
	// Get the values at given addresses
	int l = *( int *)p;
	int r = *( int *)q;

	// left has higher value.
	if (l>r)
		return 1;

	// equality
	else if (l==r)
		return (0);
    
    // righit has higher value
	return -1;
}

void print_int( void* data){
    int d = *( int *)data;
    printf("%d\n",d);
}