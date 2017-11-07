#pragma once
/**********************************************************************
    class: Introspection (introspection.h)

    author: S. Hau
    date: November 7, 2017

    NOT IMPLEMENTED YET. ONLY ROUGH TESTS WERE MADE.
**********************************************************************/

//struct TypeInfo
//{
//    Size            size;
//    const char*     name;
//    // TypeInfo*       inheritsFrom;
//    // Members      members;
//    // Operators    operations;
//};
//
//
//class IntrospectionManager
//{
//public:
//    static IntrospectionManager* Get()
//    {
//        static IntrospectionManager s_manager;
//        return &s_manager;
//    }
//
//    template <class T>
//    void registerType(Size size, const char* name)
//    {
//        TypeInfo* typeInfo = const_cast<TypeInfo*>(getType<T>());
//        typeInfo->size = size;
//        typeInfo->name = _strdup(name);
//        m_typeInfos[name] = getType<T>();
//    }
//
//    template <class T>
//    const TypeInfo* getType() const
//    {
//        static TypeInfo typeInfo;
//        return &typeInfo;
//    }
//
//    // Serialization
//
//private:
//    std::map<const char*, const TypeInfo*> m_typeInfos;
//    IntrospectionManager() {}
//    // TODO: Move etc.
//};
//
//#define REGISTER_TYPE(T) IntrospectionManager::Get()->registerType<T>( sizeof(T), #T )
//#define REGISTER_MEMBER(T, M) 
//#define REGISTER_BASE(T, B)
//
//#define GET_TYPE(T) IntrospectionManager::Get()->getType<T>()
//
//class Variable
//{
//    const TypeInfo* m_typeInfo = nullptr;
//    void*           m_data = nullptr;
//
//public:
//    template <class T>
//    Variable(T val)
//    {
//        m_typeInfo = GET_TYPE(T);
//        T* data = (T*)malloc(sizeof T);
//        *data = val;
//        m_data = data;
//    }
//
//    ~Variable() { delete m_data; }
//
//    template <class T>
//    T& getValue()
//    {
//        return *(static_cast<T*>(m_data));
//    }
//};
//
////template <class FunctionType, FunctionType FunctionPtr, class Arg0>
////void Call( Variable* ret, Variable *args, U32 argCount )
////{
////    ASSERT( argCount == 0 );
////    (*FunctionPtr)( args[0].getValue<Arg0>() );
////};
//
//template <class FunctionType, FunctionType FunctionPtr>
//void Call(Variable* ret, Variable *args, U32 argCount)
//{
//    ASSERT(argCount == 0);
//    (*FunctionPtr)();
//};
//
//class Function
//{
//public:
//    Function(void(*call)(Variable*, Variable*, U32)) : m_call(call) {}
//    void operator()(Variable* ret = nullptr, Variable *args = nullptr, U32 argCount = 0) { m_call(ret, args, argCount); }
//
//private:
//    void(*m_call) (Variable*, Variable*, U32);
//};
//
////template <class FunctionType, FunctionType FunctionPtr, class Arg0>
////Function MakeFunction( void(*fn)( Arg0 ) )
////{
////    return Function( &Call<FunctionType, FunctionPtr, Arg0> );
////}
//
//template <class FunctionType, FunctionType FunctionPtr>
//Function MakeFunction(void(*fn)(void))
//{
//    return Function(&Call<FunctionType, FunctionPtr>);
//}
//
//#define MAKE_FUNCTION(F) MakeFunction<decltype( &F ), &F>( &F )
//
//void test()
//{
//    printf("HELLO I AM IO\n");
//}
//
//void registerTypes()
//{
//    REGISTER_TYPE(I8);
//    REGISTER_TYPE(U8);
//    REGISTER_TYPE(I16);
//    REGISTER_TYPE(U16);
//    REGISTER_TYPE(I32);
//    REGISTER_TYPE(U32);
//    REGISTER_TYPE(I64);
//    REGISTER_TYPE(U64);
//    REGISTER_TYPE(F32);
//    REGISTER_TYPE(F64);
//    REGISTER_TYPE(StringID);
//    REGISTER_TYPE(const char*);
//    REGISTER_TYPE(A);
//
//    //Function f = MAKE_FUNCTION(test);
//    //f();
//
//
//    const TypeInfo* sinfo = GET_TYPE(const char*);
//    const TypeInfo* sinfo2 = GET_TYPE(F64);
//    const TypeInfo* sinfo3 = GET_TYPE(I32);
//    const TypeInfo* sinfo4 = GET_TYPE(I8);
//    const TypeInfo* sinfo5 = GET_TYPE(A);
//
//    /*  Variable v = 5;
//    int val = v.getValue<int>();*/
//
//    A a(634);
//    Variable v2 = a;
//    int a2 = v2.getValue<int>();
//
//    system("pause");
//}