#include <stdio.h>

#define PS_WIDTH 560
#define PS_HEIGHT 560
#define NOTANUMBER 100001
#define MAX 1000
#define MIN -1000

/* Function to create a PostScript file plotting points in the true coordinate system */
int create_postscript(double *x, int x_length, double *y, int y_length, double x_min, double x_max, double y_min, double y_max, char *outpath) {
    /* File pointer */
    FILE *out;
    double scale_x, scale_y;
    double x_offset, y_offset;
    double x_next, y_next;
    int num_ticks_x = 10;
    int num_ticks_y = 10;
    int i = 0;
    double x_point, y_point;
    double x_tick_length = x_max - x_min;
		double y_tick_length = y_max - y_min;
		double x_tick, y_tick;

    /* Open the output file */
    out = fopen(outpath, "w");
    if (out == NULL) {
        perror("Error opening output file");
        return -1;
    }
    scale_x = PS_WIDTH / (x_max - x_min);
    scale_y = PS_HEIGHT / (y_max - y_min);

    x_offset = (x_min + x_max) / 2.0 * scale_x;
    y_offset = (y_min + y_max) / 2.0 * scale_y;

    fprintf(out, "%%!PS-Adobe-3.0\n");
    fprintf(out, "%%%%Creator: C Function Graph Generator\n");
    fprintf(out, "%%%%Title: Function Graph\n");
    fprintf(out, "%%%%Pages: 1\n");
    fprintf(out, "%%%%EndComments\n");
    fprintf(out, "320 420 translate\n");
    fprintf(out, "%.2f %.2f translate\n", -x_offset, -y_offset);
    fprintf(out, "%.2f %.2f scale\n", scale_x, scale_y);

    fprintf(out, "%.2f %.2f moveto %.2f %.2f lineto %.2f %.2f lineto 0 setlinewidth stroke\n", x_min, y_max, x_min, y_min, x_max, y_min);

    for (i = 0; i <= num_ticks_x; i++) {
        x_tick = x_min + i * (x_max - x_min) / num_ticks_x;
        fprintf(out, "%.2f %.2f moveto %.2f %.2f lineto stroke\n", x_tick, y_min, x_tick, y_min + x_tick_length);
    }

    for (i = 0; i <= num_ticks_y; i++) {
        y_tick = y_min + i * (y_max - y_min) / num_ticks_y;
        fprintf(out, "%.2f %.2f moveto %.2f %.2f lineto stroke\n", x_min, y_tick, x_min + y_tick_length, y_tick);
    }
		fprintf(out, "/Helvetica findfont\n1 scalefont\nsetfont\n");    

		fprintf(out, "%.2f %.2f moveto (%.2f) show\n", x_min, y_min - 1, x_min);
		fprintf(out, "%.2f %.2f moveto (%.2f) show\n", x_max - 2, y_min - 1, x_max);

		fprintf(out, "90 rotate\n%.2f %.2f moveto (%.2f) show\n-90 rotate\n", x_min, y_max + 0.1, y_min);
		fprintf(out, "90 rotate\n%.2f %.2f moveto (%.2f) show\n-90 rotate\n", x_max - 2, y_max + 0.1, y_max);

    x_point = x[0];
    y_point = y[0];
    fprintf(out, "newpath\n 1 16 div setlinewidth\n %.2f %.2f moveto\n", x_point, y_point);
    for (i = 1; i < x_length; i++) {
        x_next = x[i];
        y_next = y[i];
        if (y_next >= y_max || y_next <= y_min || y_point >= y_max || y_point <= y_min)
            fprintf(out, "stroke\nnewpath\n1 16 div setlinewidth\n%.2f %.2f moveto\n", x_next, y_next);
        else
            fprintf(out, "%.2f %.2f lineto\n", x_next, y_next);
        x_point = x_next;
        y_point = y_next;
    }
    fprintf(out, "stroke\n");

    fprintf(out, "showpage\n");

    fclose(out);

    return 0;
}
