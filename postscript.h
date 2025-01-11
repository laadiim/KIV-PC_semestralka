#ifndef __POSTSCRIPT__
#define __POSTSCRIPT__

/* draws a graph into a postscript file
 * returns
 * 0 if succesfull, 1 otherwise*/
int create_postscript(double *x, double *y, int length, double x_min, double x_max, double y_min, double y_max, char *outpath);

#endif
