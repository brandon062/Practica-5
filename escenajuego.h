#ifndef ESCENAJUEGO_H
#define ESCENAJUEGO_H

#include <QGraphicsScene>
#include <QTimer>
#include <QVector>
#include <QGraphicsPixmapItem>
#include "vector2d.h"
#include "bloqueestructura.h"

// EscenaJuego:
//  - Hereda de QGraphicsScene y contiene todo el "nivel".
//  - Controla el proyectil, la gravedad, los choques, turnos y victoria.
class EscenaJuego : public QGraphicsScene
{
    Q_OBJECT
public:
    enum Bando { Izquierda, Derecha };
    Q_ENUM(Bando)

    explicit EscenaJuego(QObject *parent = nullptr);

    Bando turnoActual() const { return m_turno; }

    // Dispara un proyectil desde el jugador del turno actual
    void dispararProyectil(double anguloGrados, double velocidad);

signals:
    void turnoCambiado(EscenaJuego::Bando nuevoTurno);
    void partidaTerminada(EscenaJuego::Bando ganador);

private slots:
    void actualizarSimulacion();

private:
    // Estado interno del proyectil (similar a una Particula).
    struct EstadoProyectil {
        bool     activo{false};
        double   masa{10.0};
        double   radio{8.0};
        Vector2D posicion;
        Vector2D velocidad;
        double   tiempoVida{0.0};
    };

    // Métodos auxiliares de la simulación
    void configurarMundo();
    void reiniciarProyectil(Bando bando, double anguloGrados, double velocidad);
    void integrar(double dt);
    void resolverChoquesParedes();
    void resolverChoquesBloques();
    void comprobarGolpeRival();
    void finalizarTurno();
    bool circuloIntersecaRect(const Vector2D &c, double r,
                              const QRectF &rect) const;

    Bando m_turno;
    EstadoProyectil m_proyectil;
    QGraphicsEllipseItem *m_itemProyectil{nullptr};
    QTimer m_temporizador;

    // Bloques que reciben daño
    QVector<BloqueEstructura*> m_bloquesIzquierda;
    QVector<BloqueEstructura*> m_bloquesDerecha;

    // Rivales: ahora sprites
    QGraphicsPixmapItem *m_rivalIzquierda{nullptr};
    QGraphicsPixmapItem *m_rivalDerecha{nullptr};

    // Cañones y sus plataformas (los cañones ahora son sprites)
    QGraphicsPixmapItem *m_canionIzquierda{nullptr};
    QGraphicsPixmapItem *m_canionDerecha{nullptr};
    QGraphicsRectItem   *m_plataformaIzquierda{nullptr};
    QGraphicsRectItem   *m_plataformaDerecha{nullptr};

    double m_ancho{1200.0};
    double m_alto{600.0};

    double m_gravedad{200.0};          // px/s^2
    double m_coefRestEstructura{0.5};  // coeficiente de restitución con bloques
    double m_factorDanio{0.02};        // constante para calcular daño
};

#endif // ESCENAJUEGO_H

