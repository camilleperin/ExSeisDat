/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2017
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include <limits.h>
#include <stdlib.h>
#include <string>

#include "json.hpp"
#include "object/objseis.hh"
#include "data/datampiio.hh"
#include "share/seis.hh"
#include "data/data.hh"
#warning in
#include <iostream>

namespace PIOL { namespace Obj {
csize_t scalSz = sizeof(float);
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////    Class functions    ///////////////////////////////////////////////

SeisFileHeader::SeisFileHeader(const std::vector<uchar> & dat)
{
    nt = 0LU;
    text = "";
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

    inc = d1 / geom_t(1000);
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
void findReplace(const std::string & matcher, const std::string & replace, std::string & target)
{
    size_t pos = target.find(matcher);
    if (pos != std::string::npos)
        target.replace(pos, matcher.size(), replace);
}

std::string removeAfterLastChar(char matcher, const std::string & target)
{
    size_t pos = target.find_last_of(matcher);
    if (pos != std::string::npos)
        return target.substr(0LU, pos);
    return target;
}

std::string removeBeforeLastChar(char matcher, const std::string & target)
{
    size_t pos = target.find_last_of(matcher);
    if (pos != std::string::npos)
        return target.substr(pos);
    return "";
}

std::string getDirName(const std::string target)
{
    return removeAfterLastChar('/', target);
}

std::string getBaseName(const std::string & target)
{
    return removeBeforeLastChar('/', target);
}

std::string getExt(const std::string & target)
{
    return removeBeforeLastChar('.', target);
}

template <class DataT = ReadSeis::DataT>
std::string getPath(Piol piol, std::string name, std::string projName)
{
    std::string path;
    if (projName ==  "")
    {
        char * cpath = realpath(name.c_str(), NULL);
        path.assign(cpath);
        free(cpath);
        path = getDirName(path);
        findReplace("/dataset", "", path);
    }
    else
    {
        const std::string confFile("~/.opencps/opencps.prefs");
        auto conf = std::make_shared<DataT>(piol, confFile);
        std::vector<uchar> cdat(conf->getFileSz());
        conf->read(0LU, cdat.size(), cdat.data());
        nlohmann::json jf = nlohmann::json::parse(cdat.begin(), cdat.end());
        path = jf["com.ogi.opencps.project_paths"][projName];
    }
    return path;
}

void ReadSeis::Init(const Opt * opt)
{
    std::vector<uchar> dat(data->getFileSz());
    data->read(0LU, dat.size(), dat.data());
    desc = std::make_shared<SeisFileHeader>(dat);

    //Create a path to the appropriate datadir folder
    std::string path = getPath(piol, name, opt->projName);

    std::string projVar = "${project}";
    for (auto name : desc->extents)
    {
        std::string ext = getExt(name);
        findReplace(projVar, path, name);
        auto tempDat = std::make_shared<DataT>(piol, name, FileMode::Read);
        if (ext.find(".tr") != std::string::npos)
            traceBlocks.push_back(tempDat);
        else if (ext.find(".hd") != std::string::npos)
            headerBlocks.push_back(tempDat);
        else if (ext.find(".db") != std::string::npos)
            dbBlocks.push_back(tempDat);
    }

    offset.resize(traceBlocks.size());
    lSz.resize(traceBlocks.size());

    //multiple number of samples per trace by sample length, add length of scalars too.
    size_t traceLen = desc->bytes * desc->ns + (desc->bytes != scalSz ? scalSz * ((desc->ns + desc->packetSz - 1LU) / desc->packetSz) : 0LU);
    //Round to the nearest multiple of scalar size (required by seis format).
    traceLen = ((traceLen + scalSz - 1LU)/scalSz)*scalSz;

    for (size_t i = 0LU; i < lSz.size(); i++)
        lSz[i] = traceBlocks[i]->getFileSz() / traceLen;

    desc->nt = std::accumulate(lSz.begin(), lSz.end(), 0LU);
    std::partial_sum(lSz.begin(), lSz.end(), offset.begin());
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

ReadSeis::ReadSeis(const Piol piol_, const std::string name_) : ReadInterface(piol_, name_, std::make_shared<DataT>(piol_, name_, FileMode::Read))
{
    Opt opt;
    Init(&opt);
}

WriteSeis::WriteSeis(const Piol piol_, const std::string name_, const Opt * opt_, std::shared_ptr<Data::Interface> data_ ) : WriteInterface(piol_, name_, data_)
{
    desc = std::make_shared<SeisFileHeader>();
    desc->inc = 0.0;
    desc->nt = desc->ns = 0LU;
}

WriteSeis::WriteSeis(const Piol piol_, const std::string name_, std::shared_ptr<Data::Interface> data_) : WriteInterface(piol_, name_, data_)
{
    desc = std::make_shared<SeisFileHeader>();
    desc->inc = 0.0;
    desc->nt = desc->ns = 0LU;
}

WriteSeis::WriteSeis(const Piol piol_, const std::string name_) : WriteInterface(piol_, name_, std::make_shared<DataT>(piol_, name_, FileMode::Write))
{
    desc = std::make_shared<SeisFileHeader>();
    desc->inc = 0.0;
    desc->nt = desc->ns = 0LU;
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
    assert(ho);
    auto d = std::dynamic_pointer_cast<SeisFileHeader>(ho);
    if (!d)
    {
        d = std::make_shared<SeisFileHeader>();
        d->bytes = sizeof(float);
        d->o1 = 0.0;
        d->d1 = ho->inc * 1000.0;
        d->n1 = ho->ns;
        d->endian = SeisF::Endian::Little;
        d->headerFile = true;
        d->extents.insert(d->extents.end(), db.begin(), db.end());
        d->extents.insert(d->extents.end(), tr.begin(), tr.end());
        d->extents.insert(d->extents.end(), hd.begin(), hd.end());
        d->packetSz = 20LU;  //Hard-code
    }

    auto jout = d->set().dump();
    std::cout << std::setw(4) << jout << std::endl;
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
