#ifndef __QY_ATTRIBUTE_H__
#define __QY_ATTRIBUTE_H__

/*
*
* �˿�����ɶ�������ѧϰ֮��,����������ҵ��;
* ���������ҵ��;,һ�鷨��׷���Լ��е�!
*
* ���ߣ� loach.h(������)
*
* ʱ�䣺 2009-06-08
*/

#include "qyutil/qyunknwn.h"
#include "qyutil/qyproperty.h"

// namespace qyUtil
DEFINE_NAMESPACE(qy)

// IQyAttribute
// {3208B374-8A36-4aac-9744-AE185BD09F30}
QY_DEFINE_GUID(QYIID_IQyAttribute, 
			   0x3208b374, 0x8a36, 0x4aac, 0x97, 0x44, 0xae, 0x18, 0x5b, 0xd0, 0x9f, 0x30);

/**
* @brief ����ֵ�ӿ�
*
* @author loach
*
* @group vloachunit
*/
struct QYNovtableM IQyAttribute : public IQyUnknown
{
	typedef void* AttrValue;
	typedef void* AttrParam;
	/** 
	* @brief �������ֵ 
	* @return ����(ʧ�� -1 ,�ɹ� 0)
	*/
	virtual int  add(unsigned int nKey ,AttrValue attr,AttrParam param = 0) = 0 ;
	/**
	* @brief ��ѯ����ֵ 
	*/
	virtual AttrValue get(unsigned int nKey , AttrParam param = 0) = 0;
	/**
	* @brief Delete
	*/
	virtual bool remove(unsigned int nKey) = 0;
};

typedef IQyAttribute* POSITION_attr;


END_NAMESPACE(qy)

#endif /* __QY_ATTRIBUTE_H__ */