#include "people_db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <conio.h>

#define INITIAL_CAPACITY 10   // Начальная емкость базы данных
#define GROWTH_FACTOR 2       // Коэффициент роста при расширении массива

// Инициализация базы данных
void init_db(PeopleDB* db) {
    db->count = 0;
    db->capacity = INITIAL_CAPACITY;
    // Выделение памяти для начального массива
    db->people = malloc(db->capacity * sizeof(Person));
    if (!db->people) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

// Освобождение ресурсов базы данных
void free_db(PeopleDB* db) {
    free(db->people);
    db->people = NULL;
    db->count = 0;
    db->capacity = 0;
}

// Увеличение емкости базы данных
void resize_db(PeopleDB* db) {
    db->capacity *= GROWTH_FACTOR;
    // Перераспределение памяти с новым размером
    Person* new_people = realloc(db->people, db->capacity * sizeof(Person));
    if (!new_people) {
        fprintf(stderr, "Memory reallocation failed\n");
        free_db(db);
        exit(1);
    }
    db->people = new_people;
}

// Добавление нового человека в базу
void add_person(PeopleDB* db, const char* name, int age, double weight) {
    // Проверка корректности возраста
    if (age < 0 || age > 150) {
        fprintf(stderr, "Error: Age cannot be negative or > 150! %d\n", age);
        return;
    }

    // Проверка корректности веса
    if (weight < 0 || weight > 200) {
        fprintf(stderr, "Error: Weight cannot be negative or > 200! %.2f\n", weight);
        return;
    }

    // Проверка необходимости расширения базы
    if (db->count >= db->capacity) {
        resize_db(db);
    }

    // Копирование данных в базу
    strncpy(db->people[db->count].name, name, MAX_NAME_LEN);
    db->people[db->count].name[MAX_NAME_LEN - 1] = '\0'; // Гарантированное завершение строки
    db->people[db->count].age = age;
    db->people[db->count].weight = weight;
    db->count++;
}

// Удаление человека по индексу
void remove_person(PeopleDB* db, int index) {
    if (index < 0 || index >= db->count) return;  // Проверка валидности индекса

    // Сдвиг элементов для заполнения пустоты
    for (int i = index; i < db->count - 1; i++) {
        db->people[i] = db->people[i + 1];
    }
    db->count--;
}

// Вывод всей базы в консоль
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

// Сохранение базы в файл
void save_to_file(const PeopleDB* db, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    // Запись данных в CSV-формате
    for (int i = 0; i < db->count; i++) {
        fprintf(file, "%s,%d,%f\n",
                db->people[i].name,
                db->people[i].age,
                db->people[i].weight);
    }
    fclose(file);
}

// Загрузка базы из файла
void load_from_file(PeopleDB* db, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file for reading");
        return;
    }

    db->count = 0;  // Сброс текущей базы
    char line[256];
    // Построчное чтение файла
    while (fgets(line, sizeof(line), file)) {
        char name[MAX_NAME_LEN];
        int age;
        double weight;

        // Разбор строки на компоненты
        if (sscanf(line, "%[^,],%d,%lf", name, &age, &weight) == 3) {
            add_person(db, name, age, weight);
        }
    }
    fclose(file);
}

// Функции сравнения для сортировки ----------------------------

// Сравнение по имени (лексикографическое)
int compare_by_name(const Person* a, const Person* b) {
    return strcmp(a->name, b->name);
}

// Сравнение по возрасту
int compare_by_age(const Person* a, const Person* b) {
    return a->age - b->age;
}

// Сравнение по весу
int compare_by_weight(const Person* a, const Person* b) {
    return (a->weight > b->weight) ? 1 : -1;
}

// Сортировка базы данных
void sort_db(PeopleDB* db, CompareFunc compare, int ascending) {
    // Пузырьковая сортировка
    for (int i = 0; i < db->count - 1; i++) {
        for (int j = 0; j < db->count - i - 1; j++) {
            int cmp = compare(&db->people[j], &db->people[j + 1]);
            // Учет направления сортировки (возрастание/убывание)
            if ((ascending && cmp > 0) || (!ascending && cmp < 0)) {
                // Обмен элементов
                Person temp = db->people[j];
                db->people[j] = db->people[j + 1];
                db->people[j + 1] = temp;
            }
        }
    }
}

// Отображение столбчатой диаграммы
void show_bar_chart(PeopleDB* db, int field) {
    if (db->count == 0) {
        printf("Database is empty. Cannot draw chart.\n");
        return;
    }

    // Находим максимальное значение для масштабирования
    double max_value = 0;
    for (int i = 0; i < db->count; i++) {
        double value = (field == 1) ? db->people[i].age : db->people[i].weight;
        if (value > max_value) max_value = value;
    }
    if (max_value <= 0) max_value = 1;  // Защита от деления на ноль

    // Инициализация графического режима
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "");

    if (graphresult() != grOk) {
        printf("Failed to initialize graphics mode.\n");
        return;
    }

    // Получение размеров экрана
    int width = getmaxx();
    int height = getmaxy();
    int margin = 50;
    int graph_width = width - 2 * margin;
    int graph_height = height - 2 * margin;

    // Расчет параметров столбцов
    int bar_width = graph_width / (db->count * 5);
    int spacing = bar_width / 2;
    if (bar_width < 10) bar_width = 10;  // Минимальная ширина столбца

    // Рисование осей
    setcolor(WHITE);
    line(margin, margin, margin, margin + graph_height); // Y ось
    line(margin, margin + graph_height, margin + graph_width, margin + graph_height); // X ось

    // Подписи оси Y
    char max_label[20];
    sprintf(max_label, "%.1f", max_value);
    outtextxy(margin - 40, margin, max_label);
    outtextxy(margin - 20, margin + graph_height, "0");

    // Рисование столбцов
    for (int i = 0; i < db->count; i++) {
        double value = (field == 1) ? db->people[i].age : db->people[i].weight;
        int bar_height = (int)((value / max_value) * graph_height);

        // Расчет координат столбца
        int left = margin + spacing + i * (bar_width + spacing);
        int top = margin + graph_height - bar_height;
        int right = left + bar_width;
        int bottom = margin + graph_height;

        // Рисование столбца
        setfillstyle(SOLID_FILL, i % 15 + 1);  // Циклическая смена цветов
        bar(left, top, right, bottom);
        rectangle(left, top, right, bottom);

        // Подпись имени
        setcolor(WHITE);
        char name_label[MAX_NAME_LEN];
        snprintf(name_label, MAX_NAME_LEN, "%s", db->people[i].name);

        // Сокращение длинных имен
        if (strlen(name_label) > 8) {
            name_label[5] = '.';
            name_label[6] = '.';
            name_label[7] = '\0';
        }

        int text_x = left + (bar_width - textwidth(name_label)) / 2;
        outtextxy(text_x, bottom + 10, name_label);

        // Подпись значения над столбцом
        char value_label[20];
        sprintf(value_label, "%.1f", value);
        int value_x = left + (bar_width - textwidth(value_label)) / 2;
        outtextxy(value_x, top - 20, value_label);
    }

    // Заголовок диаграммы
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
    setcolor(YELLOW);
    char title[50];
    sprintf(title, "Bar chart for %s", (field == 1) ? "Age" : "Weight");
    outtextxy(width/2 - textwidth(title)/2, 10, title);

    getch();  // Ожидание нажатия клавиши
    closegraph();  // Закрытие графического режима
}

// Отображение круговой диаграммы
void show_pie_chart(PeopleDB* db, int field) {
    if (db->count == 0) {
        printf("Database is empty. Cannot draw chart.\n");
        return;
    }

    // Вычисление общего значения для пропорций
    double total = 0;
    for (int i = 0; i < db->count; i++) {
        double value = (field == 1) ? db->people[i].age : db->people[i].weight;
        total += value;
    }
    if (total <= 0) {
        printf("Total is zero. Cannot draw pie chart.\n");
        return;
    }

    // Инициализация графики
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
    int radius = (height < width ? height : width) / 3;  // Адаптивный радиус

    float start_angle = 0;      // Начальный угол сектора
    float current_angle = 0;    // Текущий угол
    float remaining = 360.0;    // Оставшийся угол

    // Очистка экрана
    setfillstyle(SOLID_FILL, BLACK);
    bar(0, 0, width, height);

    // Рисование секторов
    for (int i = 0; i < db->count; i++) {
        double value = (field == 1) ? db->people[i].age : db->people[i].weight;
        float angle = (value / total) * 360.0;

        // Корректировка последнего сектора для точности
        float end_angle = start_angle + angle;
        if (i == db->count - 1) {
            end_angle = start_angle + remaining;
        } else {
            remaining -= angle;
        }

        // Установка цвета и рисование сектора
        int color = i % 15 + 1;
        setfillstyle(SOLID_FILL, color);
        setcolor(color);
        pieslice(center_x, center_y, (int)start_angle, (int)end_angle, radius);

        start_angle = end_angle;
    }

    // Рисование легенды
    int legend_x = center_x + radius + 20;
    int legend_y = center_y - radius;

    for (int i = 0; i < db->count; i++) {
        double value = (field == 1) ? db->people[i].age : db->people[i].weight;
        double percent = (value / total) * 100.0;

        // Элемент легенды (цветной квадрат)
        int color = i % 15 + 1;
        setfillstyle(SOLID_FILL, color);
        bar(legend_x, legend_y, legend_x + 20, legend_y + 20);
        setcolor(WHITE);
        rectangle(legend_x, legend_y, legend_x + 20, legend_y + 20);

        // Текст легенды
        setcolor(WHITE);
        char label[100];
        sprintf(label, "%s: %.1f (%.1f%%)", db->people[i].name, value, percent);
        outtextxy(legend_x + 25, legend_y + 5, label);

        legend_y += 30;  // Смещение для следующего элемента
    }

    // Заголовок диаграммы
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
    setcolor(YELLOW);
    char title[50];
    sprintf(title, "Pie chart for %s", (field == 1) ? "Age" : "Weight");
    outtextxy(width/2 - textwidth(title)/2, 10, title);

    getch();  // Ожидание нажатия клавиши
    closegraph();  // Закрытие графики
}
