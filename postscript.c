#include "postscript.h"
#include <stdio.h>

const double GRAPH_WIDTH = 1400;
const double GRAPH_HEIGHT = 1000;
const double PAGE_WIDTH = 1600;
const double PAGE_HEIGHT = 1200;
const double OFFSET = 100;

double transform_x(double x, double x_min, double x_max) {
  return (x - x_min) * (GRAPH_WIDTH / (x_max - x_min)) + OFFSET;
}

double transform_y(double y, double y_min, double y_max) {
  return (y - y_min) * (GRAPH_HEIGHT / (y_max - y_min)) + OFFSET;
}
/* Function to create a PostScript file plotting points in the true coordinate
 * system */
int create_postscript(double *x, double *y, int length, double x_min,
                      double x_max, double y_min, double y_max, char *outpath) {
  /* File pointer */
  FILE *out;

  int i = 0;
  int num_ticks = 10;
  double a = 0;
  double b = 0;

  double x_next, y_next;
  double x_point, y_point;
  double x_tick_length = GRAPH_WIDTH;
  double y_tick_length = GRAPH_HEIGHT;
  double x_tick, y_tick;

  /* Open the output file */
  out = fopen(outpath, "w");
  if (out == NULL) {
    perror("Could not output file.");
    return 0;
  }

  /* file header */
  fprintf(out, "%%!PS-Adobe-3.0\n");
  fprintf(out, "%%%%Creator: Math function visualizer\n");
  fprintf(out, "%%%%Title: Graph of function\n");
  fprintf(out, "%%%%Pages: 1\n");
  fprintf(out, "%%%%EndComments\n");
  fprintf(out, "<< /PageSize [%.0f %.0f] >> setpagedevice\n", PAGE_WIDTH,
          PAGE_HEIGHT);

  /* making grid */
  for (i = 0; i <= num_ticks; i++) {
    x_tick = OFFSET + i * (GRAPH_WIDTH) / num_ticks;
    fprintf(out, "%.2f %.2f moveto %.2f %.2f lineto stroke\n", x_tick, OFFSET,
            x_tick, OFFSET + y_tick_length);
  }
  for (i = 0; i <= num_ticks; i++) {
    y_tick = OFFSET + i * (GRAPH_HEIGHT) / num_ticks;
    fprintf(out, "%.2f %.2f moveto %.2f %.2f lineto stroke\n", OFFSET, y_tick,
            OFFSET + x_tick_length, y_tick);
  }

  /* setting font */
  fprintf(out, "/Helvetica findfont\n%.2f scalefont\nsetfont\n", 30.0);

  /* bottom left x corner label */
  fprintf(out, "%.2f %.2f moveto (%.2f) show\n", OFFSET, OFFSET / 1.5, x_min);

  /* bottom right x corner label */
  fprintf(out, "%.2f %.2f moveto (%.2f) show\n", GRAPH_WIDTH - OFFSET / 2.0 + OFFSET,
          OFFSET / 1.5, x_max);

  /* bottom left y corner label */
  fprintf(out, "%.2f %.2f moveto (%.2f) show\n", OFFSET / 7.0, OFFSET, y_min);

  /* top left y corner label */
  fprintf(out, "%.2f %.2f moveto (%.2f) show\n", OFFSET / 7.0,
          GRAPH_HEIGHT + OFFSET, y_max);

  fprintf(out, "%.2f %.2f moveto (x) show\n", OFFSET + GRAPH_WIDTH / 2.0,
          OFFSET / 2.0);
  fprintf(out, "%.2f %.2f moveto (f(x)) show\n", OFFSET / 2.0,
          OFFSET + GRAPH_HEIGHT / 2.0);


	if (x_min <= 0 && x_max >= 0)
	{
		fprintf(out, "6 setlinewidth 0 0 1 setrgbcolor\n%.2f %.2f moveto %.2f %.2f lineto stroke\n", transform_x(0, x_min, x_max), OFFSET, transform_x(0, x_min, x_max), OFFSET + GRAPH_HEIGHT);
		fprintf(out, "%.2f %.2f moveto (0) show\n", transform_x(0, x_min, x_max), OFFSET + GRAPH_HEIGHT + 30);
	}
	if (y_min <= 0 && y_max >= 0)
	{
		fprintf(out, "0 0 1 setrgbcolor\n%.2f %.2f moveto %.2f %.2f lineto stroke\n", OFFSET, transform_y(0, y_min, y_max), OFFSET + GRAPH_WIDTH, transform_y(0, y_min, y_max));
		fprintf(out, "%.2f %.2f moveto (0) show\n", OFFSET + GRAPH_WIDTH + 30, transform_y(0, y_min, y_max));
	}

  x_point = transform_x(x[0], x_min, x_max);
  y_point = transform_y(y[0], y_min, y_max);

  /* moving to the x_point and y_point */
  fprintf(out, "newpath\n 4 setlinewidth\n %.2f %.2f moveto\n", x_point,
          y_point);

  fprintf(out, "1 0 0 setrgbcolor\n");
  /* writing the function */
  for (i = 1; i < length; i++) {
    x_next = transform_x(x[i], x_min, x_max);
    y_next = transform_y(y[i], y_min, y_max);

    if (y_next > GRAPH_HEIGHT + OFFSET || y_next < OFFSET || y_point > GRAPH_HEIGHT + OFFSET ||
        y_point < OFFSET) {
      a = (y_next - y_point) / (x_next - x_point);
      b = y_point - a * x_point;
      if (y_next > GRAPH_HEIGHT + OFFSET && y_point < GRAPH_HEIGHT + OFFSET && y_point > OFFSET) 
			{
        fprintf(out, "%.2f %.2f lineto %.2f %.2f moveto\n", (GRAPH_HEIGHT + OFFSET - b) / a, GRAPH_HEIGHT + OFFSET, x_next, y_next);
      } 
			else if (y_next < OFFSET && y_point < GRAPH_HEIGHT + OFFSET && y_point > OFFSET) 
			{
        fprintf(out, "%.2f %.2f lineto %.2f %.2f moveto\n", (OFFSET - b) / a, OFFSET, x_next, y_next);
      } 
			else if (y_point > GRAPH_HEIGHT + OFFSET && y_next < GRAPH_HEIGHT + OFFSET && y_next > OFFSET) 
			{
        fprintf(out, "%.2f %.2f moveto %.2f %.2f lineto\n", (GRAPH_HEIGHT + OFFSET - b) / a, GRAPH_HEIGHT + OFFSET, x_next, y_next);
      }
			else if (y_point < OFFSET && y_next < GRAPH_HEIGHT + OFFSET && y_next > OFFSET)
			{
        fprintf(out, "%.2f %.2f moveto %.2f %.2f lineto\n", (OFFSET - b) / a, OFFSET, x_next, y_next);
      }
			else 
			{
        fprintf(out, "%.2f %.2f moveto\n", x_next, y_next);
      }

    } 
		else {
      fprintf(out, "%.2f %.2f lineto\n", x_next, y_next);
    }

    x_point = x_next;
    y_point = y_next;
  }

  /* drawing the last line */
  fprintf(out, "stroke\n");

  /* showing the page and closing file */
  fprintf(out, "showpage\n");
  fclose(out);

  return 1;
}
