#include "message_preferences.h"
#include "ui_message_preferences.h"
#include "message.h"

using namespace commonLib;

//-----------------------------------------------------------------------------
MessagePreferences::MessagePreferences (QWidget* parent) : QWidget(parent), m_ui(new Ui::MessagePreferences) {m_ui->setupUi(this);}
MessagePreferences::~MessagePreferences () {delete m_ui;}
//-----------------------------------------------------------------------------
void MessagePreferences::load (QSettings& settings)
{
   // m_ui->userDebugColor->setCurrentColor(color(settings,UserDebug));
   // m_ui->userStatusColor->setCurrentColor(color(settings,UserStatus));
   // m_ui->userInfoColor->setCurrentColor(color(settings,UserInfo));
   // m_ui->userWarningColor->setCurrentColor(color(settings,UserWarning));
   // m_ui->userErrorColor->setCurrentColor(color(settings,UserError));
   // m_ui->userFatalColor->setCurrentColor(color(settings,UserFatal));

   // m_ui->userDebugPrefix->setText(rawPrefix(settings,UserDebug));
   // m_ui->userStatusPrefix->setText(rawPrefix(settings,UserStatus));
   // m_ui->userInfoPrefix->setText(rawPrefix(settings,UserInfo));
   // m_ui->userWarningPrefix->setText(rawPrefix(settings,UserWarning));
   // m_ui->userErrorPrefix->setText(rawPrefix(settings,UserError));
   // m_ui->userFatalPrefix->setText(rawPrefix(settings,UserFatal));

   // m_ui->userDebugSuffix->setText(rawSuffix(settings,UserDebug));
   // m_ui->userStatusSuffix->setText(rawSuffix(settings,UserStatus));
   // m_ui->userInfoSuffix->setText(rawSuffix(settings,UserInfo));
   // m_ui->userWarningSuffix->setText(rawSuffix(settings,UserWarning));
   // m_ui->userErrorSuffix->setText(rawSuffix(settings,UserError));
   // m_ui->userFatalSuffix->setText(rawSuffix(settings,UserFatal));

   // m_ui->sysStatusColor->setCurrentColor(color(settings,SysStatus));
   // m_ui->sysInfoColor->setCurrentColor(color(settings,SysInfo));
   // m_ui->sysWarningColor->setCurrentColor(color(settings,SysWarning));
   // m_ui->sysErrorColor->setCurrentColor(color(settings,SysError));
   // m_ui->sysFatalColor->setCurrentColor(color(settings,SysFatal));

   // m_ui->sysStatusPrefix->setText(rawPrefix(settings,SysStatus));
   // m_ui->sysInfoPrefix->setText(rawPrefix(settings,SysInfo));
   // m_ui->sysWarningPrefix->setText(rawPrefix(settings,SysWarning));
   // m_ui->sysErrorPrefix->setText(rawPrefix(settings,SysError));
   // m_ui->sysFatalPrefix->setText(rawPrefix(settings,SysFatal));

   // m_ui->sysStatusSuffix->setText(rawSuffix(settings,SysStatus));
   // m_ui->sysInfoSuffix->setText(rawSuffix(settings,SysInfo));
   // m_ui->sysWarningSuffix->setText(rawSuffix(settings,SysWarning));
   // m_ui->sysErrorSuffix->setText(rawSuffix(settings,SysError));
   // m_ui->sysFatalSuffix->setText(rawSuffix(settings,SysFatal));
}
void MessagePreferences::save (QSettings& settings)
{
   // setColor(settings, UserDebug,   m_ui->userDebugColor->currentColor());
   // setColor(settings, UserStatus,  m_ui->userStatusColor->currentColor());
   // setColor(settings, UserInfo,    m_ui->userInfoColor->currentColor());
   // setColor(settings, UserWarning, m_ui->userWarningColor->currentColor());
   // setColor(settings, UserError,   m_ui->userErrorColor->currentColor());
   // setColor(settings, UserFatal,   m_ui->userFatalColor->currentColor());

   // setPrefix(settings, UserDebug,   m_ui->userDebugPrefix->text());
   // setPrefix(settings, UserStatus,  m_ui->userStatusPrefix->text());
   // setPrefix(settings, UserInfo,    m_ui->userInfoPrefix->text());
   // setPrefix(settings, UserWarning, m_ui->userWarningPrefix->text());
   // setPrefix(settings, UserError,   m_ui->userErrorPrefix->text());
   // setPrefix(settings, UserFatal,   m_ui->userFatalPrefix->text());

   // setSuffix(settings, UserDebug,   m_ui->userDebugSuffix->text());
   // setSuffix(settings, UserStatus,  m_ui->userStatusSuffix->text());
   // setSuffix(settings, UserInfo,    m_ui->userInfoSuffix->text());
   // setSuffix(settings, UserWarning, m_ui->userWarningSuffix->text());
   // setSuffix(settings, UserError,   m_ui->userErrorSuffix->text());
   // setSuffix(settings, UserFatal,   m_ui->userFatalSuffix->text());

   // setColor(settings, SysStatus,  m_ui->sysStatusColor->currentColor());
   // setColor(settings, SysInfo,    m_ui->sysInfoColor->currentColor());
   // setColor(settings, SysWarning, m_ui->sysWarningColor->currentColor());
   // setColor(settings, SysError,   m_ui->sysErrorColor->currentColor());
   // setColor(settings, SysFatal,   m_ui->sysFatalColor->currentColor());

   // setPrefix(settings, SysStatus,  m_ui->sysStatusPrefix->text());
   // setPrefix(settings, SysInfo,    m_ui->sysInfoPrefix->text());
   // setPrefix(settings, SysWarning, m_ui->sysWarningPrefix->text());
   // setPrefix(settings, SysError,   m_ui->sysErrorPrefix->text());
   // setPrefix(settings, SysFatal,   m_ui->sysFatalPrefix->text());

   // setSuffix(settings, SysStatus,  m_ui->sysStatusSuffix->text());
   // setSuffix(settings, SysInfo,    m_ui->sysInfoSuffix->text());
   // setSuffix(settings, SysWarning, m_ui->sysWarningSuffix->text());
   // setSuffix(settings, SysError,   m_ui->sysErrorSuffix->text());
   // setSuffix(settings, SysFatal,   m_ui->sysFatalSuffix->text());

}
//-----------------------------------------------------------------------------
void MessagePreferences::setDefaults ()
{
   // m_ui->userDebugColor->setCurrentColor(Color.value(UserDebug));
   // m_ui->userStatusColor->setCurrentColor(Color.value(UserStatus));
   // m_ui->userInfoColor->setCurrentColor(Color.value(UserInfo));
   // m_ui->userWarningColor->setCurrentColor(Color.value(UserWarning));
   // m_ui->userErrorColor->setCurrentColor(Color.value(UserError));
   // m_ui->userFatalColor->setCurrentColor(Color.value(UserFatal));

   // m_ui->userDebugPrefix->setText(Prefix.value(UserDebug));
   // m_ui->userStatusPrefix->setText(Prefix.value(UserStatus));
   // m_ui->userInfoPrefix->setText(Prefix.value(UserInfo));
   // m_ui->userWarningPrefix->setText(Prefix.value(UserWarning));
   // m_ui->userErrorPrefix->setText(Prefix.value(UserError));
   // m_ui->userFatalPrefix->setText(Prefix.value(UserFatal));

   // m_ui->userDebugSuffix->setText(Suffix.value(UserDebug));
   // m_ui->userStatusSuffix->setText(Suffix.value(UserStatus));
   // m_ui->userInfoSuffix->setText(Suffix.value(UserInfo));
   // m_ui->userWarningSuffix->setText(Suffix.value(UserWarning));
   // m_ui->userErrorSuffix->setText(Suffix.value(UserError));
   // m_ui->userFatalSuffix->setText(Suffix.value(UserFatal));

   // m_ui->sysStatusColor->setCurrentColor(Color.value(SysStatus));
   // m_ui->sysInfoColor->setCurrentColor(Color.value(SysInfo));
   // m_ui->sysWarningColor->setCurrentColor(Color.value(SysWarning));
   // m_ui->sysErrorColor->setCurrentColor(Color.value(SysError));
   // m_ui->sysFatalColor->setCurrentColor(Color.value(SysFatal));

   // m_ui->sysStatusPrefix->setText(Prefix.value(SysStatus));
   // m_ui->sysInfoPrefix->setText(Prefix.value(SysInfo));
   // m_ui->sysWarningPrefix->setText(Prefix.value(SysWarning));
   // m_ui->sysErrorPrefix->setText(Prefix.value(SysError));
   // m_ui->sysFatalPrefix->setText(Prefix.value(SysFatal));

   // m_ui->sysStatusSuffix->setText(Suffix.value(SysStatus));
   // m_ui->sysInfoSuffix->setText(Suffix.value(SysInfo));
   // m_ui->sysWarningSuffix->setText(Suffix.value(SysWarning));
   // m_ui->sysErrorSuffix->setText(Suffix.value(SysError));
   // m_ui->sysFatalSuffix->setText(Suffix.value(SysFatal));
}
//-----------------------------------------------------------------------------
