#include "UModernDiagramScopePath.h"
#include "UModernDiagramExternalSinkLayout.h"

#include <gtest/gtest.h>

#include <QPointF>
#include <QRectF>
#include <QSet>
#include <QSizeF>
#include <QString>

using namespace UModernDiagramScopePath;

namespace {

constexpr const char* kNeuron =
    "NeuronTimeLearnerBranch.Neuron";
constexpr const char* kDendrite85 =
    "NeuronTimeLearnerBranch.Neuron.Dendrite1_85";
constexpr const char* kExcChannel85 =
    "NeuronTimeLearnerBranch.Neuron.Dendrite1_85.ExcChannel";
constexpr const char* kInhChannel85 =
    "NeuronTimeLearnerBranch.Neuron.Dendrite1_85.InhChannel";
constexpr const char* kExcChannel84 =
    "NeuronTimeLearnerBranch.Neuron.Dendrite1_84.ExcChannel";
constexpr const char* kInhChannel84 =
    "NeuronTimeLearnerBranch.Neuron.Dendrite1_84.InhChannel";
constexpr const char* kExcSynapse85 =
    "NeuronTimeLearnerBranch.Neuron.Dendrite1_85.ExcSynapse1";

} // namespace

TEST(UModernDiagramScopePath, TopChild_StripsFullScopeBeforeFirstSegment)
{
    EXPECT_EQ(topChildName(kExcChannel85, kDendrite85, QStringLiteral("Dendrite1_85")),
              QStringLiteral("ExcChannel"));
    EXPECT_EQ(topChildName(kExcChannel84, kDendrite85, QStringLiteral("Dendrite1_85")),
              QStringLiteral("NeuronTimeLearnerBranch"));
    EXPECT_EQ(topChildName(kExcChannel85, kNeuron, QStringLiteral("Neuron")),
              QStringLiteral("Dendrite1_85"));
}

TEST(UModernDiagramScopePath, Dendrite85_SiblingChannelExport_IsOutgoingSink)
{
    const QSet<QString> visible = {
        QStringLiteral("ExcSynapse1"),
        QStringLiteral("InhSynapse1"),
        QStringLiteral("ExcChannel"),
        QStringLiteral("InhChannel")
    };

    EXPECT_TRUE(isOutgoingExternalExport(
        QStringLiteral("Output"), kExcChannel85,
        QStringLiteral("ChannelInputs"), kExcChannel84,
        kDendrite85, QStringLiteral("Dendrite1_85"), visible));

    EXPECT_TRUE(isOutgoingExternalExport(
        QStringLiteral("Output"), kInhChannel85,
        QStringLiteral("ChannelInputs"), kInhChannel84,
        kDendrite85, QStringLiteral("Dendrite1_85"), visible));
}

TEST(UModernDiagramScopePath, Dendrite85_InternalSynapseToChannel_NotOutgoingSink)
{
    const QSet<QString> visible = {
        QStringLiteral("ExcSynapse1"),
        QStringLiteral("InhSynapse1"),
        QStringLiteral("ExcChannel"),
        QStringLiteral("InhChannel")
    };

    EXPECT_FALSE(isOutgoingExternalExport(
        QStringLiteral("Output"), kExcSynapse85,
        QStringLiteral("SynapticInputs"), kExcChannel85,
        kDendrite85, QStringLiteral("Dendrite1_85"), visible));
}

TEST(UModernDiagramScopePath, NeuronScope_SiblingDendriteLink_NotOutgoingSink)
{
    const QSet<QString> visible = {
        QStringLiteral("Dendrite1_85"),
        QStringLiteral("Dendrite1_84"),
        QStringLiteral("Soma"),
        QStringLiteral("LTZone")
    };

    EXPECT_FALSE(isOutgoingExternalExport(
        QStringLiteral("Output"), kExcChannel85,
        QStringLiteral("ChannelInputs"), kExcChannel84,
        kNeuron, QStringLiteral("Neuron"), visible));
}

TEST(UModernDiagramScopePath, NeuronScope_NoLeaveNeuronExports_InStructTrainSample)
{
    const QSet<QString> visible = {
        QStringLiteral("Dendrite1_85"),
        QStringLiteral("Dendrite1_84"),
        QStringLiteral("Soma")
    };
    const QString outside = QStringLiteral("PatternResponseAnalyzer");

    EXPECT_FALSE(resolvesToVisibleNode(
        outside, kNeuron, QStringLiteral("Neuron"), visible));
    EXPECT_TRUE(isOutgoingExternalExport(
        QStringLiteral("Output"), kExcChannel85,
        QStringLiteral("Target"), outside,
        kNeuron, QStringLiteral("Neuron"), visible));
}

TEST(ExternalSinkLayout, SinkTopLeft_IsRightOfSourceNotLeftPocket)
{
    const QRectF src(400, 100, 80, 40);
    const QRectF nodes = QRectF(0, 0, 480, 200).united(src);
    const QSizeF item(120, 24);

    const QPointF sinkPos = ExternalSinkLayout::sinkTopLeftBesideSource(src, item, 0, 28.0, 22.0);
    EXPECT_GT(sinkPos.x(), src.right());
    EXPECT_LT(sinkPos.x(), src.right() + 28.0 + 1.0);
    EXPECT_FALSE(ExternalSinkLayout::isPlausibleSinkTopLeft(
        QPointF(-360, -72), src, nodes));
    EXPECT_TRUE(ExternalSinkLayout::isPlausibleSinkTopLeft(sinkPos, src, nodes));
}

TEST(UModernDiagramScopePath, SinkLabel_DropsCurrentLevelName)
{
    EXPECT_EQ(labelPathUnderScope(kExcChannel85, kDendrite85, QStringLiteral("Dendrite1_85")),
              QStringLiteral("ExcChannel"));
    EXPECT_EQ(labelPathUnderScope(kInhChannel85, kDendrite85, QStringLiteral("Dendrite1_85")),
              QStringLiteral("InhChannel"));
    // At Neuron scope nested channel keeps Dendrite1_85 as relative prefix (not current level).
    EXPECT_EQ(labelPathUnderScope(kExcChannel85, kNeuron, QStringLiteral("Neuron")),
              QStringLiteral("Dendrite1_85.ExcChannel"));
}
