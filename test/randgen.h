/* 
 * File:   zipf.h
 * Author: leo
 *
 * Created on November 20, 2013, 9:17 PM
 */

#ifndef ZIPF_H
#define	ZIPF_H

#ifdef	__cplusplus
extern "C" {
#endif

#define  FALSE          0       // Boolean false
#define  TRUE           1       // Boolean true

//----- Function prototypes -------------------------------------------------
int      zipf(double alpha, int n);  // Returns a Zipf random variable
double   rand_val(int seed);         // Jain's RNG
double expon(double t);
double rand_prob_vector(double cand_num[], double prob_vector[],int size);


#ifdef	__cplusplus
}
#endif

#endif	/* ZIPF_H */

