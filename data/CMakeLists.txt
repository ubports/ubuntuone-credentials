# Qt5 bits

file (GLOB PROVIDER_FILES *.provider)
file (GLOB SERVICE_FILES *.service)
file (GLOB SERVICE_TYPE_FILES *.service-type)

install (FILES ${PROVIDER_FILES} DESTINATION share/accounts/providers/)
install (FILES ${SERVICE_FILES} DESTINATION share/accounts/services/)
install (FILES ${SERVICE_TYPE_FILES} DESTINATION share/accounts/service_types/)
