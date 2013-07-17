#ifndef __QY_PROPERTY_H__
#define __QY_PROPERTY_H__

#include "qyutil/qyconfig.h"

// namespace qyUtil
DEFINE_NAMESPACE(qy)

/** ����ֵ
*
* @author loach
*
* @date 2009-06-21
*/
template< typename PROPERTY_KEY>
struct QYNovtableM IQYPropertyItem
{
	/** ���һ��(�������,�ͽ����޸�) */
	virtual int add(const PROPERTY_KEY& key,const char* value,size_t len) =0;

	/** ɾ���ض�����ֵ */
	virtual int remove(const PROPERTY_KEY& key) =0;

	/** �����������ֵ */
	virtual void clear(void) = 0;

	/** ��ȡ��Ӧ��ֵ */
	virtual const char* get(const PROPERTY_KEY& key) const =0;

	/** ��ȡ��Ӧ��ֵ */
	virtual const char* get(const PROPERTY_KEY& key , size_t& len) const = 0;

	/** ��ȡ������ֵ */
	virtual IQYPropertyItem<PROPERTY_KEY>* getChild(void) const = 0;
	virtual bool isChild(void) const = 0;
	virtual bool newChild(void) = 0;	
};

END_NAMESPACE(qy)

#endif //