#define _POSIX_C_SOURCE 200809L //POSIX includes
#include <assert.h>
#include <unistd.h>
#include <iostream>
#include "flow.hh"


/* Note for Review: orginally the -a was for increment but seemed to be refering to the size of the
gather (oInc needed to be a double (geom_t) not a long unsigned int) - updated the code to reflect
this and added a flag for the angle increment */

using namespace PIOL;
int main(int argc, char ** argv)
{
    ExSeis piol;
    std::string opt = "i:o:v:b:g:a:";  //TODO: uses a GNU extension
    std::string radon = "", angle = "", velocity = "";
    auto vBin = 20LU;
    auto oGSz = 60LU;
    auto oInc = Math::pi / geom_t(180LU);

    for (int c = getopt(argc, argv, opt.c_str()); c != -1; c = getopt(argc, argv, opt.c_str()))
        switch (c)
        {
            case 'i' :
                radon = optarg;
            break;
            case 'v' :
                velocity = optarg;
            break;
            case 'o' :
                angle = optarg;
            break;
            case 'b' :
                vBin = std::stoul(optarg);
            break;
            case 'a' :
                oInc = std::stod(optarg);
            break;
            case 'g' :
                oGSz = std::stoul(optarg);
            default :
                std::cerr << "One of the command line arguments is invalid\n";
            break;
        }

    assert(radon.size() && angle.size() && velocity.size());
    if (!piol.getRank())
        std::cout << "Radon to Angle Transformation"
                  << "\n-\tInput radon file:\t" << radon
                  << "\n-\tVelocity model file:\t" << velocity
                  << "\n-\tOutput angle file:\t" << angle
                  <<"\n-\t Gather Size:\t" << oGSz
                  << "\n-\tAngle Increment:\t" << oInc
                  << "\n-\tvBin:\t\t\t" << vBin << std::endl;
    Set set(piol, radon, angle);
    piol.isErr();
    set.toAngle(velocity, vBin, oGSz, oInc);

    piol.isErr();
    if (!piol.getRank())
        std::cout << "Begin output\n";

    return 0;
}



