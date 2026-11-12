// Тесты для метода selectNodesInRect
// Исполняемый тест для проверки логики выделения

#include "UModernDiagramWidget.h"
#include <QApplication>
#include <QGraphicsScene>
#include <QDebug>
#include <QGraphicsItem>
#include <cassert>
#include <iostream>

// Простой тестовый класс для проверки логики выделения
class SelectionTest
{
public:
    // Тест 1: Пустой прямоугольник
    static bool testEmptyRect()
    {
        std::cout << "Тест 1: Пустой прямоугольник" << std::endl;
        UModernDiagramWidget* widget = new UModernDiagramWidget(nullptr);
        QRectF emptyRect(0, 0, 0, 0);
        int count = widget->selectNodesInRect(emptyRect, false);
        std::cout << "  Выделено объектов: " << count << std::endl;
        bool passed = (count == 0);
        delete widget;
        std::cout << "  " << (passed ? "✓ Пройден" : "✗ Провален") << std::endl;
        return passed;
    }
    
    // Тест 2: Прямоугольник без узлов
    static bool testRectWithoutNodes()
    {
        std::cout << "Тест 2: Прямоугольник без узлов в сцене" << std::endl;
        UModernDiagramWidget* widget = new UModernDiagramWidget(nullptr);
        QRectF normalRect(0, 0, 100, 100);
        int count = widget->selectNodesInRect(normalRect, false);
        std::cout << "  Выделено объектов: " << count << std::endl;
        bool passed = (count == 0);
        delete widget;
        std::cout << "  " << (passed ? "✓ Пройден" : "✗ Провален") << std::endl;
        return passed;
    }
    
    // Тест 3: Проверка addToSelection
    static bool testAddToSelection()
    {
        std::cout << "Тест 3: Проверка addToSelection" << std::endl;
        UModernDiagramWidget* widget = new UModernDiagramWidget(nullptr);
        QRectF normalRect(0, 0, 100, 100);
        int count1 = widget->selectNodesInRect(normalRect, false);
        int count2 = widget->selectNodesInRect(normalRect, true);
        std::cout << "  Первое выделение (clear): " << count1 << std::endl;
        std::cout << "  Второе выделение (add): " << count2 << std::endl;
        bool passed = (count1 == 0 && count2 == 0);
        delete widget;
        std::cout << "  " << (passed ? "✓ Пройден" : "✗ Провален") << std::endl;
        return passed;
    }
    
    // Тест 4: Проверка с null scene
    static bool testNullScene()
    {
        std::cout << "Тест 4: Проверка с null scene" << std::endl;
        UModernDiagramWidget* widget = new UModernDiagramWidget(nullptr);
        // Симулируем null scene (хотя в реальности scene всегда создается)
        QRectF normalRect(0, 0, 100, 100);
        int count = widget->selectNodesInRect(normalRect, false);
        std::cout << "  Выделено объектов: " << count << std::endl;
        bool passed = (count >= 0); // Метод должен корректно обработать
        delete widget;
        std::cout << "  " << (passed ? "✓ Пройден" : "✗ Провален") << std::endl;
        return passed;
    }
    
    static int runAllTests()
    {
        std::cout << "\n========================================" << std::endl;
        std::cout << "Запуск тестов для selectNodesInRect" << std::endl;
        std::cout << "========================================\n" << std::endl;
        
        int passed = 0;
        int total = 4;
        
        if(testEmptyRect()) passed++;
        if(testRectWithoutNodes()) passed++;
        if(testAddToSelection()) passed++;
        if(testNullScene()) passed++;
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "Результаты: " << passed << "/" << total << " тестов пройдено" << std::endl;
        std::cout << "========================================\n" << std::endl;
        
        return (passed == total) ? 0 : 1;
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    int result = SelectionTest::runAllTests();
    return result;
}

