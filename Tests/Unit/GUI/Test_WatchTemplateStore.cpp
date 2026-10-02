#include "Plot/WatchTemplateStore.h"
#include "Plot/PlotDocument.h"

#include <gtest/gtest.h>

#include <QTemporaryDir>

TEST(WatchTemplateStore, RoundTripEmptyPanels)
{
    NMSDK::Plot::PlotDocument doc;
    doc.schemaVersion = NMSDK::Plot::PlotDocument::CurrentSchemaVersion;
    doc.gridRows = 1;
    doc.gridCols = 2;
    doc.denseGrid = true;
    NMSDK::Plot::PlotPanel a;
    a.id = QStringLiteral("panel_a");
    a.title = QStringLiteral("A");
    NMSDK::Plot::PlotPanel b;
    b.id = QStringLiteral("panel_b");
    b.title = QStringLiteral("B");
    doc.panels = {a, b};

    QTemporaryDir tmp;
    ASSERT_TRUE(tmp.isValid());
    const QString path = tmp.filePath(QStringLiteral("sample.watch.xml"));
    QString err;
    ASSERT_TRUE(NMSDK::Plot::saveWatchTemplateFile(path, doc, &err)) << err.toStdString();

    NMSDK::Plot::PlotDocument loaded;
    ASSERT_TRUE(NMSDK::Plot::loadWatchTemplateFile(path, loaded, &err)) << err.toStdString();
    EXPECT_EQ(loaded.gridCols, 2);
    EXPECT_EQ(loaded.gridRows, 1);
    EXPECT_TRUE(loaded.denseGrid);
    ASSERT_EQ(loaded.panels.size(), 2);
    EXPECT_EQ(loaded.panels[0].title, QStringLiteral("A"));
    EXPECT_EQ(loaded.panels[1].title, QStringLiteral("B"));

    const QString oldId = loaded.panels[0].id;
    NMSDK::Plot::reassignPlotObjectIds(loaded);
    EXPECT_NE(loaded.panels[0].id, oldId);
}
