#include "HttpKeyValue.h"
#include "HttpProtocol.h"

NAMESPACE_ZL_START

HttpKeyValue::HttpKeyValue()
{
	Initialise();
}

std::string HttpKeyValue::GetReasonDesc(int code)
{
	auto iter = code_desc_.find(code);
	return iter != code_desc_.end() ? iter->second : "";
}

std::string HttpKeyValue::GetContentType(const std::string& file_type)
{
	auto iter = content_type_.find(file_type);
	return iter != content_type_.end() ? iter->second : "";
}

void HttpKeyValue::Initialise()
{ 
	content_type_["htm"]  = "text/html";
	content_type_["html"] = "text/html";
	content_type_["css"]  = "text/css";
	content_type_["txt"]  = "text/plain";
	content_type_["c"]    = "text/plain";
	content_type_["cpp"]  = "text/plain";
	content_type_["cxx"]  = "text/plain";
	content_type_["h"]    = "text/plain";
	content_type_["jpeg"] = "image/jpeg";
	content_type_["jpg"]  = "image/jpeg";
	content_type_["png"]  = "image/png";
	content_type_["bmp"]  = "image/bmp";
	content_type_["gif"]  = "image/gif";
	content_type_["ico"]  = "image/x-icon";
	content_type_["mpg"]  = "video/mpeg";
	content_type_["asf"]  = "video/x-ms-asf";
	content_type_["avi"]  = "video/x-msvideo";
	content_type_["doc"]  = "application/msword";
	content_type_["exe"]  = "application/octet-stream";
	content_type_["rar"]  = "application/octet-stream";
	content_type_["zip"]  = "application/zip";
	content_type_["*"]    = "application/octet-stream";


	/* 1xx Infomational */
	code_desc_[HttpResponseCode::HttpStatusContinue]                     = ZL_HTTP_STATUS_CONTINUE_S;
	code_desc_[HttpResponseCode::HttpStatusSwichingProtocols]            = ZL_HTTP_STATUS_SWITCHING_PROTOCOLS_S;

	/* 2xx Succesful */
	code_desc_[HttpResponseCode::HttpStatusOk]                           = ZL_HTTP_STATUS_OK_S;
	code_desc_[HttpResponseCode::HttpStatsuCreated]                      = ZL_HTTP_STATUS_CREATED_S;
	code_desc_[HttpResponseCode::HttpStatusAccepted]                     = ZL_HTTP_STATUS_ACCEPTED_S;
	code_desc_[HttpResponseCode::HttpStatusNonAuthorizedInformation]     = ZL_HTTP_STATUS_NON_AUTHORITATIVE_INFORMATION_S;
	code_desc_[HttpResponseCode::HttpStatusNoContent]                    = ZL_HTTP_STATUS_NO_CONTENT_S;
	code_desc_[HttpResponseCode::HttpStatusResetContent]                 = ZL_HTTP_STATUS_RESET_CONTENT_S;
	code_desc_[HttpResponseCode::HttpStatusPartialContent]               = ZL_HTTP_STATUS_PARTIAL_CONTENT_S;

	/* 3xx Redirection */
	code_desc_[HttpResponseCode::HttpStatusMultipleChoices]              = ZL_HTTP_STATUS_MULTIPLE_CHOICES_S;
	code_desc_[HttpResponseCode::HttpStatusMovedPermanetly]              = ZL_HTTP_STATUS_MOVED_PERMANENTLY_S;
	code_desc_[HttpResponseCode::HttpStatusFound]                        = ZL_HTTP_STATUS_FOUND_S;
	code_desc_[HttpResponseCode::HttpStatusSeeOther]                     = ZL_HTTP_STATUS_SEE_OTHER_S;
	code_desc_[HttpResponseCode::HttpStatusNotModified]                  = ZL_HTTP_STATUS_NOT_MODIFIED_S;
	code_desc_[HttpResponseCode::HttpStatusUseProxy]                     = ZL_HTTP_STATUS_USE_PROXY_S;
	code_desc_[HttpResponseCode::HttpStatusTemporaryRedirection]         = ZL_HTTP_STATUS_TEMPORARY_REDIRECT_S;

	/* 4xx Client Error */
	code_desc_[HttpResponseCode::HttpStatusBadRequest]                   = ZL_HTTP_STATUS_BAD_REQUEST_S;
	code_desc_[HttpResponseCode::HttpStatusUnauthorized]                 = ZL_HTTP_STATUS_UNAUTHORIZED_S;
	code_desc_[HttpResponseCode::HttpStatusPaymentRequired]              = ZL_HTTP_STATUS_PAYMENT_REQUIRED_S;
	code_desc_[HttpResponseCode::HttpStatusForbidden]                    = ZL_HTTP_STATUS_FORBIDDEN_S;
	code_desc_[HttpResponseCode::HttpStatusNotFound]                     = ZL_HTTP_STATUS_NOT_FOUND_S;
	code_desc_[HttpResponseCode::HttpStatusMethodNotAllowed]             = ZL_HTTP_STATUS_METHOD_NOT_ALLOWED_S;
	code_desc_[HttpResponseCode::HttpStatusNotAcceptable]                = ZL_HTTP_STATUS_NOT_ACCEPTABLE_S;
	code_desc_[HttpResponseCode::HttpStatusProxyAuthenticationRequired]  = ZL_HTTP_STATUS_PROXY_AUTHENICATION_REQUIRED_S;
	code_desc_[HttpResponseCode::HttpStatusRequestTimeOut]               = ZL_HTTP_STATUS_REQUEST_TIME_OUT_S;
	code_desc_[HttpResponseCode::HttpStatusConflict]                     = ZL_HTTP_STATUS_CONFLICT_S;
	code_desc_[HttpResponseCode::HttpStatusGone]                         = ZL_HTTP_STATUS_GONE_S;
	code_desc_[HttpResponseCode::HttpStatusLengthRequired]               = ZL_HTTP_STATUS_LENGTH_REQUIRED_S;
	code_desc_[HttpResponseCode::HttpStatusProconditionFailed]           = ZL_HTTP_STATUS_PRECONDITION_FAILED_S;
	code_desc_[HttpResponseCode::HttpStatusRequestEntityTooLarge]        = ZL_HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE_S;
	code_desc_[HttpResponseCode::HttpStatusRequestURITooLarge]           = ZL_HTTP_STATUS_REQUEST_URI_TOO_LARGE_S;
	code_desc_[HttpResponseCode::HttpStatusUnsupportedMediaType]         = ZL_HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE_S;
	code_desc_[HttpResponseCode::HttpStatusRequestedRangeNotSatisfiable] = ZL_HTTP_STATUS_REQUEST_RANGE_NOT_SATISFIABLE_S;
	code_desc_[HttpResponseCode::HttpStatusExpectationFailed]            = ZL_HTTP_STATUS_EXPECTATION_FAILED_S;

	/* 5xx Server Error */
	code_desc_[HttpResponseCode::HttpStatusInternalServerError]          = ZL_HTTP_STATUS_INTERNAL_SERVER_ERROR_S;
	code_desc_[HttpResponseCode::HttpStatusNotImplemented]               = ZL_HTTP_STATUS_NOT_IMPLEMENTED_S;
	code_desc_[HttpResponseCode::HttpStatusBadGateway]                   = ZL_HTTP_STATUS_BAD_GATEWAY_S;
	code_desc_[HttpResponseCode::HttpStatusServiceUnavaliable]           = ZL_HTTP_STATUS_SERVICE_UNAVAILABLE_S;
	code_desc_[HttpResponseCode::HttpStatusGatewayTimeOut]               = ZL_HTTP_STATUS_GATEWAY_TIME_OUT_S;
	code_desc_[HttpResponseCode::HttpStatusHttpVersionNotSupported]      = ZL_HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED_S;
}


NAMESPACE_ZL_END