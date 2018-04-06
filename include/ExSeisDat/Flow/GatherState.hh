////////////////////////////////////////////////////////////////////////////////
/// @file
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_FLOW_GATHERSTATE_HH
#define EXSEISDAT_FLOW_GATHERSTATE_HH

#include "ExSeisDat/PIOL/Uniray.hh"
#include "ExSeisDat/PIOL/typedefs.h"

#include <cstddef>
#include <vector>

namespace PIOL {

/*! A parent class to allow gather operations to maintain a state.
 */
struct GatherState {
    /// Virtual destructor
    virtual ~GatherState() = default;

    /*! A virtual function which can be overridden to create the
     *  gather-operation state.
     *  @param[in] offset A list of gather-numbers to be processed by the local
     *             process.
     *  @param[in] gather The global array of gathers.
     */
    virtual void makeState(
      const std::vector<size_t>& offset,
      const Uniray<size_t, llint, llint>& gather) = 0;
};

}  // namespace PIOL

#endif  // EXSEISDAT_FLOW_GATHERSTATE_HH
