#ifndef __QY_PARSER_H__
#define __QY_PARSER_H__

///////////////////////////////////////////////////////
//	***             * * * *      ****        ****    //
//	***  	      *         *     **          **     //
//	***  	     *           *    **          **     //
//	***  	     *           *    **************     //
//	***	     **  *           *    **          **     //
//	***********   *         *     **          **     //
//	***********	    * * * *      ****        ****    // 
///////////////////////////////////////////////////////2009-08-20 @loach
/*
*
* �˿�����ɶ�������ѧϰ֮��,����������ҵ��;
* ���������ҵ��;,һ�鷨��׷���Լ��е�!
*
* ���ߣ� loach.h(������)
*
* ʱ�䣺 2009-09-21
*/

#include "qyutil/qyconfig.h"

class QyParserA;
class QyParserW;

#ifdef UNICODE
typedef QyParserW QyParser;
#else
typedef QyParserA QyParser;
#endif

// declare CParserW_qy, et. al.
#include "qyutil/qyparser-template-def-unichar.h"
#include "qyutil/qyparserT.h"
#include "qyutil/qyparser-template-undef.h"

  // declare CParserA_qy, et. al.
#include "qyutil/qyparser-template-def-char.h"
#include "qyutil/qyparserT.h"
#include "qyutil/qyparser-template-undef.h"



#endif /* __QY_PARSER_H__ */
