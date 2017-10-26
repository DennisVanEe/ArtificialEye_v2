/*  Subroutine to calculate a Cartesian product rational B-spline
    surface using open uniform knot vectors (see Eq. (7.1)).

	C code for An Introduction to NURBS
	by David F. Rogers. Copyright (C) 2000 David F. Rogers,
	All rights reserved.
	
	Name: rbspsurf.c
	Language: C
	Subroutines called: knot.c, basis.c sumrbas.c
	Book reference: Chapter 7, Section 7.1, Alg. p. 308

    b[]         = array containing the polygon net points
                  b[1] = x-component
                  b[2] = y-component
                  b[3] = z-component
                  b[4] = h-component
                  Note: Bi,j = b[] has dimensions of n*m*3 with j varying fastest
                      The polygon net is n x m
    k           = order in the u direction
    l           = order in the w direction
    mbasis[]    = array containing the nonrational basis functions for one value of w (see Eq. (3.2))
    mpts        = the number of polygon vertices in w direction
    nbasis[]    = array containing the nonrational basis functions for one value of u (see Eq. (3.2))
    npts        = the number of polygon vertices in u direction
    p1          = number of parametric lines in the u direction
    p2          = number of parametric lines in the w direction
    q[]         = array containing the resulting surface
                  q[1] = x-component
                  q[2] = y-component
                  q[3] = z-component
                       for a fixed value of u the next m elements contain
                       the values for the curve q[u[sub i],w] q has dimensions
                       of p1*p2*3. The display surface is p1 x p2
*/
#include <stdio.h>

rbspsurf(b,k,l,npts,mpts,p1,p2,q)

int k,l;
int npts,mpts;
int p1,p2;

float b[],q[];

{

/*   allows for 20 data points with basis function of order 5 */

    int i,j,j1,jbas;
    int icount;
    int uinc,winc;
    int nplusc,mplusc;
    int x[30],y[30];
    int temp;

    float nbasis[30],mbasis[30];
    float pbasis;
    float h;
    float sum, sumrbas();
    float u,w;
    float stepu,stepw;
        
/*  printf("in bsplsurf.c \n"); */

/*    zero and redimension the arrays */

    nplusc = npts + k;
    mplusc = mpts + l;    

    for (i = 1; i <= nplusc; i++){
        x[i] = 0;
    }
    for (i = 1; i <= mplusc; i++){
        y[i] = 0;
    }
    for (i = 1; i <= npts; i++){
        nbasis[i] = 0.;
    }
    for (i = 1; i <= mpts; i++){
        mbasis[i] = 0.;
    }

    temp = 3*(p1*p2);

    for (i = 1; i <= 3*p1*p2; i++){
        q[i] = 0.;
    }

/*   generate the open uniform knot vectors */

    knot(npts,k,x);       /*  calculate u knot vector */
    knot(mpts,l,y);       /*  calculate w knot vector */

    icount = 1;

/*    calculate the points on the B-spline surface */

    stepu = (float)x[nplusc]/(float)(p1-1);
    stepw = (float)y[mplusc]/(float)(p2-1);
    u = 0.;
    for (uinc = 1; uinc <= p1; uinc++){
        if ((float)x[nplusc] - u < 5e-6){
            u = (float)x[nplusc];
        }
        basis(k,u,npts,x,nbasis);    /* basis function for this value of u */
        w = 0.;
        for (winc = 1; winc <= p2; winc++){
            if ((float)y[mplusc] - w < 5e-6){
                w = (float)y[mplusc];
            }
            basis(l,w,mpts,y,mbasis);    /* basis function for this value of w */
            sum = sumrbas(b,nbasis,mbasis,npts,mpts);
/*        	printf("in rbspsurf u,w,sum = %f %f %f \n",u,w,sum);*/
            for (i = 1; i <= npts; i++){
                if (nbasis[i] != 0.){
                jbas = 4*mpts*(i-1);
                for (j = 1; j <= mpts; j++){
                    if (mbasis[j] != 0.){
                        j1 = jbas +4*(j-1) + 1;
                        pbasis = b[j1+3]*nbasis[i]*mbasis[j]/sum;
                        q[icount] = q[icount]+b[j1]*pbasis;  /* calculate surface point */
                        q[icount+1] = q[icount+1]+b[j1+1]*pbasis;
                        q[icount+2] = q[icount+2]+b[j1+2]*pbasis;
                    }
                }
                }
            }
            icount = icount + 3;
            w = w + stepw;
        }
        u = u + stepu;
    }
}
