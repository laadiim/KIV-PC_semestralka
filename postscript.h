#ifndef __POSTSCRIPT__
#define __POSTSCRIPT__

const double GRAPH_WIDTH = 1500;
const double GRAPH_HEIGHT = 1100;
const double PAGE_WIDTH = 1600;
const double PAGE_HEIGHT = 1200;
const double OFFSET = 100;

/* draws a graph into a postscript file
 * returns
 * 0 if succesfull, 1 otherwise*/
int create_postscript(double *x, double *y, int length, double x_min, double x_max, double y_min, double y_max, char *outpath);

#endif
