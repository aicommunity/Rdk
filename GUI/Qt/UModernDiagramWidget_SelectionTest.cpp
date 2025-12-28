// Тесты для метода selectNodesInRect
// Этот файл можно запустить отдельно для тестирования логики выделения

#include "UModernDiagramWidget.h"
#include <QApplication>
#include <QGraphicsScene>
#include <QTest>
#include <QDebug>
#include <QGraphicsItem>
#include <cassert>

// Простой тестовый класс для проверки логики выделения
class SelectionTest
{
public:
    // Тест 1: Пустой прямоугольник
    static void testEmptyRect()
    {
        qDebug() << "Тест 1: Пустой прямоугольник";
        UModernDiagramWidget* widget = new UModernDiagramWidget(nullptr);
        QRectF emptyRect(0, 0, 0, 0);
        int count = widget->selectNodesInRect(emptyRect, false);
        qDebug() << "  Выделено объектов:" << count;
        assert(count == 0);
        delete widget;
        qDebug() << "  ✓ Пройден";
    }
    
    // Тест 2: Прямоугольник без узлов
    static void testRectWithoutNodes()
    {
        qDebug() << "Тест 2: Прямоугольник без узлов в сцене";
        UModernDiagramWidget* widget = new UModernDiagramWidget(nullptr);
        QRectF normalRect(0, 0, 100, 100);
        int count = widget->selectNodesInRect(normalRect, false);
        qDebug() << "  Выделено объектов:" << count;
        assert(count == 0);
        delete widget;
        qDebug() << "  ✓ Пройден";
    }
    
    // Тест 3: Проверка addToSelection
    static void testAddToSelection()
    {
        qDebug() << "Тест 3: Проверка addToSelection";
        UModernDiagramWidget* widget = new UModernDiagramWidget(nullptr);
        QRectF normalRect(0, 0, 100, 100);
        int count1 = widget->selectNodesInRect(normalRect, false);
        int count2 = widget->selectNodesInRect(normalRect, true);
        qDebug() << "  Первое выделение (clear):" << count1;
        qDebug() << "  Второе выделение (add):" << count2;
        assert(count1 == 0);
        assert(count2 == 0);
        delete widget;
        qDebug() << "  ✓ Пройден";
    }
    
    // Тест 4: Проверка логики выделения с реальными узлами
    // Этот тест требует полной инициализации виджета с приложением
    static void testSelectionLogic()
    {
        qDebug() << "Тест 4: Проверка логики выделения";
        qDebug() << "  Этот тест требует полной инициализации виджета";
        qDebug() << "  Для полного тестирования нужно:";
        qDebug() << "    1. Создать UModernDiagramWidget с приложением";
        qDebug() << "    2. Добавить несколько NodeItem в сцену";
        qDebug() << "    3. Вызвать selectNodesInRect с прямоугольником, покрывающим узлы";
        qDebug() << "    4. Проверить, что все узлы внутри прямоугольника выделены";
        qDebug() << "  ✓ Тест пропущен (требует полной инициализации)";
    }
    
    static void runAllTests()
    {
        qDebug() << "\n========================================";
        qDebug() << "Запуск тестов для selectNodesInRect";
        qDebug() << "========================================\n";
        
        testEmptyRect();
        testRectWithoutNodes();
        testAddToSelection();
        testSelectionLogic();
        
        qDebug() << "\n========================================";
        qDebug() << "Все базовые тесты завершены!";
        qDebug() << "========================================\n";
    }
};

// Для запуска тестов можно использовать:
// int main(int argc, char *argv[])
// {
//     QApplication app(argc, argv);
//     SelectionTest::runAllTests();
//     return 0;
// }

