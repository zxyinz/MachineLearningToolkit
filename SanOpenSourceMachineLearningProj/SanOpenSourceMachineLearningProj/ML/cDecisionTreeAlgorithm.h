#include"iostream"
#include"vector"
#include"Core\SanTypes.h"
using namespace std;
using namespace San;
#ifndef __CDECISIONTREEALGORITHM_H__
#define __CDECISIONTREEALGORITHM_H__
template<typename Type>
struct TREE
{
protected:
	uint32				ID;
	Type				Data;
	TREE<Type>*			pTrunk;
	vector<TREE<Type>*>	OffshootPtrArray;
public:
	TREE(const uint32 ID, const Type &InitData, TREE<Type>* pTrunk=nullptr)
		:ID(ID), Data(InitData), pTrunk(pTrunk)
	{
		this->OffshootPtrArray.clear();
	};

	~TREE()
	{ 
		if (this->pTrunk != nullptr)
		{
			this->pTrunk->OffshootPtrArray[this->ID] = nullptr; //Didn't adjust the ID for rest subnode;
		}
		for (uint32 seek = 0; seek < this->OffshootPtrArray.size(); seek = seek + 1)
		{
			if (this->OffshootPtrArray[seek] != nullptr)
			{
				delete (this->OffshootPtrArray[seek]);
			}
		}
		this->ID = 0;
		this->pTrunk = nullptr;
	};

	/*Operator Ovarload*/
	TREE<Type>*& operator[](const uint32 Index)
	{ 
		return this->OffshootPtrArray[Index];
		//return Index < this->OffshootPtrArray.size() ? this->OffshootPtrArray[Index] : nullptr;
	};

	const TREE<Type>*& operator[](const uint32 Index) const
	{ 
		return this->OffshootPtrArray[Index];
		//return Index < this->OffshootPtrArray.size() ? this->OffshootPtrArray[Index] : nullptr;
	};

	/*Tree Node Create & Delete*/
	TREE<Type>* iCreateNode(const Type &Data)
	{
		TREE<Type>* pNode = new TREE<Type>(this->OffshootPtrArray.size(), Data, this);
		this->OffshootPtrArray.push_back(pNode);
		return pNode;
	};
	void iDeleteNode(const uint32 Index)
	{
		if (Index < this->OffshootPtrArray.size())
		{
			delete this->OffshootPtrArray[Index]; //Didn't adjust the ID for rest subnode;
		}
	};
	void iClear()
	{
		for (uint32 seek = 0; seek < this->OffshootPtrArray.size(); seek = seek + 1)
		{
			delete this->OffshootPtrArray[seek];
		}
		this->OffshootPtrArray.clear();
	}

	/*Tree State*/
	Type& iGetData()
	{
		return this->Data;
	}
	uint32 iGetSize() const
	{
		return this->OffshootPtrArray.size();
	}
	uint32 iGetID() const
	{
		return this->ID;
	}
	TREE<Type>* iGetTrunkPtr() const
	{
		return this->pTrunk;
	}
	const TREE<Type>* iGetTrunkPtrC() const
	{
		return this->pTrunk;
	}
	bool iIsRoot() const
	{
		return (this->pTrunk == nullptr);
	};
	bool iIsLeaf() const
	{
		return (this->OffshootPtrArray.size() == 0);
	};
};

struct ATTRIBUTENODE
{
public:
	SString			strName;
	uint32			Index;
	vector<sfloat>	InstanceCount; // Example: YES - No.; NO - No.
	sfloat			Extropy;
public:
	ATTRIBUTENODE()
		:strName(_SSTR("")),
		Index(0),
		Extropy(0.0)
	{
	};
	~ATTRIBUTENODE()
	{
	};
};

struct ATTRIBUTE
{
	SString			strName;
	uint32			ID;
	vector<SString>	ValueArray;
	vector<sfloat>	Boundary;			// Continues boundary array
	bool			bIsContinuesValue;
	bool			bIsTaken;			// For inner attribute array use;
};

typedef pair<vector<SString>, sfloat>	INSTANCE;
typedef TREE<ATTRIBUTENODE>*			lpDECISIONTREENODE;

typedef pair<vector<SString>, SString>		RULE;
typedef vector<RULE>						RULEARRAY;

class cDecisionTreeAlgorithm
{
private:
	vector<ATTRIBUTE>	m_OuterArributeArray;	// Outer attribute array is the array contain all the attribute 
												// which the user gives to the learner (this class)

	vector<ATTRIBUTE>	m_InnerAttributeArray;	// Inner attribute array is the array contain all the translated attribute 
												// which the learner generated by itself to prcess the data 
												// (devide the continues value to several parts, and generate the unknown value)
	vector<SString>		m_ResultArray;			// All the possible value of the result
	lpDECISIONTREENODE	m_pTreeRoot;			// The root node of actual decision tree
	RULEARRAY			m_RuleArray;			// Rule array
protected:
	vector<ATTRIBUTE>	_GenerateInnerAttribute(const ATTRIBUTE &Attribute, const vector<INSTANCE> &InstanceSpace) const;
	INSTANCE			_GenerateInnerInstance(const INSTANCE &Instance) const;
	//vector<INSTANCE>	_CalcUnknownInstance(vector<INSTANCE> &InstanceSpace);
	sfloat				_CalcInformationGain(const uint32 AttributeID, vector<INSTANCE> &InstanceSpace, const sfloat PreviousEntropy);
	bool				_GenerateDecisionTree(vector<INSTANCE> InstanceSpace, vector<ATTRIBUTE> AttributeSpace, lpDECISIONTREENODE pNode);
	SString				_SearchDecisionTree(const INSTANCE &Instance, const lpDECISIONTREENODE pNode) const;
	void				_PrintTree(const lpDECISIONTREENODE pNode, const SString &PrevString, SString &strOutputString) const;
	bool				_GenerateRuleArray(const lpDECISIONTREENODE pNode, RULE CurrentRule);
	bool				_RulePredict(const RULE &Rule, const INSTANCE Instance, SString &strResult) const;
	bool				_RuleCompare(const RULE &Rule1, const RULE &Rule2) const;
public:
	cDecisionTreeAlgorithm();
	~cDecisionTreeAlgorithm();
	bool iCreateAttribute(const SString &strAttributeName, vector<SString> &ValueArray);
	bool iCreateAttribute(const SString &strAttributeName, vector<sfloat> &BoundaryArray);
	void iDeleteAttribute(const SString &strAttributeName);
	void iClearAttributeArray();
	bool iUpdateResultAttribute(const SString &strResultName, vector<SString> &ValueArray);
	bool iGenerateTree(const vector<INSTANCE> &InstanceSpace);
	bool iGenerateRule();
	bool iPruningTree(const vector<INSTANCE> &InstanceSpace);
	SString iPredict(const INSTANCE &Instance) const;
	SString iPrediceByRuleSet(const INSTANCE &Instance) const;
	SString iPrintTree() const;
	SString iPrintInnerAttribute() const;
	SString iPrintRule() const;
};
#endif