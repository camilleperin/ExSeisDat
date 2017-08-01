/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief The Seis specific File layer interface
 *   \details Seis implementations for File layer
*//*******************************************************************************************/
#ifndef PIOLFILESEIS_INCLUDE_GUARD
#define PIOLFILESEIS_INCLUDE_GUARD
#include <memory>
#include <type_traits>
#include "global.hh"
#include "file/file.hh"
#include "file/dynsegymd.hh"
#include "object/objseis.hh" //For the makes
namespace PIOL { namespace File {
/*! The Seis implementation of the file layer
 */
class ReadSeis : public ReadInterface
{
    public :
    typedef Obj::ReadSeis DObj;
    /*! \brief The Seis options structure.
     */
    struct Opt : public DObj::Opt
    {
        typedef ReadSeis Type;  //!< The Type of the class this structure is nested in

        /*! Constructor which provides the default Rules
         */
        Opt(void);
    };

    /*! \brief The Seis-Object class constructor.
     *  \param[in] piol_   This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_   The name of the file associated with the instantiation.
     *  \param[in] segyOpt The Seis-File options
     *  \param[in] obj_    A shared pointer to the object layer
     */
    ReadSeis(const Piol piol_, const std::string name_, const ReadSeis::Opt * opt, std::shared_ptr<Obj::ReadInterface> obj_);

    /*! \brief The Seis-Object class constructor.
     *  \param[in] piol_   This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_   The name of the file associated with the instantiation.
     *  \param[in] obj_    A shared pointer to the object layer
     */
    ReadSeis(const Piol piol_, const std::string name_, std::shared_ptr<Obj::ReadInterface> obj_);

    ReadSeis(const Piol piol_, const std::string name_) : ReadInterface(piol_, name_, std::make_shared<DObj>(piol_, name_))
    { }

    void readTrace(csize_t offset, csize_t sz, trace_t * trace, Param * prm = const_cast<Param *>(PARAM_NULL), csize_t skip = 0) const;

    void readTrace(csize_t sz, csize_t * offset, trace_t * trace, Param * prm = const_cast<Param *>(PARAM_NULL), csize_t skip = 0) const;

//    void readTraceNonMono(csize_t sz, csize_t * offset, trace_t * trace, Param * prm = const_cast<Param *>(PARAM_NULL), csize_t skip = 0) const;
    protected :
    void init(const Opt *);
};

/*! The Seis implementation of the file layer
 */
class WriteSeis : public WriteInterface
{
    protected :
    void deinit(void);
    public :
    typedef Obj::WriteSeis DObj;
    /*! \brief The Seis options structure.
     */
    struct Opt : public DObj::Opt
    {
        typedef WriteSeis Type; //!< The Type of the class this structure is nested in

        /*! Constructor which provides the default Rules
         */
        Opt(void);
    };

    /*! \brief The Seis-Object class constructor.
     *  \param[in] piol_   This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_   The name of the file associated with the instantiation.
     *  \param[in] segyOpt The Seis-File options
     *  \param[in] obj_    A shared pointer to the object layer
     */
    WriteSeis(const Piol piol_, const std::string name_, const WriteSeis::Opt * opt, std::shared_ptr<Obj::WriteInterface> obj_);

    /*! \brief The Seis-Object class constructor.
     *  \param[in] piol_   This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_   The name of the file associated with the instantiation.
     *  \param[in] obj_    A shared pointer to the object layer
     */
    WriteSeis(const Piol piol_, const std::string name_, std::shared_ptr<Obj::WriteInterface> obj_);

    WriteSeis(const Piol piol_, const std::string name_) : WriteInterface(piol_, name_, std::make_shared<DObj>(piol_, name_))
    { }

    /*! \brief Destructor. Processes any remaining flags
     */
    ~WriteSeis(void);

    void writeText(const std::string text_);

    void writeNs(csize_t ns_);

    void writeNt(csize_t nt_);

    void writeInc(const geom_t inc_);

    void writeTrace(csize_t offset, csize_t sz, trace_t * trace, const Param * prm = PARAM_NULL, csize_t skip = 0);

    void writeTrace(csize_t sz, csize_t * offset, trace_t * trace, const Param * prm = PARAM_NULL, csize_t skip = 0);
};
}}
#endif
