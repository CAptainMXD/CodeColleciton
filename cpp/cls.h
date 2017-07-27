#define CLSID(cclass,fclass,cid) 	CLSID_(cid,fclass::CLS_ID)
#define CLSID_(cid,fid) 	public:\
	enum { CLS_ID = (fid | cid), CLS_CID = cid, }; \
virtual bool IsKindOf(uint64 cls) { return (CLS_ID & cls) > 0; } \
virtual uint64 GetClsID() { return CLS_CID; }



const uint64 CLSID_GameObject			= 0x00000001;
const uint64 CLSID_Character         		= 0x00000002;
const uint64 CLSID_Unit			 	= 0x00000004;
const uint64 CLSID_Creature			= 0x00000008;
const uint64 CLSID_SmartUnit	  		= 0x00000010;




class GameObject : public mars::GameObjectClsQuery
{
	CLSID(GameObject,GameObjectClsQuery,CLSID_GameObject);
}




class GameObjectClsQuery
{

	public:
		enum{
			CLS_ID= fid|cid,
			CLS_CID=cid,
		};
		virtual bool IsKindOf(long long cls){return (CLS_ID& cls)>0;}
		virtual long long GetClsID(){return CLS_CID;}
	public:
		template<class T>
			T * QueryObject()
			{
				if(!IsKindOf(T::CLS_CID))
					return NULL;
				return (T*)this;		
			}
};
