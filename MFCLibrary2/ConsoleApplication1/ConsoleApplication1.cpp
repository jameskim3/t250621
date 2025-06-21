#pragma comment (lib, "d:\\install\\CreateDLL.lib")
#include "../MFCLibrary2/MyDLL.h"

int main()
{
	char name[] = "홍길동";
	char address[] = "서울시";

	Person* person1 = CreatePerson(name, 35, address);
	ShowInfo(person1);
	DeletePerson(person1);

	getchar();
}