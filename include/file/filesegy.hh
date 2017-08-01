/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief The SEGY specific File layer interface
 *   \details SEGY implementations for File layer
*//*******************************************************************************************/
#ifndef PIOLFILESEGY_INCLUDE_GUARD
#define PIOLFILESEGY_INCLUDE_GUARD
#include <memory>
#include <type_traits>
#include "global.hh"
#include "file/file.hh"
#include "share/segy.hh"
#include "file/dynsegymd.hh"
#include "object/objsegy.hh" //For the makes
namespace PIOL { namespace File {
/*! The SEG-Y implementation of the file layer
 */
class ReadSEGY : public ReadInterface
{
    public :
    typedef Obj::ReadSEGY DObj;
    /*! \brief The SEG-Y options structure.
     */
    struct Opt : DObj::Opt
    {
        typedef ReadSEGY Type;  //!< The Type of the class this structure is nested in

        /*! Constructor which provides the default Rules
         */
        Opt(void);
    };

    protected :
    SEGY::Format format;        //!< Type formats
    unit_t incFactor;           //!< The increment factor
    /*! \brief This function initialises the SEGY specific portions of the class.
     *  \param[in] opt The SEGY-File options
     */
    void init(const ReadSEGY::Opt * opt);

    public :
    /*! \brief The SEGY-Object class constructor.
     *  \param[in] piol_   This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_   The name of the file associated with the instantiation.
     *  \param[in] opt The SEGY-File options
     *  \param[in] obj_    A shared pointer to the object layer
     */
    ReadSEGY(const Piol piol_, const std::string name_, const Opt * opt, std::shared_ptr<Obj::ReadInterface> obj_);

    /*! \brief The SEGY-Object class constructor.
     *  \param[in] piol_   This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_   The name of the file associated with the instantiation.
     *  \param[in] obj_    A shared pointer to the object layer
     */
    ReadSEGY(const Piol piol_, const std::string name_, std::shared_ptr<Obj::ReadInterface> obj_);

    /*! \brief The SEGY-Object class constructor with default object layer
     *  \param[in] piol_   This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_   The name of the file associated with the instantiation.
     */
    ReadSEGY(const Piol piol_, const std::string name_);

    void readTrace(csize_t offset, csize_t sz, trace_t * trace, Param * prm = const_cast<Param *>(PARAM_NULL), csize_t skip = 0) const;

    void readTrace(csize_t sz, csize_t * offset, trace_t * trace, Param * prm = const_cast<Param *>(PARAM_NULL), csize_t skip = 0) const;
};

#ifdef GONE
/*! A SEGY class for velocity models
 */
class ReadSEGYModel : public Model3dInterface, public ReadSEGY
{
    public :
    /*!
     \param[in] piol_ The piol object.
     \param[in] name_ The name of the file.
     \param[in] obj_ A shared pointer for the object layer object.
     */
    ReadSEGYModel(const Piol piol_, const std::string name_, std::shared_ptr<Obj::ReadInterface> obj_);

    std::vector<trace_t> readModel(csize_t offset, csize_t sz, const Uniray<size_t, llint, llint> & gather);

    std::vector<trace_t> readModel(csize_t sz, csize_t * offset, const Uniray<size_t, llint, llint> & gather);
};
#endif

/*! The SEG-Y implementation of the file layer
 */
class WriteSEGY : public WriteInterface
{
    public :
    typedef Obj::WriteSEGY DObj;

    /*! \brief The SEG-Y options structure.
     */
    struct Opt : public DObj::Opt
    {
        typedef WriteSEGY Type; //!< The Type of the class this structure is nested in

        /*! Constructor which provides the default Rules
         */
        Opt(void);
    };

    private :
    bool nsSet = false;

    SEGY::Format format;              //!< Type formats

    /*! State flags structure for SEGY
     */
    struct Flags
    {
        uint64_t writeHO : 1;   //!< The header should be written before SEGY object is deleted
        uint64_t resize : 1;    //!< The file should be resized before SEGY object is deleted.
        uint64_t stalent : 1;   //!< The nt value is stale and should be resynced.
    } state;                    //!< State flags are stored in this structure

    unit_t incFactor;           //!< The increment factor

    /*! \brief This function initialises the SEGY specific portions of the class.
     *  \param[in] opt_ The SEGY-File options
     */
    void init(const WriteSEGY::Opt * opt_);

    void deinit(void);

    /*! Calculate the number of traces currently stored (or implied to exist).
     *  \return Return the number of traces
     */
    size_t calcNt(void);

    public :
    /*! \brief The SEGY-Object class constructor.
     *  \param[in] piol_   This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_   The name of the file associated with the instantiation.
     *  \param[in] opt_ The SEGY-File options
     *  \param[in] obj_    A shared pointer to the object layer
     */
    WriteSEGY(const Piol piol_, const std::string name_, const Opt * opt_, std::shared_ptr<Obj::WriteInterface> obj_);

    /*! \brief The SEGY-Object class constructor.
     *  \param[in] piol_   This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_   The name of the file associated with the instantiation.
     *  \param[in] obj_    A shared pointer to the object layer
     */
    WriteSEGY(const Piol piol_, const std::string name_, std::shared_ptr<Obj::WriteInterface> obj_);

    /*! \brief Destructor. Processes any remaining flags
     */
    ~WriteSEGY(void);

    WriteSEGY(const Piol piol_, const std::string name_) : WriteInterface(piol_, name_, std::make_shared<DObj>(piol_, name_))
    { }

    void writeText(const std::string text_);

    void writeNs(csize_t ns_);

    void writeNt(csize_t nt_);

    void writeInc(const geom_t inc_);

    void writeTrace(csize_t offset, csize_t sz, trace_t * trace, const Param * prm = PARAM_NULL, csize_t skip = 0);

    void writeTrace(csize_t sz, csize_t * offset, trace_t * trace, const Param * prm = PARAM_NULL, csize_t skip = 0);
};
}}
#endif
