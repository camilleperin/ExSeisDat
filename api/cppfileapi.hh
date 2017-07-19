/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date Autumn 2016
 *   \brief
 *   \details Primary C++ API header
 *//*******************************************************************************************/
#ifndef PIOLCPPFILEAPI_INCLUDE_GUARD
#define PIOLCPPFILEAPI_INCLUDE_GUARD
#include "global.hh"
#include "file/file.hh"
#include "file/dynsegymd.hh"
namespace PIOL {
/*! This class provides access to the ExSeisPIOL class but with a simpler API
 */
class ExSeis
{
    Piol piol;                            //!< The PIOL object.

    public :
    /*! Constructor with optional maxLevel and which initialises MPI.
     * \param[in] maxLevel The maximum log level to be recorded.
     */
    ExSeis(const Log::Verb maxLevel = Log::Verb::None);

    /*! Constructor where one can also initialise MPI optionally.
     *  \param[in] initComm Initialise MPI if true
     *  \param[in] maxLevel The maximum log level to be recorded.
     */
    ExSeis(bool initComm, const Log::Verb maxLevel = Log::Verb::None);

    // TODO Abstract the communicator to MPI::Comm
    /*! Don't initialise MPI. Use the supplied communicator
     *  \param[in] comm The MPI communicator
     *  \param[in] maxLevel The maximum log level to be recorded.
     */
    ExSeis(MPI_Comm comm, const Log::Verb maxLevel = Log::Verb::None);

    /*! Cast to ExSeisPIOL *
     */
    operator ExSeisPIOL * ()
    {
        return piol.get();
    }

    /*! Cast to ExSeisPIOL shared_ptr
     */
    operator Piol ()
    {
        return piol;
    }

    /*! Shortcut to get the commrank.
     *  \return The comm rank.
     */
    size_t getRank(void)
    {
        return piol->comm->getRank();
    }

    /*! Shortcut to get the number of ranks.
     *  \return The comm number of ranks.
     */
    size_t getNumRank(void)
    {
        return piol->comm->getNumRank();
    }

    /*! Shortcut for a communication barrier
     */
    void barrier(void) const
    {
        piol->comm->barrier();
    }

    /*! Return the maximum value amongst the processes
     *  \param[in] n The value to take part in the reduction
     *  \return Return the maximum value amongst the processes
     */
    size_t max(size_t n) const
    {
        return piol->comm->max(n);
    }

    /*! \brief A function to check if an error has occured in the PIOL. If an error has occured the log is printed, the object destructor is called
     *  and the code aborts.
     * \param[in] msg A message to be printed to the log.
     */
    void isErr(const std::string msg = "") const;
};
}
#endif
