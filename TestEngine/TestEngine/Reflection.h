#pragma once

#include <string>
#include <map>
#include <vector>

#include <iostream>

#include "Core\CoreManager.h"
#include "Misc.h"

//===== HERE COMES THE MACROS =====//
// Creates a unique name
#define UNIQUE_NAME MYCONCAT(GENERATED_NAME_, __COUNTER__)
// Concats x and y
#define MYCONCAT(x,y) _MYCONCAT(x,y)
// DON'T CALL THIS ONE
#define _MYCONCAT(x,y) x##y


//===== THERE GOES THE MACROS =====//

namespace Reflection 
{
	//====== eTYPECATEGORY =======//
	enum class eTYPECATEGORY
	{
		ERROR,			// You're not supposed to get this!
		POD,			// Plain Old Data (e.g int, float, etc)
		CONTAINER,		// Them STL containers
		CLASS			// Both structs and classes are in this category
	};
	//====== eTYPE =======//

	//====== Type =======//

    // Forward declare so that Type can friendclass it
    class TypeManager;

    template <typename T>
    class TypeCreator;

	// Contains the information about a type 
    class Type
    {
    public:

		// Typedef for the tostring function (input is the pointer to the instance, output is the string)
		typedef std::string(*ToStringFn)(void*);

        // Returns the name of this type
        std::string GetTypeName() const { return _typeName; }
        // Returns the size of this type
        size_t GetSize() const { return _size; }
        // Returns the type category of this type
        eTYPECATEGORY GetTypeCat() const { return _typeCat; }
        
        
        // A struct to describe a member and allows for accessing it
        class Member
        {
        public:

            // Getters
            std::string GetName() const { return _name; }
            const Type* GetType() const { return _type; }
            size_t GetOffset() const { return _offset; }
            const Type* GetParent() const { return _parent; }
			void* GetPtr(void* parentInst) const {
				size_t parentInstPtrVal = (size_t)parentInst;
				parentInstPtrVal += _offset;
				return reinterpret_cast<void*>(parentInstPtrVal);
			}
			template <typename ValueType>
			ValueType& Value(void* parentInst) {
				size_t parentInstPtrVal = (size_t)parentInst;
				parentInstPtrVal += _offset;
				ValueType& val = *reinterpret_cast<ValueType*>(parentInstPtrVal);
				return val;
			}

        private:
            // The name of the member
            std::string _name;
            // The type of the member
            const Type* _type;
            // The offset of this member (used to access it)
            size_t _offset;
            // The parent of this member (for backwards access)
            const Type* _parent;

            friend class Type;
        };

        // Return the members of the type
        const std::vector<Member>& GetMembers() const { return _members; }
		// Returns the pointer to the first member with the given name
		const Member* GetMemberByName(std::string name);

		// Call to convert a given type instance to a string
		std::string ToString(void* instPtr) const { return _toStringFn(instPtr); }

    private:

        // CANNOT BE CONSTRUCTED EXCEPT BY TYPEMANAGER
        Type(std::string typeName = "",
                size_t typeSize = 0,
                eTYPECATEGORY typeCat = eTYPECATEGORY::ERROR, 
				ToStringFn toStringFn = nullptr,
                std::vector<Member> members = std::vector<Member>()):
            _typeName(typeName),
            _size(typeSize),
            _typeCat(typeCat),
			_toStringFn(toStringFn),
            _members(members)
            {}

        void _Init(std::string typeName = "",
            size_t typeSize = 0,
            eTYPECATEGORY typeCat = eTYPECATEGORY::ERROR,
			ToStringFn toStringFn = nullptr,
            std::vector<Member> members = std::vector<Member>()) {
            _typeName = typeName;
            _size = typeSize;
            _typeCat = typeCat;
            _members = members;
			_toStringFn = toStringFn;
        }

        void _AddMember(std::string name, const Type* type, size_t offset)
        {
            Member newMember;
            newMember._name = name;
            newMember._offset = offset;
            newMember._type = type;
            newMember._parent = this;
            _members.push_back(newMember);
        }

        // Name of this type
        std::string _typeName;
        // Size of this type
        size_t _size;
        // The type category of this type (e.g POD)
        eTYPECATEGORY _typeCat;
        // Children types
        std::vector<Member> _members;
		// The ToString function
		ToStringFn _toStringFn;
        
        friend class TypeManager;
        template <typename T>
        friend class TypeCreator;
    };

	//====== Type =======//

    //====== TypeCreator =======//
    template <typename T>
    class TypeCreator
    {
    public:
        
        // So that whenever it is created, you WILL init
        TypeCreator(std::string typeName = "",
            size_t typeSize = 0,
            eTYPECATEGORY typeCat = eTYPECATEGORY::ERROR,
            std::vector<Type::Member> members = std::vector<Type::Member>())
        {
            Init(typeName, typeSize, typeCat, members);
        }

        // Sets up the type
        static void Init(std::string typeName = "",
            size_t typeSize = 0,
            eTYPECATEGORY typeCat = eTYPECATEGORY::ERROR,
            std::vector<Type::Member> members = std::vector<Type::Member>())
        {
            _Get()->_Init(typeName, typeSize, typeCat, _ToString, members);
            _RegisterMembers();
            TypeManager::Get().RegisterType(Get());
        }

        // Call this to grab the type instance
        static const Type* Get() {
            return _Get();
        }

    private:
        static Type* _Get() {
            static Type instance;
            return &instance;
        }

        // The user-defined classes will specialize this
        static void _RegisterMembers() {
            // NOTHING HERE
        }

		// The user-defined classes will specialize this if needed
		static std::string _ToString(void* instPtr) {
			return std::to_string(*static_cast<T*>(instPtr));
		}
    };

	// Override for string case
	std::string TypeCreator<std::string>::_ToString(void* instPtr) {
		return *static_cast<std::string*>(instPtr);
	}


    //====== TypeCreator =======//

	//====== TypeManager =======//
	// Grab all your type data from this singleton
	class TypeManager : public Core::IManager<TypeManager>
	{
	public:

		// Singleton gettor
		//static TypeManager& Get();

		// Call to get the type
        // Returns the type from the typename
		const Type* GetType(std::string typeName);

		// DON'T CALL THIS (for adding types into the typeManager)
        static void RegisterType(const Type* typePtr);

		// NO TOUCHIE
		TypeManager(const TypeManager&)		=	delete;
		void operator=(const TypeManager&)	=	delete;
	private:

		// Default constructor
		//TypeManager():_types(){}
        TypeManager(){}

		virtual bool _Init() { return true; }
		virtual void _Update(){}
		virtual void _ShutDown(){}

        // Call this to get the map (the static map is inside)
        static std::map<std::string, const Type*>& _GetTypeMap();

		friend class Core::IManager<TypeManager>;

	};

	//====== TypeManager =======//

    static TypeCreator<float> UNIQUE_NAME = TypeCreator<float>("float", sizeof(float), eTYPECATEGORY::POD);
    static TypeCreator<int> UNIQUE_NAME = TypeCreator<int>("int", sizeof(int), eTYPECATEGORY::POD);
    static TypeCreator<std::string> UNIQUE_NAME = TypeCreator<std::string>("std::string", sizeof(std::string), eTYPECATEGORY::POD);
    
	//====== TypeCreator =======//

	//====== TestCase =========//
	namespace Test {
		// Test struct
		struct TestStruct
		{

			TestStruct(int i, float f, std::string str) : _i(i), _f(f), _str(str), _ci(123) {}

			int _i;
			const int _ci;
			float _f;
			std::string _str;

		};

        

		// Call this to make sure reflection is working
		void RunReflectionTest();
	}

	// Override for TestStruct case
	std::string TypeCreator<Test::TestStruct>::_ToString(void* instPtr) {
		return "TestStruct";
	}

   

	//====== TestCase =========//



}




// Type describes a type
// 1) Name
// 2) Size
// 3) Type category
// 4) Gettors/Settors
// 5) Children types
// TypeCreator contains the unique instance of each type (static instance inside function,t hen you get pointer)
// TypeManager contains all these unique instances of a Type
