#ifndef PEOPLE_DB_H
#define PEOPLE_DB_H

#include <graphics.h>

#define MAX_NAME_LEN 50

typedef struct {
    char name[MAX_NAME_LEN];
    int age;
    double weight;
} Person;

typedef struct {
    Person* people;
    int count;
    int capacity;
} PeopleDB;

// Инициализация базы
void init_db(PeopleDB* db);
void free_db(PeopleDB* db);

// Добавление человека
void add_person(PeopleDB* db, const char* name, int age, double weight);

// Удаление по индексу
void remove_person(PeopleDB* db, int index);

// Вывод таблицы
void print_db(const PeopleDB* db);

// Сохранение в файл
void save_to_file(const PeopleDB* db, const char* filename);

// Загрузка из файла
void load_from_file(PeopleDB* db, const char* filename);

// Сортировка
typedef int (*CompareFunc)(const Person*, const Person*);
void sort_db(PeopleDB* db, CompareFunc compare, int ascending);

// Функции сравнения для сортировки
int compare_by_name(const Person* a, const Person* b);
int compare_by_age(const Person* a, const Person* b);
int compare_by_weight(const Person* a, const Person* b);

// Функции для отображения диаграмм
void show_bar_chart(PeopleDB* db, int field);
void show_pie_chart(PeopleDB* db, int field);

#endif
