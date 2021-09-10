#include "frameviewer.h"
#include <QQuickWindow>
#include <QSGSimpleRectNode>
#include <QSGSimpleTextureNode>
#include <QTransform>

FrameViewer::FrameViewer(QQuickItem* parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
}

QSGNode* FrameViewer::updatePaintNode(QSGNode* old_node, QQuickItem::UpdatePaintNodeData*)
{
    if(m_image.isNull())
    {
        delete old_node;
        return nullptr;
    }

    QSGTransformNode* node = nullptr;
    QSGSimpleTextureNode* texture_node = nullptr;

    if(old_node)
    {
        node = static_cast<QSGTransformNode*>(old_node);
        texture_node = static_cast<QSGSimpleTextureNode*>(node->childAtIndex(0));
    }
    else
    {
        node = new QSGTransformNode();
        texture_node = new QSGSimpleTextureNode();
        texture_node->setOwnsTexture(true);
        texture_node->setRect(QRect(QPoint(0, 0), m_image.size()));
        node->appendChildNode(texture_node);
    }

    if(texture_node->rect().size() != m_image.size())
        texture_node->setRect(QRect(QPoint(0, 0), m_image.size()));

    if(m_image.format() == QImage::Format_RGBA8888)
    {
        QImage img(m_image.size(), QImage::Format_Grayscale8);
        for(int i = 0; i < m_image.size().height(); ++i)
        {
            const int* src = reinterpret_cast<const int*>(m_image.constBits()
                                                          + i * m_image.bytesPerLine());
            quint8* dst = reinterpret_cast<quint8*>(img.bits() + i * img.bytesPerLine());
            for(int j = 0; j < m_image.size().width(); ++j)
                dst[j] = qBound(0, (src[j] + 255) / 2, 255);
        }
        texture_node->setTexture(window()->createTextureFromImage(img));
    }
    else
        texture_node->setTexture(window()->createTextureFromImage(m_image));

    QMatrix4x4 m;
    m.scale(float(width()) / m_image.size().width(), float(height()) / m_image.size().height());
    node->setMatrix(m);

    return node;
}

void FrameViewer::setImage(const QImage& frame)
{
    if(frame.isNull())
        return;

    m_image = frame;
    update();
    emit imageChanged();
}

QImage FrameViewer::image() const
{
    return m_image;
}
