/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "json.hpp"
#include "object/objseis.hh"
#include "data/datampiio.hh"
#include "share/seis.hh"
#include "data/data.hh"
#warning in
#include <iostream>

namespace PIOL { namespace Obj {
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////    Class functions    ///////////////////////////////////////////////

SeisFileHeader::SeisFileHeader(const std::vector<uchar> & dat)
{
    nlohmann::json jf = nlohmann::json::parse(dat.begin(), dat.end());
    SeisF::get(&bytes, "bytes", jf);
    SeisF::get(&o1, "o1", jf);
    SeisF::get(&d1, "d1", jf);
    SeisF::get(&n1, "n1", jf);

    std::string end;
    SeisF::get(&end, "endianness", jf);
    endian = (end == "little" ? SeisF::Endian::Little : SeisF::Endian::Big);

    SeisF::get(&headerFile, "separateHeaders", jf);
    SeisF::get(extents, "extents", jf);
    SeisF::get(&packetSz, "packet", jf);

    inc = d1;
    ns = n1;
}

nlohmann::json SeisFileHeader::set(void)
{
    nlohmann::json jf = {
        {"bytes", bytes},
        {"o1", o1},
        {"d1", d1},
        {"n1", n1},
        {"endianness", (endian == SeisF::Endian::Little ? "little" : "big")},
        {"separateHeaders", headerFile},
        {"extents", extents},
        {"packet", packetSz}
    };
    return jf;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////      Constructor & Destructor      ///////////////////////////////

std::vector<std::shared_ptr<Data::Interface>> makeMultiData(Piol piol, std::vector<std::string> & names, FileMode mode)
{
    std::vector<std::shared_ptr<Data::Interface>> data;
    for (auto name : names)
        data.push_back(std::make_shared<Data::MPIIO>(piol, name, mode));
    return data;
}

void ReadSeis::Init(const Opt * opt)
{
//TODO: This function will need to detect the file structure based on the header
    std::vector<uchar> dat(data->getFileSz());
    data->read(0LU, dat.size(), dat.data());

    desc = std::make_shared<SeisFileHeader>(dat);

    std::cout << "n1 " << desc->n1 << " o1 " << desc->o1 << " d1 " << desc->d1
              << " endianness " << (desc->endian == SeisF::Endian::Little ? "little" : "big")
              <<  " packetSz " << desc->packetSz << std::endl << " extents ";
    for (auto ext : desc->extents)
        std::cout << ext << " " << std::endl;

    std::cout << desc->set() << std::endl;

    traceBlocks = makeMultiData(piol, desc->extents, FileMode::Read);
}

ReadSeis::ReadSeis(const Piol piol_, const std::string name_, const Opt * opt, std::shared_ptr<Data::Interface> data_) : ReadInterface(piol_, name_, data_)
{
    Init(opt);
}

ReadSeis::ReadSeis(const Piol piol_, const std::string name_, std::shared_ptr<Data::Interface> data_) : ReadInterface(piol_, name_, data_)
{
    Opt opt;
    Init(&opt);
}

WriteSeis::~WriteSeis(void)
{
    writeHO(desc);
}
///////////////////////////////////       Member functions      ///////////////////////////////////


size_t ReadSeis::getFileSz(void) const
{
    return 0LU;
}

void WriteSeis::setFileSz(csize_t sz) const
{
}

std::shared_ptr<FileMetadata> ReadSeis::readHO(void) const
{
    return desc;
}

void WriteSeis::writeHO(const std::shared_ptr<FileMetadata> ho) const
{
    auto jout = desc->set().dump();
    data->write(0LU, jout.size(), reinterpret_cast<const uchar *>(jout.data()));
}

void ReadSeis::readDO(csize_t offset, csize_t ns, csize_t sz, uchar * d) const
{
}

void WriteSeis::writeDO(csize_t offset, csize_t ns, csize_t sz, const uchar * d) const
{
}

void ReadSeis::readDOMD(csize_t offset, csize_t ns, csize_t sz, uchar * md) const
{
}

void WriteSeis::writeDOMD(csize_t offset, csize_t ns, csize_t sz, const uchar * md) const
{
}

void ReadSeis::readDODF(csize_t offset, csize_t ns, csize_t sz, uchar * df) const
{
}

void WriteSeis::writeDODF(csize_t offset, csize_t ns, csize_t sz, const uchar * df) const
{
}

void ReadSeis::readDO(csize_t * offset, csize_t ns, csize_t sz, uchar * d) const
{
}

void WriteSeis::writeDO(csize_t * offset, csize_t ns, csize_t sz, const uchar * d) const
{
}

void ReadSeis::readDOMD(csize_t * offset, csize_t ns, csize_t sz, uchar * md) const
{
}

void WriteSeis::writeDOMD(csize_t * offset, csize_t ns, csize_t sz, const uchar * md) const
{
}

void ReadSeis::readDODF(csize_t * offset, csize_t ns, csize_t sz, uchar * df) const
{
}

void WriteSeis::writeDODF(csize_t * offset, csize_t ns, csize_t sz, const uchar * df) const
{
}
}}
