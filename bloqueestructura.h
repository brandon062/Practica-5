#ifndef BLOQUEESTRUCTURA_H
#define BLOQUEESTRUCTURA_H

#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include <QBrush>
#include <QPen>
#include <algorithm>    // std::max, std::min

// BloqueEstructura:
//  - Representa un bloque "100" o "200" del edificio.
//  - Tiene una resistencia (vida) y se puede "dañar".
//  - Muestra la vida actual en un texto encima del bloque.
class BloqueEstructura : public QGraphicsRectItem
{
public:
    BloqueEstructura(const QRectF &rect, double resistencia,
                     QGraphicsItem *parent = nullptr)
        : QGraphicsRectItem(rect, parent),
        m_resistencia(resistencia),
        m_destruido(false)
    {
        setBrush(QBrush(Qt::lightGray));
        setPen(QPen(Qt::black));

        // Creamos un texto hijo que muestra la vida del bloque.
        m_etiquetaVida = new QGraphicsSimpleTextItem(this);
        actualizarTextoVida();
    }

    bool destruido() const { return m_destruido; }
    double resistencia() const { return m_resistencia; }

    // Resta vida al bloque en función del daño recibido.
    // Cambia color según la vida y actualiza el texto.
    void aplicarDanio(double danio){
        if (m_destruido) return;

        m_resistencia -= danio;
        if (m_resistencia <= 0.0){
            m_resistencia = 0.0;
            m_destruido = true;
            setBrush(QBrush(Qt::darkGray));
            setOpacity(0.3);   // se ve "roto"
        } else {
            // Color según porcentaje de vida (simplemente cosmético).
            double ratio = std::max(0.0, std::min(1.0, m_resistencia / 200.0));
            int tono = 30 + int(120 * ratio);
            QColor c = QColor::fromHsv(tono, 255, 220);
            setBrush(QBrush(c));
        }

        actualizarTextoVida();
    }

private:
    // Actualiza el texto de vida (número) y lo centra en el bloque.
    void actualizarTextoVida(){
        if (!m_etiquetaVida) return;

        m_etiquetaVida->setText(QString::number(int(m_resistencia)));

        // Centramos el texto respecto al rectángulo local del bloque.
        QRectF r = rect();  // coordenadas locales
        QRectF textoBBox = m_etiquetaVida->boundingRect();

        qreal x = r.left() + (r.width()  - textoBBox.width())  / 2.0;
        qreal y = r.top()  + (r.height() - textoBBox.height()) / 2.0;

        m_etiquetaVida->setPos(x, y);
    }

    double m_resistencia;
    bool   m_destruido;
    QGraphicsSimpleTextItem *m_etiquetaVida{nullptr};
};

#endif // BLOQUEESTRUCTURA_H
