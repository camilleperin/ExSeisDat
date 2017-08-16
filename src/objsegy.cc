/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details
 *//*******************************************************************************************/
#include "object/objsegy.hh"
#include "data/datampiio.hh"
#include "share/segy.hh"
#include "data/data.hh"
namespace PIOL { namespace Obj {
///////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////    Class functions    ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////      Constructor & Destructor      ///////////////////////////////
//pragma to ignore unusued-paramter warnings here
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
WriteSEGY::WriteSEGY(Piol piol_, std::string name_, const Opt * opt_, std::shared_ptr<Data::Interface> data_) : WriteInterface(piol_, name_, data_)
{
}
WriteSEGY::WriteSEGY(Piol piol_, std::string name_, std::shared_ptr<Data::Interface> data_) : WriteInterface(piol_, name_, data_)
{
}
ReadSEGY::ReadSEGY(Piol piol_, std::string name_, const Opt * opt_, std::shared_ptr<Data::Interface> data_) : ReadInterface(piol_, name_, data_)
{
    Init(opt_);
}
ReadSEGY::ReadSEGY(Piol piol_, std::string name_, std::shared_ptr<Data::Interface> data_) : ReadInterface(piol_, name_, data_)
{
    const Opt opt;
    Init(&opt);
}

ReadSEGY::ReadSEGY(const Piol piol_, const std::string name_) : ReadInterface(piol_, name_, std::make_shared<DataT>(piol_, name_, FileMode::Read))
{
    const Opt opt;
    Init(&opt);
}

#pragma GCC diagnostic pop

///////////////////////////////////       Member functions      ///////////////////////////////////
SEGYFileHeader::SEGYFileHeader(ExSeisPIOL * piol, std::string name, const ReadSEGY::Opt * opt, csize_t fsz, std::vector<uchar> & buf)
{
    incFactor = opt->incFactor;
    if (fsz >= SEGSz::getHOSz())
    {
        ns = SEGY::getMd(SEGY::Hdr::NumSample, buf.data());
        nt = SEGSz::getNt(fsz, ns);
        inc = geom_t(SEGY::getMd(SEGY::Hdr::Increment, buf.data())) * incFactor;
        format = static_cast<SEGY::Format>(SEGY::getMd(SEGY::Hdr::Type, buf.data()));

        getAscii(piol, name, SEGSz::getTextSz(), buf.data());
        for (size_t i = 0LU; i < SEGSz::getTextSz(); i++)
            text.push_back(buf[i]);
    }
    else
    {
        format = SEGY::Format::IEEE;
        ns = 0LU;
        nt = 0LU;
        inc = geom_t(0);
        text = "";
    }
}

WriteSEGY::~WriteSEGY(void)
{
}

void ReadSEGY::Init(const ReadSEGY::Opt * opt)
{
    auto buf = std::vector<uchar>(SEGSz::getHOSz());
    data->read(0LU, SEGSz::getHOSz(), buf.data());
    desc = std::make_shared<SEGYFileHeader>(piol.get(), name, opt, getFileSz(), buf);
}
#warning simplify all calls that use ns

size_t ReadSEGY::getFileSz(void) const
{
    return data->getFileSz();
}

std::shared_ptr<FileMetadata> ReadSEGY::readHO(void) const
{
    return desc;
}

void ReadSEGY::readDO(csize_t offset, csize_t ns, csize_t sz, uchar * d) const
{
    data->read(SEGSz::getDOLoc(offset, ns), sz * SEGSz::getDOSz(ns), d);
}

void WriteSEGY::setFileSz(csize_t sz) const
{
    return data->setFileSz(sz);
}

void WriteSEGY::writeHO(const std::shared_ptr<FileMetadata> ho) const
{
    if (ho)
    {
        auto sho = dynamic_cast<const SEGYFileHeader *>(ho.get());
        std::vector<uchar> buf(SEGSz::getHOSz());
        for (size_t i = 0; i < ho->text.size(); i++)
            buf[i] = ho->text[i];

        SEGY::setMd(SEGY::Hdr::NumSample, int16_t(ho->ns), buf.data());
        SEGY::setMd(SEGY::Hdr::Type, int16_t(SEGY::Format::IEEE), buf.data());
        SEGY::setMd(SEGY::Hdr::Increment, int16_t(std::lround(ho->inc / (sho ? sho->incFactor : SI::Micro))), buf.data());

        //Currently these are hard-coded entries:
        SEGY::setMd(SEGY::Hdr::Units,      0x0001, buf.data());    //The unit system.
        SEGY::setMd(SEGY::Hdr::SEGYFormat, 0x0100, buf.data());    //The version of the SEGY format.
        SEGY::setMd(SEGY::Hdr::FixedTrace, 0x0001, buf.data());    //We always deal with fixed traces at present.
        SEGY::setMd(SEGY::Hdr::Extensions, 0x0000, buf.data());    //We do not support text extensions at present.

        data->write(0LU, SEGSz::getHOSz(), buf.data());
    }
    else
        data->write(0LU, 0LU, nullptr);
}

void WriteSEGY::writeDO(csize_t offset, csize_t ns, csize_t sz, const uchar * d) const
{
    data->write(SEGSz::getDOLoc(offset, ns), sz * SEGSz::getDOSz(ns), d);
}

void ReadSEGY::readDOMD(csize_t offset, csize_t ns, csize_t sz, uchar * md) const
{
    data->read(SEGSz::getDOLoc(offset, ns), SEGSz::getMDSz(), SEGSz::getDOSz(ns), sz, md);
}

void WriteSEGY::writeDOMD(csize_t offset, csize_t ns, csize_t sz, const uchar * md) const
{
    data->write(SEGSz::getDOLoc(offset, ns), SEGSz::getMDSz(), SEGSz::getDOSz(ns), sz, md);
}

void ReadSEGY::readDODF(csize_t offset, csize_t ns, csize_t sz, uchar * df) const
{
    data->read(SEGSz::getDODFLoc(offset, ns), SEGSz::getDFSz(ns), SEGSz::getDOSz(ns), sz, df);
}

void WriteSEGY::writeDODF(csize_t offset, csize_t ns, csize_t sz, const uchar * df) const
{
    data->write(SEGSz::getDODFLoc(offset, ns), SEGSz::getDFSz(ns), SEGSz::getDOSz(ns), sz, df);
}

//TODO: Add optional validation in this layer?
void ReadSEGY::readDO(csize_t * offset, csize_t ns, csize_t sz, uchar * d) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data->read(SEGSz::getDOSz(ns), sz, dooff.data(), d);
}

void WriteSEGY::writeDO(csize_t * offset, csize_t ns, csize_t sz, const uchar * d) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data->write(SEGSz::getDOSz(ns), sz, dooff.data(), d);
}

void ReadSEGY::readDOMD(csize_t * offset, csize_t ns, csize_t sz, uchar * md) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data->read(SEGSz::getMDSz(), sz, dooff.data(), md);
}

void WriteSEGY::writeDOMD(csize_t * offset, csize_t ns, csize_t sz, const uchar * md) const
{
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDOLoc(offset[i], ns);
    data->write(SEGSz::getMDSz(), sz, dooff.data(), md);
}

void ReadSEGY::readDODF(csize_t * offset, csize_t ns, csize_t sz, uchar * df) const
{
    if (ns == 0)
        return;
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDODFLoc(offset[i], ns);
    data->read(SEGSz::getDFSz(ns), sz, dooff.data(), df);
}

void WriteSEGY::writeDODF(csize_t * offset, csize_t ns, csize_t sz, const uchar * df) const
{
    if (ns == 0)
        return;
    std::vector<size_t> dooff(sz);
    for (size_t i = 0; i < sz; i++)
        dooff[i] = SEGSz::getDODFLoc(offset[i], ns);
    data->write(SEGSz::getDFSz(ns), sz, dooff.data(), df);
}
}}
