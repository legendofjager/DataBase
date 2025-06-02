#include "people_db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <conio.h>

#define INITIAL_CAPACITY 10   // ��������� ������� ���� ������
#define GROWTH_FACTOR 2       // ����������� ����� ��� ���������� �������

// ������������� ���� ������
void init_db(PeopleDB* db) {
    db->count = 0;
    db->capacity = INITIAL_CAPACITY;
    // ��������� ������ ��� ���������� �������
    db->people = malloc(db->capacity * sizeof(Person));
    if (!db->people) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

// ������������ �������� ���� ������
void free_db(PeopleDB* db) {
    free(db->people);
    db->people = NULL;
    db->count = 0;
    db->capacity = 0;
}

// ���������� ������� ���� ������
void resize_db(PeopleDB* db) {
    db->capacity *= GROWTH_FACTOR;
    // ����������������� ������ � ����� ��������
    Person* new_people = realloc(db->people, db->capacity * sizeof(Person));
    if (!new_people) {
        fprintf(stderr, "Memory reallocation failed\n");
        free_db(db);
        exit(1);
    }
    db->people = new_people;
}

// ���������� ������ �������� � ����
void add_person(PeopleDB* db, const char* name, int age, double weight) {
    // �������� ������������ ��������
    if (age < 0 || age > 150) {
        fprintf(stderr, "Error: Age cannot be negative or > 150! %d\n", age);
        return;
    }

    // �������� ������������ ����
    if (weight < 0 || weight > 200) {
        fprintf(stderr, "Error: Weight cannot be negative or > 200! %.2f\n", weight);
        return;
    }

    // �������� ������������� ���������� ����
    if (db->count >= db->capacity) {
        resize_db(db);
    }

    // ����������� ������ � ����
    strncpy(db->people[db->count].name, name, MAX_NAME_LEN);
    db->people[db->count].name[MAX_NAME_LEN - 1] = '\0'; // ��������������� ���������� ������
    db->people[db->count].age = age;
    db->people[db->count].weight = weight;
    db->count++;
}

// �������� �������� �� �������
void remove_person(PeopleDB* db, int index) {
    if (index < 0 || index >= db->count) return;  // �������� ���������� �������

    // ����� ��������� ��� ���������� �������
    for (int i = index; i < db->count - 1; i++) {
        db->people[i] = db->people[i + 1];
    }
    db->count--;
}

// ����� ���� ���� � �������
void print_db(const PeopleDB* db) {
    printf("%-20s %-10s %-10s\n", "Name", "Age", "Weight");
    printf("--------------------------------\n");
    for (int i = 0; i < db->count; i++) {
        printf("%-20s %-10d %-10.1f\n",
               db->people[i].name,
               db->people[i].age,
               db->people[i].weight);
    }
}

// ���������� ���� � ����
void save_to_file(const PeopleDB* db, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    // ������ ������ � CSV-�������
    for (int i = 0; i < db->count; i++) {
        fprintf(file, "%s,%d,%f\n",
                db->people[i].name,
                db->people[i].age,
                db->people[i].weight);
    }
    fclose(file);
}

// �������� ���� �� �����
void load_from_file(PeopleDB* db, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file for reading");
        return;
    }

    db->count = 0;  // ����� ������� ����
    char line[256];
    // ���������� ������ �����
    while (fgets(line, sizeof(line), file)) {
        char name[MAX_NAME_LEN];
        int age;
        double weight;

        // ������ ������ �� ����������
        if (sscanf(line, "%[^,],%d,%lf", name, &age, &weight) == 3) {
            add_person(db, name, age, weight);
        }
    }
    fclose(file);
}

// ������� ��������� ��� ���������� ----------------------------

// ��������� �� ����� (������������������)
int compare_by_name(const Person* a, const Person* b) {
    return strcmp(a->name, b->name);
}

// ��������� �� ��������
int compare_by_age(const Person* a, const Person* b) {
    return a->age - b->age;
}

// ��������� �� ����
int compare_by_weight(const Person* a, const Person* b) {
    return (a->weight > b->weight) ? 1 : -1;
}

// ���������� ���� ������
void sort_db(PeopleDB* db, CompareFunc compare, int ascending) {
    // ����������� ����������
    for (int i = 0; i < db->count - 1; i++) {
        for (int j = 0; j < db->count - i - 1; j++) {
            int cmp = compare(&db->people[j], &db->people[j + 1]);
            // ���� ����������� ���������� (�����������/��������)
            if ((ascending && cmp > 0) || (!ascending && cmp < 0)) {
                // ����� ���������
                Person temp = db->people[j];
                db->people[j] = db->people[j + 1];
                db->people[j + 1] = temp;
            }
        }
    }
}

// ����������� ���������� ���������
void show_bar_chart(PeopleDB* db, int field) {
    if (db->count == 0) {
        printf("Database is empty. Cannot draw chart.\n");
        return;
    }

    // ������� ������������ �������� ��� ���������������
    double max_value = 0;
    for (int i = 0; i < db->count; i++) {
        double value = (field == 1) ? db->people[i].age : db->people[i].weight;
        if (value > max_value) max_value = value;
    }
    if (max_value <= 0) max_value = 1;  // ������ �� ������� �� ����

    // ������������� ������������ ������
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "");

    if (graphresult() != grOk) {
        printf("Failed to initialize graphics mode.\n");
        return;
    }

    // ��������� �������� ������
    int width = getmaxx();
    int height = getmaxy();
    int margin = 50;
    int graph_width = width - 2 * margin;
    int graph_height = height - 2 * margin;

    // ������ ���������� ��������
    int bar_width = graph_width / (db->count * 5);
    int spacing = bar_width / 2;
    if (bar_width < 10) bar_width = 10;  // ����������� ������ �������

    // ��������� ����
    setcolor(WHITE);
    line(margin, margin, margin, margin + graph_height); // Y ���
    line(margin, margin + graph_height, margin + graph_width, margin + graph_height); // X ���

    // ������� ��� Y
    char max_label[20];
    sprintf(max_label, "%.1f", max_value);
    outtextxy(margin - 40, margin, max_label);
    outtextxy(margin - 20, margin + graph_height, "0");

    // ��������� ��������
    for (int i = 0; i < db->count; i++) {
        double value = (field == 1) ? db->people[i].age : db->people[i].weight;
        int bar_height = (int)((value / max_value) * graph_height);

        // ������ ��������� �������
        int left = margin + spacing + i * (bar_width + spacing);
        int top = margin + graph_height - bar_height;
        int right = left + bar_width;
        int bottom = margin + graph_height;

        // ��������� �������
        setfillstyle(SOLID_FILL, i % 15 + 1);  // ����������� ����� ������
        bar(left, top, right, bottom);
        rectangle(left, top, right, bottom);

        // ������� �����
        setcolor(WHITE);
        char name_label[MAX_NAME_LEN];
        snprintf(name_label, MAX_NAME_LEN, "%s", db->people[i].name);

        // ���������� ������� ����
        if (strlen(name_label) > 8) {
            name_label[5] = '.';
            name_label[6] = '.';
            name_label[7] = '\0';
        }

        int text_x = left + (bar_width - textwidth(name_label)) / 2;
        outtextxy(text_x, bottom + 10, name_label);

        // ������� �������� ��� ��������
        char value_label[20];
        sprintf(value_label, "%.1f", value);
        int value_x = left + (bar_width - textwidth(value_label)) / 2;
        outtextxy(value_x, top - 20, value_label);
    }

    // ��������� ���������
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
    setcolor(YELLOW);
    char title[50];
    sprintf(title, "Bar chart for %s", (field == 1) ? "Age" : "Weight");
    outtextxy(width/2 - textwidth(title)/2, 10, title);

    getch();  // �������� ������� �������
    closegraph();  // �������� ������������ ������
}

// ����������� �������� ���������
void show_pie_chart(PeopleDB* db, int field) {
    if (db->count == 0) {
        printf("Database is empty. Cannot draw chart.\n");
        return;
    }

    // ���������� ������ �������� ��� ���������
    double total = 0;
    for (int i = 0; i < db->count; i++) {
        double value = (field == 1) ? db->people[i].age : db->people[i].weight;
        total += value;
    }
    if (total <= 0) {
        printf("Total is zero. Cannot draw pie chart.\n");
        return;
    }

    // ������������� �������
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "");

    if (graphresult() != grOk) {
        printf("Failed to initialize graphics mode.\n");
        return;
    }

    int width = getmaxx();
    int height = getmaxy();
    int center_x = width / 2;
    int center_y = height / 2;
    int radius = (height < width ? height : width) / 3;  // ���������� ������

    float start_angle = 0;      // ��������� ���� �������
    float current_angle = 0;    // ������� ����
    float remaining = 360.0;    // ���������� ����

    // ������� ������
    setfillstyle(SOLID_FILL, BLACK);
    bar(0, 0, width, height);

    // ��������� ��������
    for (int i = 0; i < db->count; i++) {
        double value = (field == 1) ? db->people[i].age : db->people[i].weight;
        float angle = (value / total) * 360.0;

        // ������������� ���������� ������� ��� ��������
        float end_angle = start_angle + angle;
        if (i == db->count - 1) {
            end_angle = start_angle + remaining;
        } else {
            remaining -= angle;
        }

        // ��������� ����� � ��������� �������
        int color = i % 15 + 1;
        setfillstyle(SOLID_FILL, color);
        setcolor(color);
        pieslice(center_x, center_y, (int)start_angle, (int)end_angle, radius);

        start_angle = end_angle;
    }

    // ��������� �������
    int legend_x = center_x + radius + 20;
    int legend_y = center_y - radius;

    for (int i = 0; i < db->count; i++) {
        double value = (field == 1) ? db->people[i].age : db->people[i].weight;
        double percent = (value / total) * 100.0;

        // ������� ������� (������� �������)
        int color = i % 15 + 1;
        setfillstyle(SOLID_FILL, color);
        bar(legend_x, legend_y, legend_x + 20, legend_y + 20);
        setcolor(WHITE);
        rectangle(legend_x, legend_y, legend_x + 20, legend_y + 20);

        // ����� �������
        setcolor(WHITE);
        char label[100];
        sprintf(label, "%s: %.1f (%.1f%%)", db->people[i].name, value, percent);
        outtextxy(legend_x + 25, legend_y + 5, label);

        legend_y += 30;  // �������� ��� ���������� ��������
    }

    // ��������� ���������
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
    setcolor(YELLOW);
    char title[50];
    sprintf(title, "Pie chart for %s", (field == 1) ? "Age" : "Weight");
    outtextxy(width/2 - textwidth(title)/2, 10, title);

    getch();  // �������� ������� �������
    closegraph();  // �������� �������
}
