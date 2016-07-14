#include <vector>
#include <cassert>
#include <string>
#include <iostream>
#include <utility>
#include "global.hh"
#include "data/datampiio.hh"
#include "file/filesegy.hh"
#include "set/man.hh"
#include "parallel.hh"
#include "comm/mpi.hh"
using namespace PIOL;
using PIOL::File::Interface;

void copyTestPolymorphism(Comms::MPI & comm, File::Interface & out, Set::Manager & in)
{
    if (!comm.getRank()) std::cout << "Copy Data\n";
    auto header = in.readHeader();

    auto traces = in.readTraces();

    std::vector<File::GridArray> grids;
    std::vector<File::CoordArray> coords;
    size_t offset = in.readTraceHeader(grids, coords);

    if (!comm.getRank()) std::cout << "Write header\n";
    out.writeHeader(header);

//    if (!comm.getRank()) std::cout << "Write Coords\n";
//    out.writeCoord(coords.first, coords.second);
    if (!comm.getRank()) std::cout << "Write Trace Headers\n";
    out.writeTraceHeader(offset, grids, coords);

    if (!comm.getRank()) std::cout << "Write Data\n";
    out.writeTraces(offset, traces.second);
}

int main(int argc, char ** argv)
{
    assert(argc > 2);
    std::string inFile(argv[1]);
    std::string outFile(argv[2]);

    //MPI Init
    auto comm = std::make_shared<Comms::MPI>(MPI_COMM_WORLD);

    if (!comm->getRank()) std::cout << "In file " << inFile << std::endl;

    Set::Manager seg(comm, std::unique_ptr<File::Interface>(new File::SEGY(comm, inFile, PIOL::Data::Type::MPIIO)));

//    testPolymorphismGets(seg);

    if (!comm->getRank()) std::cout << "Out file " << outFile << std::endl;

    File::SEGY out(comm, outFile, PIOL::Data::Type::MPIIO);
    copyTestPolymorphism(*comm, out, seg);
    if (!comm->getRank()) std::cout << "Terminate" << std::endl;
    return 0;
}

