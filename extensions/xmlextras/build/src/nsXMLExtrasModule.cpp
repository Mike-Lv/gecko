/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: NPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is 
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the NPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the NPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "nsIGenericFactory.h"
#include "nsIServiceManager.h"
#include "nsDOMSerializer.h"
#include "nsXMLHttpRequest.h"
#include "nsDOMParser.h"
#include "nsIAppStartupNotifier.h"
#include "nsICategoryManager.h"
#include "nsIObserver.h"
#include "nsIScriptNameSpaceManager.h"
#include "nsString.h"
#include "nsXPIDLString.h"
#include "nsDOMCID.h"
#include "prprf.h"

#ifdef MOZ_SOAP
#include "nsSOAPHeaderBlock.h"
#include "nsSOAPParameter.h"
#include "nsSOAPCall.h"
#include "nsSOAPResponse.h"
#include "nsSOAPEncoding.h"
#include "nsSOAPFault.h"
#include "nsDefaultSOAPEncoder.h"
#include "nsHTTPSOAPTransport.h"
#include "nsSchemaLoader.h"
#include "nsSchemaPrivate.h"
#include "nsWSDLLoader.h"
#include "nsWSDLPrivate.h"
#endif

#include "nsString.h"
#include "prprf.h"
#include "nsIScriptNameSpaceManager.h"


////////////////////////////////////////////////////////////////////////
// Define the contructor function for the objects
//
// NOTE: This creates an instance of objects by using the default constructor
//
NS_GENERIC_FACTORY_CONSTRUCTOR(nsDOMSerializer)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsXMLHttpRequest)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsDOMParser)
#ifdef MOZ_SOAP
NS_GENERIC_FACTORY_CONSTRUCTOR(nsSOAPCall)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsSOAPResponse)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsSOAPEncoding)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsSOAPHeaderBlock)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsSOAPParameter)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsDefaultSOAPEncoder_1_1)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsDefaultSOAPEncoder_1_2)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsHTTPSOAPTransport)
NS_GENERIC_FACTORY_CONSTRUCTOR(nsHTTPSSOAPTransport)
NS_DECL_CLASSINFO(nsSOAPCall)
NS_DECL_CLASSINFO(nsSOAPResponse)
NS_DECL_CLASSINFO(nsSOAPEncoding)
NS_DECL_CLASSINFO(nsSOAPFault)
NS_DECL_CLASSINFO(nsSOAPHeaderBlock)
NS_DECL_CLASSINFO(nsSOAPParameter)
NS_DECL_CLASSINFO(nsHTTPSOAPTransport)
NS_DECL_CLASSINFO(nsHTTPSOAPTransportCompletion)
NS_DECL_CLASSINFO(nsHTTPSSOAPTransport)

NS_GENERIC_FACTORY_CONSTRUCTOR(nsSchemaLoader)
NS_DECL_CLASSINFO(nsSchemaLoader)
NS_DECL_CLASSINFO(nsSchema)
NS_DECL_CLASSINFO(nsSchemaBuiltinType)
NS_DECL_CLASSINFO(nsSchemaListType)
NS_DECL_CLASSINFO(nsSchemaUnionType)
NS_DECL_CLASSINFO(nsSchemaRestrictionType)
NS_DECL_CLASSINFO(nsSchemaComplexType)
NS_DECL_CLASSINFO(nsSchemaTypePlaceholder)
NS_DECL_CLASSINFO(nsSchemaModelGroup)
NS_DECL_CLASSINFO(nsSchemaModelGroupRef)
NS_DECL_CLASSINFO(nsSchemaAnyParticle)
NS_DECL_CLASSINFO(nsSchemaElement)
NS_DECL_CLASSINFO(nsSchemaElementRef)
NS_DECL_CLASSINFO(nsSchemaAttribute)
NS_DECL_CLASSINFO(nsSchemaAttributeRef)
NS_DECL_CLASSINFO(nsSchemaAttributeGroup)
NS_DECL_CLASSINFO(nsSchemaAttributeGroupRef)
NS_DECL_CLASSINFO(nsSchemaAnyAttribute)
NS_DECL_CLASSINFO(nsSchemaFacet)
NS_DECL_CLASSINFO(nsSOAPArray)
NS_DECL_CLASSINFO(nsSOAPArrayType)

NS_GENERIC_FACTORY_CONSTRUCTOR(nsWSDLLoader)
NS_DECL_CLASSINFO(nsWSDLLoader)
NS_DECL_CLASSINFO(nsWSDLPort)
NS_DECL_CLASSINFO(nsWSDLOperation)
NS_DECL_CLASSINFO(nsWSDLMessage)
NS_DECL_CLASSINFO(nsWSDLPart)
NS_DECL_CLASSINFO(nsSOAPPortBinding)
NS_DECL_CLASSINFO(nsSOAPOperationBinding)
NS_DECL_CLASSINFO(nsSOAPMessageBinding)
NS_DECL_CLASSINFO(nsSOAPPartBinding)
#endif

class nsXMLExtrasNameset : public nsISupports
{
public:
  nsXMLExtrasNameset();
  virtual ~nsXMLExtrasNameset();

  // nsISupports
  NS_DECL_ISUPPORTS
};

nsXMLExtrasNameset::nsXMLExtrasNameset()
{
  NS_INIT_ISUPPORTS();
}

nsXMLExtrasNameset::~nsXMLExtrasNameset()
{
}

NS_INTERFACE_MAP_BEGIN(nsXMLExtrasNameset)
  NS_INTERFACE_MAP_ENTRY(nsISupports)
NS_INTERFACE_MAP_END

NS_IMPL_ADDREF(nsXMLExtrasNameset)
NS_IMPL_RELEASE(nsXMLExtrasNameset)

#define NS_XML_EXTRAS_CID                          \
 { /* 33e569b0-40f8-11d4-9a41-000064657374 */      \
  0x33e569b0, 0x40f8, 0x11d4,                      \
 {0x9a, 0x41, 0x00, 0x00, 0x64, 0x65, 0x73, 0x74} }

#define NS_XML_EXTRAS_CONTRACTID "@mozilla.org/xmlextras;1"

class nsXMLExtras : public nsISupports {
public:
  nsXMLExtras();
  virtual ~nsXMLExtras();

  NS_DEFINE_STATIC_CID_ACCESSOR(NS_XML_EXTRAS_CID);

  // nsISupports
  NS_DECL_ISUPPORTS
};

nsXMLExtras::nsXMLExtras()
{
  NS_INIT_ISUPPORTS();
}

nsXMLExtras::~nsXMLExtras()
{
}

NS_IMPL_ADDREF(nsXMLExtras)
NS_IMPL_RELEASE(nsXMLExtras)

NS_INTERFACE_MAP_BEGIN(nsXMLExtras)
  NS_INTERFACE_MAP_ENTRY(nsISupports)
NS_INTERFACE_MAP_END


NS_GENERIC_FACTORY_CONSTRUCTOR(nsXMLExtras)

static NS_METHOD 
RegisterXMLExtras(nsIComponentManager *aCompMgr,
                  nsIFile *aPath,
                  const char *registryLocation,
                  const char *componentType,
                  const nsModuleComponentInfo *info)
{
  nsresult rv = NS_OK;

  nsCOMPtr<nsICategoryManager> catman =
    do_GetService(NS_CATEGORYMANAGER_CONTRACTID, &rv);

  if (NS_FAILED(rv))
    return rv;

  nsXPIDLCString previous;
  rv = catman->AddCategoryEntry(JAVASCRIPT_GLOBAL_CONSTRUCTOR_CATEGORY,
                                "XMLSerializer",
                                NS_XMLSERIALIZER_CONTRACTID,
                                PR_TRUE, PR_TRUE, getter_Copies(previous));
  NS_ENSURE_SUCCESS(rv, rv);

  rv = catman->AddCategoryEntry(JAVASCRIPT_GLOBAL_CONSTRUCTOR_CATEGORY,
                                "XMLHttpRequest",
                                NS_XMLHTTPREQUEST_CONTRACTID,
                                PR_TRUE, PR_TRUE, getter_Copies(previous));
  NS_ENSURE_SUCCESS(rv, rv);

  rv = catman->AddCategoryEntry(JAVASCRIPT_GLOBAL_CONSTRUCTOR_CATEGORY,
                                "DOMParser",
                                NS_DOMPARSER_CONTRACTID,
                                PR_TRUE, PR_TRUE, getter_Copies(previous));
  NS_ENSURE_SUCCESS(rv, rv);

#ifdef MOZ_SOAP
  rv = catman->AddCategoryEntry(JAVASCRIPT_GLOBAL_CONSTRUCTOR_CATEGORY,
                                "SOAPCall",
                                NS_SOAPCALL_CONTRACTID,
                                PR_TRUE, PR_TRUE, getter_Copies(previous));
  NS_ENSURE_SUCCESS(rv, rv);

  rv = catman->AddCategoryEntry(JAVASCRIPT_GLOBAL_CONSTRUCTOR_CATEGORY,
                                "SOAPResponse",
                                NS_SOAPRESPONSE_CONTRACTID,
                                PR_TRUE, PR_TRUE, getter_Copies(previous));
  NS_ENSURE_SUCCESS(rv, rv);

  rv = catman->AddCategoryEntry(JAVASCRIPT_GLOBAL_CONSTRUCTOR_CATEGORY,
                                "SOAPEncoding",
                                NS_SOAPENCODING_CONTRACTID,
                                PR_TRUE, PR_TRUE, getter_Copies(previous));
  NS_ENSURE_SUCCESS(rv, rv);

  rv = catman->AddCategoryEntry(JAVASCRIPT_GLOBAL_CONSTRUCTOR_CATEGORY,
                                "SOAPHeaderBlock",
                                NS_SOAPHEADERBLOCK_CONTRACTID,
                                PR_TRUE, PR_TRUE, getter_Copies(previous));
  NS_ENSURE_SUCCESS(rv, rv);

  rv = catman->AddCategoryEntry(JAVASCRIPT_GLOBAL_CONSTRUCTOR_CATEGORY,
                                "SOAPParameter",
                                NS_SOAPPARAMETER_CONTRACTID,
                                PR_TRUE, PR_TRUE, getter_Copies(previous));
  NS_ENSURE_SUCCESS(rv, rv);

  rv = catman->AddCategoryEntry(JAVASCRIPT_GLOBAL_CONSTRUCTOR_CATEGORY,
                                "SchemaLoader",
                                NS_SCHEMALOADER_CONTRACTID,
                                PR_TRUE, PR_TRUE, getter_Copies(previous));
  NS_ENSURE_SUCCESS(rv, rv);
  rv = catman->AddCategoryEntry(JAVASCRIPT_GLOBAL_CONSTRUCTOR_CATEGORY,
                                "WSDLLoader",
                                NS_WSDLLOADER_CONTRACTID,
                                PR_TRUE, PR_TRUE, getter_Copies(previous));
  NS_ENSURE_SUCCESS(rv, rv);
#endif

  return rv;
}

////////////////////////////////////////////////////////////////////////
// Define a table of CIDs implemented by this module along with other
// information like the function to create an instance, contractid, and
// class name.
//
static nsModuleComponentInfo components[] = {
  { "XMLExtras component", NS_XML_EXTRAS_CID, NS_XML_EXTRAS_CONTRACTID,
    nsXMLExtrasConstructor, RegisterXMLExtras },
  { "XML Serializer", NS_XMLSERIALIZER_CID, NS_XMLSERIALIZER_CONTRACTID,
    nsDOMSerializerConstructor },
  { "XMLHttpRequest", NS_XMLHTTPREQUEST_CID, NS_XMLHTTPREQUEST_CONTRACTID,
    nsXMLHttpRequestConstructor },
  { "DOM Parser", NS_DOMPARSER_CID, NS_DOMPARSER_CONTRACTID,
    nsDOMParserConstructor },
#ifdef MOZ_SOAP
  { "SOAP Call", NS_SOAPCALL_CID, NS_SOAPCALL_CONTRACTID,
    nsSOAPCallConstructor, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSOAPCall), 
    nsnull, &NS_CLASSINFO_NAME(nsSOAPCall), 
    nsIClassInfo::DOM_OBJECT },
  { "SOAP Response", NS_SOAPRESPONSE_CID, NS_SOAPRESPONSE_CONTRACTID,
    nsSOAPResponseConstructor, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSOAPResponse), 
    nsnull, &NS_CLASSINFO_NAME(nsSOAPResponse), 
    nsIClassInfo::DOM_OBJECT },
  { "SOAP Encoding", NS_SOAPENCODING_CID, NS_SOAPENCODING_CONTRACTID,
    nsSOAPEncodingConstructor, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSOAPEncoding), 
    nsnull, &NS_CLASSINFO_NAME(nsSOAPEncoding), 
    nsIClassInfo::DOM_OBJECT },
  { "SOAP Fault", NS_SOAPFAULT_CID, NS_SOAPFAULT_CONTRACTID,
    nsnull, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSOAPFault), 
    nsnull, &NS_CLASSINFO_NAME(nsSOAPFault), 
    nsIClassInfo::DOM_OBJECT },
  { "SOAP Header Block", NS_SOAPHEADERBLOCK_CID, NS_SOAPHEADERBLOCK_CONTRACTID,
    nsSOAPHeaderBlockConstructor, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSOAPHeaderBlock), 
    nsnull, &NS_CLASSINFO_NAME(nsSOAPHeaderBlock), 
    nsIClassInfo::DOM_OBJECT },
  { "SOAP Parameter", NS_SOAPPARAMETER_CID, NS_SOAPPARAMETER_CONTRACTID,
    nsSOAPParameterConstructor, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSOAPParameter), 
    nsnull, &NS_CLASSINFO_NAME(nsSOAPParameter), 
    nsIClassInfo::DOM_OBJECT },
  { "Default SOAP 1.1 Encoder", NS_DEFAULTSOAPENCODER_1_1_CID,
    NS_DEFAULTSOAPENCODER_1_1_CONTRACTID, 
    nsDefaultSOAPEncoder_1_1Constructor },
  { "Default SOAP 1.2 Encoder", NS_DEFAULTSOAPENCODER_1_2_CID,
    NS_DEFAULTSOAPENCODER_1_2_CONTRACTID, 
    nsDefaultSOAPEncoder_1_2Constructor },
  { "HTTP SOAP Transport", NS_HTTPSOAPTRANSPORT_CID,
    NS_HTTPSOAPTRANSPORT_CONTRACTID, 
    nsHTTPSOAPTransportConstructor, nsnull, nsnull, nsnull,
    NS_CI_INTERFACE_GETTER_NAME(nsHTTPSOAPTransport), 
    nsnull, &NS_CLASSINFO_NAME(nsHTTPSOAPTransport), 
    nsIClassInfo::DOM_OBJECT },
  { "HTTP SOAP Transport Completion", NS_HTTPSOAPTRANSPORTCOMPLETION_CID,
    NS_HTTPSOAPTRANSPORTCOMPLETION_CONTRACTID, 
    nsnull, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsHTTPSOAPTransportCompletion), 
    nsnull, &NS_CLASSINFO_NAME(nsHTTPSOAPTransportCompletion), 
    nsIClassInfo::DOM_OBJECT },
  { "HTTPS SOAP Transport", NS_HTTPSSOAPTRANSPORT_CID,
    NS_HTTPSSOAPTRANSPORT_CONTRACTID, 
    nsHTTPSSOAPTransportConstructor, nsnull, nsnull, nsnull,
    NS_CI_INTERFACE_GETTER_NAME(nsHTTPSSOAPTransport), 
    nsnull, &NS_CLASSINFO_NAME(nsHTTPSSOAPTransport), 
    nsIClassInfo::DOM_OBJECT },
  { "SchemaLoader", NS_SCHEMALOADER_CID, NS_SCHEMALOADER_CONTRACTID,
    nsSchemaLoaderConstructor, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSchemaLoader), nsnull,
    &NS_CLASSINFO_NAME(nsSchemaLoader), nsIClassInfo::DOM_OBJECT },
  { "Schema", NS_SCHEMA_CID, NS_SCHEMA_CONTRACTID, nsnull, nsnull, 
    nsnull, nsnull, NS_CI_INTERFACE_GETTER_NAME(nsSchema), nsnull, 
    &NS_CLASSINFO_NAME(nsSchema), nsIClassInfo::DOM_OBJECT },
  { "SchemaBuiltinType", NS_SCHEMABUILTINTYPE_CID, 
    NS_SCHEMABUILTINTYPE_CONTRACTID, nsnull, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSchemaBuiltinType), nsnull, 
    &NS_CLASSINFO_NAME(nsSchemaBuiltinType), nsIClassInfo::DOM_OBJECT },
  { "SchemaListType", NS_SCHEMALISTTYPE_CID, NS_SCHEMALISTTYPE_CONTRACTID, 
    nsnull, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSchemaListType), nsnull, 
    &NS_CLASSINFO_NAME(nsSchemaListType), nsIClassInfo::DOM_OBJECT },
  { "SchemaUnionType", NS_SCHEMAUNIONTYPE_CID, NS_SCHEMAUNIONTYPE_CONTRACTID, 
    nsnull, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSchemaUnionType), nsnull, 
    &NS_CLASSINFO_NAME(nsSchemaUnionType), nsIClassInfo::DOM_OBJECT },
  { "SchemaRestrictionType", NS_SCHEMARESTRICTIONTYPE_CID, 
    NS_SCHEMARESTRICTIONTYPE_CONTRACTID, nsnull, nsnull, 
    nsnull, nsnull, NS_CI_INTERFACE_GETTER_NAME(nsSchemaRestrictionType), 
    nsnull, &NS_CLASSINFO_NAME(nsSchemaRestrictionType), 
    nsIClassInfo::DOM_OBJECT },
  { "SchemaComplexType", NS_SCHEMACOMPLEXTYPE_CID, 
    NS_SCHEMACOMPLEXTYPE_CONTRACTID, nsnull, nsnull, 
    nsnull, nsnull, NS_CI_INTERFACE_GETTER_NAME(nsSchemaComplexType), 
    nsnull, &NS_CLASSINFO_NAME(nsSchemaComplexType), 
    nsIClassInfo::DOM_OBJECT },
  { "SchemaTypePlaceholder", NS_SCHEMATYPEPLACEHOLDER_CID, 
    NS_SCHEMATYPEPLACEHOLDER_CONTRACTID, nsnull, nsnull, 
    nsnull, nsnull, NS_CI_INTERFACE_GETTER_NAME(nsSchemaTypePlaceholder), 
    nsnull, &NS_CLASSINFO_NAME(nsSchemaTypePlaceholder), 
    nsIClassInfo::DOM_OBJECT },
  { "SchemaModelGroup", NS_SCHEMAMODELGROUP_CID, 
    NS_SCHEMAMODELGROUP_CONTRACTID, nsnull, nsnull, 
    nsnull, nsnull, NS_CI_INTERFACE_GETTER_NAME(nsSchemaModelGroup), nsnull, 
    &NS_CLASSINFO_NAME(nsSchemaModelGroup), nsIClassInfo::DOM_OBJECT },
  { "SchemaModelGroupRef", NS_SCHEMAMODELGROUPREF_CID, 
    NS_SCHEMAMODELGROUPREF_CONTRACTID, nsnull, nsnull, 
    nsnull, nsnull, NS_CI_INTERFACE_GETTER_NAME(nsSchemaModelGroupRef), 
    nsnull, &NS_CLASSINFO_NAME(nsSchemaModelGroupRef), 
    nsIClassInfo::DOM_OBJECT },
  { "SchemaAnyParticle", NS_SCHEMAANYPARTICLE_CID, 
    NS_SCHEMAANYPARTICLE_CONTRACTID, nsnull, nsnull, 
    nsnull, nsnull, NS_CI_INTERFACE_GETTER_NAME(nsSchemaAnyParticle), 
    nsnull, &NS_CLASSINFO_NAME(nsSchemaAnyParticle), 
    nsIClassInfo::DOM_OBJECT },
  { "SchemaElement", NS_SCHEMAELEMENT_CID, NS_SCHEMAELEMENT_CONTRACTID, 
    nsnull, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSchemaElement), nsnull, 
    &NS_CLASSINFO_NAME(nsSchemaElement), 
    nsIClassInfo::DOM_OBJECT },
  { "SchemaElementRef", NS_SCHEMAELEMENTREF_CID, 
    NS_SCHEMAELEMENTREF_CONTRACTID, nsnull, nsnull, 
    nsnull, nsnull, NS_CI_INTERFACE_GETTER_NAME(nsSchemaElementRef), nsnull, 
    &NS_CLASSINFO_NAME(nsSchemaElementRef), nsIClassInfo::DOM_OBJECT },
  { "SchemaAttribute", NS_SCHEMAATTRIBUTE_CID, NS_SCHEMAATTRIBUTE_CONTRACTID, 
    nsnull, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSchemaAttribute), nsnull, 
    &NS_CLASSINFO_NAME(nsSchemaAttribute), nsIClassInfo::DOM_OBJECT },
  { "SchemaAttributeRef", NS_SCHEMAATTRIBUTEREF_CID, 
    NS_SCHEMAATTRIBUTEREF_CONTRACTID, nsnull, nsnull, 
    nsnull, nsnull, NS_CI_INTERFACE_GETTER_NAME(nsSchemaAttributeRef), 
    nsnull, &NS_CLASSINFO_NAME(nsSchemaAttributeRef), 
    nsIClassInfo::DOM_OBJECT },
  { "SchemaAttributeGroup", NS_SCHEMAATTRIBUTEGROUP_CID, 
    NS_SCHEMAATTRIBUTEGROUP_CONTRACTID, nsnull, nsnull, 
    nsnull, nsnull, NS_CI_INTERFACE_GETTER_NAME(nsSchemaAttributeGroup), 
    nsnull, &NS_CLASSINFO_NAME(nsSchemaAttributeGroup), 
    nsIClassInfo::DOM_OBJECT },
  { "SchemaAttributeGroupRef", NS_SCHEMAATTRIBUTEGROUPREF_CID, 
    NS_SCHEMAATTRIBUTEGROUPREF_CONTRACTID, nsnull, nsnull, 
    nsnull, nsnull, NS_CI_INTERFACE_GETTER_NAME(nsSchemaAttributeGroupRef), 
    nsnull, &NS_CLASSINFO_NAME(nsSchemaAttributeGroupRef), 
    nsIClassInfo::DOM_OBJECT },
  { "SchemaAnyAttribute", NS_SCHEMAANYATTRIBUTE_CID, 
    NS_SCHEMAANYATTRIBUTE_CONTRACTID, nsnull, nsnull, 
    nsnull, nsnull, NS_CI_INTERFACE_GETTER_NAME(nsSchemaAnyAttribute), 
    nsnull, &NS_CLASSINFO_NAME(nsSchemaAnyAttribute), 
    nsIClassInfo::DOM_OBJECT },
  { "SchemaFacet", NS_SCHEMAFACET_CID, NS_SCHEMAFACET_CONTRACTID, 
    nsnull, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSchemaFacet), nsnull, 
    &NS_CLASSINFO_NAME(nsSchemaFacet), nsIClassInfo::DOM_OBJECT },
  { "SOAPArray", NS_SOAPARRAY_CID, NS_SOAPARRAY_CONTRACTID, 
    nsnull, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSOAPArray), nsnull, 
    &NS_CLASSINFO_NAME(nsSOAPArray), nsIClassInfo::DOM_OBJECT },
  { "SOAPArrayType", NS_SOAPARRAYTYPE_CID, NS_SOAPARRAYTYPE_CONTRACTID, 
    nsnull, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSOAPArrayType), nsnull, 
    &NS_CLASSINFO_NAME(nsSOAPArrayType), nsIClassInfo::DOM_OBJECT },
  { "WSDLLoader", NS_WSDLLOADER_CID, NS_WSDLLOADER_CONTRACTID,
    nsWSDLLoaderConstructor, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsWSDLLoader), nsnull,
    &NS_CLASSINFO_NAME(nsWSDLLoader), nsIClassInfo::DOM_OBJECT },
  { "WSDLPort", NS_WSDLPORT_CID, NS_WSDLPORT_CONTRACTID, 
    nsnull, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsWSDLPort), nsnull, 
    &NS_CLASSINFO_NAME(nsWSDLPort), nsIClassInfo::DOM_OBJECT },
  { "WSDLOperation", NS_WSDLOPERATION_CID, NS_WSDLOPERATION_CONTRACTID, 
    nsnull, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsWSDLOperation), nsnull, 
    &NS_CLASSINFO_NAME(nsWSDLOperation), nsIClassInfo::DOM_OBJECT },
  { "WSDLMessage", NS_WSDLMESSAGE_CID, NS_WSDLMESSAGE_CONTRACTID, 
    nsnull, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsWSDLMessage), nsnull, 
    &NS_CLASSINFO_NAME(nsWSDLMessage), nsIClassInfo::DOM_OBJECT },
  { "WSDLPart", NS_WSDLPART_CID, NS_WSDLPART_CONTRACTID, 
    nsnull, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsWSDLPart), nsnull, 
    &NS_CLASSINFO_NAME(nsWSDLPart), nsIClassInfo::DOM_OBJECT },
  { "SOAPPortBinding", NS_SOAPPORTBINDING_CID, NS_SOAPPORTBINDING_CONTRACTID, 
    nsnull, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSOAPPortBinding), nsnull, 
    &NS_CLASSINFO_NAME(nsSOAPPortBinding), nsIClassInfo::DOM_OBJECT },
  { "SOAPOperationBinding", NS_SOAPOPERATIONBINDING_CID, 
    NS_SOAPOPERATIONBINDING_CONTRACTID, nsnull, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSOAPOperationBinding), nsnull, 
    &NS_CLASSINFO_NAME(nsSOAPOperationBinding), nsIClassInfo::DOM_OBJECT },
  { "SOAPMessageBinding", NS_SOAPMESSAGEBINDING_CID, 
    NS_SOAPMESSAGEBINDING_CONTRACTID, nsnull, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSOAPMessageBinding), nsnull, 
    &NS_CLASSINFO_NAME(nsSOAPMessageBinding), nsIClassInfo::DOM_OBJECT },
  { "SOAPPartBinding", NS_SOAPPARTBINDING_CID, NS_SOAPPARTBINDING_CONTRACTID, 
    nsnull, nsnull, nsnull, nsnull, 
    NS_CI_INTERFACE_GETTER_NAME(nsSOAPPartBinding), nsnull, 
    &NS_CLASSINFO_NAME(nsSOAPPartBinding), nsIClassInfo::DOM_OBJECT },
#endif
};

void PR_CALLBACK
XMLExtrasModuleDestructor(nsIModule* self)
{
#ifdef MOZ_SOAP
  nsSchemaAtoms::DestroySchemaAtoms();
  nsWSDLAtoms::DestroyWSDLAtoms();
#endif    
}

NS_IMPL_NSGETMODULE_WITH_DTOR(nsXMLExtrasModule, components, 
                              XMLExtrasModuleDestructor)
