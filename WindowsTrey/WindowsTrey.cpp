#include <iostream>
#include <windows.h>
#include <shellapi.h> // API - application programming interface

/*
Windows trey
*/

// Сообщение для обработки иконки в трее
#define WM_SYSICON (WM_USER + 1) 
// Сообщение для выхода
#define WM_COMMAND_EXIT (WM_USER + 2)
// Сообщение для отображения уведомления
#define WM_COMMAND_NOTIFY (WM_USER + 3)
#define WM_COMMAND_NOTIFY1 (WM_USER + 4)

using namespace std;

NOTIFYICONDATA nid; // Структура для хранит данные об иконке

// Функция инициализации иконки
void InitNotifyIconData(HWND hWnd) {
	// Указываем размер структуры
	nid.cbSize = sizeof(NOTIFYICONDATA);

	// Размер структуры
	nid.hWnd = hWnd;

	// Идентификатор иконки
	nid.uID = 1;

	// Флаги: иконка | подсказка | сообщение
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;

	// Указываем сообщение, которое будет установлено окну
	// при взаимодействии с иконкой
	nid.uCallbackMessage = WM_SYSICON;

	nid.hIcon = (HICON)LoadImage(NULL, TEXT("C:/Users/User/Downloads/xterm_color_32x32_22444.ico"),
		IMAGE_ICON, 32, 32,
		LR_LOADFROMFILE | LR_SHARED);

	// Устанавливаем текст и размер текста при наведении
	memcpy(nid.szTip, TEXT("baba"), 2 * sizeof(TCHAR));

	//// Сама функция добавления значка в трею
	Shell_NotifyIcon(NIM_ADD, &nid);

}

void CleanUp() {
	Shell_NotifyIcon(NIM_DELETE, &nid);
}


// Отображение уведомлений
void ShowNotification(const wchar_t* title, const wchar_t* message) {
	NOTIFYICONDATA nidNotify = {};
	nidNotify.cbSize = sizeof(NOTIFYICONDATA);
	nidNotify.hWnd = nid.hWnd;
	nidNotify.uID = nid.uID;
	nidNotify.uFlags = NIF_INFO;
	wcscpy_s(nidNotify.szInfoTitle, title);
	wcscpy_s(nidNotify.szInfo, message);
	nidNotify.uTimeout = 5000; // Ждёт 5 секунд
	Shell_NotifyIcon(NIM_MODIFY, &nidNotify);
}

// обработчик сообщения для окна
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_SYSICON:
		if (lParam == WM_RBUTTONDOWN) {
			// Создаём всплывающее меню
			HMENU hMenu = CreatePopupMenu();
			// Добавляем пункт уведомления
			AppendMenu(hMenu, MF_STRING, WM_COMMAND_NOTIFY, TEXT("Show notification"));
			AppendMenu(hMenu, MF_STRING, WM_COMMAND_NOTIFY1, TEXT("Show notification1"));
			AppendMenu(hMenu, MF_STRING, WM_COMMAND_EXIT, TEXT("Exit"));
			

			// Получаем позицию курсора
			POINT pt;
			GetCursorPos(&pt);
			SetForegroundWindow(hWnd);
			TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
			DestroyMenu(hMenu);
		}
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == WM_COMMAND_NOTIFY) {
			ShowNotification(TEXT("Title"), TEXT("Description"));
		}
		else if (LOWORD(wParam) == WM_COMMAND_EXIT) {
			CleanUp();
			PostQuitMessage(0);
		}
		else if (LOWORD(wParam) == WM_COMMAND_NOTIFY1) {
			ShowNotification(TEXT("Privet"), TEXT("Dryg"));
			break;
	case WM_DESTROY:
		CleanUp();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		return 0;

	}
}

// Точка входа для WINDOWS приложения
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
	WNDCLASS wc = {}; // Структура для хранения информации о классе окна
	wc.lpfnWndProc = WindowProc; // Указываем обработчик сообщений
	wc.hInstance = hInstance; // Дескриптор приложения
	wc.lpszClassName = TEXT("WindowsTrey");

	RegisterClass(&wc); // Регистрируем класс окна
	HWND hWnd = CreateWindow(wc.lpszClassName, TEXT("Tray app"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	// Инициализация иконки в трее
	InitNotifyIconData(hWnd);

	MSG msg;
	// Цикл для обработки сообщений
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
