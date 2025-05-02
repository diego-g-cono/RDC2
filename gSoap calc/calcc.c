#include "calc.nsmap"
#include "soapH.h"

int main(){
	struct soap *soap = soap_new();
	double sum;

	if(soap_call_ns2__add(soap, NULL, NULL, 1.23, 4.56, &sum) == SOAP_OK)
		printf("Sum = %g\n", sum);
	else
		soap_print_fault(soap, stderr);

	soap_destroy(soap);
	soap_end(soap);
	soap_free(soap);
}
