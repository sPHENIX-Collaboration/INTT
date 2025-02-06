// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME InttEvent_dict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "InttEvent.h"
#include "ConvertInttData.h"
#include "InttDecode.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static TClass *InttHit_Dictionary();
   static void InttHit_TClassManip(TClass*);
   static void *new_InttHit(void *p = nullptr);
   static void *newArray_InttHit(Long_t size, void *p);
   static void delete_InttHit(void *p);
   static void deleteArray_InttHit(void *p);
   static void destruct_InttHit(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::InttHit*)
   {
      ::InttHit *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::InttHit));
      static ::ROOT::TGenericClassInfo 
         instance("InttHit", "InttEvent.h", 18,
                  typeid(::InttHit), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &InttHit_Dictionary, isa_proxy, 4,
                  sizeof(::InttHit) );
      instance.SetNew(&new_InttHit);
      instance.SetNewArray(&newArray_InttHit);
      instance.SetDelete(&delete_InttHit);
      instance.SetDeleteArray(&deleteArray_InttHit);
      instance.SetDestructor(&destruct_InttHit);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::InttHit*)
   {
      return GenerateInitInstanceLocal((::InttHit*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::InttHit*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *InttHit_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::InttHit*)nullptr)->GetClass();
      InttHit_TClassManip(theClass);
   return theClass;
   }

   static void InttHit_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *InttEvent_Dictionary();
   static void InttEvent_TClassManip(TClass*);
   static void *new_InttEvent(void *p = nullptr);
   static void *newArray_InttEvent(Long_t size, void *p);
   static void delete_InttEvent(void *p);
   static void deleteArray_InttEvent(void *p);
   static void destruct_InttEvent(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::InttEvent*)
   {
      ::InttEvent *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::InttEvent));
      static ::ROOT::TGenericClassInfo 
         instance("InttEvent", "InttEvent.h", 72,
                  typeid(::InttEvent), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &InttEvent_Dictionary, isa_proxy, 4,
                  sizeof(::InttEvent) );
      instance.SetNew(&new_InttEvent);
      instance.SetNewArray(&newArray_InttEvent);
      instance.SetDelete(&delete_InttEvent);
      instance.SetDeleteArray(&deleteArray_InttEvent);
      instance.SetDestructor(&destruct_InttEvent);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::InttEvent*)
   {
      return GenerateInitInstanceLocal((::InttEvent*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::InttEvent*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *InttEvent_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::InttEvent*)nullptr)->GetClass();
      InttEvent_TClassManip(theClass);
   return theClass;
   }

   static void InttEvent_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_InttHit(void *p) {
      return  p ? new(p) ::InttHit : new ::InttHit;
   }
   static void *newArray_InttHit(Long_t nElements, void *p) {
      return p ? new(p) ::InttHit[nElements] : new ::InttHit[nElements];
   }
   // Wrapper around operator delete
   static void delete_InttHit(void *p) {
      delete ((::InttHit*)p);
   }
   static void deleteArray_InttHit(void *p) {
      delete [] ((::InttHit*)p);
   }
   static void destruct_InttHit(void *p) {
      typedef ::InttHit current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::InttHit

namespace ROOT {
   // Wrappers around operator new
   static void *new_InttEvent(void *p) {
      return  p ? new(p) ::InttEvent : new ::InttEvent;
   }
   static void *newArray_InttEvent(Long_t nElements, void *p) {
      return p ? new(p) ::InttEvent[nElements] : new ::InttEvent[nElements];
   }
   // Wrapper around operator delete
   static void delete_InttEvent(void *p) {
      delete ((::InttEvent*)p);
   }
   static void deleteArray_InttEvent(void *p) {
      delete [] ((::InttEvent*)p);
   }
   static void destruct_InttEvent(void *p) {
      typedef ::InttEvent current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::InttEvent

namespace ROOT {
   static TClass *vectorlEInttHitmUgR_Dictionary();
   static void vectorlEInttHitmUgR_TClassManip(TClass*);
   static void *new_vectorlEInttHitmUgR(void *p = nullptr);
   static void *newArray_vectorlEInttHitmUgR(Long_t size, void *p);
   static void delete_vectorlEInttHitmUgR(void *p);
   static void deleteArray_vectorlEInttHitmUgR(void *p);
   static void destruct_vectorlEInttHitmUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<InttHit*>*)
   {
      vector<InttHit*> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<InttHit*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<InttHit*>", -2, "vector", 423,
                  typeid(vector<InttHit*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEInttHitmUgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<InttHit*>) );
      instance.SetNew(&new_vectorlEInttHitmUgR);
      instance.SetNewArray(&newArray_vectorlEInttHitmUgR);
      instance.SetDelete(&delete_vectorlEInttHitmUgR);
      instance.SetDeleteArray(&deleteArray_vectorlEInttHitmUgR);
      instance.SetDestructor(&destruct_vectorlEInttHitmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<InttHit*> >()));

      ::ROOT::AddClassAlternate("vector<InttHit*>","std::vector<InttHit*, std::allocator<InttHit*> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<InttHit*>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEInttHitmUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<InttHit*>*)nullptr)->GetClass();
      vectorlEInttHitmUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEInttHitmUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEInttHitmUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<InttHit*> : new vector<InttHit*>;
   }
   static void *newArray_vectorlEInttHitmUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<InttHit*>[nElements] : new vector<InttHit*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEInttHitmUgR(void *p) {
      delete ((vector<InttHit*>*)p);
   }
   static void deleteArray_vectorlEInttHitmUgR(void *p) {
      delete [] ((vector<InttHit*>*)p);
   }
   static void destruct_vectorlEInttHitmUgR(void *p) {
      typedef vector<InttHit*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<InttHit*>

namespace {
  void TriggerDictionaryInitialization_InttEvent_dict_Impl() {
    static const char* headers[] = {
"InttEvent.h",
"ConvertInttData.h",
"InttDecode.h",
nullptr
    };
    static const char* includePaths[] = {
"/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/release/release_new/new.6/include",
"/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/release/release_new/new.6/include",
"/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/root-6.26.06.p01/include/",
"/direct/sphenix+tg+tg01/commissioning/INTT/repositories/INTT/felix/decoder_hit_base/ver2/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "InttEvent_dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$InttEvent.h")))  InttHit;
class __attribute__((annotate("$clingAutoload$InttEvent.h")))  InttEvent;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "InttEvent_dict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "InttEvent.h"
#include "ConvertInttData.h"
#include "InttDecode.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"Init", payloadCode, "@",
"InttEvent", payloadCode, "@",
"InttHit", payloadCode, "@",
"Run", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("InttEvent_dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_InttEvent_dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_InttEvent_dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_InttEvent_dict() {
  TriggerDictionaryInitialization_InttEvent_dict_Impl();
}
