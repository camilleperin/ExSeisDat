/* OpenCPS_tool_v1
===begin_json_info===
{
    "name"       : "Output (PIOL)",
    "purpose"    : "Output traces and parameters to disk using the PIOL.",
    "categories" : ["ICHEC"],
    "guiwidgets" :
    {
        "OutputName" :
        {
            "widget"  : "string",
            "label"   : "Output file name:",
            "tooltip" : "Output a file with this name."
        }
    }
}
===end_json_info===
===begin_html_help===
===end_html_help===
*/

#include "tools/tool.hpp"
#include "flow/fileparameters.hpp"
#include "cppfileapi.hh"

class OutPIOL : public ogi::Tool
{
    PIOL::ExSeis piol;
    PIOL::File::WriteDirect * ofile;
    ogi::FileParameter oname;

    ogi::Header<double> xSrc;
    ogi::Header<double> ySrc;
    ogi::Header<double> xRcv;
    ogi::Header<double> yRcv;
    ogi::Header<int64_t> trcNum;

    public:
    OutPIOL(void) : Tool(), piol(false), oname("OutputName", *this, "w"),
              xSrc("SRC_X", headersIn()), ySrc("SRC_Y", headersIn()),
              xRcv("RCV_X", headersIn()), yRcv("RCV_Y", headersIn()),
              trcNum("TRACE_NUMBER", headersIn())
    {
    }

    void update(void)
    {
        twosets = false;
    }

    void startExecution(void)
    {
        ofile = new PIOL::File::WriteDirect(piol, oname.getPath());
    }
    void finishExecution(void)
    {
        delete ofile;
    }

    int execute(int ntr, char * const * hd, float * const * trc)
    {
        size_t lnt = ntr;
        size_t ns = n1In;

        PIOL::File::Param prm(lnt);

        std::vector<size_t> offset(lnt);
        std::vector<float> trcw(lnt * ns);
        for (size_t i = 0; i < lnt; i++)
        {
            offset[i] = trcNum(hd[i]);
            std::copy(trc[i], trc[i] + ns, trcw.data() + i * ns);
            setPrm(i, PIOL::Meta::xSrc, xSrc(hd[i]), &prm);
            setPrm(i, PIOL::Meta::ySrc, ySrc(hd[i]), &prm);
            setPrm(i, PIOL::Meta::xRcv, xRcv(hd[i]), &prm);
            setPrm(i, PIOL::Meta::yRcv, yRcv(hd[i]), &prm);

            //TODO: <---- Set more headers here ---->
        }

        ofile->writeTrace(lnt, offset.data(), trcw.data(), &prm);
        return 0;
    }
};

extern "C" OutPIOL *construct_OutPIOL(void)
{
    return new OutPIOL();
}
