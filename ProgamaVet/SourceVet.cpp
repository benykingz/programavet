#include <windows.h>
#include "resource.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <commdlg.h>
#include <time.h>

#define MI_TIMER 555

// todo pagar, modificar y eliminar citas || mostrar por orden cronologico 

time_t UnixDate;
struct tm* RealUnixDate;

using namespace std;

int iContMedID = 0;

int iContPetID = 0;


#pragma region booleanos

BOOL UserNew = FALSE;

BOOL bmessage;

BOOL bNewUser;

BOOL bFirstUser;

BOOL g_fShown = FALSE;

BOOL bCitaAlt;

BOOL SelectCita = FALSE;

BOOL bImagenSeleccionada;

BOOL bImagenSelecPet1;

BOOL bImagenSelecPet2;

BOOL bImagenSelecPet3;

#pragma endregion booleanos

//lista de medicos
struct med {
	string user;
	string pasword;
	string name;
	string ced;
	char bitmapDoc[MAX_PATH];
	int IDm;
	med *nextM;
	med *prevM;
}*originM = NULL, *auxM = NULL;

struct CurrentDoc {
	string userC;
	string paswordC;
	string nameC;
	string cedC;

	char cbitmapDoc[MAX_PATH];
	int IDmC;
};

CurrentDoc cCurr;

//lista de citas

struct Calendario {
	string dia;
	string mes;
	string año;
};

struct cita {
	Calendario fechaCita;
	int IDp;
	int iIDvinculo;
	float costo;
	string cliente;
	string tipMascota;
	string nameMascota;
	string motivo;
	string telefono;
	char bitmapPet1[MAX_PATH];
	char bitmapPet2[MAX_PATH];
	char bitmapPet3[MAX_PATH];
	cita *nextC;
	cita *prevC;
}*originC = NULL, *auxC = NULL;

struct Currentcita {
	Calendario cfechaCita;
	int currIDp;
	int curriIDvinculo;
	float currcosto;
	string currcliente;
	string currtipMascota;
	string currnameMascota;
	string currmotivo;
	char currbitmapPet1[MAX_PATH];
	char currbitmapPet2[MAX_PATH];
	char currbitmapPet3[MAX_PATH];
};

Currentcita cCurrCita;

Calendario cFechaActual;

#pragma region stream

fstream binarioM;
fstream binarioMedId;
fstream binarioPet;
fstream binarioPetId;

#pragma endregion stream

#pragma region funciones
void registro();
void guardarMed(med *originM);
void cargaMed();
void guardariContMedID(int iContMedID);
void cargaiContMedID();
void guardarPet(cita *originC);
void cargaPet();
void guardariContPetID(int iContPetID);
void cargaiContPetID();

#pragma endregion funciones

#pragma region CALLBACKS
BOOL CALLBACK flogin(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fprincipal(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK finfoadoc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fsalir(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK faltacita(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK faltauser(HWND, UINT, WPARAM, LPARAM);
#pragma endregion CALLBACKS

#pragma region Salidas
BOOL bloginExit = false;
BOOL bprincipalExit = false;
BOOL bSalidaExit = false;
BOOL X_EXIT = false;

#pragma endregion Salidas

#pragma region CONDICIONALES
CONST int NumUserExit = 10;
CONST int NumInfExit = 20;
CONST int NumLoginExit = 101;
CONST int NumPrincipalExit = 113;
CONST int NumSalidaExit = 120;

#pragma endregion CONDICIONALES


HINSTANCE hGlobalInst;

char original_path[MAX_PATH];

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hprev, PSTR cmdline, int cShow) {

	GetCurrentDirectory(MAX_PATH, original_path);
	hGlobalInst = hinst;
	HWND hlogin = CreateDialog(hinst, MAKEINTRESOURCE(ID_DIALOG_LOG), NULL, flogin);
	ShowWindow(hlogin, cShow);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 1;

}

#pragma region CALLS

//Botones del Login
BOOL CALLBACK flogin(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	switch (msg) {

	case WM_INITDIALOG:
		cargaMed();
		cargaiContMedID();
		cargaPet();
		cargaiContPetID();
		return TRUE;
		break;

	case WM_COMMAND:

#pragma region Login
		// button login //
		if (LOWORD(wparam) == ID_BUTTON_LOGIN && HIWORD(wparam) == BN_CLICKED) {
			HWND hTxtUserName = GetDlgItem(hwnd, ID_EDIT_USUARIO);
			HWND hTxtPassWord = GetDlgItem(hwnd, ID_EDIT_CONTRASENA);
			int iUserNameLenght = GetWindowTextLength(hTxtUserName);
			int iPassWordLenght = GetWindowTextLength(hTxtPassWord);
			char cUserName[30];
			char cPassWord[10];

			if (iUserNameLenght < 1 || iPassWordLenght < 1) {
				MessageBox(NULL, "Ingrese todos los datos", "Error", MB_ICONERROR);
				break;
			}

			else {
				GetWindowText(hTxtUserName, cUserName, ++iUserNameLenght);
				GetWindowText(hTxtPassWord, cPassWord, ++iPassWordLenght);

				if (originM == NULL) {
					MessageBox(NULL, "No existen usuarios registrados", "XD", MB_ICONERROR);
					break;
				}

				auxM = originM;

				while (auxM != NULL) {
					if (auxM->user.compare(cUserName) == 0 && auxM->pasword.compare(cPassWord) == 0) {

						cCurr.userC = auxM->user;
						cCurr.paswordC = auxM->pasword;
						cCurr.IDmC = auxM->IDm;
						cCurr.nameC = auxM->name;
						cCurr.cedC = auxM->ced;
						strcpy_s(cCurr.cbitmapDoc, auxM->bitmapDoc);

						auxM = originM;

						bNewUser = FALSE;

						HWND hPrincipal = CreateDialog(hGlobalInst, MAKEINTRESOURCE(ID_DIALOG_PRIN), NULL, fprincipal);
						ShowWindow(hPrincipal, SW_SHOW);

						SetTimer(hPrincipal, MI_TIMER, 1000, (TIMERPROC)NULL);

						DestroyWindow(hwnd);
						while (auxM != NULL) {
							auxM = auxM->nextM;
						}
						bmessage = TRUE;
					}
					else {
						auxM = auxM->nextM;
					}
				}
				if (bmessage == FALSE) {
					MessageBox(NULL, "No existe el usuario que ingreso / contraseña invalida", "XD", MB_ICONERROR);

					SetWindowText(hTxtUserName, "");
					SetWindowText(hTxtPassWord, "");
					break;
				}
			}
		}
		else if (LOWORD(wparam) == ID_BUTTON_USERNEW && HIWORD(wparam) == BN_CLICKED) {

			int resultuser = DialogBox(hGlobalInst, MAKEINTRESOURCE(ID_DIALOG_NEWUSER), hwnd, faltauser);

			if (X_EXIT == TRUE) {

			}
			else {
				bloginExit = FALSE;
				DestroyWindow(hwnd);

				HWND hPrincipal = CreateDialog(hGlobalInst, MAKEINTRESOURCE(ID_DIALOG_PRIN), NULL, fprincipal);
				ShowWindow(hPrincipal, SW_SHOW);

				SetTimer(hPrincipal, MI_TIMER, 1000, (TIMERPROC)NULL);
			}
		}

		break;
#pragma endregion Login

	case WM_CLOSE:
		bloginExit = TRUE;
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		if (bloginExit)
			PostQuitMessage(NumLoginExit);


		break;

	default:
		DefWindowProc(hwnd, msg, wparam, lparam);
		break;
	}
	return FALSE;
}


//Botones altauser
BOOL CALLBACK faltauser(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	switch (msg) {

	case WM_COMMAND:

		if (LOWORD(wparam) == ID_NEW_BTN_REG && HIWORD(wparam) == BN_CLICKED) {

			HWND hTxtNewUserName = GetDlgItem(hwnd, ID_NEW_EDIT_USER);
			HWND hTxtNewPassWord = GetDlgItem(hwnd, ID_NEW_EDIT_PAS);

			int iUserLenght = GetWindowTextLength(hTxtNewUserName);
			int iPassWordLenght = GetWindowTextLength(hTxtNewPassWord);

			char cNewUser[30];
			char cNewPassWord[15];

			if (iUserLenght < 1 || iPassWordLenght < 1) {
				MessageBox(NULL, "Ingrese todos los datos", "Error", MB_ICONERROR);
				break;
			}

			else {

				bool Digit = FALSE;

				GetWindowText(hTxtNewUserName, cNewUser, ++iUserLenght);
				GetWindowText(hTxtNewPassWord, cNewPassWord, ++iPassWordLenght);


				for (int i = 0; i < iUserLenght; i++) {

					if (isdigit(cNewUser[i]) == 0) {
						// no es digito
					}
					else {
						Digit = TRUE;
						MessageBox(NULL, "Usuario no debe tener numeros", "Error", MB_ICONERROR);
						SetWindowText(hTxtNewUserName, "");
						break;
					}
				}

				if (Digit == TRUE) {
					break;
				}

				if (originM != NULL) {
					auxM = originM;
					while (auxM != NULL) {
						if (auxM->user.compare(cNewUser) == 0) {
							Digit = TRUE;
							MessageBox(NULL, "Usuario ya existe", "Error", MB_ICONERROR);
							SetWindowText(hTxtNewUserName, "");
							break;
						}
						else {
							auxM = auxM->nextM;
						}
					}
				}

				if (Digit == TRUE) {
					break;
				}


				if (originM == NULL) {
					bFirstUser = TRUE;

					cCurr.userC = cNewUser;
					cCurr.paswordC = cNewPassWord;
					cCurr.IDmC = iContMedID;

					bNewUser = TRUE;

					X_EXIT = FALSE;
					EndDialog(hwnd, NumUserExit);

					//	HWND hPrincipal = CreateDialog(hGlobalInst, MAKEINTRESOURCE(ID_DIALOG_PRIN), NULL, fprincipal);
					//	ShowWindow(hPrincipal, SW_SHOW);

					break;
				}
				else {

					auxM = originM;
					while (auxM->nextM != NULL) {
						auxM = auxM->nextM;
					}

					bFirstUser = FALSE;

					cCurr.userC = cNewUser;
					cCurr.paswordC = cNewPassWord;
					cCurr.IDmC = iContMedID;

					bNewUser = TRUE;

					X_EXIT = FALSE;
					EndDialog(hwnd, NumUserExit);

					//	HWND hPrincipal = CreateDialog(hGlobalInst, MAKEINTRESOURCE(ID_DIALOG_PRIN), NULL, fprincipal);
					//	ShowWindow(hPrincipal, SW_SHOW);

					break;

				}

			}
		}

		break;

	case WM_CLOSE:
		X_EXIT = TRUE;
		EndDialog(hwnd, NumUserExit);
		break;

	default:
		//	DefWindowProc(hwnd, msg, wparam, lparam);
		break;
	}
	return FALSE;
}


//Botones del Principal
BOOL CALLBACK fprincipal(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	switch (msg) {

	case WM_COMMAND:
		if (LOWORD(wparam) == ID_INFORMACIONDEDOCTOR) {

			int resultinfdoc = DialogBox(hGlobalInst, MAKEINTRESOURCE(ID_DIALOG_INF), hwnd, finfoadoc);

			if (resultinfdoc == NumInfExit) {
			}

			HWND hLblDocName = GetDlgItem(hwnd, ID_LBL_DocName);
			SetWindowText(hLblDocName, cCurr.nameC.c_str());

			HWND hLblDocCed = GetDlgItem(hwnd, ID_LBL_docced);
			SetWindowText(hLblDocCed, cCurr.cedC.c_str());

			HBITMAP hImagenDoc = NULL;

			hImagenDoc = (HBITMAP)LoadImage(NULL, cCurr.cbitmapDoc, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControl = GetDlgItem(hwnd, ID_IMAGE_DOCPRIN);
			SendMessage(hPictureControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenDoc);

		}
		else if (LOWORD(wparam) == ID_NUEVACITA) {
			bCitaAlt = TRUE;

			time(&UnixDate);
			RealUnixDate = localtime(&UnixDate);

			char cDay[5];
			char cMont[5];
			char cYear[5];

			_itoa(RealUnixDate->tm_mday, cDay, 10);
			_itoa(RealUnixDate->tm_mon + 1, cMont, 10);
			_itoa(RealUnixDate->tm_year + 1900, cYear, 10);

			cFechaActual.dia = cDay;
			cFechaActual.mes = cMont;
			cFechaActual.año = cYear;

			int resultsalir = DialogBox(hGlobalInst, MAKEINTRESOURCE(ID_ALTA_PET), hwnd, faltacita);

			auxC = originC;
			int index = 0;

			HWND hLbCitas = GetDlgItem(hwnd, ID_LB_CITAS);

			SendMessage(hLbCitas, LB_RESETCONTENT, NULL, NULL);

			BOOL bCitasExist = false;

			while (auxC != NULL) {

				if (auxC->iIDvinculo == cCurr.IDmC) {

					bCitasExist = true;

					SendMessage(hLbCitas, LB_ADDSTRING, NULL, (LPARAM)auxC->nameMascota.c_str());
					SendMessage(hLbCitas, LB_SETITEMDATA, (WPARAM)index, (LPARAM)auxC->IDp);
					index++;
				}

				auxC = auxC->nextC;
			}
			
			auxC = originC;

#pragma region ShowPetInfo
			HWND hLblPetName = GetDlgItem(hwnd, ID_SHOW_PETNAME);
			SetWindowText(hLblPetName, "");

			HWND hLblPetType = GetDlgItem(hwnd, ID_SHOW_TYPEPET);
			SetWindowText(hLblPetType, "");

			HWND hLblPetMotive = GetDlgItem(hwnd, ID_SHOWMOTIVEPET);
			SetWindowText(hLblPetMotive, "");

			HWND hLblDatePet = GetDlgItem(hwnd, ID_SHOWDATEPET);
			SetWindowText(hLblDatePet, "");

			HWND hLblCostPet = GetDlgItem(hwnd, ID_SHOWCOSTPET);
			SetWindowText(hLblCostPet, "");

			HWND hLblClientName = GetDlgItem(hwnd, ID_SHOWCLIENTNAME);
			SetWindowText(hLblClientName, "");

			HWND hLblPhone = GetDlgItem(hwnd, ID_SHOWPHONE);
			SetWindowText(hLblPhone, "");

			HBITMAP hImagenPet1 = NULL;
			hImagenPet1 = (HBITMAP)LoadImage(NULL, "", IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP1 = GetDlgItem(hwnd, ID_SHOWPIC1);
			SendMessage(hPictureControlP1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet1);

			HBITMAP hImagenPet2 = NULL;
			hImagenPet2 = (HBITMAP)LoadImage(NULL, "", IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP2 = GetDlgItem(hwnd, ID_SHOWPIC2);
			SendMessage(hPictureControlP2, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet2);

			HBITMAP hImagenPet3 = NULL;
			hImagenPet3 = (HBITMAP)LoadImage(NULL, "", IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP3 = GetDlgItem(hwnd, ID_SHOWPIC3);
			SendMessage(hPictureControlP3, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet3);
#pragma endregion ShowPetInfo

			HWND btnEliminar = GetDlgItem(hwnd, ID_ELIMINAR);
			HWND btnMod = GetDlgItem(hwnd, ID_MOD);
			HWND btnPagar = GetDlgItem(hwnd, ID_PAGAR);

			if (bCitasExist == false) {
				EnableWindow(btnEliminar, false);
				EnableWindow(btnMod, false);
				EnableWindow(btnPagar, false);
			}
			else {
				EnableWindow(btnEliminar, true);
				EnableWindow(btnMod, true);
				EnableWindow(btnPagar, true);
			}
			if (SelectCita == false) {
				EnableWindow(btnEliminar, false);
				EnableWindow(btnMod, false);
				EnableWindow(btnPagar, false);
			}
			else {
				EnableWindow(btnEliminar, true);
				EnableWindow(btnMod, true);
				EnableWindow(btnPagar, true);
			}

		}
		else if (LOWORD(wparam) == ID_SALIR) {

			int resultsalir = DialogBox(hGlobalInst, MAKEINTRESOURCE(ID_DIALOG_SALIR), hwnd, fsalir);
			DestroyWindow(hwnd);

		}
		else if (LOWORD(wparam) == ID_LB_CITAS && HIWORD(wparam) == LBN_SELCHANGE) {

			SelectCita == true;

			HWND btnEliminar = GetDlgItem(hwnd, ID_ELIMINAR);
			HWND btnMod = GetDlgItem(hwnd, ID_MOD);
			HWND btnPagar = GetDlgItem(hwnd, ID_PAGAR);

			EnableWindow(btnEliminar, true);
			EnableWindow(btnMod, true);
			EnableWindow(btnPagar, true);

			HWND hlbCitas = GetDlgItem(hwnd, ID_LB_CITAS);
			int iSelectionIndex = SendMessage(hlbCitas, LB_GETCURSEL, NULL, NULL);
			int idSelected = SendMessage(hlbCitas, LB_GETITEMDATA, (WPARAM)iSelectionIndex, NULL);

			cCurrCita.currIDp = idSelected;

			auxC = originC;
			while (auxC->IDp != idSelected) {
				auxC = auxC->nextC;
			}

#pragma region ShowPetInfo

			HWND hLblPetName = GetDlgItem(hwnd, ID_SHOW_PETNAME);
			SetWindowText(hLblPetName, auxC->nameMascota.c_str());

			HWND hLblPetType = GetDlgItem(hwnd, ID_SHOW_TYPEPET);
			SetWindowText(hLblPetType, auxC->tipMascota.c_str());

			HWND hLblPetMotive = GetDlgItem(hwnd, ID_SHOWMOTIVEPET);
			SetWindowText(hLblPetMotive, auxC->motivo.c_str());

			string sDatePetShow = auxC->fechaCita.dia + "/" + auxC->fechaCita.mes + "/" + auxC->fechaCita.año;

			HWND hLblDatePet = GetDlgItem(hwnd, ID_SHOWDATEPET);
			SetWindowText(hLblDatePet, sDatePetShow.c_str());

			string straux = to_string(auxC->costo);

			HWND hLblCostPet = GetDlgItem(hwnd, ID_SHOWCOSTPET);
			SetWindowText(hLblCostPet, straux.c_str());

			HWND hLblClientName = GetDlgItem(hwnd, ID_SHOWCLIENTNAME);
			SetWindowText(hLblClientName, auxC->cliente.c_str());

			HWND hLblPhone = GetDlgItem(hwnd, ID_SHOWPHONE);
			SetWindowText(hLblPhone, auxC->telefono.c_str());

			HBITMAP hImagenPet1 = NULL;
			hImagenPet1 = (HBITMAP)LoadImage(NULL, auxC->bitmapPet1, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP1 = GetDlgItem(hwnd, ID_SHOWPIC1);
			SendMessage(hPictureControlP1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet1);

			HBITMAP hImagenPet2 = NULL;
			hImagenPet2 = (HBITMAP)LoadImage(NULL, auxC->bitmapPet2, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP2 = GetDlgItem(hwnd, ID_SHOWPIC2);
			SendMessage(hPictureControlP2, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet2);

			HBITMAP hImagenPet3 = NULL;
			hImagenPet3 = (HBITMAP)LoadImage(NULL, auxC->bitmapPet3, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP3 = GetDlgItem(hwnd, ID_SHOWPIC3);
			SendMessage(hPictureControlP3, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet3);

#pragma endregion ShowPetInfo

		}
		else if (LOWORD(wparam) == ID_ELIMINAR) {

			auxC = originC;
			while (auxC->IDp != cCurrCita.currIDp) {
				auxC = auxC->nextC;
			}


			if (auxC->nextC == NULL && auxC->prevC == NULL) {
				delete auxC;
				originC = auxC = NULL;
			}
			else if (auxC->prevC == NULL) { //PRIMERO
				originC = auxC->nextC;
				delete auxC;
				originC->prevC = NULL;
				auxC = originC;
			}
			else if (auxC->nextC == NULL) { //ULTIMO
				auxC->prevC->nextC = NULL;
				delete auxC;
				auxC = originC;
			}
			else {
				auxC->prevC->nextC = auxC->nextC;
				auxC->nextC->prevC = auxC->prevC;
				delete auxC;
				auxC = originC;
			}

			auxC = originC;
			int index = 0;
			HWND hLbCitas = GetDlgItem(hwnd, ID_LB_CITAS);
			SendMessage(hLbCitas, LB_RESETCONTENT, NULL, NULL);

			bool bCitasExist = false;

			while (auxC != NULL) {

				if (auxC->iIDvinculo == cCurr.IDmC) {

					bCitasExist = true;

					SendMessage(hLbCitas, LB_ADDSTRING, NULL, (LPARAM)auxC->nameMascota.c_str());
					SendMessage(hLbCitas, LB_SETITEMDATA, (WPARAM)index, (LPARAM)auxC->IDp);
					index++;
				}
				auxC = auxC->nextC;

			}auxC = originC;

			HWND btnEliminar = GetDlgItem(hwnd, ID_ELIMINAR);
			HWND btnMod = GetDlgItem(hwnd, ID_MOD);
			HWND btnPagar = GetDlgItem(hwnd, ID_PAGAR);

#pragma region ShowPetInfo
			HWND hLblPetName = GetDlgItem(hwnd, ID_SHOW_PETNAME);
			SetWindowText(hLblPetName, "");

			HWND hLblPetType = GetDlgItem(hwnd, ID_SHOW_TYPEPET);
			SetWindowText(hLblPetType, "");

			HWND hLblPetMotive = GetDlgItem(hwnd, ID_SHOWMOTIVEPET);
			SetWindowText(hLblPetMotive, "");

			HWND hLblDatePet = GetDlgItem(hwnd, ID_SHOWDATEPET);
			SetWindowText(hLblDatePet, "");

			HWND hLblCostPet = GetDlgItem(hwnd, ID_SHOWCOSTPET);
			SetWindowText(hLblCostPet, "");

			HWND hLblClientName = GetDlgItem(hwnd, ID_SHOWCLIENTNAME);
			SetWindowText(hLblClientName, "");

			HWND hLblPhone = GetDlgItem(hwnd, ID_SHOWPHONE);
			SetWindowText(hLblPhone, "");

			HBITMAP hImagenPet1 = NULL;
			hImagenPet1 = (HBITMAP)LoadImage(NULL, "", IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP1 = GetDlgItem(hwnd, ID_SHOWPIC1);
			SendMessage(hPictureControlP1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet1);

			HBITMAP hImagenPet2 = NULL;
			hImagenPet2 = (HBITMAP)LoadImage(NULL, "", IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP2 = GetDlgItem(hwnd, ID_SHOWPIC2);
			SendMessage(hPictureControlP2, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet2);

			HBITMAP hImagenPet3 = NULL;
			hImagenPet3 = (HBITMAP)LoadImage(NULL, "", IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP3 = GetDlgItem(hwnd, ID_SHOWPIC3);
			SendMessage(hPictureControlP3, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet3);
#pragma endregion ShowPetInfo

			if (bCitasExist == false) {
				EnableWindow(btnEliminar, false);
				EnableWindow(btnMod, false);
				EnableWindow(btnPagar, false);
			}
			else {
				EnableWindow(btnEliminar, true);
				EnableWindow(btnMod, true);
				EnableWindow(btnPagar, true);
			}
			if (SelectCita == false) {
				EnableWindow(btnEliminar, false);
				EnableWindow(btnMod, false);
				EnableWindow(btnPagar, false);
			}
			else {
				EnableWindow(btnEliminar, true);
				EnableWindow(btnMod, true);
				EnableWindow(btnPagar, true);
			}

		}
		else if (LOWORD(wparam) == ID_MOD) {

			bCitaAlt = FALSE;

			time(&UnixDate);
			RealUnixDate = localtime(&UnixDate);

			char cDay[5];
			char cMont[5];
			char cYear[5];

			_itoa(RealUnixDate->tm_mday, cDay, 10);
			_itoa(RealUnixDate->tm_mon + 1, cMont, 10);
			_itoa(RealUnixDate->tm_year + 1900, cYear, 10);

			cFechaActual.dia = cDay;
			cFechaActual.mes = cMont;
			cFechaActual.año = cYear;

			int resultsalir = DialogBox(hGlobalInst, MAKEINTRESOURCE(ID_ALTA_PET), hwnd, faltacita);

			auxC = originC;
			int index = 0;
			HWND hLbCitas = GetDlgItem(hwnd, ID_LB_CITAS);
			SendMessage(hLbCitas, LB_RESETCONTENT, NULL, NULL);

			bool bCitasExist = false;

			while (auxC != NULL) {

				if (auxC->iIDvinculo == cCurr.IDmC) {

					bCitasExist = true;

					SendMessage(hLbCitas, LB_ADDSTRING, NULL, (LPARAM)auxC->nameMascota.c_str());
					SendMessage(hLbCitas, LB_SETITEMDATA, (WPARAM)index, (LPARAM)auxC->IDp);
					index++;
				}

				auxC = auxC->nextC;
			}auxC = originC;

			HWND btnEliminar = GetDlgItem(hwnd, ID_ELIMINAR);
			HWND btnMod = GetDlgItem(hwnd, ID_MOD);
			HWND btnPagar = GetDlgItem(hwnd, ID_PAGAR);

#pragma region ShowPetInfo
			HWND hLblPetName = GetDlgItem(hwnd, ID_SHOW_PETNAME);
			SetWindowText(hLblPetName, "");

			HWND hLblPetType = GetDlgItem(hwnd, ID_SHOW_TYPEPET);
			SetWindowText(hLblPetType, "");

			HWND hLblPetMotive = GetDlgItem(hwnd, ID_SHOWMOTIVEPET);
			SetWindowText(hLblPetMotive, "");

			HWND hLblDatePet = GetDlgItem(hwnd, ID_SHOWDATEPET);
			SetWindowText(hLblDatePet, "");

			HWND hLblCostPet = GetDlgItem(hwnd, ID_SHOWCOSTPET);
			SetWindowText(hLblCostPet, "");

			HWND hLblClientName = GetDlgItem(hwnd, ID_SHOWCLIENTNAME);
			SetWindowText(hLblClientName, "");

			HWND hLblPhone = GetDlgItem(hwnd, ID_SHOWPHONE);
			SetWindowText(hLblPhone, "");

			HBITMAP hImagenPet1 = NULL;
			hImagenPet1 = (HBITMAP)LoadImage(NULL, "", IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP1 = GetDlgItem(hwnd, ID_SHOWPIC1);
			SendMessage(hPictureControlP1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet1);

			HBITMAP hImagenPet2 = NULL;
			hImagenPet2 = (HBITMAP)LoadImage(NULL, "", IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP2 = GetDlgItem(hwnd, ID_SHOWPIC2);
			SendMessage(hPictureControlP2, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet2);

			HBITMAP hImagenPet3 = NULL;
			hImagenPet3 = (HBITMAP)LoadImage(NULL, "", IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP3 = GetDlgItem(hwnd, ID_SHOWPIC3);
			SendMessage(hPictureControlP3, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet3);
#pragma endregion ShowPetInfo

			if (bCitasExist == false) {
				EnableWindow(btnEliminar, false);
				EnableWindow(btnMod, false);
				EnableWindow(btnPagar, false);
			}
			else {
				EnableWindow(btnEliminar, true);
				EnableWindow(btnMod, true);
				EnableWindow(btnPagar, true);
			}
			if (SelectCita == false) {
				EnableWindow(btnEliminar, false);
				EnableWindow(btnMod, false);
				EnableWindow(btnPagar, false);
			}
			else {
				EnableWindow(btnEliminar, true);
				EnableWindow(btnMod, true);
				EnableWindow(btnPagar, true);
			}

		}
		else if (LOWORD(wparam) == ID_PAGAR) {

		}

		break;

	case WM_INITDIALOG:

		if (bNewUser != TRUE) {

			HWND hLblDocName = GetDlgItem(hwnd, ID_LBL_DocName);
			SetWindowText(hLblDocName, cCurr.nameC.c_str());

			HWND hLblDocCed = GetDlgItem(hwnd, ID_LBL_docced);
			SetWindowText(hLblDocCed, cCurr.cedC.c_str());

#pragma region ShowPetInfo
			HWND hLblPetName = GetDlgItem(hwnd, ID_SHOW_PETNAME);
			SetWindowText(hLblPetName, "");

			HWND hLblPetType = GetDlgItem(hwnd, ID_SHOW_TYPEPET);
			SetWindowText(hLblPetType, "");

			HWND hLblPetMotive = GetDlgItem(hwnd, ID_SHOWMOTIVEPET);
			SetWindowText(hLblPetMotive, "");

			HWND hLblDatePet = GetDlgItem(hwnd, ID_SHOWDATEPET);
			SetWindowText(hLblDatePet, "");

			HWND hLblCostPet = GetDlgItem(hwnd, ID_SHOWCOSTPET);
			SetWindowText(hLblCostPet, "");

			HWND hLblClientName = GetDlgItem(hwnd, ID_SHOWCLIENTNAME);
			SetWindowText(hLblClientName, "");

			HWND hLblPhone = GetDlgItem(hwnd, ID_SHOWPHONE);
			SetWindowText(hLblPhone, "");

			HBITMAP hImagenPet1 = NULL;
			hImagenPet1 = (HBITMAP)LoadImage(NULL, "", IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP1 = GetDlgItem(hwnd, ID_SHOWPIC1);
			SendMessage(hPictureControlP1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet1);

			HBITMAP hImagenPet2 = NULL;
			hImagenPet2 = (HBITMAP)LoadImage(NULL, "", IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP2 = GetDlgItem(hwnd, ID_SHOWPIC2);
			SendMessage(hPictureControlP2, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet2);

			HBITMAP hImagenPet3 = NULL;
			hImagenPet3 = (HBITMAP)LoadImage(NULL, "", IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP3 = GetDlgItem(hwnd, ID_SHOWPIC3);
			SendMessage(hPictureControlP3, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet3);

#pragma endregion     ShowPetInfo

			HBITMAP hImagenDoc = NULL;
			hImagenDoc = (HBITMAP)LoadImage(NULL, cCurr.cbitmapDoc, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControl = GetDlgItem(hwnd, ID_IMAGE_DOCPRIN);
			SendMessage(hPictureControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenDoc);

			auxC = originC;
			int index = 0;
			HWND hLbCitas = GetDlgItem(hwnd, ID_LB_CITAS);
			SendMessage(hLbCitas, LB_RESETCONTENT, NULL, NULL);

			bool bCitasExist = false;

			while (auxC != NULL) {

				if (auxC->iIDvinculo == cCurr.IDmC) {

					bCitasExist = true;

					SendMessage(hLbCitas, LB_ADDSTRING, NULL, (LPARAM)auxC->nameMascota.c_str());
					SendMessage(hLbCitas, LB_SETITEMDATA, (WPARAM)index, (LPARAM)auxC->IDp);
					index++;
				}

				auxC = auxC->nextC;
			}auxC = originC;

			HWND btnEliminar = GetDlgItem(hwnd, ID_ELIMINAR);
			HWND btnMod = GetDlgItem(hwnd, ID_MOD);
			HWND btnPagar = GetDlgItem(hwnd, ID_PAGAR);

			if (bCitasExist == false) {
				EnableWindow(btnEliminar, false);
				EnableWindow(btnMod, false);
				EnableWindow(btnPagar, false);
			}
			else {
				EnableWindow(btnEliminar, true);
				EnableWindow(btnMod, true);
				EnableWindow(btnPagar, true);
			}
			if (SelectCita == false) {
				EnableWindow(btnEliminar, false);
				EnableWindow(btnMod, false);
				EnableWindow(btnPagar, false);
			}
			else {
				EnableWindow(btnEliminar, true);
				EnableWindow(btnMod, true);
				EnableWindow(btnPagar, true);
			}

		}

		return TRUE;

		break;

	case WM_WINDOWPOSCHANGED:
		if ((((WINDOWPOS*)lparam)->flags & SWP_SHOWWINDOW) && !g_fShown && bNewUser == TRUE) {

			HWND btnEliminar = GetDlgItem(hwnd, ID_ELIMINAR);
			HWND btnMod = GetDlgItem(hwnd, ID_MOD);
			HWND btnPagar = GetDlgItem(hwnd, ID_PAGAR);

			EnableWindow(btnEliminar, false);
			EnableWindow(btnMod, false);
			EnableWindow(btnPagar, false);

			HWND hLblDocName = GetDlgItem(hwnd, ID_LBL_DocName);
			SetWindowText(hLblDocName, "");

			HWND hLblDocCed = GetDlgItem(hwnd, ID_LBL_docced);
			SetWindowText(hLblDocCed, "");

#pragma region ShowPetInfo
			HWND hLblPetName = GetDlgItem(hwnd, ID_SHOW_PETNAME);
			SetWindowText(hLblPetName, "");

			HWND hLblPetType = GetDlgItem(hwnd, ID_SHOW_TYPEPET);
			SetWindowText(hLblPetType, "");

			HWND hLblPetMotive = GetDlgItem(hwnd, ID_SHOWMOTIVEPET);
			SetWindowText(hLblPetMotive, "");

			HWND hLblDatePet = GetDlgItem(hwnd, ID_SHOWDATEPET);
			SetWindowText(hLblDatePet, "");

			HWND hLblCostPet = GetDlgItem(hwnd, ID_SHOWCOSTPET);
			SetWindowText(hLblCostPet, "");

			HWND hLblClientName = GetDlgItem(hwnd, ID_SHOWCLIENTNAME);
			SetWindowText(hLblClientName, "");

			HWND hLblPhone = GetDlgItem(hwnd, ID_SHOWPHONE);
			SetWindowText(hLblPhone, "");

			HBITMAP hImagenPet1 = NULL;
			hImagenPet1 = (HBITMAP)LoadImage(NULL, "", IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP1 = GetDlgItem(hwnd, ID_SHOWPIC1);
			SendMessage(hPictureControlP1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet1);

			HBITMAP hImagenPet2 = NULL;
			hImagenPet2 = (HBITMAP)LoadImage(NULL, "", IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP2 = GetDlgItem(hwnd, ID_SHOWPIC2);
			SendMessage(hPictureControlP2, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet2);

			HBITMAP hImagenPet3 = NULL;
			hImagenPet3 = (HBITMAP)LoadImage(NULL, "", IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP3 = GetDlgItem(hwnd, ID_SHOWPIC3);
			SendMessage(hPictureControlP3, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet3);
#pragma endregion ShowPetInfo

			g_fShown = TRUE;

			int resultinfdoc = DialogBox(hGlobalInst, MAKEINTRESOURCE(ID_DIALOG_INF), hwnd, finfoadoc);
			if (resultinfdoc == NumInfExit) {
			}

			SetWindowText(hLblDocName, cCurr.nameC.c_str());

			SetWindowText(hLblDocCed, cCurr.cedC.c_str());

			auxC = originC;
			int index = 0;
			HWND hLbCitas = GetDlgItem(hwnd, ID_LB_CITAS);
			SendMessage(hLbCitas, LB_RESETCONTENT, NULL, NULL);

			bool bCitasExist = false;
			while (auxC != NULL) {

				if (auxC->iIDvinculo == cCurr.IDmC) {

					bCitasExist = true;

					SendMessage(hLbCitas, LB_ADDSTRING, NULL, (LPARAM)auxC->nameMascota.c_str());
					SendMessage(hLbCitas, LB_SETITEMDATA, (WPARAM)index, (LPARAM)auxC->IDp);
					index++;
				}

				auxC = auxC->nextC;
			}auxC = originC;

			HBITMAP hImagenDoc = NULL;

			hImagenDoc = (HBITMAP)LoadImage(NULL, cCurr.cbitmapDoc, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControl = GetDlgItem(hwnd, ID_IMAGE_DOCPRIN);
			SendMessage(hPictureControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenDoc);

			bNewUser = FALSE;
		}
		break;


	case WM_CLOSE:
		//	DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(NumPrincipalExit);
		break;

	default:
		DefWindowProc(hwnd, msg, wparam, lparam);
		break;
	}

	return FALSE;
}


//Botones de info doc
BOOL CALLBACK finfoadoc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	switch (msg) {

	case WM_INITDIALOG: {
		bImagenSeleccionada = FALSE;

		if (bFirstUser == FALSE) {

			HWND hTxtNewDocName = GetDlgItem(hwnd, ID_INFDOC_GETNAME);
			HWND hTxtNewCed = GetDlgItem(hwnd, ID_INFODIC_GETCED);

			SetWindowText(hTxtNewDocName, cCurr.nameC.c_str());
			SetWindowText(hTxtNewCed, cCurr.cedC.c_str());

			HBITMAP hImagenDoc = NULL;
			hImagenDoc = (HBITMAP)LoadImage(NULL, cCurr.cbitmapDoc, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControl = GetDlgItem(hwnd, ID_IMAGE_DOC);
			SendMessage(hPictureControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenDoc);

			bImagenSeleccionada = TRUE;

		}

		return TRUE;
	}
						break;

	case WM_COMMAND:

		if (LOWORD(wparam) == ID_INFODOC_ACT && HIWORD(wparam) == BN_CLICKED) {


			HWND hTxtNewDocName = GetDlgItem(hwnd, ID_INFDOC_GETNAME);
			HWND hTxtNewCed = GetDlgItem(hwnd, ID_INFODIC_GETCED);

			int	 iDocLenght = GetWindowTextLength(hTxtNewDocName);
			int	 iCedLenght = GetWindowTextLength(hTxtNewCed);

			char cDocName[30];
			char cDocCed[9];

#pragma region Condiciones registro 

			if (iDocLenght < 1 || iCedLenght < 1) {
				MessageBox(NULL, "Ingrese todos los datos", "Error", MB_ICONERROR);
				break;
			}

			GetWindowText(hTxtNewDocName, cDocName, ++iDocLenght);
			GetWindowText(hTxtNewCed, cDocCed, ++iCedLenght);

			bool cedDigit = FALSE;

			for (int i = 0; i < iCedLenght - 1; i++) {

				if (isdigit(cDocCed[i]) == 0) {
					cedDigit = TRUE;
					MessageBox(NULL, "cedula debe tener solo numeros", "Error", MB_ICONERROR);
					SetWindowText(hTxtNewCed, "");
					break;
				}
				else {

				}
			}

			if (cedDigit == TRUE) {
				break;
			}

			if (iCedLenght > 9 || iCedLenght < 9) {
				MessageBox(NULL, "La cedula debe tener 8 digitos", "Error", MB_ICONERROR);
				SetWindowText(hTxtNewCed, "");
				break;
			}

			bool Digit = FALSE;
			for (int i = 0; i < iDocLenght - 1; i++) {
				if (isdigit(cDocName[i]) == 0) {
				}
				else {
					Digit = TRUE;
					MessageBox(NULL, "Nombre de doctor no debe tener numeros", "Error", MB_ICONERROR);
					SetWindowText(hTxtNewDocName, "");
					break;
				}
			}
			if (Digit == TRUE) {
				break;
			}

			if (bImagenSeleccionada == FALSE) {
				MessageBox(NULL, "Debe seleccionar para continuar", "Alerta", MB_ICONWARNING);
				break;
			}



#pragma endregion Condiciones registro 

			cCurr.nameC = cDocName;
			cCurr.cedC = cDocCed;

			registro();
			auxM = originM;

			EndDialog(hwnd, NumInfExit);

			break;

		}
		else if (LOWORD(wparam) == ID_SAVE_IMG && HIWORD(wparam) == BN_CLICKED) {

			OPENFILENAME ImagenBusquedaDoc;
			ZeroMemory(&ImagenBusquedaDoc, sizeof(ImagenBusquedaDoc));
			char cDireccionImagen[MAX_PATH] = " ";

			ImagenBusquedaDoc.hwndOwner = hwnd;
			ImagenBusquedaDoc.nMaxFile = MAX_PATH;
			ImagenBusquedaDoc.lpstrDefExt = ".bmp";
			ImagenBusquedaDoc.lStructSize = sizeof(OPENFILENAME);
			ImagenBusquedaDoc.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			ImagenBusquedaDoc.lpstrFile = cDireccionImagen;
			ImagenBusquedaDoc.lpstrFilter = "Mostrar imagenes\0*.bmp";

			if (GetOpenFileName(&ImagenBusquedaDoc) == TRUE) {
				bImagenSeleccionada = TRUE;

				strcpy_s(cCurr.cbitmapDoc, cDireccionImagen);
				HBITMAP hImagenDoc = NULL;
				hImagenDoc = (HBITMAP)LoadImage(NULL, cDireccionImagen, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
				HWND hPictureControl = GetDlgItem(hwnd, ID_IMAGE_DOC);
				SendMessage(hPictureControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenDoc);

				break;
			}
			else {
				bImagenSeleccionada = FALSE;

				strcpy_s(cCurr.cbitmapDoc, cDireccionImagen);
				HBITMAP hImagenDoc = NULL;
				hImagenDoc = (HBITMAP)LoadImage(NULL, cDireccionImagen, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
				HWND hPictureControl = GetDlgItem(hwnd, ID_IMAGE_DOC);
				SendMessage(hPictureControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenDoc);

				MessageBox(NULL, "No selecciono una imagen", "Alerta", MB_ICONWARNING);
				break;

			}

			break;
		}
		else if (LOWORD(wparam) == ID_AGENDA && HIWORD(wparam) == BN_CLICKED) {

			if (bNewUser == TRUE) {

				MessageBox(NULL, "Ingrese todos los datos para ir a agenda", "XD", MB_ICONERROR);

				break;
			}
			MessageBox(NULL, "Se volvera a la pantalla inicial", "XD", MB_ICONINFORMATION);
			EndDialog(hwnd, NumInfExit);
			break;
		}
		else if (LOWORD(wparam) == ID_SALIR) {
			if (bNewUser == TRUE) {

				MessageBox(NULL, "Para salir ingrese todos los datos", "XD", MB_ICONERROR);

				break;
			}
			int resultsalir = DialogBox(hGlobalInst, MAKEINTRESOURCE(ID_DIALOG_SALIR), hwnd, fsalir);
			EndDialog(hwnd, NumInfExit);
		}
		else if (LOWORD(wparam) == ID_NUEVACITA) {
			if (bNewUser == TRUE) {

				MessageBox(NULL, "Para hacer citas ingrese todos los datos", "XD", MB_ICONERROR);

				break;
			}
			int resultsalir = DialogBox(hGlobalInst, MAKEINTRESOURCE(ID_NUEVACITA), hwnd, faltacita);
			EndDialog(hwnd, NumInfExit);
		}

		break;

	case WM_CLOSE:
		//	EndDialog(hwnd, NumInfExit);
		break;

	case WM_DESTROY:
		EndDialog(hwnd, NumInfExit);
		break;

	default:
		//	DefWindowProc(hwnd, msg, wparam, lparam);
		break;

	}

	return FALSE;
}


//botones alta pet
BOOL CALLBACK faltacita(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	switch (msg) {

	case WM_INITDIALOG: {


		HWND cbComboTipo;
		cbComboTipo = GetDlgItem(hwnd, ID_CMB_TIPO);

		SendMessage(cbComboTipo, CB_ADDSTRING, 0, (LPARAM) "Gato ");
		SendMessage(cbComboTipo, CB_ADDSTRING, 1, (LPARAM) "Perro ");
		SendMessage(cbComboTipo, CB_ADDSTRING, 2, (LPARAM) "hamster ");

		SendMessage(cbComboTipo, CB_SETCURSEL, 0, 0);

		if (bCitaAlt == TRUE) {

			strcpy_s(cCurrCita.currbitmapPet1, "");
			strcpy_s(cCurrCita.currbitmapPet2, "");
			strcpy_s(cCurrCita.currbitmapPet3, "");

			bImagenSelecPet1 = FALSE;

			bImagenSelecPet2 = FALSE;

			bImagenSelecPet3 = FALSE;

		}
		if (bCitaAlt == FALSE) {

			auxC = originC;
			while (auxC->IDp != cCurrCita.currIDp) {
				auxC = auxC->nextC;
			}

			HWND hTxtNewPetName = GetDlgItem(hwnd, ID_PET_NAME);
			HWND hTxtNewPetMotive = GetDlgItem(hwnd, ID_MOTIVE_PET);
			HWND hTxtNewClientName = GetDlgItem(hwnd, ID_CLIENT_NAME);
			HWND hTxtNewTelephone = GetDlgItem(hwnd, ID_TELE_ALT);
			HWND hTxtNewPrice = GetDlgItem(hwnd, ID_PRECIO);
			HWND hTxtNewTipo = GetDlgItem(hwnd, ID_CMB_TIPO);

			HWND hTxtNewDateDay = GetDlgItem(hwnd, ID_GETDAY);
			HWND hTxtNewDateMonth = GetDlgItem(hwnd, ID_GETMONTH);
			HWND hTxtNewDateYear = GetDlgItem(hwnd, ID_GETYEAR);

			SetWindowText(hTxtNewPetName, auxC->nameMascota.c_str());
			SetWindowText(hTxtNewPetMotive, auxC->motivo.c_str());
			SetWindowText(hTxtNewClientName, auxC->cliente.c_str());
			SetWindowText(hTxtNewTelephone, auxC->telefono.c_str());

			string straux = to_string(auxC->costo);
			SetWindowText(hTxtNewPrice, straux.c_str());

			SetWindowText(hTxtNewTipo, auxC->tipMascota.c_str());

			SetWindowText(hTxtNewDateDay, auxC->fechaCita.dia.c_str());
			SetWindowText(hTxtNewDateMonth, auxC->fechaCita.mes.c_str());
			SetWindowText(hTxtNewDateYear, auxC->fechaCita.año.c_str());

			bImagenSelecPet1 = TRUE;

			bImagenSelecPet2 = TRUE;

			bImagenSelecPet3 = TRUE;


			HBITMAP hImagenPet1 = NULL;
			hImagenPet1 = (HBITMAP)LoadImage(NULL, auxC->bitmapPet1, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP1 = GetDlgItem(hwnd, ID_SHOWBIT1_ALT);
			SendMessage(hPictureControlP1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet1);

			HBITMAP hImagenPet2 = NULL;
			hImagenPet2 = (HBITMAP)LoadImage(NULL, auxC->bitmapPet2, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP2 = GetDlgItem(hwnd, ID_SHOWBIT2_ALT);
			SendMessage(hPictureControlP2, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet2);

			HBITMAP hImagenPet3 = NULL;
			hImagenPet3 = (HBITMAP)LoadImage(NULL, auxC->bitmapPet3, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP3 = GetDlgItem(hwnd, ID_SHOWBIT3_ALT);
			SendMessage(hPictureControlP3, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet3);
		}

	}
						break;

	case WM_COMMAND:

		if (LOWORD(wparam) == ID_PET_ALTA && HIWORD(wparam) == BN_CLICKED) {

#pragma region obtencion

			HWND hTxtNewPetName = GetDlgItem(hwnd, ID_PET_NAME);
			HWND hTxtNewPetMotive = GetDlgItem(hwnd, ID_MOTIVE_PET);
			HWND hTxtNewClientName = GetDlgItem(hwnd, ID_CLIENT_NAME);
			HWND hTxtNewTelephone = GetDlgItem(hwnd, ID_TELE_ALT);
			HWND hTxtNewPrice = GetDlgItem(hwnd, ID_PRECIO);
			HWND hTxtNewTipo = GetDlgItem(hwnd, ID_CMB_TIPO);

			HWND hTxtNewDateDay = GetDlgItem(hwnd, ID_GETDAY);
			HWND hTxtNewDateMonth = GetDlgItem(hwnd, ID_GETMONTH);
			HWND hTxtNewDateYear = GetDlgItem(hwnd, ID_GETYEAR);


			int	 iPetNameLenght = GetWindowTextLength(hTxtNewPetName);
			int	 iPetMotiveLenght = GetWindowTextLength(hTxtNewPetMotive);
			int	 iClientNameLenght = GetWindowTextLength(hTxtNewClientName);
			int	 iPhoneLenght = GetWindowTextLength(hTxtNewTelephone);
			int	 iPriceLength = GetWindowTextLength(hTxtNewPrice);
			int	 iTypeLength = GetWindowTextLength(hTxtNewTipo);

			int	 iDateDayLength = GetWindowTextLength(hTxtNewDateDay);
			int	 iDateMonthLength = GetWindowTextLength(hTxtNewDateMonth);
			int	 iDateYearLength = GetWindowTextLength(hTxtNewDateYear);


			char cPetName[30];
			char cPetMotive[30];
			char cClientName[30];
			char cPhone[30];
			char cPrice[30];
			char ctype[30];

			char cDateDay[30];
			char cDateMonth[30];
			char cDateYear[30];

			if (iPetNameLenght < 1 || iPetMotiveLenght < 1 || iClientNameLenght < 1
				|| iPhoneLenght < 1 || iPriceLength < 1 || iTypeLength < 1
				|| iDateDayLength < 1 || iDateMonthLength < 1 || iDateYearLength < 1) {
				MessageBox(NULL, "Ingrese todos los datos", "Error", MB_ICONERROR);
				break;
			}

			GetWindowText(hTxtNewPetName, cPetName, ++iPetNameLenght);
			GetWindowText(hTxtNewPetMotive, cPetMotive, ++iPetMotiveLenght);
			GetWindowText(hTxtNewClientName, cClientName, ++iClientNameLenght);
			GetWindowText(hTxtNewTelephone, cPhone, ++iPhoneLenght);
			GetWindowText(hTxtNewPrice, cPrice, ++iPriceLength);
			GetWindowText(hTxtNewTipo, ctype, ++iTypeLength);

			GetWindowText(hTxtNewDateDay, cDateDay, ++iDateDayLength);
			GetWindowText(hTxtNewDateMonth, cDateMonth, ++iDateMonthLength);
			GetWindowText(hTxtNewDateYear, cDateYear, ++iDateYearLength);

			bool Digit = FALSE;
			for (int i = 0; i < iPetNameLenght - 1; i++) {

				if (isdigit(cPetName[i]) == 0) {
					// no es digito
				}
				else {
					Digit = TRUE;
					MessageBox(NULL, "Nombre de mascota no debe tener numeros", "Error", MB_ICONERROR);
					SetWindowText(hTxtNewPetName, "");
					break;
				}
			}
			if (Digit == TRUE) {
				break;
			}

			Digit = FALSE;
			for (int i = 0; i < iClientNameLenght - 1; i++) {
				if (isdigit(cClientName[i]) == 0) {
					// no es digito
				}
				else {
					Digit = TRUE;
					MessageBox(NULL, "Nombre de cliente no debe tener numeros", "Error", MB_ICONERROR);
					SetWindowText(hTxtNewClientName, "");
					break;
				}
			}
			if (Digit == TRUE) {
				break;
			}

			Digit = FALSE;
			for (int i = 0; i < iPhoneLenght - 1; i++) {
				if (isdigit(cPhone[i]) == 0) {
					Digit = TRUE;
					MessageBox(NULL, "telefono debe tener solo numeros", "Error", MB_ICONERROR);
					SetWindowText(hTxtNewTelephone, "");
					break;
				}
				else {
				}
			}
			if (Digit == TRUE) {
				break;
			}

			Digit = FALSE;
			for (int i = 0; i < iDateDayLength - 1; i++) {
				if (isdigit(cDateDay[i]) == 0) {
					Digit = TRUE;
					MessageBox(NULL, "Dia debe tener solo numeros", "Error", MB_ICONERROR);
					SetWindowText(hTxtNewDateDay, "");
					break;
				}
				else {
				}
			}
			if (Digit == TRUE) {
				break;
			}

			Digit = FALSE;
			for (int i = 0; i < iDateMonthLength - 1; i++) {
				if (isdigit(cDateMonth[i]) == 0) {
					Digit = TRUE;
					MessageBox(NULL, "Mes debe tener solo numeros", "Error", MB_ICONERROR);
					SetWindowText(hTxtNewDateMonth, "");
					break;
				}
				else {
				}
			}
			if (Digit == TRUE) {
				break;
			}

			Digit = FALSE;
			for (int i = 0; i < iDateYearLength - 1; i++) {
				if (isdigit(cDateYear[i]) == 0) {
					Digit = TRUE;
					MessageBox(NULL, "Año debe tener solo numeros", "Error", MB_ICONERROR);
					SetWindowText(hTxtNewDateYear, "");
					break;
				}
				else {
				}
			}
			if (Digit == TRUE) {
				break;
			}

			if ((iPhoneLenght > 9 || iPhoneLenght < 9)) {
				if ((iPhoneLenght > 11 || iPhoneLenght < 11)) {
					if ((iPhoneLenght > 13 || iPhoneLenght < 13)) {
						MessageBox(NULL, "telefono debe tener 8, 10 0 12 digitos", "Error", MB_ICONERROR);
						SetWindowText(hTxtNewTelephone, "");
						break;
					}
				}
			}

			Digit = FALSE;
			int contPunto = 0;
			for (int i = 0; i < iPriceLength - 1; i++) {
				if (isdigit(cPrice[i]) == 0) {
					char p[2] = ".";
					if (cPrice[i] == p[0]) {
						contPunto++;
					}
					else {
						Digit = TRUE;
						MessageBox(NULL, "precio debe tener solo numeros", "Error", MB_ICONERROR);
						SetWindowText(hTxtNewPrice, "");
						break;
					}
				}
				else {
				}
			}
			if (contPunto > 1) {
				MessageBox(NULL, "precio debe tener solo un punto decimal", "Error", MB_ICONERROR);
				SetWindowText(hTxtNewPrice, "");
				break;
			}
			if (Digit == TRUE) {
				break;
			}

			if (bImagenSelecPet1 == TRUE || bImagenSelecPet2 == TRUE || bImagenSelecPet3 == TRUE) {
			}
			else {
				MessageBox(NULL, "Debe seleccionar almenos una imagen para continuar", "Alerta", MB_ICONWARNING);
				break;
			}

			int iAuxDia = 0;
			iAuxDia = atoi(cDateDay);

			int iAuxMonth = 0;
			iAuxMonth = atoi(cDateMonth);

			int iAuxYear = 0;
			iAuxYear = atoi(cDateYear);

			BOOL bValidDate = false;

			if (iAuxMonth >= 1 && iAuxMonth <= 12)
			{

				if (iAuxMonth == 1 || iAuxMonth == 3 || iAuxMonth == 5 ||
					iAuxMonth == 7 || iAuxMonth == 8 || iAuxMonth == 10 ||
					iAuxMonth == 12) {
					if (iAuxDia >= 1 && iAuxDia <= 31) {
						bValidDate = true;

					}
					else {
						bValidDate = false;

					}
				}
				else
					if (iAuxMonth == 4 || iAuxMonth == 6 || iAuxMonth == 9 || iAuxMonth == 11)
					{
						if (iAuxDia >= 1 && iAuxDia <= 30) {
							bValidDate = true;

						}
						else {
							bValidDate = false;

						}
					}
					else
						if (iAuxMonth == 2)
						{
							if (iAuxYear % 4 == 0 && iAuxYear % 100 != 0 || iAuxYear % 400 == 0) {
								if (iAuxDia >= 1 && iAuxDia <= 29) {
									bValidDate = true;

								}
								else {
									bValidDate = false;

								}
							}
							else {
								if (iAuxDia >= 1 && iAuxDia <= 28) {
									bValidDate = true;

								}
								else {
									bValidDate = false;

								}
							}

						}

			}
			else {
				MessageBox(NULL, "Mes debe ser entre 1 y 12", "Error", MB_ICONERROR);
				SetWindowText(hTxtNewDateMonth, "");
				break;
			}
			if (bValidDate == false) {
				MessageBox(NULL, "Fecha invalida", "Error", MB_ICONERROR);
				SetWindowText(hTxtNewDateMonth, "");
				SetWindowText(hTxtNewDateDay, "");
				SetWindowText(hTxtNewDateYear, "");
				break;
			}

#pragma endregion obtencion

			if (bCitaAlt == TRUE) {

				if (originC == NULL) {

					originC = new cita;

					originC->nameMascota = cPetName;
					originC->motivo = cPetMotive;
					originC->IDp = iContPetID;
					originC->tipMascota = ctype;
					originC->cliente = cClientName;
					originC->telefono = cPhone;
					originC->costo = atof(cPrice);

					strcpy_s(originC->bitmapPet1, cCurrCita.currbitmapPet1);
					strcpy_s(originC->bitmapPet2, cCurrCita.currbitmapPet2);
					strcpy_s(originC->bitmapPet3, cCurrCita.currbitmapPet3);

					originC->fechaCita.dia = cDateDay;
					originC->fechaCita.mes = cDateMonth;
					originC->fechaCita.año = cDateYear;

					originC->iIDvinculo = cCurr.IDmC;

					iContPetID++;

					originC->nextC = NULL;
					originC->prevC = NULL;

					MessageBox(NULL, "paciente Registrado Exitosamente", "XD", MB_ICONINFORMATION);

				}

				else {

					auxC = originC;
					while (auxC->nextC != NULL) {
						auxC = auxC->nextC;
					}

					auxC->nextC = new cita;
					auxC->nextC->prevC = auxC;
					auxC = auxC->nextC;

					auxC->nameMascota = cPetName;
					auxC->motivo = cPetMotive;
					auxC->IDp = iContPetID;
					auxC->tipMascota = ctype;
					auxC->cliente = cClientName;
					auxC->telefono = cPhone;
					auxC->costo = atof(cPrice);

					strcpy_s(auxC->bitmapPet1, cCurrCita.currbitmapPet1);
					strcpy_s(auxC->bitmapPet2, cCurrCita.currbitmapPet2);
					strcpy_s(auxC->bitmapPet3, cCurrCita.currbitmapPet3);

					auxC->fechaCita.dia = cDateDay;
					auxC->fechaCita.mes = cDateMonth;
					auxC->fechaCita.año = cDateYear;

					auxC->iIDvinculo = cCurr.IDmC;

					iContPetID++;

					auxC->nextC = NULL;

					auxC = originC;

					MessageBox(NULL, "paciente Registrado Exitosamente", "XD", MB_ICONINFORMATION);

				}
			}
			else {

				auxC->nameMascota = cPetName;
				auxC->motivo = cPetMotive;
				auxC->tipMascota = ctype;
				auxC->cliente = cClientName;
				auxC->telefono = cPhone;
				auxC->costo = atof(cPrice);

				strcpy_s(auxC->bitmapPet1, cCurrCita.currbitmapPet1);
				strcpy_s(auxC->bitmapPet2, cCurrCita.currbitmapPet2);
				strcpy_s(auxC->bitmapPet3, cCurrCita.currbitmapPet3);

				auxC->fechaCita.dia = cDateDay;
				auxC->fechaCita.mes = cDateMonth;
				auxC->fechaCita.año = cDateYear;

				MessageBox(NULL, "Actualizado Exitosamente", "XD", MB_ICONINFORMATION);


			}

			EndDialog(hwnd, NumInfExit);
		}

		else if (LOWORD(wparam) == ID_BIT1 && HIWORD(wparam) == BN_CLICKED) {
			OPENFILENAME ImagenBusquedaPet1;
			ZeroMemory(&ImagenBusquedaPet1, sizeof(ImagenBusquedaPet1));
			char cDireccionImagen[MAX_PATH] = " ";

			ImagenBusquedaPet1.hwndOwner = hwnd;
			ImagenBusquedaPet1.nMaxFile = MAX_PATH;
			ImagenBusquedaPet1.lpstrDefExt = ".bmp";
			ImagenBusquedaPet1.lStructSize = sizeof(OPENFILENAME);
			ImagenBusquedaPet1.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			ImagenBusquedaPet1.lpstrFile = cDireccionImagen;
			ImagenBusquedaPet1.lpstrFilter = "Mostrar imagenes\0*.bmp";

			if (GetOpenFileName(&ImagenBusquedaPet1) == TRUE) {
				bImagenSelecPet1 = TRUE;

				strcpy_s(cCurrCita.currbitmapPet1, cDireccionImagen);
				HBITMAP hImagenDoc = NULL;
				hImagenDoc = (HBITMAP)LoadImage(NULL, cDireccionImagen, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
				HWND hPictureControl = GetDlgItem(hwnd, ID_SHOWBIT1_ALT);
				SendMessage(hPictureControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenDoc);
				break;
			}
			else {
				bImagenSelecPet1 = FALSE;

				strcpy_s(cCurrCita.currbitmapPet1, cDireccionImagen);
				HBITMAP hImagenDoc = NULL;
				hImagenDoc = (HBITMAP)LoadImage(NULL, cDireccionImagen, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
				HWND hPictureControl = GetDlgItem(hwnd, ID_SHOWBIT1_ALT);
				SendMessage(hPictureControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenDoc);

				MessageBox(NULL, "No selecciono una imagen", "Alerta", MB_ICONWARNING);
				break;

			}
		}

		else if (LOWORD(wparam) == ID_BIT2 && HIWORD(wparam) == BN_CLICKED) {
			OPENFILENAME ImagenBusquedaPet2;
			ZeroMemory(&ImagenBusquedaPet2, sizeof(ImagenBusquedaPet2));
			char cDireccionImagen[MAX_PATH] = " ";

			ImagenBusquedaPet2.hwndOwner = hwnd;
			ImagenBusquedaPet2.nMaxFile = MAX_PATH;
			ImagenBusquedaPet2.lpstrDefExt = ".bmp";
			ImagenBusquedaPet2.lStructSize = sizeof(OPENFILENAME);
			ImagenBusquedaPet2.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			ImagenBusquedaPet2.lpstrFile = cDireccionImagen;
			ImagenBusquedaPet2.lpstrFilter = "Mostrar imagenes\0*.bmp";

			if (GetOpenFileName(&ImagenBusquedaPet2) == TRUE) {
				bImagenSelecPet2 = TRUE;

				strcpy_s(cCurrCita.currbitmapPet2, cDireccionImagen);
				HBITMAP hImagenDoc = NULL;
				hImagenDoc = (HBITMAP)LoadImage(NULL, cDireccionImagen, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
				HWND hPictureControl = GetDlgItem(hwnd, ID_SHOWBIT2_ALT);
				SendMessage(hPictureControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenDoc);
				break;
			}
			else {
				bImagenSelecPet2 = FALSE;

				strcpy_s(cCurrCita.currbitmapPet2, cDireccionImagen);
				HBITMAP hImagenDoc = NULL;
				hImagenDoc = (HBITMAP)LoadImage(NULL, cDireccionImagen, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
				HWND hPictureControl = GetDlgItem(hwnd, ID_SHOWBIT2_ALT);
				SendMessage(hPictureControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenDoc);

				MessageBox(NULL, "No selecciono una imagen", "Alerta", MB_ICONWARNING);
				break;

			}
		}

		else if (LOWORD(wparam) == ID_BIT3 && HIWORD(wparam) == BN_CLICKED) {
			OPENFILENAME ImagenBusquedaPet3;
			ZeroMemory(&ImagenBusquedaPet3, sizeof(ImagenBusquedaPet3));
			char cDireccionImagen[MAX_PATH] = " ";

			ImagenBusquedaPet3.hwndOwner = hwnd;
			ImagenBusquedaPet3.nMaxFile = MAX_PATH;
			ImagenBusquedaPet3.lpstrDefExt = ".bmp";
			ImagenBusquedaPet3.lStructSize = sizeof(OPENFILENAME);
			ImagenBusquedaPet3.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			ImagenBusquedaPet3.lpstrFile = cDireccionImagen;
			ImagenBusquedaPet3.lpstrFilter = "Mostrar imagenes\0*.bmp";

			if (GetOpenFileName(&ImagenBusquedaPet3) == TRUE) {
				bImagenSelecPet3 = TRUE;

				strcpy_s(cCurrCita.currbitmapPet3, cDireccionImagen);
				HBITMAP hImagenDoc = NULL;
				hImagenDoc = (HBITMAP)LoadImage(NULL, cDireccionImagen, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
				HWND hPictureControl = GetDlgItem(hwnd, ID_SHOWBIT3_ALT);
				SendMessage(hPictureControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenDoc);
				break;
			}
			else {
				bImagenSelecPet3 = FALSE;

				strcpy_s(cCurrCita.currbitmapPet3, cDireccionImagen);
				HBITMAP hImagenDoc = NULL;
				hImagenDoc = (HBITMAP)LoadImage(NULL, cDireccionImagen, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
				HWND hPictureControl = GetDlgItem(hwnd, ID_SHOWBIT3_ALT);
				SendMessage(hPictureControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenDoc);

				MessageBox(NULL, "No selecciono una imagen", "Alerta", MB_ICONWARNING);
				break;
			}
		}

		break;

	}
	return FALSE;
}


//botones de salir
BOOL CALLBACK fsalir(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	switch (msg) {

	case WM_COMMAND:
		if (LOWORD(wparam) == ID_SALIR_GUARDAR && HIWORD(wparam) == BN_CLICKED) {
			guardarMed(originM);
			guardariContMedID(iContMedID);
			guardarPet(originC);

			guardariContPetID(iContPetID);
			EndDialog(hwnd, NumSalidaExit);
		}
		else if (LOWORD(wparam) == ID_SALIR_NOGUARD && HIWORD(wparam) == BN_CLICKED) {

			EndDialog(hwnd, NumSalidaExit);
		}
		break;

	case WM_INITDIALOG:

		return TRUE;
		break;

	case WM_CLOSE:
		//	DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		//	PostQuitMessage(NumPrincipalExit);
		break;

	default:
		//	DefWindowProc(hwnd, msg, wparam, lparam);
		break;
	}
	return FALSE;
}

#pragma endregion CALLS

#pragma region Lectura_Escritura

void cargaMed() {
	binarioM.open("Medicos.bin", ios::binary | ios::in | ios::ate);
	if (binarioM.is_open()) {
		int sizeOfFile = binarioM.tellg();
		if (sizeOfFile < 1) {
			binarioM.close();
			return;
		}
		for (int i = 0; i < sizeOfFile / sizeof(med); i++) {
			med* tempM = new med;
			binarioM.seekg(i * sizeof(med));
			if (originM == NULL) {
				originM = new med;
				binarioM.read(reinterpret_cast<char*>(tempM), sizeof(med));
				originM->user = tempM->user;
				originM->name = tempM->name;
				originM->pasword = tempM->pasword;
				originM->ced = tempM->ced;
				strcpy_s(originM->bitmapDoc, tempM->bitmapDoc);
				originM->IDm = tempM->IDm;

				originM->nextM = NULL;
				originM->prevM = NULL;
				auxM = originM;
			}
			else {
				auxM = originM;
				while (auxM->nextM != NULL) {
					auxM = auxM->nextM;
				}
				binarioM.read(reinterpret_cast<char*>(tempM), sizeof(med));
				auxM->nextM = new med;
				auxM->nextM->prevM = auxM;
				auxM = auxM->nextM;
				auxM->user = tempM->user;
				auxM->name = tempM->name;
				auxM->pasword = tempM->pasword;
				auxM->ced = tempM->ced;
				strcpy_s(auxM->bitmapDoc, tempM->bitmapDoc);
				auxM->IDm = tempM->IDm;
				auxM = auxM->nextM = NULL;
				auxM = originM;
			}
			delete reinterpret_cast<char*>(tempM);
		}
		binarioM.close();
	}
	else {
		MessageBox(NULL, "No se pudo cargar archivo ¨med¨", "XD", MB_ICONERROR);
	}
}


void guardarMed(med *originM) {

	const char* file_name = "\\Medicos.bin ";
	binarioM.open(original_path + std::string(file_name), ios::binary | ios::out | ios::trunc);

	if (binarioM.is_open()) {
		while (originM != NULL) {

			binarioM.write(reinterpret_cast<char*>(originM), sizeof(med));

			med* tempM = originM;
			originM = originM->nextM;
			delete tempM;

		}
		MessageBox(NULL, "Guardado info de med", "XD", MB_ICONINFORMATION);

		binarioM.flush();
		binarioM.close();
	}
	else {
		MessageBox(NULL, "Error al guardar", "XD", MB_ICONERROR);
	}
	return;
}


void cargaiContMedID() {
	const char* FILENAM = "MedicosId.bin";
	int toRestore = 0;
	ifstream i(FILENAM, ios::binary);
	i.read((char*)&iContMedID, sizeof(iContMedID));
	int x = iContMedID;
	i.close();
}


void guardariContMedID(int iContMedID) {

	const char* file_name = "\\MedicosId.bin ";
	binarioMedId.open(original_path + std::string(file_name), ios::binary | ios::out | ios::trunc);

	if (binarioMedId.is_open()) {


		binarioMedId.write(reinterpret_cast<const char*>(&iContMedID), sizeof(iContMedID));

		binarioMedId.close();
	}
	else {

	}
	return;
}


void cargaiContPetID() {
	const char* FILENAM = "CitasId.bin";
	int toRestore = 0;
	ifstream i(FILENAM, ios::binary);
	i.read((char*)&iContPetID, sizeof(iContPetID));
	int x = iContPetID;
	i.close();
}


void guardariContPetID(int iContPetID) {

	const char* file_name = "\\CitasId.bin ";
	binarioPetId.open(original_path + std::string(file_name), ios::binary | ios::out | ios::trunc);

	if (binarioPetId.is_open()) {

		binarioPetId.write(reinterpret_cast<const char*>(&iContPetID), sizeof(iContPetID));

		binarioPetId.close();
	}
	else {

	}
	return;
}


void guardarPet(cita *originC) {

	const char* file_name = "\\Citas.bin ";
	binarioPet.open(original_path + std::string(file_name), ios::binary | ios::out | ios::trunc);

	if (binarioPet.is_open()) {
		while (originC != NULL) {

			int lmao = sizeof(cita);
			binarioPet.write(reinterpret_cast<char*>(originC), sizeof(cita));

			cita* tempC = originC;
			originC = originC->nextC;
			delete tempC;

		}
		//	MessageBox(NULL, "Guardado info de pet", "XD", MB_ICONINFORMATION);

		binarioPet.flush();
		binarioPet.close();
	}
	else {
		MessageBox(NULL, "Error al guardar", "XD", MB_ICONERROR);
	}
	return;
}


void cargaPet() {
	binarioPet.open("Citas.bin ", ios::binary | ios::in | ios::ate);
	if (binarioPet.is_open()) {
		int sizeOfFile = binarioPet.tellg();
		if (sizeOfFile < 1) {
			binarioPet.close();
			return;
		}
		for (int i = 0; i < sizeOfFile / sizeof(cita); i++) {
			cita* tempC = new cita;
			binarioPet.seekg(i * sizeof(cita));
			if (originC == NULL) {
				originC = new cita;
				binarioPet.read(reinterpret_cast<char*>(tempC), sizeof(cita));

				originC->nameMascota = tempC->nameMascota;
				originC->motivo = tempC->motivo;
				originC->IDp = tempC->IDp;
				originC->tipMascota = tempC->tipMascota;
				originC->cliente = tempC->cliente;
				originC->telefono = tempC->telefono;
				originC->costo = tempC->costo;

				strcpy_s(originC->bitmapPet1, tempC->bitmapPet1);
				strcpy_s(originC->bitmapPet2, tempC->bitmapPet2);
				strcpy_s(originC->bitmapPet3, tempC->bitmapPet3);

				originC->fechaCita.dia = tempC->fechaCita.dia;
				originC->fechaCita.mes = tempC->fechaCita.mes;
				originC->fechaCita.año = tempC->fechaCita.año;

				originC->iIDvinculo = tempC->iIDvinculo;



				originC->nextC = NULL;
				originC->prevC = NULL;
				auxC = originC;
			}
			else {
				auxC = originC;
				while (auxC->nextC != NULL) {
					auxC = auxC->nextC;
				}
				binarioPet.read(reinterpret_cast<char*>(tempC), sizeof(cita));
				auxC->nextC = new cita;
				auxC->nextC->prevC = auxC;
				auxC = auxC->nextC;

				auxC->nameMascota = tempC->nameMascota;
				auxC->motivo = tempC->motivo;
				auxC->IDp = tempC->IDp;
				auxC->tipMascota = tempC->tipMascota;
				auxC->cliente = tempC->cliente;
				auxC->telefono = tempC->telefono;
				auxC->costo = tempC->costo;

				strcpy_s(auxC->bitmapPet1, tempC->bitmapPet1);
				strcpy_s(auxC->bitmapPet2, tempC->bitmapPet2);
				strcpy_s(auxC->bitmapPet3, tempC->bitmapPet3);

				auxC->fechaCita.dia = tempC->fechaCita.dia;
				auxC->fechaCita.mes = tempC->fechaCita.mes;
				auxC->fechaCita.año = tempC->fechaCita.año;

				auxC->iIDvinculo = tempC->iIDvinculo;



				auxC = auxC->nextC = NULL;
				auxC = originC;
			}


			delete reinterpret_cast<char*>(tempC);
		}
		binarioPet.close();
	}
	else {
		MessageBox(NULL, "No se pudo cargar archivo ¨pet", "XD", MB_ICONERROR);
	}
}


void registro() {

	//Registrar usuario
	if (bNewUser == TRUE) {

		if (originM == NULL) {

			bNewUser = FALSE;

			bFirstUser = TRUE;

			originM = new med;

			originM->user = cCurr.userC;
			originM->pasword = cCurr.paswordC;
			originM->IDm = cCurr.IDmC;
			originM->name = cCurr.nameC;
			originM->ced = cCurr.cedC;

			strcpy_s(originM->bitmapDoc, cCurr.cbitmapDoc);

			iContMedID++;

			originM->nextM = NULL;
			originM->prevM = NULL;


			MessageBox(NULL, "Primer Usuario Registrado Exitosamente", "XD", MB_ICONINFORMATION);

		}
		else {

			bNewUser = FALSE;

			auxM->nextM = new med;
			auxM->nextM->prevM = auxM;
			auxM = auxM->nextM;

			auxM->user = cCurr.userC;
			auxM->pasword = cCurr.paswordC;
			auxM->IDm = cCurr.IDmC;
			auxM->name = cCurr.nameC;
			auxM->ced = cCurr.cedC;

			strcpy_s(auxM->bitmapDoc, cCurr.cbitmapDoc);

			iContMedID++;

			auxM->nextM = NULL;

			MessageBox(NULL, "Usuario generico Registrado actualizado", "XD", MB_ICONINFORMATION);

		}

	}

	//modificar usuario
	else {

		auxM = originM;
		while (auxM->IDm != cCurr.IDmC) {
			auxM = auxM->nextM;
		}
		auxM->name = cCurr.nameC;
		auxM->ced = cCurr.cedC;
		strcpy_s(auxM->bitmapDoc, cCurr.cbitmapDoc);

		MessageBox(NULL, "Usuario Actualizado Exitosamente", "XD", MB_ICONINFORMATION);
		return;
	}

}

#pragma endregion Lectura_Escritura
