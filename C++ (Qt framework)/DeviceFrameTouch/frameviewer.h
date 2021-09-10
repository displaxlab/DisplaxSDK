#ifndef FRAMEVIEWER_H
#define FRAMEVIEWER_H

#include <QImage>
#include <QQuickItem>

class FrameViewer: public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QImage image READ image WRITE setImage NOTIFY imageChanged)

public:
    explicit FrameViewer(QQuickItem* parent = nullptr);

    QSGNode* updatePaintNode(QSGNode* old_node, UpdatePaintNodeData*);

    void setImage(const QImage& image);
    QImage image() const;

signals:
    void imageChanged();

private:
    QImage m_image;
};

#endif // FRAMEVIEWER_H
