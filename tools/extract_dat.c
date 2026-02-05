/* Extract bitmaps from an Allegro 4 .dat file and save as BMP files.
   Build: cc -o extract_dat extract_dat.c -I../deps/allegro-legacy/include \
          -L../deps/allegro-legacy/build/lib -lalleg -framework Cocoa
   Usage: ./extract_dat path/to/file.dat output_dir/
*/
#include <allegro.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <datfile> <output_dir>\n", argv[0]);
        return 1;
    }

    const char *datfile = argv[1];
    const char *outdir = argv[2];

    allegro_init();
    set_color_depth(24);

    mkdir(outdir, 0755);

    DATAFILE *dat = load_datafile(datfile);
    if (!dat) {
        fprintf(stderr, "Failed to load datafile: %s\n", datfile);
        return 1;
    }

    for (int i = 0; dat[i].type != DAT_END; i++) {
        const char *name = get_datafile_property(&dat[i], DAT_ID('N','A','M','E'));
        if (!name) name = "UNKNOWN";

        printf("Object %d: type=%.4s name=%s", i, (char*)&dat[i].type, name);

        if (dat[i].type == DAT_BITMAP || dat[i].type == DAT_ID('B','M','P',' ')) {
            BITMAP *bmp = (BITMAP*)dat[i].dat;
            if (bmp) {
                char path[512];
                /* Convert name to lowercase for filename */
                char lname[256];
                int j;
                for (j = 0; name[j] && j < 255; j++)
                    lname[j] = (name[j] >= 'A' && name[j] <= 'Z') ? name[j] + 32 : name[j];
                lname[j] = 0;

                snprintf(path, sizeof(path), "%s/%s.bmp", outdir, lname);
                save_bitmap(path, bmp, NULL);
                printf(" -> saved %s (%dx%d, %dbpp)\n", path, bmp->w, bmp->h, bitmap_color_depth(bmp));
            } else {
                printf(" -> NULL bitmap!\n");
            }
        } else {
            printf(" (skipped, not a bitmap)\n");
        }
    }

    unload_datafile(dat);
    allegro_exit();
    return 0;
}
END_OF_MAIN()
