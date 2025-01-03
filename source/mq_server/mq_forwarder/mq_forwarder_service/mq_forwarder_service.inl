inline void CMqForwarderService::CMqForwarderServiceHandler::HandleException()
{
    this->GetService().OnServiceNetworkError(this);
}

inline int32_t CMqForwarderService::CMqForwarderServiceHandler::OnGetMessage(char* pBuffer, const SOS::s_size_t nBufferLen, int32_t& nBytes)
{
    return this->GetService().OnHtsMessage(pBuffer, nBufferLen, nBytes, this);
}