#include "config.h"

#ifdef WITH_QT
#include <QtGui>
#endif

/*******************************************************************************************************************************
 *   Interfaces, types, parameters and constants   													       *
 *******************************************************************************************************************************/
namespace terastitcher
{
    /*******************
    *    PARAMETERS    *
    ********************
    ---------------------------------------------------------------------------------------------------------------------------*/
	std::string version = num2str<int>(TERASTITCHER_MAJOR) + "." + num2str<int>(TERASTITCHER_MINOR) + "." + num2str<int>(TERASTITCHER_PATCH);
	std::string teraconverter_version     = "3.1.0";
	std::string terastitcher2_version     = "1.0.0";
	std::string mdatagenerator_version    = "1.0.0";
	std::string mergedisplacement_version = "1.0.0";
    int DEBUG = NO_DEBUG;                    //debug level
#ifdef WITH_QT
	std::string qtversion = QT_VERSION_STR;
#else	
	std::string qtversion = "";
#endif
    /*-------------------------------------------------------------------------------------------------------------------------*/
}