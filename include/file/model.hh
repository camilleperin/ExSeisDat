/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date January 2017
 *   \brief The File layer interface
 *   \details The File layer interface is a base class which specific File implementations
 *   work off
*//*******************************************************************************************/
#ifndef PIOLFILEMODEL_INCLUDE_GUARD
#define PIOLFILEMODEL_INCLUDE_GUARD
#include "global.hh"
#include "share/param.hh"
#include "share/uniray.hh"
#include "file/file.hh"
#warning make generic
#include "file/dynsegymd.hh"
namespace PIOL { namespace File {
/*! \brief An intitial class for 3d volumes
 */
template <class T>
class ReadModel : public T
{
    protected :
    void makeReadModel(void)
    {
        std::vector<size_t> vlist = {0LU, 1LU, this->readNt() - 1LU};
        File::Param prm(vlist.size());
        this->readParam(vlist.size(), vlist.data(), &prm);

        llint il0 = File::getPrm<llint>(0LU, Meta::il, &prm);
        llint xl0 = File::getPrm<llint>(0LU, Meta::xl, &prm);

        llint ilInc = File::getPrm<llint>(1LU, Meta::il, &prm) - il0;
        llint ilNum = (ilInc ? (File::getPrm<llint>(2LU, Meta::il, &prm) - il0) / ilInc : 0LU);
        llint xlNum = (this->readNt() / (ilNum ? ilNum : 1LU));
        llint xlInc = (File::getPrm<llint>(2LU, Meta::xl, &prm) - xl0) / xlNum;

        ilInc = (ilInc ? ilInc : 1LU);
        xlInc = (xlInc ? xlInc : 1LU);

        il = std::make_tuple(il0, ilNum, ilInc);
        xl = std::make_tuple(xl0, xlNum, xlInc);
    }

    public :
    std::tuple<llint, llint, llint> il;  //!< Parameters for the inline coordinate (start, count, increment)
    std::tuple<llint, llint, llint> xl;  //!< Parameters for the crossline coordinate (start, count, increment)

    ReadModel(const Piol piol_, const std::string name_) : T(piol_, name_)
    {
        makeReadModel();
    }

    /*! read the 3d file based on il and xl that match those in the given \c gather array.
     *  \param[in] offset the offset into the global array
     *  \param[in] sz the number of gathers for the local process
     *  \param[in] gather a structure which contains the il and xl coordinates of interest
     *  \return return a vector of traces containing the trace values requested
     */
    std::vector<trace_t> readModel(csize_t offset, csize_t sz, const Uniray<size_t, llint, llint> & gather)
    {
        std::vector<trace_t> trc(sz * this->readNs());
        std::vector<size_t> offsets(sz);
        for (size_t i = 0; i < sz; i++)
        {
            auto val = gather[offset + i];
            /* The below can be translated to:
             * trace number = ilNumber * xlInc + xlNumber
             * much like indexing in a 2d array.
             */
            offsets[i] = ((std::get<1>(val) - std::get<0>(il)) / std::get<2>(il)) * std::get<1>(xl)
                      + ((std::get<2>(val) - std::get<0>(xl)) / std::get<2>(xl));
        }

        this->readTrace(offsets.size(), offsets.data(), trc.data(), const_cast<Param *>(PARAM_NULL), 0LU);
        return trc;
    }

    /*! Read the 3d file based on il and xl that match those in the given \c gather array.
     *  \param[in] sz The number of offsets for the local process
     *  \param[in] offset the offset into the global array
     *  \param[in] gather A structure which contains the il and xl coordinates of interest
     *  \return Return a vector of traces containing the trace values requested
     */
    std::vector<trace_t> readModel(csize_t sz, csize_t * offset, const Uniray<size_t, llint, llint> & gather)
    {
        std::vector<trace_t> trc(sz * this->readNs());
        std::vector<size_t> offsets(sz);
        for (size_t i = 0; i < sz; i++)
        {
            auto val = gather[offset[i]];
            offsets[i] = ((std::get<1>(val) - std::get<0>(il)) / std::get<2>(il)) * std::get<1>(xl)
                      + ((std::get<2>(val) - std::get<0>(xl)) / std::get<2>(xl));
        }

        this->readTrace(offsets.size(), offsets.data(), trc.data(), const_cast<Param *>(PARAM_NULL), 0LU);
        return trc;
    }
};
}}
#endif
