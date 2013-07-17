#include <assert.h>
#include <string>
#include <map>
//--------------------------------------------------------------------------------------------//
//
// namespace qyUtil
DEFINE_NAMESPACE(qy)

class QyTree;

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
		Clear();
	}
private:
	/** ���һЩ˽����Ϣ[UI�ϵ�һЩֵ]
	*  ��һ�����棬������ͬһ�����ݣ�������ı��ֵ�Ԫ�ز�һ�����Ϳ���ͨ����������
	*  ��Ҫʱ��ͨ���ӿ�ȡ
	*
	* @param PNodeprivateCB �Ļص���(��Ҫ���������� ppֵ)
	*/
	int addPrivate(const char* key , QyTree::PrivateValue pp , PNodeprivateCB lpPrivateCB )
	{
		if( 0 == queryPrivate( key , NULL ) )
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
		while( it != items_.end() )
		{
			if( (*it).first == key )
			{
				if( pp )
				{
					(*pp) = it->second.p;
				}
				items_.erase( it );
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
		while( it != items_.end() )
		{
			if( (*it).first == key )
			{
				if(pp )
				{
					(*pp) = it->second.p;
				}
				return 0;
			}
			it++;
		}
		return -1;
	}

	void Clear()
	{
		itemMAP::iterator it = items_.begin();
		while( it != items_.end() )
		{
			TItem& tm = (*it).second;
			if( tm.cb )
				tm.cb( (it)->first.c_str() , tm.p );
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

/////////////////////////////////////////////////////////////////////////
//
// static function
/** �������� */
class QyTreeFunction
{
public:
	friend class QyTree;

	/** ������������ */
	static bool isfind(HTREENODE hFind,UINT nFlag,UINT nQFlag,PNodequeryCB pQueryCB,void* pQueryCondition){
		bool bIs = false;
		if( QyTree::TN_NULL == nFlag || hFind->getFlags(nFlag) )
		{		
			if( nQFlag == QyTree::QF_ROOT && hFind ->isRoot() )
				bIs = true;
			else if( nQFlag == QyTree::QF_NODE && !hFind ->isRoot() )
				bIs = true;
			else if( nQFlag == QyTree::QF_ALL )
				bIs = true;					

			if( bIs )
			{
				if( pQueryCB )
					return pQueryCB(hFind,pQueryCondition);			
			}
		}
		return bIs;
	}

	/** �����ӽ�� */
	static HTREENODE getchildnode(HTREENODE hNode,UINT nFlag,UINT nQFlag,PNodequeryCB pQueryCB,void* pQueryCondition){
		//��������
		if( hNode && hNode->isHasChilds() )
		{	
			// ��������,û��չ�����߲��ɼ�,�Ͳ���ʾ����
			if( nFlag && 
				( nFlag & QyTree::TN_VISIBLE_BIT) && 
				( !hNode->getFlags(QyTree::TN_VISIBLE_BIT) || !hNode->getFlags(QyTree::TN_EXPAND_BIT) ) )
			{
				return NULL;
			}
			if( nQFlag == QyTree::QF_ROOT && hNode ->getRootCounts() <= 0 ) //ֻ��ѯ��
				return NULL;

			if( nQFlag == QyTree::QF_NODE && (hNode->getNodeCounts() - hNode->getRootCounts() )<= 0 ) //ֻ��ѯ���
				return NULL;

			return hNode->getNextNode(NULL,nFlag,nQFlag,pQueryCB,pQueryCondition);
		}
		return NULL;
	}

	///
	/// ɾ��һ��
	///
	static void DeleteLink(HTREENODE hNode){
		if ( !hNode )
		{
			return;
		}

		HTREENODE next=hNode->next_;
		HTREENODE prev=hNode->prev_;
		if( prev ) prev->next_ = next;
		if( next ) next->prev_ = prev;	
		if( hNode->parent_ && hNode == hNode->parent_->child_ )
		{
			hNode->prev_ = 0;
			if ( next )
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
	static void InsertBack(HTREENODE hNode,HTREENODE hInsNode){
		HTREENODE hInNext = hInsNode->next_;

		HTREENODE hNext = hNode;
		while( (hNext->next_) ) 
			hNext = hNext->next_;

		hNode->level_   = hInsNode->level_;
		hInsNode->next_ = hNode;
		hNode->prev_    = hInsNode;
		hNext->next_    = hInNext;
		hNode->parent_  = hInsNode->parent_;
		if( hInNext ) hInNext->prev_ = hNode;
	}
	///
	/// ��hNode,����hInsNodeǰ��
	///
	static void InsertFront(HTREENODE hNode,HTREENODE hInsNode){
		HTREENODE hNext = hNode;
		while( (hNext->next_) ) 
			hNext = hNext->next_;

		HTREENODE hPrev   = hInsNode->prev_;
		hNode->prev_    = hPrev;
		hNext->next_    = hInsNode;
		hInsNode->prev_ = hNode;
		if( hPrev ) hPrev->next_ = hNode;
		if( hInsNode->parent_ && hInsNode == hInsNode->parent_->child_ ) {
			hInsNode->parent_->child_ = hNode;
		}
		hNode->parent_ = hInsNode->parent_;
		hNode->level_  = hInsNode->level_;
	}
	///
	/// �ݹ���� hNode
	///
	static int RecurGetNode(HTREENODE hParent,HTREENODE hNode){
		if( !hParent || !hNode )
			return -1;

		HTREENODE hTmp = hParent->child_;
		if( !hTmp )
			return -1;   

		while( hTmp )
		{
			if( hTmp == hNode  )
				return 0;

			if( hTmp->isHasChilds() ) 
			{
				if( 0 == RecurGetNode(hTmp,hNode) )
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
		if( hNode && hNode->parent_ ) 
		{
			if ( hNode->isRoot() ) {
				if( !bAdd ) {
					hNode->parent_->modifyCounts( -(hNode->getNodeCounts()+1),-(hNode->getRootCounts()+1) );
				}else{
					hNode->parent_->modifyCounts(  (hNode->getNodeCounts()+1), (hNode->getRootCounts()+1) );
				}
			}else{
				if( !bAdd ) {
					hNode->parent_->modifyCounts(-1,0 );
				}else{
					hNode->parent_->modifyCounts( 1,0 );
				}
			}
		}
	}

	/**
	* @brief �޸�������ͬ���ֵܽ��ĸ�����뼶����Ϣ AND  ͳ�Ƽ���
	*/
	static void ModifySiblingAndStatCount(HTREENODE hNode,
		const int nLevel, const HTREENODE hParent,int &nCounts,int &nRootCounts)
	{
		HTREENODE hTmp = hNode;
		while( hTmp )
		{
			hTmp->parent_ = hParent;
			hTmp->level_  = nLevel;

			if( hTmp->isHasChilds() )
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

END_NAMESPACE(qy)
