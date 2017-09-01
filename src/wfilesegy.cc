/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details WriteSEGY functions
 *//*******************************************************************************************/
#include <cmath>
#include <cstring>
#include <limits>
#include "global.hh"
#include "file/filesegy.hh"
#include "object/object.hh"
#include "share/misc.hh"
#warning debug
#include <iostream>
namespace PIOL { namespace File {
///////////////////////////////      Constructor & Destructor      ///////////////////////////////
WriteSEGY::Opt::Opt(void)
{
}

WriteSEGY::WriteSEGY(const Piol piol_, const std::string name_, const Opt * opt, std::shared_ptr<Obj::WriteInterface> obj_)
    : WriteInterface(piol_, name_, obj_)
{
    init(opt);
}

WriteSEGY::WriteSEGY(const Piol piol_, const std::string name_, std::shared_ptr<Obj::WriteInterface> obj_)
    : WriteInterface(piol_, name_, obj_)
{
    Opt opt;
    init(&opt);
}

WriteSEGY::WriteSEGY(const Piol piol_, const std::string name_)
    : WriteInterface(piol_, name_, std::make_shared<DObj>(piol_, name_))
{
    Opt opt;
    init(&opt);
}

void WriteSEGY::deinit(void)
{
    //Write the header object at de-initialisation time since we dont control when
    //before that that each of the file parameters (e.g ns, inc, text etc) are set.
    if (!piol->log->isErr())    //TODO: On error this can be a source of a deadlock
    {
        calcNt();
        if (state.resize)
            obj->setFileSz(SEGSz::getFileSz(nt, ns));
        if (state.writeHO)
        {
            if (!piol->comm->getRank())
            {
                std::vector<uchar> buf(SEGSz::getHOSz());
                auto ho = std::make_shared<Obj::SEGYFileHeader>();
                ho->inc = inc;
                ho->text = text;
                ho->ns = ns;
                ho->nt = nt;
                ho->incFactor = incFactor;
                obj->writeHO(ho);
            }
            else
                obj->writeHO(NULL);
        }
    }
}

WriteSEGY::~WriteSEGY(void)
{
    deinit();
}

///////////////////////////////////       Member functions      ///////////////////////////////////
void WriteSEGY::init(const WriteSEGY::Opt * opt)
{
    incFactor = opt->incFactor;
    memset(&state, 0, sizeof(Flags));
    format = SEGY::Format::IEEE;
    ns = 0LU;
    nt = 0LU;
    inc = geom_t(0);
    text = "";
    state.writeHO = true;
}

size_t WriteSEGY::calcNt(void)
{
    if (state.stalent)
    {
        nt = piol->comm->max(nt);
        state.stalent = false;
        state.resize = true;
    }
    return nt;
}

void WriteSEGY::writeText(const std::string text_)
{
    if (text != text_)
    {
        text = text_;
        text.resize(SEGSz::getTextSz());
        state.writeHO = true;
    }
}

void WriteSEGY::writeNs(csize_t ns_)
{
    if (ns_ > size_t(std::numeric_limits<int16_t>::max()))
    {
        piol->log->record(name, Log::Layer::File, Log::Status::Error, "Ns value is too large for SEG-Y", Log::Verb::None);
        return;
    }

    if (ns != ns_)
    {
        ns = ns_;
        state.resize = true;
        state.writeHO = true;
    }
    nsSet = true;
}

void WriteSEGY::writeNt(csize_t nt_)
{
    if (nt != nt_)
    {
        nt = nt_;
        state.resize = true;
    }
    state.stalent = false;
}

void WriteSEGY::writeInc(const geom_t inc_)
{
    if (std::isnormal(inc_) == false)
    {
        piol->log->record(name, Log::Layer::File, Log::Status::Error,
            "The SEG-Y Increment " + std::to_string(inc_) + " is not normal.", Log::Verb::None);
        return;
    }

    if (inc != inc_)
    {
        inc = inc_;
        state.writeHO = true;
    }
}

/*! Template function for writing SEG-Y traces and parameters, random and contiguous.
 *  \tparam T The type of offset (pointer or size_t)
 *  \param[in] obj The object-layer object.
 *  \param[in] ns The number of samples per trace.
 *  \param[in] offset The offset(s). If T == size_t * this is an array, otherwise its a single offset.
 *  \param[in] sz The number of traces to write
 *  \param[in] trc Pointer to trace array.
 *  \param[in] prm Pointer to parameter structure.
 *  \param[in] skip Skip \c skip entries in the parameter structure
 */
template <typename T>
void writeTraceT(Obj::WriteInterface * obj, csize_t ns, T offset, csize_t sz, trace_t * trc, const Param * prm, csize_t skip)
{
    //This function performs the same thing as readTraceT but in opposite order basically.

    //reverse the bytes on the input trace set to convert it to big endian.
    //we switch the input rather than creating a second array to save space.
    //At the end of this function, we convert back to little endian.
    uchar * tbuf = reinterpret_cast<uchar *>(trc);
    if (trc != TRACE_NULL && trc != nullptr)
        for (size_t i = 0; i < ns * sz; i++)
            reverse4Bytes(&tbuf[i*sizeof(float)]);

    if (prm == PARAM_NULL)
        obj->writeDODF(offset, ns, sz, tbuf);
    else
    {
        csize_t blockSz = (trc == TRACE_NULL ? SEGSz::getMDSz() : SEGSz::getDOSz(ns));
        std::vector<uchar> alloc(blockSz * sz);
        uchar * buf = (sz ? alloc.data() : nullptr);
        insertParam(sz, prm, buf, blockSz - SEGSz::getMDSz(), skip);

        if (trc == TRACE_NULL)
            obj->writeDOMD(offset, ns, sz, buf);
        else
        {
            for (size_t i = 0; i < sz; i++)
                std::copy(&tbuf[i * SEGSz::getDFSz(ns)], &tbuf[(i+1) * SEGSz::getDFSz(ns)],
                           &buf[i * SEGSz::getDOSz(ns) + SEGSz::getMDSz()]);
            obj->writeDO(offset, ns, sz, buf);
        }
    }

    if (trc != TRACE_NULL && trc != nullptr)
        for (size_t i = 0; i < ns * sz; i++)
            reverse4Bytes(&tbuf[i*sizeof(float)]);
}

void WriteSEGY::writeTrace(csize_t offset, csize_t sz, trace_t * trc, const Param * prm, csize_t skip)
{
    if (!nsSet)
        piol->log->record(name, Log::Layer::File, Log::Status::Error,
            "The number of samples per trace (ns) has not been set. The output is probably erroneous.", Log::Verb::None);

    writeTraceT(obj.get(), ns, offset, sz, trc, prm, skip);
    state.stalent = true;
    nt = std::max(offset + sz, nt);
}

void WriteSEGY::writeTrace(csize_t sz, csize_t * offset, trace_t * trc, const Param * prm, csize_t skip)
{
    if (!nsSet)
        piol->log->record(name, Log::Layer::File, Log::Status::Error,
            "The number of samples per trace (ns) has not been set. The output is probably erroneous.", Log::Verb::None);

    writeTraceT(obj.get(), ns, offset, sz, trc, prm, skip);
    state.stalent = true;
    if (sz)
        nt = std::max(offset[sz-1LU]+1LU, nt);
}
}}
