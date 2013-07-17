#include "qynode.h"
#include <assert.h>
#include <string>
#include <map>

DEFINE_NAMESPACE(qy)

/**
 * @brief ����˽����Ϣ��ȡ��
 *
 * @author 2009-07-05
 *
 */
class QyTreePrivate
{
public:
	QyTreePrivate(){
	}
	~QyTreePrivate(){
		clear();
	}
private:
	/** ���һЩ˽����Ϣ[UI�ϵ�һЩֵ]
     *  ��һ�����棬������ͬһ�����ݣ�������ı��ֵ�Ԫ�ز�һ�����Ϳ���ͨ����������
     *  ��Ҫʱ��ͨ���ӿ�ȡ
     *
     * @param PNodeprivateCB �Ļص���(��Ҫ���������� ppֵ)
     */
	int addPrivate(const char* key , QyTree::PrivateValue pp , PNodeprivateCB lpPrivateCB)
	{
		if (0 == queryPrivate( key , NULL))
		{
			return -1;
		}
		TItem tm;
		tm.cb = lpPrivateCB;
		tm.p  = pp;
		items_[key] = tm ;
        
		return 0;
	}
    
	/** ɾ�����˽����Ϣ
     *
     * @param pp if pp != NULL ����ɾ���Ƿ�����,Ȼ�����������pp���д���
     */
	int removePrivate(const char* key,QyTree::PrivateValue* pp)
	{
		itemMAP::iterator it = items_.begin();
		while( it != items_.end())
		{
			if ((*it).first == key)
			{
				if (pp)
				{
					(*pp) = it->second.p;
				}
				items_.erase( it);
				return 0;
			}
			it++;
		}
		return -1;
	}
	/**
     * ��ѯ��Ӧ��ֵ.
     */
	int queryPrivate(const char* key,QyTree::PrivateValue* pp)
	{
		itemMAP::iterator it = items_.begin();
		while( it != items_.end())
		{
			if ((*it).first == key)
			{
				if (pp)
				{
					(*pp) = it->second.p;
				}
				return 0;
			}
			it++;
		}
		return -1;
	}
    
	void clear()
	{
		itemMAP::iterator it = items_.begin();
		while( it != items_.end())
		{
			TItem& tm = (*it).second;
			if (tm.cb)
				tm.cb( (it)->first.c_str() , tm.p);
			it++;
		}
		items_.clear();
	}
private:
	/// һЩ˽�����ԵĴ�ȡ
	typedef struct tagItem
	{
		QyTree::PrivateValue p;
		PNodeprivateCB cb;
	}TItem;
	typedef std::map<std::string,TItem> itemMAP;
	itemMAP items_;
    
	/// id
	QyNodeID id_;
    
	friend class QyTree;
};

class QyTreeFunction
{
public:
	friend class QyTree;
    
	/** ������������ */
	static bool isfind(HTREENODE hFind,UINT nFlag,UINT nQFlag,PNodequeryCB pQueryCB,void* pQueryCondition){
		bool bIs = false;
		if (QyTree::TN_NULL == nFlag || hFind->getFlags(nFlag))
		{
			if (nQFlag == QyTree::QF_ROOT && hFind ->isRoot())
				bIs = true;
			else if (nQFlag == QyTree::QF_NODE && !hFind ->isRoot())
				bIs = true;
			else if (nQFlag == QyTree::QF_ALL)
				bIs = true;
            
			if (bIs)
			{
				if (pQueryCB)
					return pQueryCB(hFind,pQueryCondition);
			}
		}
		return bIs;
	}
    
	/** �����ӽ�� */
	static HTREENODE getchildnode(HTREENODE hNode,UINT nFlag,UINT nQFlag,PNodequeryCB pQueryCB,void* pQueryCondition){
		//��������
		if (hNode && hNode->isHasChilds())
		{
			// ��������,û��չ�����߲��ɼ�,�Ͳ���ʾ����
			if (nFlag &&
              (nFlag & QyTree::TN_VISIBLE_BIT) &&
              (!hNode->getFlags(QyTree::TN_VISIBLE_BIT) || !hNode->getFlags(QyTree::TN_EXPAND_BIT)))
			{
				return NULL;
			}
			if (nQFlag == QyTree::QF_ROOT && hNode ->getRootCounts() <= 0) //ֻ��ѯ��
				return NULL;
            
			if (nQFlag == QyTree::QF_NODE && (hNode->getNodeCounts() - hNode->getRootCounts())<= 0) //ֻ��ѯ���
				return NULL;
            
			return hNode->getNextNode(NULL,nFlag,nQFlag,pQueryCB,pQueryCondition);
		}
		return NULL;
	}
    
	///
	/// ɾ��һ��
	///
	static void deleteLink(HTREENODE hNode){
		if(!hNode)
		{
			return;
		}
        
		HTREENODE next=hNode->next_;
		HTREENODE prev=hNode->prev_;
		if (prev) prev->next_ = next;
		if (next) next->prev_ = prev;
		if (hNode->parent_ && hNode == hNode->parent_->child_)
		{
			hNode->prev_ = 0;
			if(next)
			{
				next->prev_ = NULL;
			}
			hNode->parent_->child_ = hNode->next_;
            
		}
		hNode->parent_ = 0;
		hNode->prev_   = 0;
		hNode->next_   = 0;
		hNode->level_  = 0;
	}
	///
	/// ��hInsNode ������һ��hNode
	///
	static void insertBack(HTREENODE hNode,HTREENODE hInsNode){
		HTREENODE hInNext = hInsNode->next_;
        
		HTREENODE hNext = hNode;
		while( (hNext->next_))
			hNext = hNext->next_;
        
		hNode->level_   = hInsNode->level_;
		hInsNode->next_ = hNode;
		hNode->prev_    = hInsNode;
		hNext->next_    = hInNext;
		hNode->parent_  = hInsNode->parent_;
		if (hInNext) hInNext->prev_ = hNode;
	}
	///
	/// ��hNode,����hInsNodeǰ��
	///
	static void insertFront(HTREENODE hNode,HTREENODE hInsNode){
		HTREENODE hNext = hNode;
		while( (hNext->next_))
			hNext = hNext->next_;
        
		HTREENODE hPrev   = hInsNode->prev_;
		hNode->prev_    = hPrev;
		hNext->next_    = hInsNode;
		hInsNode->prev_ = hNode;
		if (hPrev) hPrev->next_ = hNode;
		if (hInsNode->parent_ && hInsNode == hInsNode->parent_->child_) {
			hInsNode->parent_->child_ = hNode;
		}
		hNode->parent_ = hInsNode->parent_;
		hNode->level_  = hInsNode->level_;
	}
	///
	/// �ݹ���� hNode
	///
	static int recurGetNode(HTREENODE hParent,HTREENODE hNode){
		if (!hParent || !hNode)
			return -1;
        
		HTREENODE hTmp = hParent->child_;
		if (!hTmp)
			return -1;
        
		while( hTmp)
		{
			if (hTmp == hNode )
				return 0;
            
			if (hTmp->isHasChilds())
			{
				if (0 == recurGetNode(hTmp,hNode))
					return 0;
			}
			hTmp = hTmp->next_;
		}
		return  -1;
	}
	/**
     * @brief �ݹ�����޸����������й����Ľ��
     */
	static void modifyCounts(HTREENODE hNode,BOOL bAdd=TRUE){
		if (hNode && hNode->parent_)
		{
			if(hNode->isRoot()) {
				if (!bAdd) {
					hNode->parent_->modifyCounts( -(hNode->getNodeCounts()+1),-(hNode->getRootCounts()+1));
				}else{
					hNode->parent_->modifyCounts(  (hNode->getNodeCounts()+1), (hNode->getRootCounts()+1));
				}
			}else{
				if (!bAdd) {
					hNode->parent_->modifyCounts(-1,0);
				}else{
					hNode->parent_->modifyCounts( 1,0);
				}
			}
		}
	}
    
	/**
     * @brief �޸�������ͬ���ֵܽ��ĸ�����뼶����Ϣ AND  ͳ�Ƽ���
     */
	static void modifySiblingAndStatCount(HTREENODE hNode,
                                          const int nLevel, const HTREENODE hParent,int &nCounts,int &nRootCounts)
	{
		HTREENODE hTmp = hNode;
		while( hTmp)
		{
			hTmp->parent_ = hParent;
			hTmp->level_  = nLevel;
            
			if (hTmp->isHasChilds())
			{
				nCounts += hTmp->getNodeCounts()+1;
				nRootCounts += hTmp->getRootCounts()+1;
                
				hTmp->modifyChildLevel();
			}
			else
				nCounts += 1;
            
			hTmp = hTmp->next_;
		}
	}
};


QyNode::QyNode()
:QyUnknown(NULL)
{
}

QyNode::~QyNode(){	
}

//-----------------------------------------------------------------------------------//

// �¼�֪ͨ(��������)
#define NOTIFY_OBJSERVER(code,param1,param2) { IQyNodeObjserver* pObjserver = GetNodeObjserver();\
	if (pObjserver) pObjserver->onNodeNotify(code,(void*)(param1),(void*)(param2),0);}

// ��������
#define NOTIFY_OBJSERVER3(code,param1,param2,param3) { IQyNodeObjserver* pObjserver = GetNodeObjserver();\
	if (pObjserver) pObjserver->onNodeNotify(code,(void*)(param1),(void*)(param2),(void*)(param3));}

/// Ĭ�ϵıȽϺ���
int qyutil_default_compare_callback(HTREENODE node1,HTREENODE node2)
{
	if (node1->isRoot()){ 

		if (node2->isRoot()){
			return 0;
		}
		return -1;
	}
	if (node2->isRoot())
		return 1;

	return 0;
}
//-----------------------------------------------------------------------------------//
QyTree::QyTree(const QyNodeID& inId ,QyTree::enNodeType type ,HNODE node,UINT nFlags)
: flags_(nFlags)
, attribute_(NULL)
, modeType_( type)
{		
	node_ = node;
	qyAddRef(node_);
	parent_ = NULL;
	next_   = NULL;
	prev_   = NULL;
	child_  = NULL;
	rootCounts_ = childCounts_ = 0;
	level_ = -1;
	d_ptr_ = new QyTreePrivate();
	setId( inId);
}

QyTree::~QyTree()
{
	// �ͷŵ�
	onNodeCommand(CMD_ITEM_RELEASE);
	qyRelease(attribute_);
	qyRelease(node_);
	removeAllChild();
	HTREENODE hTmp=0,hNode = next_;
	while(hNode)
	{
		hTmp  = hNode;
		hNode = hNode->next_;				
		hTmp->next_ = 0;
		delete hTmp;		
		hTmp = NULL;
	}
	next_ = 0;
	parent_ = NULL;
	prev_ = NULL;
	delete d_ptr_;
}

HTREENODE QyTree::insertSiblingNode(enNodeType type,
									   const QyNodeID& inId,
									   const HNODE node,									   
									   const HTREENODE pInsNode,BOOL bBack,PNodeCompareCB lpCompareCB)
{
	if (!node){		
		return NULL;
	}
	if(pInsNode &&
		(pInsNode->getParentNode() != this ->getParentNode())){
			// ������ͬһ�������ܲ���
			return NULL;
	}

	HTREENODE hNewNode = new QyTree(inId,type,node);
	if (NULL == hNewNode) {
		return NULL;
	}

	HTREENODE hInsNode = NULL;	
	if (!pInsNode)
	{
		HTREENODE hHeader = getHeadNode();
		hInsNode = compare(hNewNode,hHeader,lpCompareCB);
		if (!hInsNode)// ��ͬ���ֵ����һ�����			
			hInsNode = getTailNode();
	}

	if (!bBack)
		QyTreeFunction::insertFront(hNewNode,hInsNode);
	else
		QyTreeFunction::insertBack(hNewNode,hInsNode);		

	//�����丸���,��������+
	HTREENODE hParent = hNewNode->parent_;
	if (hParent)
	{
		hParent->onNodeCommand(CMD_ITEM_ADD,(void*)hNewNode);
		hParent->modifyCounts(1,hNewNode->isRoot()?1:0);
		NOTIFY_OBJSERVER(IQyNodeObjserver::ENC_NODE_Add,hParent,hNewNode);
	}
	return hNewNode;
}

int QyTree::sortSliblingNode(PNodeCompareCB lpCompareCB)
{
	HTREENODE h1 = getHeadNode();
	HTREENODE h2 = h1->next_;
	if (this == h1 || NULL == h2) // ��ʾֻ��һ����㣬����Ҫ����
		return 0;

	HTREENODE h3 , hFind = NULL;
	while( h2)
	{ 	
		h3 = h2;
		while( h3)
		{
			int nResult = 0;
			if (lpCompareCB)
			{
				nResult = lpCompareCB(h3,h1);
			}
			else
			{
				nResult = qyutil_default_compare_callback(h3,h1);
			}
			if (nResult < 0)
			{
				hFind = h3;
				break;
			}
			h3 = h3->next_;
		}
		if (hFind)
		{
			// �ѵ�һ�����������,��hFind���뵽h1֮ǰ
			// Ȼ��� h1 ���ɵڶ������Ƚϵ�ֵ

			QyTreeFunction::deleteLink( hFind);
			QyTreeFunction::insertFront( hFind , h1);
			h2 = h1->next_;
			hFind = NULL;
		}
		else
		{
			h1 = h2;
			h2 = h2->next_;
		}
	}
	return 0;
}

HTREENODE QyTree::insertChildNode(enNodeType type,
									 const QyNodeID& inId ,
									 const HNODE node ,									 
									 const HTREENODE pInsNode,BOOL bBack,PNodeCompareCB lpCompareCB)
{
	if (!node)
	{
		return NULL;
	}
	HTREENODE hInsNode = 0;
	HTREENODE hNewNode = new QyTree(inId , type , node);
	if (NULL == hNewNode){		
		return NULL;
	}
	if (NULL == child_) 
	{
		childCounts_ = 0;
		hNewNode->parent_ = this;
		hNewNode->level_  = this->level_+1;
		child_  = hNewNode;
	} else {	
		//���ӽ��,����������ӵ��ӽ����
		hInsNode = pInsNode;
		if (!hInsNode) 
		{	
			// ���Ƚ�������,�ɹ�����0
			hInsNode = compare(hNewNode,child_,lpCompareCB);
			if (!hInsNode)
			{ 
				//�嵽βȥ
				if (child_){
					hInsNode = child_->getTailNode();
				}
			}
		}
		if (hInsNode && hInsNode->getParentNode() == this) 
		{
			if (!bBack)
				QyTreeFunction::insertFront(hNewNode,hInsNode);
			else 
				QyTreeFunction::insertBack(hNewNode,hInsNode);
		}
		else
		{
			delete hNewNode;
			hNewNode = 0;
		}
	}

	if (hNewNode)
	{
		this->onNodeCommand(CMD_ITEM_ADD,(void*)hNewNode);
		//�����丸���,��������+
		modifyCounts(1,hNewNode->isRoot()?1:0);

		NOTIFY_OBJSERVER(IQyNodeObjserver::ENC_NODE_Add,hNewNode->parent_,hNewNode);
	}
	return hNewNode;
}
/**
* @brief ɾ��һ�����,�ͷ��ڴ�
*/
int QyTree::deleteNode(HTREENODE hNode)
{
	if (hNode == NULL)
		return -1;

	// �����������Ƿ�����������ӽ��(add by loach 2009-06-14,���һ����ȫ���)
	if (FALSE == hNode->queryPNode( this)){
		assert(0);
		return -1;
	}

	HTREENODE hParent = hNode->parent_;
	//�����丸���,����-
	if (hParent)
	{
		if (hNode->isRoot())
		{
			hParent->modifyCounts( -(hNode->getNodeCounts()+1),-(hNode->getRootCounts()+1)) ;
		} 
		else 
		{
			hParent->modifyCounts( -1,0) ;
		}
		hParent->onNodeCommand(CMD_ITEM_DELETE,(void*)hNode);
	}
	// ɾ��һ��
	QyTreeFunction::deleteLink(hNode);

	NOTIFY_OBJSERVER(IQyNodeObjserver::ENC_NODE_Delete,hParent , hNode);

	delete hNode;
	hNode = NULL;

	return 0;
}
/**
* @brief ����һ�����,�����ͷ�
*/
int QyTree::detachNode(const HTREENODE hNode)
{
	if (hNode == NULL)
		return -1;

	// �����������Ƿ�����������ӽ��(add by loach 2009-06-14,���һ����ȫ���)
	if (FALSE == hNode->queryPNode( this)){
		assert(0);
		return -1;
	}
	if (hNode->parent_) 
	{	
		//�����丸���,��������-
		if(hNode->isRoot()) {
			hNode->parent_->modifyCounts( -(hNode->getNodeCounts()+1),-(hNode->getRootCounts()+1));
		}else{
			hNode->parent_->modifyCounts( -1,0);
		}
		hNode->parent_->onNodeCommand(CMD_ITEM_DELETE,(void*)hNode);
	}
	NOTIFY_OBJSERVER(IQyNodeObjserver::ENC_NODE_Remove,hNode->parent_,hNode);
	QyTreeFunction::deleteLink(hNode);
	return 0;
}
/**
* @brief ���һ�����,hNode�����ڶ���������������Ч,������һ���ֲ�����
*/
int QyTree::attachNode(HTREENODE hNode,PNodeCompareCB lpCompareCB)// throw(enThrowError)
{
	if (NULL == hNode) 
		return -1;

	if (!hNode ->node_) {
		//throw Err_throw_node_null;
		return -1;
	}
	int nCounts=0,nRootCounts=0;

	//ͳ���ֵܽ��ļ���
	QyTreeFunction::modifySiblingAndStatCount(hNode,(this->level_+1),this,nCounts,nRootCounts);

	if (NULL == child_) 
	{
		childCounts_=0;
		hNode->level_  = this->level_+1;
		hNode->prev_   = NULL;
		hNode->next_   = NULL;
		hNode->parent_ = this;
		child_         = hNode;
	}
	else 
	{
		HTREENODE hInsNode = compare(hNode,child_,lpCompareCB);
		if (hInsNode){
			QyTreeFunction::insertFront( hNode,hInsNode);
		}else{
			if(child_){
				hInsNode = child_->getTailNode();
			}
			QyTreeFunction::insertBack(hNode,hInsNode);
		}	
	}

	//�����丸���,��������+
	if (hNode->parent_) 
	{	
		hNode->parent_->onNodeCommand(CMD_ITEM_ADD,(void*)hNode);
		hNode->parent_->modifyCounts(nCounts,nRootCounts);

		NOTIFY_OBJSERVER(IQyNodeObjserver::ENC_NODE_Add,hNode->parent_,hNode);
	}
	return 0;
}

BOOL QyTree::queryPNode(const HTREENODE hNode)
{
	HTREENODE hParent = this;
	for(;;)
	{
		hParent = hParent->getParentNode();
		if (NULL == hParent) 
			return FALSE;

		if (hNode == hParent)
			return TRUE;	
	}
	return FALSE;
}

HTREENODE QyTree::getTopNode(void)
{
	HTREENODE hParent = this;
	for(;;)
	{
		HTREENODE hTemp = hParent->getParentNode();
		if (NULL == hTemp) 
			return hParent;
		hParent = hTemp;
	}
	return this;
}

/**
* @brief �Ƿ��������Ľ�����
* ������������,�����ӽ��,�ֵܽ��
*/
BOOL QyTree::isHasNode(const HTREENODE hNode)
{
	if (NULL == hNode)
		return FALSE;

	// �����������Ƿ�����������ӽ��(add by loach 2009-06-14,���һ����ȫ���)
	if (FALSE == hNode->queryPNode( this)){
		return FALSE;
	}

	HTREENODE hTmp = child_;	
	while( hTmp)
	{
		if (hTmp == hNode)
			return TRUE;
		if (hTmp->isHasChilds()) 
		{
			if (0 == QyTreeFunction::recurGetNode(hTmp,hNode))
				return TRUE;
		}
		hTmp = hTmp->next_;
	}

	return FALSE;
}

HTREENODE QyTree::isHasNode(const HNODE node,UINT nQFlag,PNodequeryCB pQueryCB,void* pQueryCondition)
{
	bool bIs  = false;
	HTREENODE hFindNode = NULL,hNode = child_;

	while(hNode)
	{
		bIs = (node == hNode->getNode());
		if (bIs)
		{
			bIs = false;
			if (nQFlag == QF_ROOT && hNode ->isRoot())
				bIs = true;			
			else if (nQFlag == QF_NODE && !hNode->isRoot())
				bIs = true;
			else if (nQFlag == QF_ALL)
				bIs = true;			

			if (bIs)
			{
				if (pQueryCB)
				{//����������
					if (pQueryCB(hNode,pQueryCondition))
						return hNode;	
				}
				else
				{//û����������
					return hNode;
				}
			}
		}

		// ��ֵ
		if (hNode->isHasChilds()) 
		{
			//����,���ӽ��û����
			if (nQFlag == QF_ROOT && hNode ->getRootCounts() <= 0)
				goto L1;

			// ���ӽ�㣬��û���ӽ��
			else if (nQFlag == QF_NODE &&(hNode->getNodeCounts() - hNode ->getRootCounts()) <= 0)
				goto L1;

			hFindNode = hNode->isHasNode(node,nQFlag,pQueryCB,pQueryCondition);//�ݹ����
			if (NULL != hFindNode)
				return hFindNode;
		}
L1:
		hNode = hNode->next_;
	}
	return NULL;
}

#define ID_MATCH(Id,inId)(Id == inId)

HTREENODE QyTree::isHasNode(const QyNodeID& inId,UINT nQFlag,PNodequeryCB pQueryCB,void* pQueryCondition)
{	
	static QyNodeID _Nil_id;
	QyNodeID Id;
	HTREENODE hFindNode = NULL,hNode = child_;
	BOOL bId  = FALSE;

	if (inId == _Nil_id)
		return NULL;
		
	while(hNode)
	{
		bId = ID_MATCH(hNode->d_ptr_->id_ , inId);
		if (bId)
		{
			bId = false;
			if (nQFlag == QF_ROOT && hNode ->isRoot())
				bId = true;
			else if (nQFlag == QF_NODE && !hNode->isRoot())
				bId = true;
			else if (nQFlag == QF_ALL)
				bId = true;

			if (bId)
			{
				if (pQueryCB)
				{
					if (pQueryCB(hNode,pQueryCondition))
						return hNode;
				}
				else
				{
					return hNode;
				}
			}
		}

		// ��ֵ
		if (hNode->isHasChilds())
		{
			if (nQFlag == QF_ROOT && hNode ->getRootCounts() <= 0)
				goto L1; //����,���ӽ��û����

			else if (nQFlag == QF_NODE &&
				( hNode->getNodeCounts() - hNode ->getRootCounts()) <= 0)
				goto L1;

			hFindNode = hNode->isHasNode(inId,nQFlag,pQueryCB,pQueryCondition);//�ݹ����
			if (NULL != hFindNode)
				return hFindNode;
		}
L1:
		hNode = hNode->next_;
	}
	return NULL;
}
/** 
* @brief �Ƿ�����ӽ��
*/
BOOL QyTree::isHasChilds() const
{
	return child_?TRUE:FALSE;
}

BOOL QyTree::isRoot() const
{ 
	return (modeType_ == NT_ROOT);
}

/**
* @brief ��ȡ�����
*/
HTREENODE QyTree::getParentNode() const
{return parent_;}

/**
* @brief ��ȡ��һ��ͬ���ֵܽ��
*/
HTREENODE QyTree::getNextNode() const
{ return next_;}
/**
* @brief ��ȡ��һ��ͬ���ֵܽ��
*/
HTREENODE QyTree::getPrevNode() const
{ return prev_;}
/**
* @brief ��ȡ�ӽ��
*/
HTREENODE QyTree::getChildNode() const
{ return child_;}
/**
* @brief ��ȡβ�����
*/
HTREENODE QyTree::getTailNode() const
{
	HTREENODE hNode = const_cast<HTREENODE>(this);	
	while( hNode){
		if (NULL == hNode->next_)
			break;
		hNode = hNode->next_;
	}
	return hNode;
}
/**
* @brief ��ȡͷ���
*/
HTREENODE QyTree::getHeadNode() const
{
	HTREENODE hNode = const_cast<HTREENODE>(this);	
	while( hNode){
		if (NULL == hNode->prev_)
			break;
		hNode = hNode->prev_;
	}
	return hNode;
}
/**
* @brief ��ȡ���н�������(�������������)
*/
int QyTree::getNodeCounts() const
{ return childCounts_;}
/**
* @brief ��ȡ����������(�������ӽ������)
*/
int QyTree::getRootCounts() const
{ return rootCounts_;}
/** 
* @brief ɾ�������ӽ��
*/
void QyTree::removeAllChild()
{		
	HTREENODE hNode = child_;
	HTREENODE hTmp  = 0;
	while( hNode)
	{
		hTmp  = hNode;
		hNode = hNode->next_;				
		hTmp->next_ = 0;

		// �ݹ����
		if (hTmp->isHasChilds())
			hTmp->removeAllChild();

		// ɾ�������
		this->onNodeCommand(CMD_ITEM_DELETE,(void*)hTmp);

		// ɾ��
		delete hTmp;
		hTmp = NULL;

	}
	child_ = NULL;
	//�����丸���,����-
	if (parent_ && (childCounts_> 0 || rootCounts_ > 0))
	{	
		parent_->modifyCounts( -childCounts_,-rootCounts_) ;
	}
	childCounts_ = 0;
	rootCounts_  = 0;
}
void QyTree::counts(int nNodeCounts, int nRootCounts)
{
	childCounts_+=nNodeCounts;
	rootCounts_ +=nRootCounts;
	if (childCounts_ <= 0) {
		child_ =0;
		childCounts_ = 0;		
	}
	if (rootCounts_< 0) rootCounts_ = 0;
}
int QyTree::onNodeCommand( UINT eCmd,void* param0)
{
	if (node_)
		return node_->onNodeCommand(this,eCmd,param0);
	return 0;
}
/**
* @brief �޸����Ľ����
*/
void QyTree::modifyCounts(int nNodeCounts, int nRootCounts)
{
	this->counts(nNodeCounts,nRootCounts);

	HTREENODE hParent = getParentNode();
	while( hParent) {
		hParent ->counts(nNodeCounts,nRootCounts);
		hParent = hParent ->getParentNode();
	}
}
/**
* @brief �������ӽ��ļ���
*/
void QyTree::modifyChildLevel()
{
	HTREENODE hNode = child_;
	while(hNode) 
	{
		hNode->level_ = level_+1;
		if (hNode->isHasChilds()) hNode->modifyChildLevel();
		hNode = hNode->next_;
	}
}

void QyTree::setFlags(int bits,bool isTrue)
{
	if (isTrue){
		flags_|=bits;
		return ;
	}
	flags_&=~bits;
	NOTIFY_OBJSERVER3(IQyNodeObjserver::ENC_NODE_Flags_bit,this,&bits,&isTrue);
}

void QyTree::setFlags(int nFlags)
{
	UINT old = flags_;
	flags_ = (nFlags & 0xFFFFFFFF);
	NOTIFY_OBJSERVER3(IQyNodeObjserver::ENC_NODE_Flags_update,this,&flags_,&old);
}

int  QyTree::getFlags() const
{
	return flags_;
}

BOOL QyTree::getFlags(int bits) const
{
	return(flags_ & bits)?TRUE:FALSE;
}

int QyTree::getLevel(void) const
{ return level_; }

void QyTree::setLevel(unsigned short level)
{
	level_ = level;
	modifyChildLevel();
}

HNODE QyTree::getNode(void) const{
	return node_;
}

/** ��HTREENODE ����һ��idֵ( add by loach 2009-06-08)
*
* @param lpId 
*/
void QyTree::setId(const QyNodeID& inId)
{
	d_ptr_->id_ = inId;
}

//void QyTree::getId(QyNodeID& outId) const{
//	outId = d_ptr_->id_;
//}

const char* QyTree::getId(size_t* pOutlen) const{
	if (pOutlen)
		*pOutlen = d_ptr_->id_.length();
	return d_ptr_->id_.c_str();
}

HTREENODE QyTree::getNextNode(HTREENODE hNode,
								 UINT nFlag,
								 UINT nQFlag,
								 PNodequeryCB pQueryCB,void* pQueryCondition) const
{
	nQFlag &= QF_ALL;
	HTREENODE hParent = 0,hFind = 0;
	if (!hNode)
	{ // �������ӽ�㿪ʼ��
		hFind = this->getChildNode();
		if (hFind && QyTreeFunction::isfind(hFind,nFlag,nQFlag,pQueryCB,pQueryCondition))
		{
			return hFind;
		}
		else
		{
			hParent = hFind;
		}
	}
	else
	{
		hParent = hNode;
	}

	hFind = QyTreeFunction::getchildnode(hParent,nFlag,nQFlag,pQueryCB,pQueryCondition);
	if (NULL != hFind)
		return hFind;

	while( hParent) 
	{
		HTREENODE hNext = hParent->getNextNode();
		while( hNext)
		{
			if (QyTreeFunction::isfind(hNext,nFlag,nQFlag,pQueryCB,pQueryCondition))
				return hNext;

			hFind = QyTreeFunction::getchildnode(hNext,nFlag,nQFlag,pQueryCB,pQueryCondition);
			if (NULL != hFind)
				return hFind;

			hNext = hNext ->getNextNode();
		}
		hParent = hParent ->getParentNode();
		if (NULL == hParent || hParent == this)
			return NULL;
	}
	return NULL;
}

void QyTree::checkNode(int nFlag)
{
	if (nFlag == QyTree::CF_REVERSE)
	{ //��ѡ
		setFlags(TN_CHECK_BIT,!getFlags(TN_CHECK_BIT));
	}else if (nFlag == QyTree::CF_SELECT)
	{ //ѡ��
		setFlags(TN_CHECK_BIT,true);
	}else if (nFlag == QyTree::CF_NOSELECT)
	{ //ȫѡ
		setFlags(TN_CHECK_BIT,false);
	}
	if (isHasChilds())
	{
		HTREENODE hNode = child_;	
		while(hNode)
		{
			hNode->checkNode(nFlag);
			hNode = hNode->next_;
		}
	}
}

void QyTree::checkSelfNode(int nFlag)
{
	if (nFlag == QyTree::CF_REVERSE) { //��ѡ
		setFlags(TN_CHECK_BIT,!getFlags(TN_CHECK_BIT));
	}else if (nFlag == QyTree::CF_SELECT) { //ѡ��
		setFlags(TN_CHECK_BIT,true);
	}else if (nFlag == QyTree::CF_NOSELECT) { //ȫѡ
		setFlags(TN_CHECK_BIT,false);
	}
}

int QyTree::dragNode(HTREENODE srcNode,
						HTREENODE destNode,
						BOOL bRoot,
						PNodeCompareCB lpCompareCB)
{
	if (!srcNode || !destNode || srcNode == destNode)
		return -1;
	if (!srcNode->isRoot()) { //���Ǹ�
		if (!destNode->isRoot())
		{
			//���Ǹ�
			if (srcNode->getParentNode() == destNode->getParentNode())
			{
				return -2; //��ͬ���ֵ�,����Ҫ�϶�
			}
		}else if (srcNode->getParentNode() == destNode) { // destNode �Ǹ�
			return -2; //�Ѿ���destNode���ӽ��,Ҳ����Ҫ�϶�
		}			
	}
	if (!bRoot && srcNode->isRoot()) 
	{
		// ����������϶�,��Ҫ�����ж�
		return -3;
	}
	// �ȴ�ԭ���������������
	HTREENODE hParent = 0 ,  hOldParent = srcNode->getParentNode();
	// ���destNode������,��ֱ���ƶ�destNode����
	if (destNode->isRoot()) {
		hParent = destNode;
	}
	else
	{
		hParent = destNode->getParentNode();
	}

	if (hParent)
	{
		hOldParent->detachNode(srcNode);
		hParent->attachNode(srcNode,lpCompareCB);
		return 0;
	}	
	return -1;
}

// private ��˽�к���
// �ȽϺ���
HTREENODE QyTree::compare(HTREENODE item1,
							 HTREENODE item2,
							 PNodeCompareCB lpCompareCB)
{
	// �������������lpCompareCB == NULL
	if (!getFlags(TN_SORT_BIT) && (NULL == lpCompareCB))
		return NULL;

	if (NULL  == item1 || NULL == item2)
		return NULL;

	if (item1 == item2)
		return NULL;		

	HTREENODE hNode = item2;
	int nResult = 0;

	// �Ƚ�
	while( hNode)
	{ 		
		if (lpCompareCB)
		{
			nResult = lpCompareCB(item1,hNode);
		}
		else
		{
			nResult = qyutil_default_compare_callback(item1 , hNode);
		}
		if (nResult < 0)
			return hNode;

		hNode = hNode->next_;
	}
	return NULL;
}

//------------------------------------------------------------------------//
//
// add by loach 2009-06-08

/** ���һЩ˽����Ϣ[UI�ϵ�һЩֵ]
*  ��һ�����棬������ͬһ�����ݣ�������ı��ֵ�Ԫ�ز�һ�����Ϳ���ͨ����������
*  ��Ҫʱ��ͨ���ӿ�ȡ
*/
int QyTree::addPrivate(const char* key , PrivateValue pp ,PNodeprivateCB lpPrivateCB)
{
	if (d_ptr_)
		return d_ptr_->addPrivate(key , pp ,lpPrivateCB);
	return -1;
}

/** ɾ�����˽����Ϣ
*
* @param pp if pp != NULL ����ɾ���Ƿ�����,Ȼ�����������pp���д���
*/
int QyTree::removePrivate(const char* key,PrivateValue* pp)
{
	if (d_ptr_)
		return d_ptr_->removePrivate(key,pp);
	return -1;
}

/**
* ��ѯ��Ӧ��ֵ.
*/
int QyTree::queryPrivate(const char* key,PrivateValue* pp)
{
	if (d_ptr_)
		return d_ptr_->queryPrivate(key,pp);
	return -1;
}

END_NAMESPACE(qy)
