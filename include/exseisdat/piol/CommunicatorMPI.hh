////////////////////////////////////////////////////////////////////////////////
/// @file
/// @author Cathal O Broin - cathal@ichec.ie - first commit
/// @copyright TBD. Do not distribute
/// @date July 2016
/// @brief The MPI communicator
/// @details The MPI communicator class is responsible for the concrete
///          implementation of all MPI communication specific features. That is,
///          everything within the MPI specification except those features which
///          are related to MPI-IO or overlap with MPI-IO.
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_COMMUNICATORMPI_HH
#define EXSEISDAT_PIOL_COMMUNICATORMPI_HH

#include "exseisdat/piol/CommunicatorInterface.hh"
#include "exseisdat/utils/logging/Log.hh"
#include "exseisdat/utils/typedefs.hh"

#include <mpi.h>

namespace exseis {
namespace piol {

using namespace exseis::utils::typedefs;

/*! @brief Set whether PIOL should manage MPI initialization / finalization.
 *      By default, PIOL will manage MPI if it calls MPI_Init, and it will call
 *      MPI_Init if the piol::CommunicatorMPI class is initialized before
 * MPI_Init is called. If PIOL Is managing MPI, it will call MPI_Finalize on
 * program exit.
 */
void manage_mpi(bool);


/*! @brief The MPI communication class. All MPI communication specific routines
 *         should be wrapped up and accessible from this class.
 */
class CommunicatorMPI : public CommunicatorInterface {
  private:
    /// The MPI communicator.
    MPI_Comm m_comm;

    /// For logging messages
    exseis::utils::Log* m_log;

  public:
    /*! @brief The MPI-Communicator options structure.
     */
    struct Opt {
        /// This variable defines the default MPI communicator.
        MPI_Comm comm = MPI_COMM_WORLD;
    };

    /*! @brief The constructor.
     *  @param[in] log Pointer to log object
     *  @param[in] opt Any options for the communication layer.
     */
    CommunicatorMPI(exseis::utils::Log* log, const CommunicatorMPI::Opt& opt);

    /*! @brief Retrieve the MPI communicator associated with the ExSeisPIOL.
     *  @return The MPI communicator.
     */
    MPI_Comm get_comm() const;

    std::vector<float> gather(const std::vector<float>& in) const override;
    std::vector<double> gather(const std::vector<double>& in) const override;
    std::vector<exseis::utils::Integer> gather(
        const std::vector<exseis::utils::Integer>& in) const override;
    std::vector<size_t> gather(const std::vector<size_t>& in) const override;

    /// @brief TMP
    /// @tparam T TMP
    /// @param[in] val TMP
    /// @return TMP
    /// @todo DELETE ME
    template<class T>
    std::vector<T> gather(const T& val) const
    {
        return CommunicatorInterface::gather<T>(val);
    }

    size_t sum(size_t val) const override;

    size_t max(size_t val) const override;
    size_t min(size_t val) const override;

    size_t offset(size_t val) const override;

    void barrier(void) const override;
};

}  // namespace piol
}  // namespace exseis

#endif  // EXSEISDAT_PIOL_COMMUNICATORMPI_HH
