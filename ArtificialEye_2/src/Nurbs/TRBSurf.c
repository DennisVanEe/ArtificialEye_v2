/*
	Test program for C code from An Introduction to NURBS
	by David F. Rogers. Copyright (C) 2000 David F. Rogers,
	All rights reserved.
	
	Name: trbsurf.c
	Purpose: Test rational B-spline surface generator
	Language: C
	Subroutines called: rbspsurf.c
	Book reference: Chapter 7, Section 7.1, Alg. p 308
*/

	main(){

	int i;
	int npts,mpts;
	int k,l;
	int p1,p2;

	float b[66];
	float q[701];

	char header[80];
    int hdrlen;
/*
	Data for the standard test control net.
	Comment out to use file input.
*/
	npts = 4;
	mpts = 4;
	k = 3;
	l = 3;

	p1 = 5;
	p2 = 5;

/*	printf("k,l,npts,mpts,p1,p2 = %d %d %d %d %d %d \n",k,l,npts,mpts,p1,p2);*/

	for (i = 1; i <= 4*npts; i++){
		b[i] = 0.;
	}

	for (i = 1; i <= 3*p1*p2; i++){
		q[i] = 0.;
	}

/*
	This is a standard test control polygon from Ex. 6.1, p. 184
	Comment out to use file input <filename>.rnp. Data is in the
	form x=b[1], y=b[2], z=b[3], h=b[4], etc. All h are 1.0.
	Thus, this is a nonrational B-spline surface and the results
	should be the same as for tbsurf.c
*/
	b[1] = -15.;
	b[2] = 0.;
	b[3] = 15.;
	b[4] = 1;
	b[5] = -15.;
	b[6] = 5.;
	b[7] = 5.;
	b[8] = 1;
	b[9] = -15.;
	b[10] = 5.;
	b[11] = -5.;
	b[12] = 1;
	b[13] = -15.;
	b[14] = 0.;
	b[15] = -15.;
	b[16] = 1;

	b[17] = -5.;
	b[18] = 5.;
	b[19] = 15.;
	b[20] = 1;
	b[21] = -5.;
	b[22] = 10.;
	b[23] = 5.;
	b[24] = 1;
	b[25] = -5.;
	b[26] = 10.;
	b[27] = -5.;
	b[28] = 1;
	b[29] = -5.;
	b[30] = 5.;
	b[31] = -15.;
	b[32] = 1;

	b[33] = 5.;
	b[34] = 5.;
	b[35] = 15.;
	b[36] = 1;
	b[37] = 5.;
	b[38] = 10.;
	b[39] = 5.;
	b[40] = 1;
	b[41] = 5.;
	b[42] = 10.;
	b[43] = -5.;
	b[44] = 1;
	b[45] = 5.;
	b[46] = 0.;
	b[47] = -15.;
	b[48] = 1;

	b[49] = 15.;
	b[50] = 0.;
	b[51] = 15.;
	b[52] = 1;
	b[53] = 15.;
	b[54] = 5.;
	b[55] = 5.;
	b[56] = 1;
	b[57] = 15.;
	b[58] = 5.;
	b[59] = -5.;
	b[60] = 1;
	b[61] = 15.;
	b[62] = 0.;
	b[63] = -15.;
	b[64] = 1;

/*
	Uncomment the file input statement below and comment out the standard
    control net data above to use file input. Also needs to be compiled
	with the file rdrnp.c
*/
/*  get polygon netpoint file */
/*    rdrnp(header,&hdrlen,&k,&l,&npts,&mpts,b); */

	rbspsurf(b,k,l,npts,mpts,p1,p2,q);

/*	Use the next two lines for getting performance timings
    and comment out the line above.
*/
/*	for (i = 1; i<=1000; i++){ 
	rbspsurf(b,k,l,npts,mpts,p1,p2,q);
	}
*/

	printf("\nPolygon points\n\n");

	for (i = 1; i <= 4*npts*mpts; i=i+4){
		printf(" %f %f %f %f\n",b[i],b[i+1],b[i+2],b[i+3]);
	}

	printf("\nSurface points\n\n");

	for (i = 1; i <= 3*p1*p2; i=i+3){
		printf("%f, %f, %f \n",q[i],q[i+1],q[i+2]);
	}
	
/*	To get output in the format of an sgf file
	remove the comment and compile with sgfout.c
*/
/*
	sgfout(p1,p2,q);
*/
}
