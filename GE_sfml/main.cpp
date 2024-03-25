#include "header/CommonHF.h"
#include "header/Game.h"

int main()
{
    try
    {
        Game g("config.txt");
        g.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}