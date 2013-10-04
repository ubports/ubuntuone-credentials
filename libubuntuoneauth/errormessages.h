#ifndef ERROR_MESSAGES_H
#define ERROR_MESSAGES_H


#define NO_HTTP_REASON "No HTTP error reason"

#define LOGIN_FAILED "LOGIN_FAILED"
#define CAPTCHA_REQUIRED "CAPTCHA_REQUIRED"
#define INVALID_CREDENTIALS "INVALID_CREDENTIALS"
#define TWOFACTOR_REQUIRED "TWOFACTOR_REQUIRED"
#define ACCOUNT_SUSPENDED "ACCOUNT_SUSPENDED"
#define ACCOUNT_DEACTIVATED "ACCOUNT_DEACTIVATED"
#define EMAIL_INVALIDATED "EMAIL_INVALIDATED"
#define CAN_NOT_RESET_PASSWORD "CAN_NOT_RESET_PASSWORD"
#define ALREADY_REGISTERED "ALREADY_REGISTERED"

enum ErrorCodes
{
  CODE_LOGIN_FAILED = 0,
  CODE_CAPTCHA_REQUIRED,
  CODE_INVALID_CREDENTIALS,
  CODE_TWOFACTOR_REQUIRED,
  CODE_ACCOUNT_SUSPENDED,
  CODE_ACCOUNT_DEACTIVATED,
  CODE_EMAIL_INVALIDATED,
  CODE_CAN_NOT_RESET_PASSWORD,
  CODE_ALREADY_REGISTERED
};

#endif // ERROR_MESSAGES_H
