#include <allegro.h>
#include <stdio.h>

int main(int argc, char **argv) {
    if (allegro_init() != 0) {
        printf("Failed to init Allegro\n");
        return 1;
    }

    printf("Allegro Legacy initialized successfully!\n");
    printf("Allegro ID: %s\n", allegro_id);

    allegro_exit();
    return 0;
}
