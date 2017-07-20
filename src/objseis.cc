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
namespace PIOL { namespace Obj {
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////    Class functions    ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////      Constructor & Destructor      ///////////////////////////////

std::vector<std::shared_ptr<Data::Interface>> makeMultiData(Piol piol, std::vector<std::string> & names, FileMode mode)
{
    std::vector<std::shared_ptr<Data::Interface>> data;
    for (auto name : names)
        data.push_back(std::make_shared<Data::MPIIO>(piol, name, mode));
    return data;
}

ReadSeis::ReadSeis(const Piol piol_, const std::string name_, const ReadSeis::Opt & opt_, std::shared_ptr<Data::Interface> data_) : ReadInterface(piol_, name_, data_)
{
//TODO: This function will need to detect the file structure based on the header
    std::vector<uchar> dat(data->getFileSz());
    data->read(0LU, dat.size(), dat.data());

    nlohmann::json jf = nlohmann::json::parse(dat.begin(), dat.end());

    SeisF::set(&desc.bytes, "bytes", jf);
    SeisF::set(&desc.o1, "o1", jf);
    SeisF::set(&desc.d1, "d1", jf);
    SeisF::set(&desc.n1, "n1", jf);

    std::string endian;
    SeisF::set(&endian, "endianness", jf);
    desc.endian = (endian == "little" ? SeisF::Endian::Little : SeisF::Endian::Big);

    SeisF::set(&desc.headerFile, "separateHeaders", jf);
    SeisF::set(desc.extents, "extents", jf);
    SeisF::set(&desc.packetSz, "packet", jf);

    traceBlocks = makeMultiData(piol, desc.extents, FileMode::Read);
}

///////////////////////////////////       Member functions      ///////////////////////////////////

size_t ReadSeis::getFileSz(void) const
{
    return 0LU;
}

void ReadSeis::readHO(uchar * ho) const
{
    assert(0);
}

void WriteSeis::writeHO(const uchar * ho) const
{
    assert(0);
}

void ReadSeis::readDO(csize_t offset, csize_t ns, csize_t sz, uchar * d) const
{
    assert(0);
}

void WriteSeis::writeDO(csize_t offset, csize_t ns, csize_t sz, const uchar * d) const
{
    assert(0);
}

void ReadSeis::readDOMD(csize_t offset, csize_t ns, csize_t sz, uchar * md) const
{
    assert(0);
}

void WriteSeis::writeDOMD(csize_t offset, csize_t ns, csize_t sz, const uchar * md) const
{
    assert(0);
}

void ReadSeis::readDODF(csize_t offset, csize_t ns, csize_t sz, uchar * df) const
{
    assert(0);
}

void WriteSeis::writeDODF(csize_t offset, csize_t ns, csize_t sz, const uchar * df) const
{
    assert(0);
}

void ReadSeis::readDO(csize_t * offset, csize_t ns, csize_t sz, uchar * d) const
{
    assert(0);
}

void WriteSeis::writeDO(csize_t * offset, csize_t ns, csize_t sz, const uchar * d) const
{
    assert(0);
}

void ReadSeis::readDOMD(csize_t * offset, csize_t ns, csize_t sz, uchar * md) const
{
    assert(0);
}

void WriteSeis::writeDOMD(csize_t * offset, csize_t ns, csize_t sz, const uchar * md) const
{
    assert(0);
}

void ReadSeis::readDODF(csize_t * offset, csize_t ns, csize_t sz, uchar * df) const
{
    assert(0);
}

void WriteSeis::writeDODF(csize_t * offset, csize_t ns, csize_t sz, const uchar * df) const
{
    assert(0);
}
}}
