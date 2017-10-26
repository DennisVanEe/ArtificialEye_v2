/*
    Subroutine to generate a B-spline open knot vector with multiplicity
    equal to the order at the ends.
	
    c            = order of the basis function
    n            = the number of defining polygon vertices
    nplus2       = index of x() for the first occurence of the maximum knot vector value
    nplusc       = maximum value of the knot vector -- $n + c$
    x()          = array containing the knot vector
*/

knot(n,c,x)

int n,c;
int x[];

{
	int nplusc,nplus2,i;

	nplusc = n + c;
	nplus2 = n + 2;

	x[1] = 0;
		for (i = 2; i <= nplusc; i++){
		    if ( (i > c) && (i < nplus2) )
				x[i] = x[i-1] + 1;
	    else
				x[i] = x[i-1];
		}
}
