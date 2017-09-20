#include "Reflection.h"

// For test case
#include <vector>
#include <iostream>

#include <typeinfo>

namespace Reflection
{
    static TypeCreator<Test::TestStruct> UNIQUE_NAME = TypeCreator<Test::TestStruct>("Test::TestStruct", sizeof(Test::TestStruct), eTYPECATEGORY::CLASS);

    void TypeCreator<Test::TestStruct>::_RegisterMembers() {
        _Get()->_AddMember("_i", TypeCreator<decltype(Test::TestStruct::_i)>::Get(),(size_t)(&static_cast<Test::TestStruct*>(nullptr)->_i));
        _Get()->_AddMember("_f", TypeCreator<decltype(Test::TestStruct::_f)>::Get(), (size_t)(&static_cast<Test::TestStruct*>(nullptr)->_f));
        _Get()->_AddMember("_str", TypeCreator<decltype(Test::TestStruct::_str)>::Get(), (size_t)(&static_cast<Test::TestStruct*>(nullptr)->_str));
    }

	//====== Type =======//

	const Type::Member* Type::GetMemberByName(std::string name)
	{
		for (int i = 0; i < _members.size(); ++i)
		{
			if (_members[i]._name == name)
				return &_members[i];
		}
		return nullptr;
	}

	//====== Type =======//

	//====== TypeManager =======//

	// Singleton gettor
	TypeManager& TypeManager::Get()
	{
		static TypeManager instance;
		return instance;
	}

    
    // Call this to get the map (the static map is inside)
    std::map<std::string, const Type*>& TypeManager::_GetTypeMap()
    {
        static std::map<std::string, const Type*> map;
        return map;
    }

    // Returns the type from the typename
    const Type* TypeManager::GetType(std::string typeName)
    {
        std::map<std::string, const Type*>& map = _GetTypeMap();

        if (map.find(typeName) != map.end())
        {
            return map[typeName];
        }
        return nullptr;

    }


    // DON'T CALL THIS (for adding types into the typeManager)
    void TypeManager::RegisterType(const Type* typePtr)
    {
        // just stuff it in
        TypeManager::Get()._GetTypeMap()[typePtr->GetTypeName()] = typePtr;
    }

	//====== TypeManager =======//



	//====== TestCase =========//

	namespace Test
	{
		/*
		class FakeReflVar
		{
		public:

			FakeReflVar(std::string name,
				std::string type,
				std::string val) :
				_name(name), _type(type), _val(val) {}

			std::string GetName() const { return _name; }

			std::string GetType() const { return _type; }

			std::string GetVal() const { return _val; }

			void AddChild(const FakeReflVar& newChild)
			{
				_children.push_back(newChild);
			}

			size_t GetVarCount() const { return _children.size(); }

			FakeReflVar& operator[](unsigned index) { return _children[index]; }
			const FakeReflVar& operator[] (unsigned index) const { return _children[index]; }

		private:
			// Name of the fake variable
			std::string _name;
			// Type of the fake variable
			std::string _type;
			// Value of the fake variable
			std::string _val;

			// Child fake variables
			std::vector<FakeReflVar> _children;

		};

		template <typename T>
		FakeReflVar GetReflVar(const T& val)
		{
			std::cout << "Test's _i is " << *static_cast<const int*>(T::__Get__i(&val)) << std::endl;

			FakeReflVar fakeVar("Fake", "FakeTestStruct", "NO VAL");
			fakeVar.AddChild(FakeReflVar("i", "int", "26"));
			fakeVar.AddChild(FakeReflVar("f", "float", "3.15"));
			fakeVar.AddChild(FakeReflVar("str", "std::string", "ITS A TRAP"));

			return fakeVar;
		}
		*/

		void RunReflectionTest()
		{
			std::cout << "Reflection test begin!" << std::endl;

			/*
			Use case:
			I want to use this for serialization + editor, so that I dont' have to hard code everything.
			E.g

			ReflVal val = theComp->GetReflVar();
			for (int i = 0; i < val.GetVarCount(); ++i)
			{
				std::cout << val[i].GetName() << ", " << val[i].GetType() << std::endl; // Prints out the name of the variable (e.g _speed) and the type (e.g float)
				std::cout << val[i].GetVal() << std::endl; // This should print out the value of the object if possible, or just the above if not.
				CreateEditorWidget(val[i]); // There shouldn't be long and specialized switch cases, it will keep going until the PODs/containers
			}
			SerializeIntoFile(val); // Same as above for CreateEditorWidget, no specialized switch cases, just stuffs it into the json file.
			*/

            float f = 2.345f;
            int i = 234;
            std::string s = "HI";

			TestStruct test(27, 3.14f, "ITS NOT A TRAP");

            // Grab TestStruct's Type and check it
            const Type* testStructType = TypeManager::Get().GetType("Test::TestStruct");
            std::vector<Reflection::Type::Member> members = testStructType->GetMembers();
            std::cout << "TestStruct's type is " << testStructType->GetTypeName() << std::endl;
            std::cout << "Its members are: " << std::endl;
			for (int i = 0; i < members.size(); ++i)
			{
				std::cout << i << ":" << members[i].GetName() << "|" << members[i].GetType()->GetTypeName() << std::endl;
				if (members[i].GetType() == Reflection::TypeCreator<float>::Get())
				{
					float& val = members[i].Value<float>(&test);
					std::cout << "Value is " << val << std::endl;
				}
				else if (members[i].GetType() == Reflection::TypeCreator<int>::Get())
				{
					int& val = members[i].Value<int>(&test);
					std::cout << "Value is " << val << std::endl;
				}
				else if (members[i].GetType() == Reflection::TypeCreator<std::string>::Get())
				{
					std::string& val = members[i].Value<std::string>(&test);
					std::cout << "Value is " << val << std::endl;
				}
			}

			/*
			FakeReflVar val = GetReflVar(test);
			for (unsigned i = 0; i < val.GetVarCount(); ++i)
			{
				std::cout << val[i].GetName() << ", " << val[i].GetType() << std::endl; // Prints out the name of the variable (e.g _speed) and the type (e.g float)
				std::cout << val[i].GetVal() << std::endl; // This should print out the value of the object if possible, or just the above if not.
			}
			*/


			std::cout << "Reflection test end!" << std::endl;
		}
	};

	//====== TestCase =========//

	

}