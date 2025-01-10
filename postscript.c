#include <stdio.h>
#include "postscript.h"


/* Function to create a PostScript file plotting points in the true coordinate system */
int create_postscript(double *x, double *y, int length, double x_min, double x_max, double y_min, double y_max, char *outpath)
{
    /* File pointer */
    FILE *out;

    int i = 0;
    int num_ticks = 10;

    double x_next, y_next;
    double x_point, y_point;
    double x_tick_length = GRAPH_WIDTH - OFFSET;
		double y_tick_length = GRAPH_HEIGHT - OFFSET;
    double len_x = (x_max - x_min) / 10.0;
    double len_y = (y_max - y_min) / 10.0;
    double label_scale = 0;
		double x_tick, y_tick;
    double modif_scale_x = 0;
    double modif_scale_y = 0;

    /* Open the output file */
    out = fopen(outpath, "w");
    if (out == NULL) 
    {
        perror("Could not output file.");
        return 1;
    }

    /* file header */
    fprintf(out, "%%!PS-Adobe-3.0\n");
    fprintf(out, "%%%%Creator: Math function visualizer\n");
    fprintf(out, "%%%%Title: Graph of function\n");
    fprintf(out, "%%%%Pages: 1\n");
    fprintf(out, "%%%%EndComments\n");
    fprintf(out, "<< /PageSize [%.0f %.0f] >> setpagedevice\n", PAGE_WIDTH, PAGE_HEIGHT);

    
    /* making grid */
    for (i = 0; i <= num_ticks; i++) 
    {
        x_tick = OFFSET + i * (GRAPH_WIDTH - OFFSET) / num_ticks;
        fprintf(out, "%.2f %.2f moveto %.2f %.2f lineto stroke\n", x_tick, OFFSET, x_tick, OFFSET + y_tick_length);
    }
    for (i = 0; i <= num_ticks; i++) 
    {
        y_tick = OFFSET + i * (GRAPH_HEIGHT - OFFSET) / num_ticks;
        fprintf(out, "%.2f %.2f moveto %.2f %.2f lineto stroke\n", OFFSET, y_tick, OFFSET + x_tick_length, y_tick);
    }

    
    /* setting font */
    fprintf(out, "/Helvetica findfont\n%.2f scalefont\nsetfont\n", 30.0);    

    /* bottom left x corner label */
    fprintf(out, "%.2f %.2f moveto (%.2f) show\n", OFFSET, OFFSET / 1.5, x_min);

    /* bottom right x corner label */
    fprintf(out, "%.2f %.2f moveto (%.2f) show\n", GRAPH_WIDTH - OFFSET, OFFSET / 1.5, x_max);

    /* bottom left y corner label */
    fprintf(out, "%.2f %.2f moveto (%.2f) show\n", OFFSET / 7.0, OFFSET, y_min);

    /* top left y corner label */
    fprintf(out, "%.2f %.2f moveto (%.2f) show\n", OFFSET / 7.0, GRAPH_HEIGHT - OFFSET / 2.0, y_max);
    

    x_point = x[0];
    y_point = y[0];

    /* moving to the x_point and y_point */
    fprintf(out, "newpath\n 4 setlinewidth\n %.2f %.2f moveto\n", x_point, y_point);

    /* writing the function */
    for (i = 1; i < length; i++) 
    {
        x_next = x[i];
        y_next = y[i];
        
        if (y_next > GRAPH_HEIGHT || y_next < OFFSET || y_point > GRAPH_HEIGHT || y_point < OFFSET)
        {
            fprintf(out, "stroke\nnewpath\n4 setlinewidth\n%.2f %.2f moveto\n", x_next, y_next);
        }
            
        else
        {
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

    return 0;
}
