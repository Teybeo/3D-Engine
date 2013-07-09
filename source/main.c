#include "app.h"

int main() {

    App Robot = {};

    if (App_Init(&Robot) == false)
        return -1;

    App_Run(&Robot);

    return 0;
}
