#include "calc.nsmap"
#include "soapH.h"

int main(){
	struct soap *soap = soap_new();
	double sum;
	double rest;
	double mult;
	double divi;
	double pot;

	if(soap_call_ns2__add(soap, NULL, NULL, 1.23, 4.56, &sum) == SOAP_OK)
		printf("Sum = %g\n", sum);
	else
		soap_print_fault(soap, stderr);
	
	if(soap_call_ns2__sub(soap, NULL, NULL, 5.00, 2.00, &rest) == SOAP_OK)
		printf("Rest = %g\n", rest);
	else
		soap_print_fault(soap, stderr);
	
	if(soap_call_ns2__mul(soap, NULL, NULL, 5.00, 2.00, &mult) == SOAP_OK)
                printf("Mult = %g\n", mult);
        else
                soap_print_fault(soap, stderr);

	if(soap_call_ns2__div(soap, NULL, NULL, 6.00, 2.00, &divi) == SOAP_OK)
                printf("Divi = %g\n", divi);
        else
                soap_print_fault(soap, stderr);
	
	if(soap_call_ns2__pow(soap, NULL, NULL, 6.00, 2.00, &pot) == SOAP_OK)
                printf("Pot = %g\n", pot);
        else
                soap_print_fault(soap, stderr);


	soap_destroy(soap);
	soap_end(soap);
	soap_free(soap);
}
