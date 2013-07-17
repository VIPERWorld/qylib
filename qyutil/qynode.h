#ifndef __QY_NODE_H__
#define __QY_NODE_H__

#include "qyutil/qyconfig.h"
#include "qyutil/qystring.h"
#include "qyutil/qyattribute.h"
#include "qyutil/qyunknwn.h"

// namespace qyUtil
DEFINE_NAMESPACE(qy)

// ǰ������
class QyTreePrivate;
class QYUTIL_API QyUnknown;

/// onNodeCommand(HTREENODE hNode , UINT eCmd,void*param0=NULL) ����������ֵ
enum eItemCommand
{
	CMD_ITEM_RELEASE, ///< QyTree ������������
	CMD_ITEM_ADD,     ///< hTreeNode ��ӽ����, eCmd=CMD_ITEM_ADD , param0 =  ����ӵĶ���(HTREENODE)
	CMD_ITEM_DELETE,  ///< hTreeNode ɾ�������, eCmd=CMD_ITEM_DELETE , param0= ��ɾ���Ķ���(HTREENODE)
};

/// ��������ָ��
typedef class QyNode* HNODE;

/// ��������ָ��
typedef class QyTree* HTREENODE;

/**
 * @brief ���ʵ����(����ʵ����,IQyAttribute ����û��ʵ��)
 *
 * @author loach
 *
 * @date 2007-01-02
*/
class QYUTIL_API QyNode : public QyUnknown , public IQyAttribute
{
public:
	/// Constructor
	QyNode();
	/// Destructor
	virtual ~QyNode();
	
	QY_DECLARE_IUNKNOWN()
private:
	/// ���ε����ַ�ʽ��copy
	QyNode(const QyNode& n)
		:QyUnknown(NULL)
	{}	
	QyNode& operator = (const QyNode& n){ return *this;}
private:
	/** ��Ӧ����(��ҪΪ�˵� HTREENODE �����仯�ǣ�Ҫ��HNODE֪��,������֪ͨ����ʲô�仯)
	 * @param eCmd(�ɲο� eItemCommand)
	 * @param hNode this��������
	*/
	virtual int onNodeCommand(HTREENODE hNode , UINT eCmd , void* param0=NULL){	return -1; }
private:
	friend class QyTree;
};

/** ���һ��֪ͨ�¼��ӿڶ���
 *
 * @author loach
 *
 * @date 2009-06-14
*/
struct IQyNodeObjserver
{
	enum eNotifyCode
	{
		ENC_NODE_Unknown=0,
		ENC_NODE_Remove,///< ���� detachNode() ֪ͨ�¼����丸�������һ����� ,param1= htreenode ,param0 = htreenode->Parent,param2=0
		ENC_NODE_Add,   ///< ���� insertSiblingNode(),insertChildNode(), attachNode() ֪ͨ�¼����丸������һ����� ,param1= htreenode ,param0 = htreenode->Parent,param2=0
		ENC_NODE_Delete,///< ���� deleteNode() ֪ͨ�¼����ϵĸ����ɾ��һ�����,,param1= htreenode ,param0 = htreenode->Parent[�ϵĽ��],param2 =0
		ENC_NODE_Flags_update, ///< setFlags(UINT nFlags) �����仯 param0=htreenode,param1 = �µ�&int,param2 = �ϵ�&int
		ENC_NODE_Flags_bit,///< setFlags(bits,isTrue)  �����仯 param0=htreenode,param1 = &bits,param2 = &isTrue
	};
	virtual void onNodeNotify(UINT eNotify,void* param0,void* param1,void* param2) = 0;
};

/// �ȽϵĻص�����
typedef int  (*PNodeCompareCB)(HTREENODE node1,HTREENODE node2);

/// add by loach 2009-06-09˽�д���Ļص�,��removePrivate��ص���һ��[ addPrivate() ]
typedef void (*PNodeprivateCB)(const char* key,void* pp);

/// ��ѯ������
typedef bool (*PNodequeryCB)(HTREENODE hTree,void* pQueryCondition);

/// node id ֵ
typedef QyStringA QyNodeID;

/**
* @ingroup qyUtil
*
* @brief ��֦��������
*
* ������ʱ,�������ⲿ���з����ڴ�,���ڲ�����Ψһ�Լ��,
* �����Ҫ���м��Ψһ��,���ڲ���֮ǰ,����isHasNode(...)
* ������Ľ�����node_����������,������ʱ,ֻ����뵽�ӽ����ȥ
*   root(�����)
*    child <-- ����
*   next  = null;
*   prev  = null;
*   parent= null;
*
* @author loach
*
* @date 2007-01-02
*
* @bug ��֧�� "=" operator
*
* 
* @modified by loach 2009-06-09
*
*  1 ����ṹ����һ�� id����ĸĴ������id ��HNODE ��������ģ���ʹ���߾������ϵ
*  2 ��� sortSliblingNode ����
*  3 ��� QyTreePrivate ��غ���
*  4 Ϊ��Ӧ�Ľӿ����IQyNodeObjserver
*  
*/
class QYUTIL_API QyTree : public t_qyPOSITION
{
public:
	typedef void* PrivateValue;

	/** ��־	 */
	enum enTNFlag 
	{
		TN_NULL        = 0x0000, ///<  �ղ���
		TN_VISIBLE_BIT = 0x0001, ///<  �Ƿ�ɼ���,�Ը���˵,��TN_EXPAND_BIT���ʹ��
		TN_SORT_BIT    = 0x0002, ///<  �Ƿ�����
		TN_CHECK_BIT   = 0x0004, ///<  �Ƿ�ѡ��
		TN_EXPAND_BIT  = 0x0008, ///<  �Ƿ�չ��,��Ҫ���ڸ�,���������ӽ��Ŀɼ���,��������־λΪ0,�������ɼ�,1�ɼ� 
		TN_LOCK_BIT    = 0x0010, ///<  �����㱻����,��Ҫ���ڸ� ,���ܽ���Expand����
		TN_ICON_EXPAND_BIT= 0x0020,///< ͼ��Ҳ��expand����,��TN_LOCK_BIT����
		TN_SELECT_BIT     = 0x0040,///< ���ѡ�У���checkbox��ͬ
		TN_MOUSEON_BIT    = 0x0080,///< ��������� 
	};

	/// �������
	enum enNodeType
	{
		NT_ROOT = 1, ///< �����
		NT_NODE = 2  ///< Ҷ��
	};

	/** ��ѯ��־ */
	enum enQueryFlag
	{
		QF_ROOT=0x01,///<ֻ��ѯ��
		QF_NODE=0x02,///<ֻ��ѯ���
		QF_ALL =0x03,///<����
	};

	/// check ��־
	enum enCheckFlag
	{
		CF_REVERSE=0,///<��ѡ
		CF_SELECT,///<ѡ��
		CF_NOSELECT,///<��ѡ
	};

	/// Constructor
	QyTree(const QyNodeID& inId , enNodeType type = NT_ROOT , HNODE node = NULL , UINT nFlags = TN_VISIBLE_BIT);
	/// Destructor
	virtual ~QyTree();

private:
	/// ���ε����ַ�ʽ��copy
	QyTree(const QyTree& t){}	
	QyTree& operator = (const QyTree& t){ return *this;}
public:
	/**
	* @brief ����һ����㵽ͬ���ֵ�,ֱ�Ӳ���,û�в�ѯ�Ƿ����,�����Ҫ��ȷ,���ȵ���isHasNode()����
	*
	* @param node ������õ��ڴ�,����ʵ�ֶ�̬����,node������������������,��Ҫ������ͷ�
	* @param pInsNode ,�����λ��,if pInsNode!=NULL,ȷ�� pInsNode �� this ��ͬ����
	*
	* @param id Ϊÿ��HTREENODE ָ��һ��id( add by loach 2009-06-08)
	*/
	HTREENODE insertSiblingNode(  enNodeType type,
		                          const QyNodeID& inId,
		                          const HNODE node,								  
		                          const HTREENODE pInsNode = NULL,BOOL  bBack = TRUE ,PNodeCompareCB lpCompareCB = NULL);

	/** 
	* @brief ����һ����㵽�ӽ��
	* ֱ�Ӳ���,û�в�ѯ�ӽ���Ƿ����,�����Ҫ��ȷ,���ȵ���isHasNode()����
	*
	* @param node ������õ��ڴ�,����ʵ�ֶ�̬����,node������������������,��Ҫ������ͷ�
	* @param pInsNode ����� ,if pInsNode!=NULL,ȷ�� pInsNode �� this ���ӽ��
	*
	* @param id Ϊÿ��HTREENODE ָ��һ��id( add by loach 2009-06-08)
	*/
	HTREENODE insertChildNode(  enNodeType type,
		                        const QyNodeID& inId,
		                        const HNODE node,								
		                        const HTREENODE pInsNode=NULL,BOOL bBack = TRUE ,PNodeCompareCB lpCompareCB = NULL);

	/**
	* @brief ɾ��һ�����,�ͷ��ڴ�
	*/
	int deleteNode(HTREENODE hNode);

	/**
	* @brief ����һ�����,�����ͷ�
	*
	* @param pObjserver �۲���(����ͨ����֪ͨ�����ɾ����)
	*/
	int detachNode(const HTREENODE hNode);

	/**
	* @brief ���һ����㵽�ӽ��,HNODE�����ڶ���������������Ч,������һ���ֲ�����
	* ����HNODE �����ǲ�ֻһ�����,�����кܶ��ֵܽ��,���ӽ��
	*/
	int attachNode(HTREENODE hNode , PNodeCompareCB lpCompareCB = NULL);

	/**
	* @brief drag src node to dest node
	*
	* �϶����� srcNode ����Ƶ� destNode ���
	*
	* @param bRoot �Ƿ��϶���,=false ���srcNode�Ǹ�,������ -3\n
	*                   = true ���srcNode�Ǹ�,Ҳ�����϶�
	* @return 0 �ɹ�\n
	*  -1 ��ʾΪNULL\n
	*  -2 �������ֵ�\n
	*  -3 srcNode�Ǹ�
	*/
	int  dragNode(HTREENODE srcNode,
		          HTREENODE destNode,
				  BOOL bRoot = FALSE,
				  PNodeCompareCB lpCompareCB = NULL);

	/**
	* @brief ���¶���ͬ�����н���������
	*
	*/
	int sortSliblingNode(PNodeCompareCB lpCompareCB);
	/**
	* @brief ���ý��ѡ��
	* @param nFlag ѡ�б�־,��ο� enCheckFlag
	*/
	void checkNode(int nFlag = QyTree::CF_REVERSE);

	/**
	* @brief ���ý��ѡ�У����ǲ��޸���Ԫ��
	* @param nFlag ѡ�б�־,��ο� enCheckFlag
	*/
	void checkSelfNode(int nFlag = QyTree::CF_REVERSE);
	/**

	* @brief �Ƿ��������Ľ�����
	* ������������,�����ӽ��,�ֵܽ��
	*/
	BOOL isHasNode(const HTREENODE hNode);

	/** ���hNode�Ƿ�Щ���ĸ���㣨���������ϼ��ĸ����)
	 *
	 *  +hNode(root)
	 *     +node(child)
	 *     +node(child)
	 *     +node(child)
	 *     +node(root)
	 *        +node(child)
	 *        +node(child)
	 *        +node(root)
	 *           +this(child)
	 *
	 *   ���� this->queryPNode(hNode) �ͻ᷵��true
	*/
	BOOL queryPNode(const HTREENODE hNode);

	/** ��ȡʵ�����(�������ϼ����) */
	HTREENODE getTopNode(void);

	/** 
	* @brief get the special item sid(Ψһ�ı�־),�Ƿ������ͬ��sid����������������
	*
	* @param lpid ��2009-06-08 by loach ����������ĺ����������)
	*              ��ָ������ HNODE ��Ӧ��ֵ�أ�����Ϊ��ָ HTREENODE ��ָ���� id
	*              node �� lpid ����������
	*
	* @param node ���������
	*
	* @param nFlags ��ο� enQueryFlag 
	* == QF_ROOT ֻ�Ҹ� \n
	* == QF_NODE ֻ�ҽ�㣨���Ǹ�)\n
	* == QF_ALL  ����\n
	*
	* @param pQueryCB ���˲�ѯ���������
	*/
	HTREENODE isHasNode( const QyNodeID& inId ,
		                 UINT nQFlag = QF_NODE , 
						 PNodequeryCB pQueryCB=NULL , void* pQueryCondition = NULL);

	HTREENODE isHasNode( const HNODE node ,
		                 UINT nQFlag = QF_NODE , 
						 PNodequeryCB pQueryCB=NULL,void* pQueryCondition = NULL);
	/** 
	* @brief �Ƿ�����ӽ��
	*/
	BOOL isHasChilds() const;
	/** 
	* @brief �Ƿ��Ǹ� 
	*/
	BOOL isRoot() const;
	/**
	* @brief ��ȡ�����(һ���ĸ���㣬���¼���ϵ)
	*/
	HTREENODE getParentNode() const;
	/**
	* @brief ��ȡ��һ��ͬ���ֵܽ��
	*/
	HTREENODE getNextNode() const;
	/**
	* @brief ��ȡ��һ��ͬ���ֵܽ��
	*/
	HTREENODE getPrevNode() const;
	/**
	* @brief ��ȡ�ӽ��
	*/
	HTREENODE getChildNode() const;
	/**
	* @brief ��ȡͬ���ֵܵ�β����� LOOP getNextNode()  ֱ����
	*/
	HTREENODE getTailNode() const;
	/**
	* @brief ��ȡͬ���ֵܵ�ͷ����� LOOP getPrevNode()  ֱ����
	*/
	HTREENODE getHeadNode() const;
	/**
	* @brief Retrieves the next item having the given relationship with the
	* specified item.
	* ��hNode ��ʼ,���»�ȡ,���Ա�������ָ�����
	*
	* @param nFlag �����enTNFlag ,== 0 ��ʾ���н��
	* @param nQFlag ��ο� enQueryFlag		
	*/
	HTREENODE getNextNode(HTREENODE hNode,
		                  UINT nFlag = TN_NULL,
		                  UINT nQFlag = QF_ALL,
						  PNodequeryCB pQueryCB = NULL,void* pQueryCondition = NULL) const;

	/**
	* @brief ��ȡ���н�������(�������������)
	*/
	int   getNodeCounts() const;
	/**
	* @brief ��ȡ����������(�������ӽ������)
	*/
	int   getRootCounts() const;
	/** 
	* @brief ɾ�������ӽ��
	*/
	void  removeAllChild(void);
	/**
	* @brief ���ñ�־
	*
	* @param bNotify =TRUE��ϣ���ڲ����� IQyNodeObjserver�ӿ���֪ͨ
	*/
	void  setFlags(int bits , bool isTrue);
	void  setFlags(int nFlags);
	/**
	* @brief ��ȡ��־
	*/
	int   getFlags() const;
	BOOL  getFlags(int bits) const;
	/** 
	* @brief ��ȡ��ȼ��� 
	*/
	int   getLevel(void) const;
	/**
	* @brief ������ȼ���
	*/
	void  setLevel(unsigned short level);

	/** 
	* @brief �������ֵ 
	* @return ����(ʧ�� false ,�ɹ� true)
	*/
	int  add(UINT nKey ,IQyAttribute::AttrValue attr,IQyAttribute::AttrParam param=NULL)
	{
		return(node_)? node_->add(nKey,attr,param): -1 ;
	}
	/**
	* @brief ��ѯ����ֵ 
	*/
	IQyAttribute::AttrValue get(UINT nKey,IQyAttribute::AttrParam param = NULL)
	{
		return(node_)? node_->get(nKey,param):0;
	}
	/**
	* @brief Delete
	*/
	bool remove(UINT nKey)
	{
		return(node_) ? node_->remove(nKey): true;
	}
	/** 
	* @brief ��ȡ������ָ��
	*/
	HNODE getNode(void) const;

	/**
	 * @brief ���߽��
	*/
	HNODE detach(void)
	{
		HNODE node = node_;
		node_ = NULL;
		return node;
	}

	void attach(HNODE node)
	{
		HNODE o = node_;		
		node_ = node;
		qyRelease(o);
	}
	
	/** ��HTREENODE ����һ��idֵ( add by loach 2009-06-08)
	 *
	*/
	void setId(const QyNodeID& inId);	
	/**
	 * @param pOutlen ,����id�ַ�������
	*/
	const char* getId( size_t* len = 0) const;

	/** ����˽������ */
	void setAttribute(IQyAttribute* lpAttribute)
	{
		IQyAttribute* old = attribute_;
		if (lpAttribute)
		{
			attribute_ = lpAttribute;
			attribute_->AddRef();
		}
		qyRelease(old);
	}

	/** ��ѯ˽������ */
	int queryAttribute(IQyAttribute** lpAttribute)
	{
		if (attribute_)
		{
			(*lpAttribute) = attribute_;
			attribute_->AddRef();
			return 0;
		}
		return -1;
	}

	//------------------------------------------------------------------------//
	//
	// add by loach 2009-06-08

	/** ���һЩ˽����Ϣ[UI�ϵ�һЩֵ]
	 *  ��һ�����棬������ͬһ�����ݣ�������ı��ֵ�Ԫ�ز�һ�����Ϳ���ͨ����������
	 *  ��Ҫʱ��ͨ���ӿ�ȡ
	 *
	 * @param PNodeprivateCB �Ļص���(��Ҫ���������� ppֵ)
	 *
	 * @return -1 �����Ѿ�����
	*/
	int addPrivate(const char* key , PrivateValue pp , PNodeprivateCB lpPrivateCB);

	/** ɾ�����˽����Ϣ
	 *
	 * @param pp if pp != NULL ����ɾ���Ƿ�����,Ȼ�����������pp���д���
	*/
	int removePrivate(const char* key , PrivateValue* pp);

	/**
	 * ��ѯ��Ӧ��ֵ.
	*/
	int queryPrivate(const char* key , PrivateValue* pp);

protected:
	/** command ��Ӧ���� */
	int  onNodeCommand( UINT eCmd,void*param0=NULL);
	/**
	* @brief �޸����Ľ����
	*/
	void modifyCounts(int nNodeCounts, int nRootCounts);
	/**
	* @brief �������ӽ��ļ���
	*/
	void modifyChildLevel();

	/** ����Ҫ����������� */
	virtual IQyNodeObjserver* getObjserver(void){return NULL;}
private:
	/** ��ȡ�۲춼���� */
	IQyNodeObjserver* GetNodeObjserver(void)
	{
		HTREENODE hParent = getTopNode();
		return(hParent) ? hParent->getObjserver() : NULL;
	}
	/** ���ü��� */
	void counts(int nNodeCounts, int nRootCounts);
	/**
	* @brief �Ƚϻص�����
	*	
	* @return �Ƚϳɹ�,����ָ��ֵ,���򷵻�NULL
	*
	* @param item1
	* @param item2
	*/
	HTREENODE compare(HTREENODE item1,HTREENODE item2,PNodeCompareCB lpCompareCB);
	
private:
	/** parent of the double link list. if parent_ is NULL,then this item is root	*/
	HTREENODE parent_;

	/** child of the double link list */
	HTREENODE child_;

	/** next sibling of the double link list */
	HTREENODE next_;

	/** prev sibling of the double link list */
	HTREENODE prev_;

	/** QyTree�����Լ�һЩ˽������(����ָHNODE������ֵ) */
	IQyAttribute* attribute_;

	/** counts of the �����ӽ��(�������и����) */
	int childCounts_;

	/** counts of �����ӽ����Ϊ���Ľ�� */
	int rootCounts_;

	/** ��ȼ���*/
	unsigned short level_;

	/** ������� */
	enNodeType modeType_;

	/** ��ο� enTNFlag */
	unsigned int flags_;

	/** ������ */
	HNODE node_;

	/** ����˽����Ϣ��ȡ */
	friend class QyTreePrivate;
	QyTreePrivate* d_ptr_;

	/// һЩ�����Ĳ���
	friend class QyTreeFunction;
};

END_NAMESPACE(qy)

#endif /* __QY_NODE_H__ */
