#include "calc.nsmap"
#include "soapH.h"
#include "soapStub.h"
#include <math.h>

int main(){
	struct soap *soap = soap_new1(SOAP_XML_INDENT);
	if (!soap_valid_socket(soap_bind(soap, NULL, 8080, 5)))
		exit(EXIT_FAILURE);

	while(soap_valid_socket(soap_accept(soap))){
		if (soap_serve(soap) != SOAP_OK)
			break;
		
		soap_destroy(soap);
		soap_end(soap);
	}

	soap_print_fault(soap, stderr);
	soap_free(soap);
}

int ns2__add(struct soap *soap, double a, double b, double *result){
	*result = a + b;
	return SOAP_OK;
}

int ns2__mul(struct soap *soap, double a, double b, double *result){
        *result = a * b;
        return SOAP_OK;
}

int ns2__div(struct soap *soap, double a, double b, double *result){
        *result = a / b;
	if (b == 0.0){
		char *msg = (char *)soap_malloc(soap, 1024);
		snprintf(msg, 1024, "Trying to divide %f by zero", a);
		return soap_sender_fault(soap, msg, NULL);
	}
        return SOAP_OK;
}

int ns2__sub(struct soap *soap, double a, double b, double *result){
        *result = a - b;
        return SOAP_OK;
}

int ns2__pow(struct soap *soap, double a, double b, double *result){
        *result = pow(a, b);
        return SOAP_OK;
}


