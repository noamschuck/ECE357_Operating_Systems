#include <stdio.h>
#include "explore_directory.h"
#include "data.h"

int main(int argc, char *arg[]) {
    metadata data = {{0, 0, 0}, 0, 1, 0, 0, 0};
    explore_directory("./test_folder", &data);
    print_metadata(data);
}
