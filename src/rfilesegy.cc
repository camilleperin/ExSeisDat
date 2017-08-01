/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief
 *   \details ReadSEGY functions
 *//*******************************************************************************************/
#include <assert.h>
#include "global.hh"
#include "file/filesegy.hh"
#include "object/object.hh"
namespace PIOL { namespace File {
///////////////////////////////      Constructor & Destructor      ///////////////////////////////
ReadSEGY::Opt::Opt(void)
{}

ReadSEGY::ReadSEGY(const Piol piol_, const std::string name_, const Opt * opt, std::shared_ptr<Obj::ReadInterface> obj_)
    : ReadInterface(piol_, name_, obj_)
{
    init(opt);
}

ReadSEGY::ReadSEGY(const Piol piol_, const std::string name_, std::shared_ptr<Obj::ReadInterface> obj_)
    : ReadInterface(piol_, name_, obj_)
{
    ReadSEGY::Opt opt;
    init(&opt);
}

ReadSEGY::ReadSEGY(const Piol piol_, const std::string name_) : ReadSEGY(piol_, name_, std::make_shared<DObj>(piol_, name_))
{
    ReadSEGY::Opt opt;
    init(&opt);
}

///////////////////////////////////       Member functions      ///////////////////////////////////
void ReadSEGY::init(const ReadSEGY::Opt * opt)
{
    assert(obj);
    auto desc = obj->readHO();
    if (!desc)
    {
        piol->log->record(name, Log::Layer::File, Log::Status::Error,
                         "Object layer returns null structure for metadata", Log::Verb::None);
        inc = geom_t(0);
        ns = 0LU;
        nt = 0LU;
        text = "";
    }
    else
    {
        inc = desc->inc;
        ns = desc->ns;
        nt = desc->nt;
        text = desc->text;
    }
}

/*! Template function for reading SEG-Y traces and parameters, random and contiguous.
 *  \tparam T The type of offset (pointer or size_t)
 *  \param[in] obj The object-layer object.
 *  \param[in] format The format of the trace data.
 *  \param[in] ns The number of samples per trace.
 *  \param[in] offset The offset(s). If T == size_t * this is an array, otherwise its a single offset.
 *  \param[in] offunc A function which given the ith trace of the local process, returns the associated trace offset.
 *  \param[in] sz The number of traces to read
 *  \param[in] trc Pointer to trace array.
 *  \param[in] prm Pointer to parameter structure.
 *  \param[in] skip Skip \c skip entries in the parameter structure
 */
template <typename T>
void readTraceT(Obj::ReadInterface * obj, const SEGY::Format format, csize_t ns, const T offset, std::function<size_t(size_t)> offunc,
                                      csize_t sz, trace_t * trc, Param * prm, csize_t skip)
{
    uchar * tbuf = reinterpret_cast<uchar *>(trc);
    if (prm == PARAM_NULL)
        obj->readDODF(offset, ns, sz, tbuf);
    else
    {
        csize_t blockSz = (trc == TRACE_NULL ? SEGSz::getMDSz() : SEGSz::getDOSz(ns));
        std::vector<uchar> alloc(blockSz * sz);
        uchar * buf = (sz ? alloc.data() : nullptr);

        if (trc == TRACE_NULL)
            obj->readDOMD(offset, ns, sz, buf);
        else
        {
            obj->readDO(offset, ns, sz, buf);
            for (size_t i = 0; i < sz; i++)
                std::copy(&buf[i * SEGSz::getDOSz(ns) + SEGSz::getMDSz()], &buf[(i+1) * SEGSz::getDOSz(ns)],
                            &tbuf[i * SEGSz::getDFSz(ns)]);
        }

        extractParam(sz, buf, prm, (trc != TRACE_NULL ? SEGSz::getDFSz(ns) : 0LU), skip);
        for (size_t i = 0; i < sz; i++)
            setPrm(i+skip, Meta::ltn, offunc(i), prm);
    }

    if (trc != TRACE_NULL && trc != nullptr)
    {
        if (format == SEGY::Format::IBM)
            for (size_t i = 0; i < ns * sz; i ++)
                trc[i] = convertIBMtoIEEE(trc[i], true);
        else
            for (size_t i = 0; i < ns * sz; i++)
                reverse4Bytes(&tbuf[i*sizeof(float)]);
    }
}

void ReadSEGY::readTrace(csize_t offset, csize_t sz, trace_t * trc, Param * prm, csize_t skip) const
{
    size_t ntz = (!sz ? sz : (offset + sz > nt ? (offset <= nt ? nt - offset : 0LU) : sz));
    if (offset >= nt && sz)   //Nothing to be read.
        piol->log->record(name, Log::Layer::File, Log::Status::Warning,
                          "readParam() was called for a zero byte read", Log::Verb::None);

    readTraceT(obj.get(), format, ns, offset, [offset] (size_t i) -> size_t { return offset + i; }, ntz, trc, prm, skip);
}

void ReadSEGY::readTrace(csize_t sz, csize_t * offset, trace_t * trc, Param * prm, csize_t skip) const
{
    readTraceT(obj.get(), format, ns, offset,  [offset] (size_t i) -> size_t { return offset[i]; }, sz, trc, prm, skip);
}
}}
