#ifndef UPARAMETER_FORM_EDITS_H
#define UPARAMETER_FORM_EDITS_H

#include <QHash>
#include <QString>
#include <QVariant>
#include <exception>

class QWidget;

/// Compares an editor with the last value presented by a form.
/// Rounded spin-box values therefore do not cause unrelated property writes.
class UParameterFormEdits
{
public:
    void clear() { initialValues.clear(); failedProperty.clear(); }

    template<class Value>
    void remember(const QWidget* editor, const Value& value)
    {
        initialValues.insert(editor, QVariant::fromValue(value));
    }

    template<class Property, class Value>
    void apply(const QWidget* editor, Property& property, const Value& value)
    {
        const auto initial = initialValues.constFind(editor);
        if (!failedProperty.isEmpty() || initial == initialValues.cend()
            || initial.value() == QVariant::fromValue(value))
            return;
        try
        {
            property.SetData(value);
            if (property.GetData() == value)
                return;
        }
        catch (const std::exception&)
        {
        }
        failedProperty = QString::fromStdString(property.GetName());
    }

    QString failedField() const { return failedProperty; }

private:
    QHash<const QWidget*, QVariant> initialValues;
    QString failedProperty;
};

#endif
