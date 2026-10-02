#include <stdio.h>

int add(int a, int b)
{
    return a + b;
}

int sub(int a, int b)
{
    return a - b;
}

int mul(int a, int b)
{
    return a * b;
}

int divide(int a, int b)
{
    if (b == 0)
    {
        printf("Khong the chia cho 0!\n");
        return 0;
    }

    return a / b;
}

int main(void)
{
    int a, b;
    int choice;

    int (*operation)(int, int);

    do
    {
        printf("\n===== MENU =====\n");
        printf("1. Cong\n");
        printf("2. Tru\n");
        printf("3. Nhan\n");
        printf("4. Chia\n");
        printf("0. Thoat\n");

        printf("Nhap lua chon: ");
        scanf("%d", &choice);

        if (choice == 0)
        {
            printf("Dang thoat chuong trinh...\n");
            break;
        }

        if (choice < 1 || choice > 4)
        {
            printf("Lua chon khong hop le!\n");
            continue;
        }

        printf("Nhap a: ");
        scanf("%d", &a);

        printf("Nhap b: ");
        scanf("%d", &b);

        switch (choice)
        {
        case 1:
            operation = add;
            break;

        case 2:
            operation = sub;
            break;

        case 3:
            operation = mul;
            break;

        case 4:
            operation = divide;
            break;
        }

        printf("Ket qua = %d\n", operation(a, b));

    } while (choice != 0);

    return 0;
}