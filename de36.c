# include "de.h"

/*------------------------Globals---------------------------------------*/

long  rnd_uni_init;                 /* serves as a seed for rnd_uni()   */
double c[MAXPOP][MAXDIM], d[MAXPOP][MAXDIM];
double (*pold)[MAXPOP][MAXDIM], (*pnew)[MAXPOP][MAXDIM], (*pswap)[MAXPOP][MAXDIM];



/*---------Function declarations----------------------------------------*/

void  assignd(int D, double a[], double b[]);
double rnd_uni(long *idum);    /* uniform pseudo random number generator */
double extern evaluate(int D, double tmp[], long *nfeval); /* obj. funct. */

/*---------Function definitions-----------------------------------------*/

void  assignd(int D, double a[], double b[])
/**C*F****************************************************************
**                                                                  **
** Assigns D-dimensional vector b to vector a.                      **
** You might encounter problems with the macro ASSIGND on some      **
** machines. If yes, better use this function although it's slower. **
**                                                                  **
***C*F*E*************************************************************/
{
   int j;
   for (j=0; j<D; j++)
   {
      a[j] = b[j];
   }
}


double rnd_uni(long *idum)
/**C*F****************************************************************
**                                                                  **
** SRC-FUNCTION   :rnd_uni()                                        **
** LONG_NAME      :random_uniform                                   **
** AUTHOR         :(see below)                                      **
**                                                                  **
** DESCRIPTION    :rnd_uni() generates an equally distributed ran-  **
**                 dom number in the interval [0,1]. For further    **
**                 reference see Press, W.H. et alii, Numerical     **
**                 Recipes in C, Cambridge University Press, 1992.  **
**                                                                  **
** FUNCTIONS      :none                                             **
**                                                                  **
** GLOBALS        :none                                             **
**                                                                  **
** PARAMETERS     :*idum    serves as a seed value                  **
**                                                                  **
** PRECONDITIONS  :*idum must be negative on the first call.        **
**                                                                  **
** POSTCONDITIONS :*idum will be changed                            **
**                                                                  **
***C*F*E*************************************************************/
{
  long j;
  long k;
  static long idum2=123456789;
  static long iy=0;
  static long iv[NTAB];
  double temp;

  if (*idum <= 0)
  {
    if (-(*idum) < 1) *idum=1;
    else *idum = -(*idum);
    idum2=(*idum);
    for (j=NTAB+7;j>=0;j--)
    {
      k=(*idum)/IQ1;
      *idum=IA1*(*idum-k*IQ1)-k*IR1;
      if (*idum < 0) *idum += IM1;
      if (j < NTAB) iv[j] = *idum;
    }
    iy=iv[0];
  }
  k=(*idum)/IQ1;
  *idum=IA1*(*idum-k*IQ1)-k*IR1;
  if (*idum < 0) *idum += IM1;
  k=idum2/IQ2;
  idum2=IA2*(idum2-k*IQ2)-k*IR2;
  if (idum2 < 0) idum2 += IM2;
  j=iy/NDIV;
  iy=iv[j]-idum2;
  iv[j] = *idum;
  if (iy < 1) iy += IMM1;
  if ((temp=AM*iy) > RNMX) return RNMX;
  else return temp;

}/*------End of rnd_uni()--------------------------*/



int main(int argc, char *argv[])
/**C*F****************************************************************
**                                                                  **
** SRC-FUNCTION   :main()                                           **
** LONG_NAME      :main program                                     **
** AUTHOR         :Rainer Storn, Kenneth Price                      **
**                                                                  **
** DESCRIPTION    :driver program for differential evolution.       **
**                                                                  **
** FUNCTIONS      :rnd_uni(), evaluate(), printf(), fprintf(),      **
**                 fopen(), fclose(), fscanf().                     **
**                                                                  **
** GLOBALS        :rnd_uni_init    input variable for rnd_uni()     **
**                                                                  **
** PARAMETERS     :argc            #arguments = 3                   **
**                 argv            pointer to argument strings      **
**                                                                  **
** PRECONDITIONS  :main must be called with three parameters        **
**                 e.g. like de1 <input-file> <output-file>, if     **
**                 the executable file is called de1.               **
**                 The input file must contain valid inputs accor-  **
**                 ding to the fscanf() section of main().          **
**                                                                  **
** POSTCONDITIONS :main() produces consecutive console outputs and  **
**                 writes the final results in an output file if    **
**                 the program terminates without an error.         **
**                                                                  **
***C*F*E*************************************************************/

{
   char  chr;             /* y/n choice variable                */
   char  *strat[] =       /* strategy-indicator                 */
   {
            "",
            "DE/best/1/exp",
            "DE/rand/1/exp",
            "DE/rand-to-best/1/exp",
            "DE/best/2/exp",
            "DE/rand/2/exp",
            "DE/best/1/bin",
            "DE/rand/1/bin",
            "DE/rand-to-best/1/bin",
            "DE/best/2/bin",
            "DE/rand/2/bin"
   };

   int   i, j, L, n;      /* counting variables                 */
   int   r1, r2, r3, r4;  /* placeholders for random indexes    */
   int   r5;              /* placeholders for random indexes    */
   int   D;               /* Dimension of parameter vector      */
   int   NP;              /* number of population members       */
   int   imin;            /* index to member with lowest energy */
   int   refresh;         /* refresh rate of screen output      */
   int   strategy;        /* choice parameter for screen output */
   int   gen, genmax, seed;   

   long  nfeval;          /* number of function evaluations     */

   double trial_cost;      /* buffer variable                    */
   double inibound_h;      /* upper parameter bound              */
   double inibound_l;      /* lower parameter bound              */
   double tmp[MAXDIM], best[MAXDIM], bestit[MAXDIM]; /* members  */
   double cost[MAXPOP];    /* obj. funct. values                 */
   double cvar;            /* computes the cost variance         */
   double cmean;           /* mean cost                          */
   double F,CR;            /* control variables of DE            */
   double cmin;            /* help variables                     */

   FILE  *fpin_ptr;
   FILE  *fpout_ptr;

/*------Initializations----------------------------*/

 if (argc != 3)                                 /* number of arguments */
 {
    printf("\nUsage : de <input-file> <output-file>\n");
    exit(1);
 }

/*-----Read input data------------------------------------------------*/

 fpin_ptr   = fopen(argv[1],"r");

 if (fpin_ptr == NULL)
 {
    printf("\nCannot open input file\n");
    exit(1);                                 /* input file is necessary */
 }

 fscanf(fpin_ptr,"%d",&strategy);       /*---choice of strategy-----------------*/
 fscanf(fpin_ptr,"%d",&genmax);         /*---maximum number of generations------*/
 fscanf(fpin_ptr,"%d",&refresh);        /*---output refresh cycle---------------*/
 fscanf(fpin_ptr,"%d",&D);              /*---number of parameters---------------*/
 fscanf(fpin_ptr,"%d",&NP);             /*---population size.-------------------*/
 fscanf(fpin_ptr,"%lf",&inibound_h);    /*---upper parameter bound for init-----*/
 fscanf(fpin_ptr,"%lf",&inibound_l);    /*---lower parameter bound for init-----*/
 fscanf(fpin_ptr,"%lf",&F);             /*---weight factor----------------------*/
 fscanf(fpin_ptr,"%lf",&CR);            /*---crossing over factor---------------*/
 fscanf(fpin_ptr,"%d",&seed);           /*---random seed------------------------*/


 fclose(fpin_ptr);

/*-----Checking input variables for proper range----------------------------*/

  if (D > MAXDIM)
  {
     printf("\nError! D=%d > MAXDIM=%d\n",D,MAXDIM);
     exit(1);
  }
  if (D <= 0)
  {
     printf("\nError! D=%d, should be > 0\n",D);
     exit(1);
  }
  if (NP > MAXPOP)
  {
     printf("\nError! NP=%d > MAXPOP=%d\n",NP,MAXPOP);
     exit(1);
  }
  if (NP <= 0)
  {
     printf("\nError! NP=%d, should be > 0\n",NP);
     exit(1);
  }
  if ((CR < 0) || (CR > 1.0))
  {
     printf("\nError! CR=%f, should be ex [0,1]\n",CR);
     exit(1);
  }
  if (seed <= 0)
  {
     printf("\nError! seed=%d, should be > 0\n",seed);
     exit(1);
  }
  if (refresh <= 0)
  {
     printf("\nError! refresh=%d, should be > 0\n",refresh);
     exit(1);
  }
  if (genmax <= 0)
  {
     printf("\nError! genmax=%d, should be > 0\n",genmax);
     exit(1);
  }
  if ((strategy < 0) || (strategy > 10))
  {
     printf("\nError! strategy=%d, should be ex {1,2,3,4,5,6,7,8,9,10}\n",strategy);
     exit(1);
  }
  if (inibound_h < inibound_l)
  {
     printf("\nError! inibound_h=%f < inibound_l=%f\n",inibound_h, inibound_l);
     exit(1);
  }


/*-----Open output file-----------------------------------------------*/

   fpout_ptr   = fopen(argv[2],"w");  /* open output file for writing */

   if (fpout_ptr == NULL)
   {
      printf("\nCannot open output file\n");
      exit(1);
   }


/*-----Initialize random number generator-----------------------------*/

 rnd_uni_init = -(long)seed;  /* initialization of rnd_uni() */
 nfeval       =  0;  /* reset number of function evaluations */



/*------Initialization------------------------------------------------*/
/*------Right now this part is kept fairly simple and just generates--*/
/*------random numbers in the range [-initfac, +initfac]. You might---*/
/*------want to extend the init part such that you can initialize-----*/
/*------each parameter separately.------------------------------------*/

   for (i=0; i<NP; i++)
   {
      for (j=0; j<D; j++) /* spread initial population members */
      {
	 c[i][j] = inibound_l + rnd_uni(&rnd_uni_init)*(inibound_h - inibound_l);
      }
      cost[i] = evaluate(D,c[i],&nfeval); /* obj. funct. value */
   }
   cmin = cost[0];
   imin = 0;
   for (i=1; i<NP; i++)
   {
      if (cost[i]<cmin)
      {
	 cmin = cost[i];
	 imin = i;
      }
   }

   assignd(D,best,c[imin]);            /* save best member ever          */
   assignd(D,bestit,c[imin]);          /* save best member of generation */

   pold = &c; /* old population (generation G)   */
   pnew = &d; /* new population (generation G+1) */

/*=======================================================================*/
/*=========Iteration loop================================================*/
/*=======================================================================*/

   gen = 0;                          /* generation counter reset */
   while ((gen < genmax) /*&& (kbhit() == 0)*/) /* remove comments if conio.h */
   {                                            /* is accepted by compiler    */
      gen++;
      imin = 0;

      for (i=0; i<NP; i++)         /* Start of loop through ensemble  */
      {
	 do                        /* Pick a random population member */
	 {                         /* Endless loop for NP < 2 !!!     */
	   r1 = (int)(rnd_uni(&rnd_uni_init)*NP);
	 }while(r1==i);            

	 do                        /* Pick a random population member */
	 {                         /* Endless loop for NP < 3 !!!     */
	   r2 = (int)(rnd_uni(&rnd_uni_init)*NP);
	 }while((r2==i) || (r2==r1));

	 do                        /* Pick a random population member */
	 {                         /* Endless loop for NP < 4 !!!     */
	   r3 = (int)(rnd_uni(&rnd_uni_init)*NP);
	 }while((r3==i) || (r3==r1) || (r3==r2));

	 do                        /* Pick a random population member */
	 {                         /* Endless loop for NP < 5 !!!     */
	   r4 = (int)(rnd_uni(&rnd_uni_init)*NP);
	 }while((r4==i) || (r4==r1) || (r4==r2) || (r4==r3));

	 if (NP >= 6) {
	   do                        /* Pick a random population member */
	     {                         /* Endless loop for NP < 6 !!!     */
	       r5 = (int)(rnd_uni(&rnd_uni_init)*NP);
	     }while((r5==i) || (r5==r1) || (r5==r2) || (r5==r3) || (r5==r4));
	 }


/*=======Choice of strategy===============================================================*/
/*=======We have tried to come up with a sensible naming-convention: DE/x/y/z=============*/
/*=======DE :  stands for Differential Evolution==========================================*/
/*=======x  :  a string which denotes the vector to be perturbed==========================*/
/*=======y  :  number of difference vectors taken for perturbation of x===================*/
/*=======z  :  crossover method (exp = exponential, bin = binomial)=======================*/
/*                                                                                        */
/*=======There are some simple rules which are worth following:===========================*/
/*=======1)  F is usually between 0.5 and 1 (in rare cases > 1)===========================*/
/*=======2)  CR is between 0 and 1 with 0., 0.3, 0.7 and 1. being worth to be tried first=*/
/*=======3)  To start off NP = 10*D is a reasonable choice. Increase NP if misconvergence=*/
/*           happens.                                                                     */
/*=======4)  If you increase NP, F usually has to be decreased============================*/
/*=======5)  When the DE/best... schemes fail DE/rand... usually works and vice versa=====*/


/*=======EXPONENTIAL CROSSOVER============================================================*/

/*-------DE/best/1/exp--------------------------------------------------------------------*/
/*-------Our oldest strategy but still not bad. However, we have found several------------*/
/*-------optimization problems where misconvergence occurs.-------------------------------*/
	 if (strategy == 1) /* strategy DE0 (not in our paper) */
	 {
	   assignd(D,tmp,(*pold)[i]);
	   n = (int)(rnd_uni(&rnd_uni_init)*D);
	   L = 0;
	   do
	   {                       
	     tmp[n] = bestit[n] + F*((*pold)[r2][n]-(*pold)[r3][n]);
	     n = (n+1)%D;
	     L++;
	   }while((rnd_uni(&rnd_uni_init) < CR) && (L < D));
	 }
/*-------DE/rand/1/exp-------------------------------------------------------------------*/
/*-------This is one of my favourite strategies. It works especially well when the-------*/
/*-------"bestit[]"-schemes experience misconvergence. Try e.g. F=0.7 and CR=0.5---------*/
/*-------as a first guess.---------------------------------------------------------------*/
	 else if (strategy == 2) /* strategy DE1 in the techreport */
	 {
	   assignd(D,tmp,(*pold)[i]);
	   n = (int)(rnd_uni(&rnd_uni_init)*D);
	   L = 0;
	   do
	   {                       
	     tmp[n] = (*pold)[r1][n] + F*((*pold)[r2][n]-(*pold)[r3][n]);
	     n = (n+1)%D;
	     L++;
	   }while((rnd_uni(&rnd_uni_init) < CR) && (L < D));
	 }
/*-------DE/rand-to-best/1/exp-----------------------------------------------------------*/
/*-------This strategy seems to be one of the best strategies. Try F=0.85 and CR=1.------*/
/*-------If you get misconvergence try to increase NP. If this doesn't help you----------*/
/*-------should play around with all three control variables.----------------------------*/
	 else if (strategy == 3) /* similiar to DE2 but generally better */
	 { 
	   assignd(D,tmp,(*pold)[i]);
	   n = (int)(rnd_uni(&rnd_uni_init)*D); 
	   L = 0;
	   do
	   {                       
	     tmp[n] = tmp[n] + F*(bestit[n] - tmp[n]) + F*((*pold)[r1][n]-(*pold)[r2][n]);
	     n = (n+1)%D;
	     L++;
	   }while((rnd_uni(&rnd_uni_init) < CR) && (L < D));
	 }
/*-------DE/best/2/exp is another powerful strategy worth trying--------------------------*/
	 else if (strategy == 4)
	 { 
	   assignd(D,tmp,(*pold)[i]);
	   n = (int)(rnd_uni(&rnd_uni_init)*D); 
	   L = 0;
	   do
	   {                           
	     tmp[n] = bestit[n] + 
		      ((*pold)[r1][n]+(*pold)[r2][n]-(*pold)[r3][n]-(*pold)[r4][n])*F;
	     n = (n+1)%D;
	     L++;
	   }while((rnd_uni(&rnd_uni_init) < CR) && (L < D));
	 }
/*-------DE/rand/2/exp seems to be a robust optimizer for many functions-------------------*/
	 else if (strategy == 5)
	 { 
	   assignd(D,tmp,(*pold)[i]);
	   n = (int)(rnd_uni(&rnd_uni_init)*D); 
	   L = 0;
	   do
	   {                           
	     tmp[n] = (*pold)[r5][n] + 
		      ((*pold)[r1][n]+(*pold)[r2][n]-(*pold)[r3][n]-(*pold)[r4][n])*F;
	     n = (n+1)%D;
	     L++;
	   }while((rnd_uni(&rnd_uni_init) < CR) && (L < D));
	 }

/*=======Essentially same strategies but BINOMIAL CROSSOVER===============================*/

/*-------DE/best/1/bin--------------------------------------------------------------------*/
	 else if (strategy == 6) 
	 {
	   assignd(D,tmp,(*pold)[i]);
	   n = (int)(rnd_uni(&rnd_uni_init)*D); 
           for (L=0; L<D; L++) /* perform D binomial trials */
           {
	     if ((rnd_uni(&rnd_uni_init) < CR) || L == (D-1)) /* change at least one parameter */
	     {                       
	       tmp[n] = bestit[n] + F*((*pold)[r2][n]-(*pold)[r3][n]);
	     }
	     n = (n+1)%D;
           }
	 }
/*-------DE/rand/1/bin-------------------------------------------------------------------*/
	 else if (strategy == 7) 
	 {
	   assignd(D,tmp,(*pold)[i]);
	   n = (int)(rnd_uni(&rnd_uni_init)*D); 
           for (L=0; L<D; L++) /* perform D binomial trials */
           {
	     if ((rnd_uni(&rnd_uni_init) < CR) || L == (D-1)) /* change at least one parameter */
	     {                       
	       tmp[n] = (*pold)[r1][n] + F*((*pold)[r2][n]-(*pold)[r3][n]);
	     }
	     n = (n+1)%D;
           }
	 }
/*-------DE/rand-to-best/1/bin-----------------------------------------------------------*/
	 else if (strategy == 8) 
	 { 
	   assignd(D,tmp,(*pold)[i]);
	   n = (int)(rnd_uni(&rnd_uni_init)*D); 
           for (L=0; L<D; L++) /* perform D binomial trials */
           {
	     if ((rnd_uni(&rnd_uni_init) < CR) || L == (D-1)) /* change at least one parameter */
	     {                       
	       tmp[n] = tmp[n] + F*(bestit[n] - tmp[n]) + F*((*pold)[r1][n]-(*pold)[r2][n]);
	     }
	     n = (n+1)%D;
           }
	 }
/*-------DE/best/2/bin--------------------------------------------------------------------*/
	 else if (strategy == 9)
	 { 
	   assignd(D,tmp,(*pold)[i]);
	   n = (int)(rnd_uni(&rnd_uni_init)*D); 
           for (L=0; L<D; L++) /* perform D binomial trials */
           {
	     if ((rnd_uni(&rnd_uni_init) < CR) || L == (D-1)) /* change at least one parameter */
	     {                       
	       tmp[n] = bestit[n] + 
		      ((*pold)[r1][n]+(*pold)[r2][n]-(*pold)[r3][n]-(*pold)[r4][n])*F;
	     }
	     n = (n+1)%D;
           }
	 }
/*-------DE/rand/2/bin--------------------------------------------------------------------*/
	 else
	 { 
	   assignd(D,tmp,(*pold)[i]);
	   n = (int)(rnd_uni(&rnd_uni_init)*D); 
           for (L=0; L<D; L++) /* perform D binomial trials */
           {
	     if ((rnd_uni(&rnd_uni_init) < CR) || L == (D-1)) /* change at least one parameter */
	     {                       
	       tmp[n] = (*pold)[r5][n] + 
		      ((*pold)[r1][n]+(*pold)[r2][n]-(*pold)[r3][n]-(*pold)[r4][n])*F;
	     }
	     n = (n+1)%D;
           }
	 }


/*=======Trial mutation now in tmp[]. Test how good this choice really was.==================*/

	 trial_cost = evaluate(D,tmp,&nfeval);  /* Evaluate new vector in tmp[] */

	 if (trial_cost <= cost[i])   /* improved objective function value ? */
	 {                                  
	    cost[i]=trial_cost;         
	    assignd(D,(*pnew)[i],tmp);
	    if (trial_cost<cmin)          /* Was this a new minimum? */
	    {                               /* if so...*/
	       cmin=trial_cost;           /* reset cmin to new low...*/
	       imin=i;
	       assignd(D,best,tmp);           
	    }                           
	 }                            
	 else
	 {
	    assignd(D,(*pnew)[i],(*pold)[i]); /* replace target with old value */
	 }
      }   /* End mutation loop through pop. */
					   
      assignd(D,bestit,best);  /* Save best population member of current iteration */

      /* swap population arrays. New generation becomes old one */

      pswap = pold;
      pold  = pnew;
      pnew  = pswap;

/*----Compute the energy variance (just for monitoring purposes)-----------*/

      cmean = 0.;          /* compute the mean value first */
      for (j=0; j<NP; j++)
      {
         cmean += cost[j];
      }
      cmean = cmean/NP;

      cvar = 0.;           /* now the variance              */
      for (j=0; j<NP; j++)
      {
         cvar += (cost[j] - cmean)*(cost[j] - cmean);
      }
      cvar = cvar/(NP-1);


/*----Output part----------------------------------------------------------*/

      if (gen%refresh==1)   /* display after every refresh generations */
      { /* ABORT works only if conio.h is accepted by your compiler */
	printf("\n\n                         PRESS ANY KEY TO ABORT"); 
	printf("\n\n\n Best-so-far cost funct. value=%-15.10g\n",cmin);

	for (j=0;j<D;j++)
	{
	  printf("\n best[%d]=%-15.10g",j,best[j]);
	}
	printf("\n\n Generation=%d  NFEs=%ld   Strategy: %s    ",gen,nfeval,strat[strategy]);
	printf("\n NP=%d    F=%-4.2g    CR=%-4.2g   cost-variance=%-10.5g\n",
               NP,F,CR,cvar);
      }

      fprintf(fpout_ptr,"%ld   %-15.10g\n",nfeval,cmin);
   }
/*=======================================================================*/
/*=========End of iteration loop=========================================*/
/*=======================================================================*/

/*-------Final output in file-------------------------------------------*/


   fprintf(fpout_ptr,"\n\n\n Best-so-far obj. funct. value = %-15.10g\n",cmin);

   for (j=0;j<D;j++)
   {
     fprintf(fpout_ptr,"\n best[%d]=%-15.10g",j,best[j]);
   }
   fprintf(fpout_ptr,"\n\n Generation=%d  NFEs=%ld   Strategy: %s    ",gen,nfeval,strat[strategy]);
   fprintf(fpout_ptr,"\n NP=%d    F=%-4.2g    CR=%-4.2g    cost-variance=%-10.5g\n",
           NP,F,CR,cvar); 

   fclose(fpout_ptr);

   return(0);
}

/*-----------End of main()------------------------------------------*/





