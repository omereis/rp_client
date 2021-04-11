//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "string"

#include "main.h"
#include "misc.h"
#include <IniFiles.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;
using namespace std;
static string g_strAppIni = "RedPitayaGui.ini";
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormDestroy(TObject *Sender)
{
	if (socket->Connected)
		socket->Close();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
	String strIni = Application->ExeName;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Button1Click(TObject *Sender)
{
	m_host.LoadFromIni (GetAppIni (g_strAppIni));
	edtHost->Text = m_host.GetAddress().c_str();
	edtPort->Text = UnicodeString (m_host.GetPort());
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::Button2Click(TObject *Sender)
{
	UploadHostParams (m_host);
	m_host.SaveToIni (GetAppIni (g_strAppIni));
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::UploadHostParams (THostParams &host)
{
	m_host.SetAddress(UnicodeToString (edtHost->Text.Trim()));
	m_host.SetHostPort(edtPort->Text.ToIntDef (5000));
}
//---------------------------------------------------------------------------

