<h2>Differential evaluations</h2>

Compile and run
<pre>
cc -O2 de36.c dj9.c
./a.out dj9.dat o
</pre>


<pre>
/***************************************************************
**                                                            **
**        D I F F E R E N T I A L     E V O L U T I O N       **
**                                                            **
** Program: de.c                                              **
** Version: 3.6                                               **
**                                                            **
** Authors: Dr. Rainer Storn                                  **
**          c/o ICSI, 1947 Center Street, Suite 600           **
**          Berkeley, CA 94707                                **
**          Tel.:   510-642-4274 (extension 192)              **
**          Fax.:   510-643-7684                              **
**          E-mail: storn@icsi.berkeley.edu                   **
**          WWW: http://http.icsi.berkeley.edu/~storn/        **
**          on leave from                                     **
**          Siemens AG, ZFE T SN 2, Otto-Hahn Ring 6          **
**          D-81739 Muenchen, Germany                         **
**          Tel:    636-40502                                 **
**          Fax:    636-44577                                 **
**          E-mail: rainer.storn@zfe.siemens.de               **
**                                                            **
**          Kenneth Price                                     **
**          836 Owl Circle                                    **
**          Vacaville, CA 95687                               **
**          E-mail: kprice@solano.community.net               **
**                                                            **
** This program implements some variants of Differential      **
** Evolution (DE) as described in part in the techreport      **
** tr-95-012.ps of ICSI. You can get this report either via   **
** ftp.icsi.berkeley.edu/pub/techreports/1995/tr-95-012.ps.Z  **
** or via WWW: http://http.icsi.berkeley.edu/~storn/litera.html*
** A more extended version of tr-95-012.ps is submitted for   **
** publication in the Journal Evolutionary Computation.       **
**                                                            **
** You may use this program for any purpose, give it to any   **
** person or change it according to your needs as long as you **
** are referring to Rainer Storn and Ken Price as the origi-  **
** nators of the the DE idea.                                 **
** If you have questions concerning DE feel free to contact   **
** us. We also will be happy to know about your experiences   **
** with DE and your suggestions of improvement.               **
**                                                            **
***************************************************************/
/**H*O*C**************************************************************
**                                                                  **
** No.!Version! Date ! Request !    Modification           ! Author **
** ---+-------+------+---------+---------------------------+------- **
**  1 + 3.1  +5/18/95+   -     + strategy DE/rand-to-best/1+  Storn **
**    +      +       +         + included                  +        **
**  1 + 3.2  +6/06/95+C.Fleiner+ change loops into memcpy  +  Storn **
**  2 + 3.2  +6/06/95+   -     + update comments           +  Storn **
**  1 + 3.3  +6/15/95+ K.Price + strategy DE/best/2 incl.  +  Storn **
**  2 + 3.3  +6/16/95+   -     + comments and beautifying  +  Storn **
**  3 + 3.3  +7/13/95+   -     + upper and lower bound for +  Storn **
**    +      +       +         + initialization            +        **
**  1 + 3.4  +2/12/96+   -     + increased printout prec.  +  Storn **
**  1 + 3.5  +5/28/96+   -     + strategies revisited      +  Storn **
**  2 + 3.5  +5/28/96+   -     + strategy DE/rand/2 incl.  +  Storn **
**  1 + 3.6  +8/06/96+ K.Price + Binomial Crossover added  +  Storn **
**  2 + 3.6  +9/30/96+ K.Price + cost variance output      +  Storn **
**  3 + 3.6  +9/30/96+   -     + alternative to ASSIGND    +  Storn **
**  4 + 3.6  +10/1/96+   -    + variable checking inserted +  Storn **
**  5 + 3.6  +10/1/96+   -     + strategy indic. improved  +  Storn **
**                                                                  **
***H*O*C*E***********************************************************/
</pre>