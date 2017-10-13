/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - ruairi.short@ichec.ie - first commit
 *   \date January 2017
 *   \copywrite LGPL v.3
 *   \brief
 *   \details This utility searches for files matching a wildcard, filters out the SEGY matches
 *            and provides details about what is in the files.
 *//*******************************************************************************************/
#include "cppfileapi.hh"
#include <iostream>
#include <glob.h>
#include <regex>
using namespace PIOL;
char * rmFormat(std::string str, std::string ending)
//TODO: Add other file formats as supported
{
    std::string strLwr = str;
    std::transform(str.begin(), str.end(), strLwr.begin(), ::tolower);
    if (strLwr.substr(strLwr.length() - 4) == ".sgy")
        str = str.substr(0, str.length()-4);
    else if (strLwr.substr(strLwr.length() - 5) == ".segy")
        str = str.substr(0, str.length()-5);
    str =str +"{,"+ ending + "}";
    return (char *)str.c_str();
}
/*! Main function for assess.
 *  \param[in] argc The number of arguments. Should be at least 2.
 *  \param[in] argv The cstring array. The second array should be a globbing pattern.
 *  \todo Switch globbing with regex
 *  \return Return 0 on success, -1 on failure
 */
int main(int argc, char ** argv)
{
    if (argc < 2)
    {
        std::cout << "Too few arguments\n";
        return -1;
    }
    ExSeis piol;
    for (size_t j = 1; j < argc; j++)
    {
        glob_t globs;
        std::cout << "Pattern: " << argv[j] << "\n";
        int err = glob(rmFormat(std::string(argv[j]), ".[sS][eE][gG][yY], .[sS][gG][yY]"), GLOB_BRACE | GLOB_TILDE | GLOB_MARK, NULL, &globs);
        if (err)
            std::cout<<"No Files Found\n";
        else
        {
            std::regex reg(".*se?gy$", std::regex_constants::icase | std::regex_constants::optimize | std::regex::extended);
            std::cout << "File Count: " << globs.gl_pathc << "\n";
            for (size_t i = 0; i < globs.gl_pathc; i++)
                if (std::regex_match(globs.gl_pathv[i], reg))
                {
                    std::cout << "File: " << globs.gl_pathv[i] << "\n";

                    File::ReadDirect file(piol, globs.gl_pathv[i]);
                    piol.isErr();
                    std::cout << "-\tNs: " << file.readNs() << "\n";
                    std::cout << "-\tNt: " << file.readNt() << "\n";
                    std::cout << "-\tInc: " << file.readInc() << "\n";
                    std::cerr << "-\tText: " << file.readText() << "\n";
                }
         }
        globfree(&globs);
    }
    return 0;
}
