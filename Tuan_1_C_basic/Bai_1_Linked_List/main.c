#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
    char id[20];
    char name[50];
    float gpa;

    struct Node *next;
} Node;

void clearInputBuffer(void)
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

Node *createNode(char id[], char name[], float gpa)
{
    Node *newNode = (Node *)malloc(sizeof(Node));

    if (newNode == NULL)
    {
        printf("Khong the cap phat bo nho!\n");
        exit(1);
    }

    strcpy(newNode->id, id);
    strcpy(newNode->name, name);

    newNode->gpa = gpa;
    newNode->next = NULL;

    return newNode;
}

void addStudent(Node **head, char id[], char name[], float gpa)
{
    Node *newNode = createNode(id, name, gpa);

    if (*head == NULL)
    {
        *head = newNode;
        return;
    }

    Node *temp = *head;

    while (temp->next != NULL)
    {
        temp = temp->next;
    }

    temp->next = newNode;
}

void displayList(Node *head)
{
    if (head == NULL)
    {
        printf("Danh sach rong!\n");
        return;
    }

    printf("\n===== DANH SACH SINH VIEN =====\n");

    while (head != NULL)
    {
        printf("MSV : %s\n", head->id);
        printf("Ten : %s\n", head->name);
        printf("GPA : %.2f\n", head->gpa);

        printf("-----------------------------\n");

        head = head->next;
    }
}

Node *searchStudent(Node *head, char id[])
{
    while (head != NULL)
    {
        if (strcmp(head->id, id) == 0)
        {
            return head;
        }

        head = head->next;
    }

    return NULL;
}

void deleteStudent(Node **head, char id[])
{
    if (*head == NULL)
    {
        printf("Danh sach rong!\n");
        return;
    }

    Node *temp = *head;
    Node *prev = NULL;

    if (strcmp(temp->id, id) == 0)
    {
        *head = temp->next;

        free(temp);

        printf("Da xoa sinh vien co MSV = %s\n", id);

        return;
    }

    while (temp != NULL && strcmp(temp->id, id) != 0)
    {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL)
    {
        printf("Khong tim thay sinh vien co MSV = %s\n", id);
        return;
    }

    prev->next = temp->next;

    free(temp);

    printf("Da xoa sinh vien co MSV = %s\n", id);
}

void freeList(Node **head)
{
    Node *temp;

    while (*head != NULL)
    {
        temp = *head;

        *head = (*head)->next;

        free(temp);
    }
}

int main(void)
{
    Node *head = NULL;

    int choice;

    char id[20];
    char name[50];

    float gpa;

    do
    {
        printf("\n========== MENU ==========\n");
        printf("1. Them sinh vien\n");
        printf("2. Hien thi danh sach\n");
        printf("3. Tim sinh vien theo MSV\n");
        printf("4. Xoa sinh vien theo MSV\n");
        printf("0. Thoat\n");
        printf("==========================\n");

        printf("Nhap lua chon: ");
        scanf("%d", &choice);

        clearInputBuffer();

        switch (choice)
        {
        case 1:
            printf("Nhap MSV: ");
            fgets(id, sizeof(id), stdin);

            id[strcspn(id, "\n")] = '\0';

            printf("Nhap ten: ");
            fgets(name, sizeof(name), stdin);

            name[strcspn(name, "\n")] = '\0';

            printf("Nhap GPA: ");
            scanf("%f", &gpa);

            clearInputBuffer();

            addStudent(&head, id, name, gpa);

            printf("Them sinh vien thanh cong!\n");

            break;

        case 2:
            displayList(head);

            break;

        case 3:
        {
            printf("Nhap MSV can tim: ");
            fgets(id, sizeof(id), stdin);

            id[strcspn(id, "\n")] = '\0';

            Node *student = searchStudent(head, id);

            if (student != NULL)
            {
                printf("\nTim thay sinh vien:\n");

                printf("MSV : %s\n", student->id);
                printf("Ten : %s\n", student->name);
                printf("GPA : %.2f\n", student->gpa);
            }
            else
            {
                printf("Khong tim thay sinh vien!\n");
            }

            break;
        }

        case 4:
            printf("Nhap MSV can xoa: ");
            fgets(id, sizeof(id), stdin);

            id[strcspn(id, "\n")] = '\0';

            deleteStudent(&head, id);

            break;

        case 0:
            printf("Dang thoat chuong trinh...\n");

            break;

        default:
            printf("Lua chon khong hop le!\n");
        }

    } while (choice != 0);

    freeList(&head);

    return 0;
}