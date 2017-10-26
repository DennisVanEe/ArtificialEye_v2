/*  Subroutine to calculate the sum of the nonrational basis functions (see \eq {6--87}).

	C code for An Introduction to NURBS
	by David F. Rogers. Copyright (C) 2000 David F. Rogers,
	All rights reserved.
	
	Name: sumrbas.c
	Language: C
	Subroutines called: none
	Book reference: Chapter 7, Section 7.1, Alg. p.309
	
    b[]         = array containing the polygon net points
                    b[1] = x-component
                    b[2] = y-component
                    b[3] = z-component
                    b[4] = homogeneous coordinate weighting factor        
                    Note: Bi,j = b[] has dimensions of n*m x 4 with j varying fastest
                     The polygon net is n x m
    mbasis[,]  = array containing the nonrational basis functions for one value of w
    mpts       = the number of polygon vertices in w direction
    nbasis[,]  = array containing the nonrational basis functions for one value of u
    npts       = the number of polygon vertices in u direction
    sum        = sum of the basis functions
*/

float sumrbas(b,nbasis,mbasis,npts,mpts)

int npts,mpts;
	
float b[],nbasis[],mbasis[];
	
{

	int i,j,jbas,j1;
	float sum;
	
 /* calculate the sum */

	sum = 0;
/*	printf("npts,mpts %d %d \n", npts,mpts);*/

	for (i = 1; i <= npts; i++){
		if (nbasis[i] != 0.){
			jbas = 4*mpts*(i-1);
		    for (j = 1; j <= mpts; j++){
/*				printf("i,j,jbas %d %d %d \n",i,j,jbas);*/
		    	if (mbasis[j] != 0.){
		    	    j1 = jbas + 4*(j-1) + 4;
/*					printf("in sumrbas j1 = %d \n",j1);*/
	    	    	sum = sum + b[j1]*nbasis[i]*mbasis[j];
				}
			}
		}
	}
	return(sum);
}
