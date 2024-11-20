#include "IOCP.h"

const UINT16 SERVER_PORT = 12345;
const UINT16 MAX_CLIENT = 100;

int main()
{
	IOCompletionPort ioCompletionPort;

	ioCompletionPort.InitSocket();

	ioCompletionPort.BindandListen(SERVER_PORT);

	ioCompletionPort.StartServer(MAX_CLIENT);

	printf("�ƹ� Ű�� ���� ������ ����մϴ�\n");
	getchar();

	ioCompletionPort.DestroyThread();
	return 0;
}