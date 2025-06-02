#include "people_db.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    PeopleDB db;
    init_db(&db);

    if (argc > 1) {
        load_from_file(&db, argv[1]);
    }

    int choice;
    do {
        printf("\nPeople Database Menu:\n");
        printf("1. Add person\n");
        printf("2. Remove person\n");
        printf("3. Print database\n");
        printf("4. Save to file\n");
        printf("5. Sort database\n");
        printf("6. Show charts\n");
        printf("7. Open existing database\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                char name[MAX_NAME_LEN];
                int age;
                double weight;

                printf("Enter name: ");
                scanf("%s", name);
                printf("Enter age: ");
                scanf("%d", &age);
                printf("Enter weight: ");
                scanf("%lf", &weight);

                add_person(&db, name, age, weight);
                break;
            }
            case 2: {
                int index;
                print_db(&db);
                printf("Enter index to remove: ");
                scanf("%d", &index);
                remove_person(&db, index);
                break;
            }
            case 3:
                print_db(&db);
                break;
            case 4: {
                char filename[100];
                printf("Enter filename to save: ");
                scanf("%s", filename);
                save_to_file(&db, filename);
                break;
            }
            case 5: {
                int field, order;
                printf("Sort by:\n1. Name\n2. Age\n3. Weight\nChoice: ");
                scanf("%d", &field);
                printf("Order:\n1. Ascending\n2. Descending\nChoice: ");
                scanf("%d", &order);

                CompareFunc compare;
                switch (field) {
                    case 1: compare = compare_by_name; break;
                    case 2: compare = compare_by_age; break;
                    case 3: compare = compare_by_weight; break;
                    default: continue;
                }

                sort_db(&db, compare, order == 1);
                break;
            }
            case 6: {
                int field, chart_type;
                printf("Chart for:\n1. Age\n2. Weight\nChoice: ");
                scanf("%d", &field);

                if (field != 1 && field != 2) {
                        printf("Invalid choice! Please select 1 or 2.\n");
                        break;
                }

                printf("Chart type:\n1. Bar chart\n2. Pie chart\nChoice: ");
                scanf("%d", &chart_type);

                if (chart_type == 1) {
                        show_bar_chart(&db, field);
                } else if (chart_type == 2) {
                    show_pie_chart(&db, field);
                } else {
                    printf("Invalid chart type selection!\n");
                }
                break;


            }
            case 7: {
                char filename[256];
                printf("Enter filename to open: ");
                scanf("%s", filename);

                free_db(&db);
                init_db(&db);

                load_from_file(&db, filename);
                break;
            }
        }
    } while (choice != 0);

    free_db(&db);
    return 0;
}
