//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Data.DB.hpp>
#include <Datasnap.DBClient.hpp>
#include <Datasnap.Win.MConnect.hpp>
#include <Datasnap.Win.SConnect.hpp>
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------
#include "HostParams.h"
#include <Vcl.AppEvnts.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TLabel *Label2;
	TEdit *edtHost;
	TEdit *edtPort;
	TButton *btnConenct;
	TButton *btnDisconnect;
	TStatusBar *StatusBar1;
	TSocketConnection *socket;
	TButton *Button1;
	TButton *Button2;
	TApplicationEvents *ApplicationEvents1;
	TPanel *Panel1;
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall btnConenctClick(TObject *Sender);
	void __fastcall ApplicationEvents1Idle(TObject *Sender, bool &Done);
protected:
	void __fastcall UploadHostParams (TSocketConnection *s);
	void __fastcall UploadHostParams (THostParams &host);
private:	// User declarations
	THostParams m_host;
public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif
