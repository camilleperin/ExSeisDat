/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief The Seis implementation of the Object layer interface
 *   \details The Seis specific implementation of the Object layer interface. The object
 *   layer is considered with course file structure issues only.
*//*******************************************************************************************/
#ifndef PIOLOBJSEIS_INCLUDE_GUARD
#define PIOLOBJSEIS_INCLUDE_GUARD
#include "global.hh"
#include "object/object.hh"
#include "data/datampiio.hh"
#include "share/seis.hh"

namespace PIOL { namespace Obj {
struct SeisFileHeader : public FileMetadata
{
    size_t bytes;
    double o1;
    double d1;
    size_t n1;
    SeisF::Endian endian;
    bool headerFile;
    std::vector<std::string> headers;
    std::vector<std::string> extents;
    size_t packetSz;

    SeisFileHeader(const std::vector<uchar> & dat);
    SeisFileHeader(void) {}

    bool operator==(SeisFileHeader & other)
    {
        return bytes == other.bytes && o1 == other.o1 && d1 == other.d1 && n1 == other.n1 &&
               endian == other.endian && headerFile == other.headerFile && headers == other.headers &&
               extents == other.extents && packetSz == other.packetSz && FileMetadata::operator==(other);
    }
};

/*! \brief The ReadSeis Obj class.
 */
class ReadSeis : public ReadInterface
{
    public :
    typedef Data::MPIIO Data;
    /*! \brief The ReadSeis options structure. Currently empty.
    */
    struct Opt
    {
        typedef ReadSeis Type;  //!< The Type of the class this structure is nested in
        /* \brief Default constructor to prevent intel warnings
         */
        Opt(void)
        {
        }
    };

    /*! \brief The ReadSeis-Obj class constructor.
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] opt_  The Seis options
     *  \param[in] data_ Pointer to the Data layer object (polymorphic).
     *  \param[in] mode  The file mode
     */
    ReadSeis(const Piol piol_, const std::string name_, const ReadSeis::Opt * opt_, std::shared_ptr<Data::Interface> data_);

    ReadSeis(const Piol piol_, const std::string name_, std::shared_ptr<Data::Interface> data_);

    ReadSeis(const Piol piol_, const std::string name_) : ReadInterface(piol_, name_, std::make_shared<Data>(piol_, name_, FileMode::Read))
    { }

    size_t getFileSz(void) const;

    std::shared_ptr<FileMetadata> readHO(void) const;

    void readDOMD(csize_t offset, csize_t ns, csize_t sz, uchar * md) const;

    void readDODF(csize_t offset, csize_t ns, csize_t sz, uchar * df) const;

    void readDO(csize_t offset, csize_t ns, csize_t sz, uchar * d) const;

    void readDO(csize_t * offset, csize_t ns, csize_t sz, uchar * d) const;

    void readDOMD(csize_t * offset, csize_t ns, csize_t sz, uchar * md) const;

    void readDODF(csize_t * offset, csize_t ns, csize_t sz, uchar * df) const;

    protected :
    std::vector<std::shared_ptr<Data::Interface>> traceBlocks;  //!< Pointer to the Data layer object (polymorphic).
    std::vector<std::shared_ptr<Data::Interface>> headerBlocks;  //!< Pointer to the Data layer object (polymorphic).
    std::shared_ptr<SeisFileHeader> desc;

    void Init(const Opt *);
};

/*! \brief The WriteSeis Obj class.
 */
class  WriteSeis : public WriteInterface
{
    protected :
    std::vector<std::shared_ptr<Data::Interface>> traceBlocks;  //!< Pointer to the Data layer object (polymorphic).
    std::vector<std::shared_ptr<Data::Interface>> headerBlocks;  //!< Pointer to the Data layer object (polymorphic).
    std::shared_ptr<SeisFileHeader> desc;

    public :
    typedef Data::MPIIO Data;
    /*! \brief The WriteSeis options structure. Currently empty.
    */
    struct Opt
    {
        typedef  WriteSeis Type;  //!< The Type of the class this structure is nested in
        /* \brief Default constructor to prevent intel warnings
         */
        Opt(void)
        {
        }
    };

    /*! \brief The WriteSeis-Obj class constructor.
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] opt_  The Seis options
     *  \param[in] data_ Pointer to the Data layer object (polymorphic).
     *  \param[in] mode  The file mode
     */
    WriteSeis(const Piol piol_, const std::string name_, const Opt * opt_, std::shared_ptr<Data::Interface> data_ ) : WriteInterface(piol_, name_, data_)
    {}

    WriteSeis(const Piol piol_, const std::string name_, std::shared_ptr<Data::Interface> data_) : WriteInterface(piol_, name_, data_)
    {}

    WriteSeis(const Piol piol_, const std::string name_) : WriteInterface(piol_, name_, std::make_shared<Data>(piol_, name_, FileMode::Write))
    {}

    void setFileSz(csize_t sz) const;

    void writeHO(const std::shared_ptr<FileMetadata> ho) const;

    void writeDOMD(csize_t offset, csize_t ns, csize_t sz, const uchar * md) const;

    void writeDODF(csize_t offset, csize_t ns, csize_t sz, const uchar * df) const;

    void writeDO(csize_t offset, csize_t ns, csize_t sz, const uchar * d) const;

    void writeDO(csize_t * offset, csize_t ns, csize_t sz, const uchar * d) const;

    void writeDOMD(csize_t * offset, csize_t ns, csize_t sz, const uchar * md) const;

    void writeDODF(csize_t * offset, csize_t ns, csize_t sz, const uchar * df) const;
};
}}
#endif
