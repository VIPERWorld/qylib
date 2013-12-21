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
* ���ߣ� loach.h ( ������)
*
* ʱ�䣺 2009-09-21
*/
DEFINE_NAMESPACE(qy)

class QyParserT;
class QyParserPrivateT;

/// �з����ص�����
/// cspacing �ָ�����( , # ... )
typedef int (*QyPLineParseCallBackT)(QyParserT* lpCompiler , CharT cspacing);

/**
* @brief �ַ��б������ӿ���,������ٵ��ҵ���Ӧ��ֵ,�����ڴ�,��ȡ�ٶ�
* ��������Ҫ�Ľӿڽ��б���
* 
* ʹ��ʱע�� : AddRef() �� Release() �����ĵ���
*/
class QYUTIL_API QyParserT
{
public:
	/// construcot
    QyParserT();
	/// destructor
    ~QyParserT();
public:
	/**
	 * @brief ��������
	*/
	int     handle(const CharT* value,bool bDefCB = true );
	/**
	 * @brief ��Ҫ���������
	*/
	int     handle(const CharT* lpszTxt ,size_t len,bool bDefCB = true );
	/**
	 * @brief ����
	 * @param pCB �ص�����
	 * @param cspacing �ָ�����
	*/
	int     handleCB(QyPLineParseCallBackT pCB , CharT cspacing = L',' );

	/**
	 * @brief �ж��ٲ���ֵ 
	*/
	size_t  counts(void) const;

	/**
	* @brief ��ȡ�ַ�����ֵ
	* @param nRow ����ֵ
	* @param lpszDefault Ĭ��ֵ
	*/
	const CharT*  string(size_t nRow , int& len,const CharT *lpszDefault = NULL);
	/**
	* @brief ��ȡ��������ֵ
	* @param nRow ����ֵ
	* @param nDefault Ĭ��ֵ
	*/
	int    intValue(size_t nRow , int nDefault = 0);

	/**
	* @brief ��ȡ˫������������ֵ
	* @param nRow ����ֵ
	* @param dDefault Ĭ��ֵ
	*/
	double doubleValue(size_t nRow , double dDefault = 0.0);
	/**
	* @brief ֵ
	*/
    const CharT* c_str(void);
	/**
	 * @brief ����
	*/
	size_t length(void);
	/**
	 * @brief ���
	*/
    void  clear();

	/*
	 * @brief λ�õ�
	*/
	void push(short row);
private:
	friend class QyParserPrivateT;
	QyParserPrivateT* d_ptr_;
};

END_NAMESPACE(qy)
