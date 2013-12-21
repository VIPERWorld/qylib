#ifndef __QY_IQYCOMFACTORY_H__ 
#define __QY_IQYCOMFACTORY_H__ 

#include "qyutil/qyconfig.h"
#include "qyutil/qyunknwn.h"

DEFINE_NAMESPACE(qy)

/** ʵ�������ص����� */
typedef QyUnknown* (STDCALL *QyComCreateInstanceCB)(IQyUnknown* pUnkOuter, HRESULT *phr);

/**
*
* @author loach
*
* @date 2007-07-24
*
* @brief ע�Ṥ���࣬Ϊ����ϵͳע��
*/
struct QyComRegister
{
	const char*      name_;
	const QY_GUID*     iid_;
	QyComCreateInstanceCB  fnCreate_;
};


/** ������COM���ɹ��� */
struct QYNovtableM IQyComFactory
{
	// ע�Ṥ����
	virtual HRESULT coRegister(QY_REFID rclsid , const QyComRegister* com) = 0;
	virtual HRESULT coUnRegister(QY_REFID rclsid , const QyComRegister* com) = 0;

    /**
     * @brief ����ʵ��
     *
     * @param rclsid ��ο�
     *
     * @param pUnkOuter ���ӿ�
     * @param riid ��ο� vguid.h guid �Ķ���
     * @param [out]ppv ���ش�����ʵ������
    */
	virtual HRESULT createInstance(QY_REFID rclsid , IQyUnknown  * pUnkOuter , QY_REFID riid , LPVOID* ppv) = 0;
};

END_NAMESPACE(qy)

#endif /* __QY_IQYCOMFACTORY_H__ */